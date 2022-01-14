package org.webrtc.effector.format;

import org.webrtc.VideoFrame;

import java.nio.ByteBuffer;

/** I420 と RGBA の間の変換を受け持つクラス。
 *
 * メモリイメージとして RGBA にしたい。libyuv の RGBA 変換はメモリ上のバイト順が
 * 逆順になる。
 * I420ToARGB() の出力はバイト順として B G R A、I420ToABGR() は R G B A  である。
 * 関数の命名として、Java の世界はメモリ順で命名し、native method では libyuv に
 * 合わせて逆順とする。
 */
public class LibYuvBridge {


    public LibYuvBridge() {}

    public void i420ToRgba(ByteBuffer dataYBuffer, int strideY,
                           ByteBuffer dataUBuffer, int strideU,
                           ByteBuffer dataVBuffer, int strideV,
                           int width, int height,
                           ByteBuffer outRgbaBuffer) {
        i420ToAbgrInternal(
                dataYBuffer, strideY,
                dataUBuffer, strideU,
                dataVBuffer, strideV,
                width, height,
                outRgbaBuffer);
    }

    public void rgbaToI420(ByteBuffer rgbaBuffer,
                           int width, int height,
                           ByteBuffer outDataYBuffer, int strideY,
                           ByteBuffer outDataUBuffer, int strideU,
                           ByteBuffer outDataVBuffer, int strideV) {
        abgrToI420Internal(
                rgbaBuffer,
                width, height,
                outDataYBuffer, strideY,
                outDataUBuffer, strideU,
                outDataVBuffer, strideV);
    }

    private native void i420ToAbgrInternal(
            ByteBuffer dataYBuffer, int strideY,
            ByteBuffer dataUBuffer, int strideU,
            ByteBuffer dataVBuffer, int strideV,
            int width, int height,
            ByteBuffer outRgbaBuffer);

    private native void abgrToI420Internal(
            ByteBuffer rgbaBuffer,
            int width, int height,
            ByteBuffer outDataYBuffer, int strideY,
            ByteBuffer outDataUBuffer, int strideU,
            ByteBuffer outDataVBuffer, int strideV);
}
