package io.anyrtc.live;

public class ArLiveUtils {

    public static ArBitrate getBitrateByResolution(ArLiveDef.ArLiveVideoResolution resolution) {
        short bitrate = 800;
        short minBitrate = 1500;
        switch(resolution) {
            case ArLiveVideoResolution160x160:
                bitrate = 100;
                minBitrate = 150;
                break;
            case ArLiveVideoResolution270x270:
                bitrate = 200;
                minBitrate = 300;
                break;
            case ArLiveVideoResolution480x480:
                bitrate = 350;
                minBitrate = 525;
                break;
            case ArLiveVideoResolution320x240:
                bitrate = 250;
                minBitrate = 375;
                break;
            case ArLiveVideoResolution480x360:
                bitrate = 400;
                minBitrate = 600;
                break;
            case ArLiveVideoResolution640x480:
                bitrate = 600;
                minBitrate = 900;
                break;
            case ArLiveVideoResolution320x180:
                bitrate = 250;
                minBitrate = 400;
                break;
            case ArLiveVideoResolution480x270:
                bitrate = 350;
                minBitrate = 550;
                break;
            case ArLiveVideoResolution640x360:
                bitrate = 500;
                minBitrate = 900;
                break;
            case ArLiveVideoResolution960x540:
                bitrate = 800;
                minBitrate = 1500;
                break;
            case ArLiveVideoResolution1280x720:
                bitrate = 1000;
                minBitrate = 1800;
                break;
            case ArLiveVideoResolution1920x1080:
                bitrate = 2500;
                minBitrate = 3000;
                break;
        }

        return new ArBitrate(bitrate, minBitrate);
    }

    public static class ArBitrate{
        public int bitrate;
        public int minBitrate;

        public ArBitrate(int minBitrate, int bitrate) {
            this.bitrate = bitrate;
            this.minBitrate = minBitrate;
        }
    }
    
}
