/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

package org.webrtc;

import static android.media.MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface;
import static android.media.MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar;
import static android.media.MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar;
import static android.media.MediaCodecInfo.CodecCapabilities.COLOR_QCOM_FormatYUV420SemiPlanar;
import static org.webrtc.MediaCodecUtils.EXYNOS_PREFIX;
import static org.webrtc.MediaCodecUtils.INTEL_PREFIX;
import static org.webrtc.MediaCodecUtils.QCOM_PREFIX;

import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.os.Build;
import androidx.annotation.Nullable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/** Factory for android hardware video encoders. */
@SuppressWarnings("deprecation") // API 16 requires the use of deprecated methods.
public class HardwareVideoEncoderFactory implements VideoEncoderFactory {
  private static final String TAG = "HardwareVideoEncoderFactory";

  // Forced key frame interval - used to reduce color distortions on Qualcomm platforms.
  private static final int QCOM_VP8_KEY_FRAME_INTERVAL_ANDROID_L_MS = 15000;
  private static final int QCOM_VP8_KEY_FRAME_INTERVAL_ANDROID_M_MS = 20000;
  private static final int QCOM_VP8_KEY_FRAME_INTERVAL_ANDROID_N_MS = 15000;

  // List of devices with poor H.264 encoder quality.
  // HW H.264 encoder on below devices has poor bitrate control - actual
  // bitrates deviates a lot from the target value.
  private static final String[] supportedH264HwCodecPrefixes = new String[]{"OMX.qcom.", "OMX.Exynos.", "OMX.MTK.", "OMX.IMG.TOPAZ.", "OMX.hisi.", "OMX.k3.", "OMX.amlogic.", "OMX.rk.", "OMX.MS."};
  private static final String[] H264_HW_EXCEPTION_MODELS = new String[]{"SAMSUNG-SGH-I337", "Nexus 7", "Nexus 4", "P6-C00", "HM 2A", "XT105", "XT109", "XT1060"};

  private static final int COLOR_QCOM_FORMATYUV420PackedSemiPlanar32m = 0x7FA30C04;

  @Nullable private final EglBase14.Context sharedContext;
  private final boolean enableIntelVp8Encoder;
  private final boolean enableH264HighProfile;
  @Nullable private final Predicate<MediaCodecInfo> codecAllowedPredicate;
  private static Set<String> hwEncoderDisabledTypes = new HashSet();
  private static String codecOmxName = "";
  private static int mH264SupportProfileHigh = 0;

  private static final int[] supportedColorList = new int[]{COLOR_FormatYUV420Planar, COLOR_FormatYUV420SemiPlanar, COLOR_QCOM_FormatYUV420SemiPlanar, COLOR_QCOM_FORMATYUV420PackedSemiPlanar32m};

  /**
   * Creates a HardwareVideoEncoderFactory that supports surface texture encoding.
   *
   * @param sharedContext The textures generated will be accessible from this context. May be null,
   *                      this disables texture support.
   * @param enableIntelVp8Encoder true if Intel's VP8 encoder enabled.
   * @param enableH264HighProfile true if H264 High Profile enabled.
   */
  public HardwareVideoEncoderFactory(
      EglBase.Context sharedContext, boolean enableIntelVp8Encoder, boolean enableH264HighProfile) {
    this(sharedContext, enableIntelVp8Encoder, enableH264HighProfile,
        /* codecAllowedPredicate= */ null);
  }

  /**
   * Creates a HardwareVideoEncoderFactory that supports surface texture encoding.
   *
   * @param sharedContext The textures generated will be accessible from this context. May be null,
   *                      this disables texture support.
   * @param enableIntelVp8Encoder true if Intel's VP8 encoder enabled.
   * @param enableH264HighProfile true if H264 High Profile enabled.
   * @param codecAllowedPredicate optional predicate to filter codecs. All codecs are allowed
   *                              when predicate is not provided.
   */
  public HardwareVideoEncoderFactory(EglBase.Context sharedContext, boolean enableIntelVp8Encoder,
      boolean enableH264HighProfile, @Nullable Predicate<MediaCodecInfo> codecAllowedPredicate) {
    // Texture mode requires EglBase14.
    if (sharedContext instanceof EglBase14.Context) {
      this.sharedContext = (EglBase14.Context) sharedContext;
    } else {
      Logging.w(TAG, "No shared EglBase.Context.  Encoders will not use texture mode.");
      this.sharedContext = null;
    }
    this.enableIntelVp8Encoder = enableIntelVp8Encoder;
    this.enableH264HighProfile = enableH264HighProfile;
    this.codecAllowedPredicate = codecAllowedPredicate;
  }

  @Deprecated
  public HardwareVideoEncoderFactory(boolean enableIntelVp8Encoder, boolean enableH264HighProfile) {
    this(null, enableIntelVp8Encoder, enableH264HighProfile);
  }

