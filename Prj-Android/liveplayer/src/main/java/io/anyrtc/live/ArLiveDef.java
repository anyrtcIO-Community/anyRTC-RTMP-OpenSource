package io.anyrtc.live;

import org.webrtc.CalledByNative;

import java.nio.ByteBuffer;
import java.util.ArrayList;

import javax.microedition.khronos.egl.EGLContext;

public class ArLiveDef {
    public ArLiveDef() {
    }

    public static final class ArLiveLogConfig {
        public int logLevel = 0;
        public boolean enableObserver = false;
        public boolean enableConsole = false;
        public boolean enableLogFile = true;
        public String logPath;

        public ArLiveLogConfig() {
        }
    }

    public static final class ArLiveLogLevel {
        public static final int ArLiveLogLevelAll = 0;
        public static final int ArLiveLogLevelDebug = 1;
        public static final int ArLiveLogLevelInfo = 2;
        public static final int ArLiveLogLevelWarning = 3;
        public static final int ArLiveLogLevelError = 4;
        public static final int ArLiveLogLevelFatal = 5;
        public static final int ArLiveLogLevelNULL = 6;

        public ArLiveLogLevel() {
        }
    }

    public static final class ArLiveTranscodingConfig {
        public int videoWidth;
        public int videoHeight;
        public int videoBitrate;
        public int videoFramerate;
        public int videoGOP;
        public int backgroundColor;
        public String backgroundImage;
        public int audioSampleRate;
        public int audioBitrate;
        public int audioChannels;
        public ArrayList<ArLiveDef.ArLiveMixStream> mixStreams;
        public String outputStreamId;

        public ArLiveTranscodingConfig() {
            this.videoWidth = 0;
            this.videoHeight = 0;
            this.videoBitrate = 0;
            this.videoFramerate = 15;
            this.videoGOP = 2;
            this.backgroundColor = 0;
            this.audioSampleRate = 48000;
            this.audioBitrate = 64;
            this.audioChannels = 1;
            this.outputStreamId = null;
        }

        public ArLiveTranscodingConfig(ArLiveDef.ArLiveTranscodingConfig original) {
            this.videoWidth = original.videoWidth;
            this.videoHeight = original.videoHeight;
            this.videoBitrate = original.videoBitrate;
            this.videoFramerate = original.videoFramerate;
            this.videoGOP = original.videoGOP;
            this.backgroundColor = original.backgroundColor;
            this.backgroundImage = original.backgroundImage;
            this.audioSampleRate = original.audioSampleRate;
            this.audioBitrate = original.audioBitrate;
            this.audioChannels = original.audioChannels;
            this.outputStreamId = original.outputStreamId;
            this.mixStreams = new ArrayList(original.mixStreams);
        }

        public String toString() {
            return "[videoWidth=" + this.videoWidth + "][videoHeight=" + this.videoHeight + "][videoBitrate=" + this.videoBitrate + "][videoFramerate=" + this.videoFramerate + "][videoGOP=" + this.videoGOP + "][backgroundColor=" + this.backgroundColor + "][backgroundImage='" + this.backgroundImage + '\'' + "][audioSampleRate=" + this.audioSampleRate + "][audioBitrate=" + this.audioBitrate + "][audioChannels=" + this.audioChannels + "][mixStreams=" + this.mixStreams + "][outputStreamId='" + this.outputStreamId + '\'' + ']';
        }
    }

    public static class ArLiveMixStream {
        public String userId;
        public String streamId;
        public int x;
        public int y;
        public int width;
        public int height;
        public int zOrder;
        public ArLiveDef.ArLiveMixInputType inputType;

        public ArLiveMixStream() {
            this.userId = "";
            this.x = 0;
            this.y = 0;
            this.width = 0;
            this.height = 0;
            this.zOrder = 0;
            this.inputType = ArLiveDef.ArLiveMixInputType.ArLiveMixInputTypeAudioVideo;
        }

        public ArLiveMixStream(String userId, int x, int y, int width, int height, int zOrder) {
            this.userId = userId;
            this.x = x;
            this.y = y;
            this.width = width;
            this.height = height;
            this.zOrder = zOrder;
            this.inputType = ArLiveDef.ArLiveMixInputType.ArLiveMixInputTypeAudioVideo;
        }