  @Nullable
  @Override
  public VideoEncoder createEncoder(VideoCodecInfo input) {
    // HW encoding is not supported below Android Kitkat.
    if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT) {
      return null;
    }

    VideoCodecMimeType type = VideoCodecMimeType.fromSdpCodecName(input.getName());
    MediaCodecInfo info = findCodecForType(type);

    if (info == null) {
      return null;
    }

    String codecName = info.getName();
    String mime = type.mimeType();
    Integer surfaceColorFormat = MediaCodecUtils.selectColorFormat(
        MediaCodecUtils.TEXTURE_COLOR_FORMATS, info.getCapabilitiesForType(mime));
    Integer yuvColorFormat = MediaCodecUtils.selectColorFormat(
        MediaCodecUtils.ENCODER_COLOR_FORMATS, info.getCapabilitiesForType(mime));

    if (type == VideoCodecMimeType.H264) {
      boolean isHighProfile = H264Utils.isSameH264Profile(
          input.params, MediaCodecUtils.getCodecProperties(type, /* highProfile= */ true));
      boolean isBaselineProfile = H264Utils.isSameH264Profile(
          input.params, MediaCodecUtils.getCodecProperties(type, /* highProfile= */ false));

      if (!isHighProfile && !isBaselineProfile) {
        return null;
      }
      if (isHighProfile && !isH264HighProfileSupported(info)) {
        return null;
      }
    }

    return new HardwareVideoEncoder(new MediaCodecWrapperFactoryImpl(), codecName, type,
        surfaceColorFormat, yuvColorFormat, input.params, getKeyFrameIntervalSec(type),
        getForcedKeyFrameIntervalMs(type, codecName), createBitrateAdjuster(type, codecName),
        sharedContext);
  }

  @Override
  public VideoCodecInfo[] getSupportedCodecs() {
    // HW encoding is not supported below Android Kitkat.
    if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT) {
      return new VideoCodecInfo[0];
    }

    List<VideoCodecInfo> supportedCodecInfos = new ArrayList<VideoCodecInfo>();
    // Generate a list of supported codecs in order of preference:
    // VP8, VP9, H264 (high profile), H264 (baseline profile) and AV1.
    for (VideoCodecMimeType type : new VideoCodecMimeType[] {VideoCodecMimeType.VP8,
             VideoCodecMimeType.VP9, VideoCodecMimeType.H264, VideoCodecMimeType.AV1}) {
      MediaCodecInfo codec = findCodecForType(type);
      if (codec != null) {
        String name = type.toSdpCodecName();
        // TODO(sakal): Always add H264 HP once WebRTC correctly removes codecs that are not
        // supported by the decoder.
        if (type == VideoCodecMimeType.H264 && isH264HighProfileSupported(codec)) {
          supportedCodecInfos.add(new VideoCodecInfo(
              name, MediaCodecUtils.getCodecProperties(type, /* highProfile= */ true)));
        }

        supportedCodecInfos.add(new VideoCodecInfo(
            name, MediaCodecUtils.getCodecProperties(type, /* highProfile= */ false)));
      }
    }

    return supportedCodecInfos.toArray(new VideoCodecInfo[supportedCodecInfos.size()]);
  }

  private @Nullable MediaCodecInfo findCodecForType(VideoCodecMimeType type) {
    for (int i = 0; i < MediaCodecList.getCodecCount(); ++i) {
      MediaCodecInfo info = null;
      try {
        info = MediaCodecList.getCodecInfoAt(i);
      } catch (IllegalArgumentException e) {
        Logging.e(TAG, "Cannot retrieve encoder codec info", e);
      }

      if (info == null || !info.isEncoder()) {
        continue;
      }

      if (isSupportedCodec(info, type)) {
        return info;
      }
    }
    return null; // No support for this type.
  }

  // Returns true if the given MediaCodecInfo indicates a supported encoder for the given type.
  private boolean isSupportedCodec(MediaCodecInfo info, VideoCodecMimeType type) {
    if (!MediaCodecUtils.codecSupportsType(info, type)) {
      return false;
    }
    // Check for a supported color format.
    if (MediaCodecUtils.selectColorFormat(
            MediaCodecUtils.ENCODER_COLOR_FORMATS, info.getCapabilitiesForType(type.mimeType()))
        == null) {
      return false;
    }
    return isHardwareSupportedInCurrentSdk(info, type) && isMediaCodecAllowed(info);
  }

  // Returns true if the given MediaCodecInfo indicates a hardware module that is supported on the
  // current SDK.
  private boolean isHardwareSupportedInCurrentSdk(MediaCodecInfo info, VideoCodecMimeType type) {
    switch (type) {
      case VP8:
        return isHardwareSupportedInCurrentSdkVp8(info);
      case VP9:
        return isHardwareSupportedInCurrentSdkVp9(info);
      case H264:
        return isHardwareSupportedInCurrentSdkH264(info);
      case AV1:
        return false;
    }
    return false;
  }

  private boolean isHardwareSupportedInCurrentSdkVp8(MediaCodecInfo info) {
    String name = info.getName();
    // QCOM Vp8 encoder is supported in KITKAT or later.
    return (name.startsWith(QCOM_PREFIX) && Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT)
        // Exynos VP8 encoder is supported in M or later.
        || (name.startsWith(EXYNOS_PREFIX) && Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
        // Intel Vp8 encoder is supported in LOLLIPOP or later, with the intel encoder enabled.
        || (name.startsWith(INTEL_PREFIX) && Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP
               && enableIntelVp8Encoder);
  }

  private boolean isHardwareSupportedInCurrentSdkVp9(MediaCodecInfo info) {
    String name = info.getName();
    return (name.startsWith(QCOM_PREFIX) || name.startsWith(EXYNOS_PREFIX))
        // Both QCOM and Exynos VP9 encoders are supported in N or later.
        && Build.VERSION.SDK_INT >= Build.VERSION_CODES.N;
  }

  private boolean isHardwareSupportedInCurrentSdkH264(MediaCodecInfo info) {
    try {
      return !hwEncoderDisabledTypes.contains("video/avc") && findHwEncoder("video/avc", supportedH264HwCodecPrefixes, supportedColorList) != null;
    } catch (Exception var1) {
      Logging.e(TAG, "isH264HwSupported failed!");
      return false;
    }
  }

  private static EncoderProperties findHwEncoder(String mime, String[] supportedHwCodecPrefixes, int[] colorList) {
    try {
      return do_findHwEncoder(mime, supportedHwCodecPrefixes, colorList);
    } catch (Exception var4) {
      return null;
    }
  }

  private static EncoderProperties do_findHwEncoder(String mime, String[] supportedHwCodecPrefixes, int[] colorList) {
    if (Build.VERSION.SDK_INT < 19) {
      return null;
    } else {
      boolean var3 = colorList[0] == COLOR_FormatSurface;
      Logging.d(TAG, "Model: " + Build.MODEL + ", hardware: " + Build.HARDWARE);
      List var4;
      if (mime.equals("video/avc")) {
        var4 = Arrays.asList(H264_HW_EXCEPTION_MODELS);
        if (var4.contains(Build.MODEL)) {
          Logging.w(TAG, "Model: " + Build.MODEL + " has black listed H.264 encoder.");
          return null;
        }

        if (Build.HARDWARE.equalsIgnoreCase("kirin970") && !var3) {
          return null;
        }
      }

      for(int var18 = 0; var18 < MediaCodecList.getCodecCount(); ++var18) {
        MediaCodecInfo var5 = MediaCodecList.getCodecInfoAt(var18);
        if (var5.isEncoder()) {
          String var6 = null;
          String[] var7 = var5.getSupportedTypes();
          int var8 = var7.length;

          int var9;
          for(var9 = 0; var9 < var8; ++var9) {
            String var10 = var7[var9];
            if (var10.equals(mime)) {
              var6 = var5.getName();
              break;
            }
          }

          if (var6 != null) {
            if (!checkMinSDKVersion(var6, var3)) {
              Logging.e(TAG, "Check min sdk version failed, " + var6);
            } else {
              Logging.d(TAG, "Found candidate encoder " + var6);
              if (var6.startsWith("OMX.") || var3) {
                codecOmxName = var6;
                MediaCodecInfo.CodecCapabilities var19 = var5.getCapabilitiesForType(mime);
                if (mime.equals("video/avc")) {
                  MediaCodecInfo.CodecProfileLevel[] var20 = var19.profileLevels;
                  var9 = var20.length;

                  for(int var23 = 0; var23 < var9; ++var23) {
                    MediaCodecInfo.CodecProfileLevel var11 = var20[var23];
                    if (var11.profile == 8) {
                      mH264SupportProfileHigh = 1;
                    }
                  }
                }

                if (var6.startsWith("OMX.amlogic.")) {
                  if (var3) {
                    return new EncoderProperties(var6, COLOR_FormatSurface, true);
                  }

                  return new EncoderProperties(var6, 19, true);
                }

                boolean var21 = false;
                String var22 = "   Color:";
                int[] var24 = var19.colorFormats;
                int var25 = var24.length;

                int var12;
                int var13;
                for(var12 = 0; var12 < var25; ++var12) {
                  var13 = var24[var12];
                  if (21 == var13) {
                    var21 = true;
                  }

                  var22 = var22 + " 0x" + Integer.toHexString(var13) + ", ";
                }

                Logging.d(TAG, var22);
                var24 = colorList;
                var25 = colorList.length;

                for(var12 = 0; var12 < var25; ++var12) {
                  var13 = var24[var12];
                  int[] var14 = var19.colorFormats;
                  int var15 = var14.length;

                  for(int var16 = 0; var16 < var15; ++var16) {
                    int var17 = var14[var16];
                    if (var17 == var13) {
                      if (var17 != 19 || !var21 || !var6.startsWith("OMX.IMG.TOPAZ.") && !var6.startsWith("OMX.hisi.") && !var6.startsWith("OMX.k3.")) {
                        Logging.d(TAG, "Found target encoder for mime " + mime + " : " + var6 + ". Color: 0x" + Integer.toHexString(var17));
                        return new EncoderProperties(var6, var17, true);
                      }

                      Logging.d(TAG, "TOPAZ,force use COLOR_FormatYUV420SemiPlanar");
                      Logging.d(TAG, "Found target encoder for mime " + mime + " : " + var6 + ". Color: 0x" + Integer.toHexString(21));
                      return new EncoderProperties(var6, 21, true);
                    }
                  }
                }
              }
            }
          }
        }
      }

      return null;
    }
  }

  private static boolean checkMinSDKVersion(String chipName, boolean isTexture) {
    if (isTexture) {
      return Build.VERSION.SDK_INT >= 19;
    } else if (chipName.startsWith("OMX.qcom.")) {
      return Build.VERSION.SDK_INT >= 19;
    } else if (chipName.startsWith("OMX.MTK.")) {
      return Build.VERSION.SDK_INT >= 21;
    } else if (chipName.startsWith("OMX.Exynos.")) {
      return Build.VERSION.SDK_INT >= 21;
    } else if (chipName.startsWith("OMX.IMG.TOPAZ.")) {
      return Build.VERSION.SDK_INT >= 21;
    } else if (chipName.startsWith("OMX.k3.")) {
      return Build.VERSION.SDK_INT >= 21;
    } else {
      return Build.VERSION.SDK_INT >= 21;
    }
  }


  private static class EncoderProperties {
    public final String codecName;
    public final int colorFormat;
    public final boolean supportedList;

    public EncoderProperties(String codecName, int colorFormat, boolean supportedList) {
      this.codecName = codecName;
      this.colorFormat = colorFormat;
      this.supportedList = supportedList;
    }
  }

  private boolean isMediaCodecAllowed(MediaCodecInfo info) {
    if (codecAllowedPredicate == null) {
      return true;
    }
    return codecAllowedPredicate.test(info);
  }

  public static void disableH264HwCodec() {
    Logging.w(TAG, "H.264 encoding is disabled by application.");
    hwEncoderDisabledTypes.add("video/avc");
  }

  private int getKeyFrameIntervalSec(VideoCodecMimeType type) {
    switch (type) {
      case VP8: // Fallthrough intended.
      case VP9:
      case AV1:
        return 100;
      case H264:
        return 20;
    }
    throw new IllegalArgumentException("Unsupported VideoCodecMimeType " + type);
  }

  private int getForcedKeyFrameIntervalMs(VideoCodecMimeType type, String codecName) {
    if (type == VideoCodecMimeType.VP8 && codecName.startsWith(QCOM_PREFIX)) {
      if (Build.VERSION.SDK_INT == Build.VERSION_CODES.LOLLIPOP
          || Build.VERSION.SDK_INT == Build.VERSION_CODES.LOLLIPOP_MR1) {
        return QCOM_VP8_KEY_FRAME_INTERVAL_ANDROID_L_MS;
      } else if (Build.VERSION.SDK_INT == Build.VERSION_CODES.M) {
        return QCOM_VP8_KEY_FRAME_INTERVAL_ANDROID_M_MS;
      } else if (Build.VERSION.SDK_INT > Build.VERSION_CODES.M) {
        return QCOM_VP8_KEY_FRAME_INTERVAL_ANDROID_N_MS;
      }
    }
    // Other codecs don't need key frame forcing.
    return 0;
  }

  private BitrateAdjuster createBitrateAdjuster(VideoCodecMimeType type, String codecName) {
    if (codecName.startsWith(EXYNOS_PREFIX)) {
      if (type == VideoCodecMimeType.VP8) {
        // Exynos VP8 encoders need dynamic bitrate adjustment.
        return new DynamicBitrateAdjuster();
      } else {
        // Exynos VP9 and H264 encoders need framerate-based bitrate adjustment.
        return new FramerateBitrateAdjuster();
      }
    }
    // Other codecs don't need bitrate adjustment.
    return new BaseBitrateAdjuster();
  }

  private boolean isH264HighProfileSupported(MediaCodecInfo info) {
    return enableH264HighProfile && Build.VERSION.SDK_INT > Build.VERSION_CODES.M
        && info.getName().startsWith(EXYNOS_PREFIX);
  }
}