        public ArLiveMixStream(ArLiveDef.ArLiveMixStream original) {
            this.userId = original.userId;
            this.streamId = original.streamId;
            this.x = original.x;
            this.y = original.y;
            this.width = original.width;
            this.height = original.height;
            this.zOrder = original.zOrder;
            this.inputType = original.inputType;
        }

        public String toString() {
            return "[userId='" + this.userId + '\'' + "][streamId='" + this.streamId + '\'' + "][x=" + this.x + "][y=" + this.y + "][width=" + this.width + "][height=" + this.height + "][zOrder=" + this.zOrder + "][inputType=" + this.inputType + ']';
        }
    }

    public static enum ArLiveMixInputType {
        ArLiveMixInputTypeAudioVideo,
        ArLiveMixInputTypePureVideo,
        ArLiveMixInputTypePureAudio;

        private ArLiveMixInputType() {
        }
    }

    public static enum ArLiveStatusChangeReason {
        ArLiveStatusChangeReasonInternal,
        ArLiveStatusChangeReasonBufferingBegin,
        ArLiveStatusChangeReasonBufferingEnd,
        ArLiveStatusChangeReasonLocalStarted,
        ArLiveStatusChangeReasonLocalStopped,
        ArLiveStatusChangeReasonRemoteStarted,
        ArLiveStatusChangeReasonRemoteStopped,
        ArLiveStatusChangeReasonRemoteOffline;

        private ArLiveStatusChangeReason() {
        }
    }

    public static enum ArLivePlayStatus {
        ArLivePlayStatusStopped,
        ArLivePlayStatusPlaying,
        ArLivePlayStatusLoading;

        private ArLivePlayStatus() {
        }
    }

    public static enum ArLivePushStatus {
        ArLivePushStatusDisconnected,
        ArLivePushStatusConnecting,
        ArLivePushStatusConnectSuccess,
        ArLivePushStatusReconnecting;

        private ArLivePushStatus() {
        }
    }

    public static final class ArLivePlayerStatistics {
        public int appCpu;
        public int systemCpu;
        public int width;
        public int height;
        public int fps;
        public int videoBitrate;
        public int audioBitrate;

        public ArLivePlayerStatistics() {
        }

        public ArLivePlayerStatistics(int appCpu, int systemCpu, int width, int height, int fps, int videoBitrate, int audioBitrate) {
            this.appCpu = appCpu;
            this.systemCpu = systemCpu;
            this.width = width;
            this.height = height;
            this.fps = fps;
            this.videoBitrate = videoBitrate;
            this.audioBitrate = audioBitrate;
        }

        @Override
        public String toString() {
            return "ArLivePlayerStatistics{" +
                    "appCpu=" + appCpu +
                    ", systemCpu=" + systemCpu +
                    ", width=" + width +
                    ", height=" + height +
                    ", fps=" + fps +
                    ", videoBitrate=" + videoBitrate +
                    ", audioBitrate=" + audioBitrate +
                    '}';
        }
    }

    public static final class ArLivePusherStatistics {
        public int appCpu;
        public int systemCpu;
        public int width;
        public int height;
        public int fps;
        public int videoBitrate;
        public int audioBitrate;

        public ArLivePusherStatistics() {
        }
    }

    public static final class ArLiveAudioFrame {
        public byte[] data;
        public int sampleRate;
        public int channel;

        public ArLiveAudioFrame() {
        }
    }

    public static enum ArLiveAudioQuality {
        ArLiveAudioQualitySpeech,
        ArLiveAudioQualityDefault,
        ArLiveAudioQualityMusic;

        private ArLiveAudioQuality() {
        }
    }

    public static final class ArLiveVideoFrame {
        public ArLiveDef.ArLivePixelFormat pixelFormat;
        public ArLiveDef.ArLiveBufferType bufferType;
        public ArLiveDef.ArLiveTexture texture;
        public byte[] data;
        public ByteBuffer buffer;
        public int width;
        public int height;
        public int rotation;
        public int stride;


        public ArLiveVideoFrame() {
            this.pixelFormat = ArLiveDef.ArLivePixelFormat.ArLivePixelFormatUnknown;
            this.bufferType = ArLiveDef.ArLiveBufferType.ArLiveBufferTypeUnknown;
        }

        @CalledByNative
        public ArLiveVideoFrame(ArLivePixelFormat pixelFormat, ArLiveBufferType bufferType, ArLiveTexture texture, byte[] data, ByteBuffer buffer, int width, int height, int rotation,int stride) {
            this.pixelFormat = pixelFormat;
            this.bufferType = bufferType;
            this.texture = texture;
            this.data = data;
            this.buffer = buffer;
            this.width = width;
            this.height = height;
            this.rotation = rotation;
            this.stride = stride;
        }
    }

    public static final class ArLiveTexture {
        public int textureId;
        public EGLContext eglContext10;
        public android.opengl.EGLContext eglContext14;

        public ArLiveTexture() {
        }
    }

    public static enum ArLiveBufferType {
        ArLiveBufferTypeUnknown,
        ArLiveBufferTypeByteBuffer,
        ArLiveBufferTypeByteArray,
        ArLiveBufferTypeTexture;
        private ArLiveBufferType() {
        }
    }


    public static enum ArLivePixelFormat {
        ArLivePixelFormatUnknown,
        ArLivePixelFormatI420,
        ArLivePixelFormatBGRA32,
        ArLivePixelFormatNV12,
        ArLivePixelFormatNV21;

        private ArLivePixelFormat() {
        }
    }

    public static enum ArLiveRotation {
        ArLiveRotation0,
        ArLiveRotation90,
        ArLiveRotation180,
        ArLiveRotation270;

        private ArLiveRotation() {
        }
    }

    public static enum ArLiveFillMode {
        ArLiveFillModeFill,
        ArLiveFillModeFit,
        ArLiveVideoScaleModeAuto;

        private ArLiveFillMode() {
        }
    }

    public static enum ArLiveMirrorType {
        ArLiveMirrorTypeAuto,
        ArLiveMirrorTypeEnable,
        ArLiveMirrorTypeDisable;

        private ArLiveMirrorType() {
        }
    }

    public static final class ArLiveVideoEncoderParam {
        public ArLiveDef.ArLiveVideoResolution videoResolution;
        public ArLiveDef.ArLiveVideoResolutionMode videoResolutionMode;
        public int videoFps;
        public int videoBitrate;
        public int minVideoBitrate;
        public ArLiveDef.ArLiveFillMode videoScaleMode;

        public ArLiveVideoEncoderParam(ArLiveDef.ArLiveVideoResolution videoResolution) {
            this.videoResolution = videoResolution;
            this.videoResolutionMode = ArLiveDef.ArLiveVideoResolutionMode.ArLiveVideoResolutionModePortrait;
            this.videoFps = 15;
            ArLiveUtils.ArBitrate bitrateByResolution = ArLiveUtils.getBitrateByResolution(videoResolution);
            this.videoBitrate = bitrateByResolution.bitrate;
            this.minVideoBitrate = bitrateByResolution.minBitrate;
            this.videoScaleMode = ArLiveFillMode.ArLiveVideoScaleModeAuto;
        }

        public String toString() {
            return "ArLiveVideoEncoderParam{videoResolution=" + this.videoResolution + ", videoResolutionMode=" + this.videoResolutionMode + ", videoFps=" + this.videoFps + ", videoBitrate=" + this.videoBitrate + ", minVideoBitrate=" + this.minVideoBitrate + '}';
        }
    }

    public static enum ArLiveVideoResolutionMode {
        ArLiveVideoResolutionModeLandscape,
        ArLiveVideoResolutionModePortrait;

        private ArLiveVideoResolutionMode() {
        }
    }

    public static enum ArLiveVideoResolution {
        ArLiveVideoResolution160x160,
        ArLiveVideoResolution270x270,
        ArLiveVideoResolution480x480,
        ArLiveVideoResolution320x240,
        ArLiveVideoResolution480x360,
        ArLiveVideoResolution640x480,
        ArLiveVideoResolution320x180,
        ArLiveVideoResolution480x270,
        ArLiveVideoResolution640x360,
        ArLiveVideoResolution960x540,
        ArLiveVideoResolution1280x720,
        ArLiveVideoResolution1920x1080;

        private ArLiveVideoResolution() {
        }
    }


    public static enum ArLiveMode {
        ArLiveMode_RTMP,
        ArLiveMode_RTC;

        private ArLiveMode() {
        }
    }
}
