package org.webrtc.effector.filter;

import android.opengl.GLES20;


public class GPUImageBeautyFilter extends GPUImageFilter {
    public static final String BILATERAL_FRAGMENT_SHADER = "" +
            "precision highp float;\n"+
            "   varying highp vec2 textureCoordinate;\n" +
            "\n" +
            "    uniform sampler2D inputImageTexture;\n" +
            "\n" +
            "    uniform highp vec2 singleStepOffset;\n" +
            "    uniform highp vec4 params;\n" +
            "    uniform highp float brightness;\n" +
            "\n" +
            "    const highp vec3 W = vec3(0.299, 0.587, 0.114);\n" +
            "    const highp mat3 saturateMatrix = mat3(\n" +
            "        1.1102, -0.0598, -0.061,\n" +
            "        -0.0774, 1.0826, -0.1186,\n" +
            "        -0.0228, -0.0228, 1.1772);\n" +
            "    highp vec2 blurCoordinates[24];\n" +
            "\n" +
            "    highp float hardLight(highp float color) {\n" +
            "    if (color <= 0.5)\n" +
            "        color = color * color * 2.0;\n" +
            "    else\n" +
            "        color = 1.0 - ((1.0 - color)*(1.0 - color) * 2.0);\n" +
            "    return color;\n" +
            "}\n" +
            "\n" +
            "    void main(){\n" +
            "    highp vec3 centralColor = texture2D(inputImageTexture, textureCoordinate).rgb;\n" +
            "    blurCoordinates[0] = textureCoordinate.xy + singleStepOffset * vec2(0.0, -10.0);\n" +
            "    blurCoordinates[1] = textureCoordinate.xy + singleStepOffset * vec2(0.0, 10.0);\n" +
            "    blurCoordinates[2] = textureCoordinate.xy + singleStepOffset * vec2(-10.0, 0.0);\n" +
            "    blurCoordinates[3] = textureCoordinate.xy + singleStepOffset * vec2(10.0, 0.0);\n" +
            "    blurCoordinates[4] = textureCoordinate.xy + singleStepOffset * vec2(5.0, -8.0);\n" +
            "    blurCoordinates[5] = textureCoordinate.xy + singleStepOffset * vec2(5.0, 8.0);\n" +
            "    blurCoordinates[6] = textureCoordinate.xy + singleStepOffset * vec2(-5.0, 8.0);\n" +
            "    blurCoordinates[7] = textureCoordinate.xy + singleStepOffset * vec2(-5.0, -8.0);\n" +
            "    blurCoordinates[8] = textureCoordinate.xy + singleStepOffset * vec2(8.0, -5.0);\n" +
            "    blurCoordinates[9] = textureCoordinate.xy + singleStepOffset * vec2(8.0, 5.0);\n" +
            "    blurCoordinates[10] = textureCoordinate.xy + singleStepOffset * vec2(-8.0, 5.0);\n" +
            "    blurCoordinates[11] = textureCoordinate.xy + singleStepOffset * vec2(-8.0, -5.0);\n" +
            "    blurCoordinates[12] = textureCoordinate.xy + singleStepOffset * vec2(0.0, -6.0);\n" +
            "    blurCoordinates[13] = textureCoordinate.xy + singleStepOffset * vec2(0.0, 6.0);\n" +
            "    blurCoordinates[14] = textureCoordinate.xy + singleStepOffset * vec2(6.0, 0.0);\n" +
            "    blurCoordinates[15] = textureCoordinate.xy + singleStepOffset * vec2(-6.0, 0.0);\n" +
            "    blurCoordinates[16] = textureCoordinate.xy + singleStepOffset * vec2(-4.0, -4.0);\n" +
            "    blurCoordinates[17] = textureCoordinate.xy + singleStepOffset * vec2(-4.0, 4.0);\n" +
            "    blurCoordinates[18] = textureCoordinate.xy + singleStepOffset * vec2(4.0, -4.0);\n" +
            "    blurCoordinates[19] = textureCoordinate.xy + singleStepOffset * vec2(4.0, 4.0);\n" +
            "    blurCoordinates[20] = textureCoordinate.xy + singleStepOffset * vec2(-2.0, -2.0);\n" +
            "    blurCoordinates[21] = textureCoordinate.xy + singleStepOffset * vec2(-2.0, 2.0);\n" +
            "    blurCoordinates[22] = textureCoordinate.xy + singleStepOffset * vec2(2.0, -2.0);\n" +
            "    blurCoordinates[23] = textureCoordinate.xy + singleStepOffset * vec2(2.0, 2.0);\n" +
            "\n" +
            "    highp float sampleColor = centralColor.g * 22.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[0]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[1]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[2]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[3]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[4]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[5]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[6]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[7]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[8]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[9]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[10]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[11]).g;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[12]).g * 2.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[13]).g * 2.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[14]).g * 2.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[15]).g * 2.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[16]).g * 2.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[17]).g * 2.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[18]).g * 2.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[19]).g * 2.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[20]).g * 3.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[21]).g * 3.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[22]).g * 3.0;\n" +
            "    sampleColor += texture2D(inputImageTexture, blurCoordinates[23]).g * 3.0;\n" +
            "\n" +
            "    sampleColor = sampleColor / 62.0;\n" +
            "\n" +
            "    highp float highPass = centralColor.g - sampleColor + 0.5;\n" +
            "\n" +
            "    for (int i = 0; i < 5; i++) {\n" +
            "        highPass = hardLight(highPass);\n" +
            "    }\n" +
            "    highp float lumance = dot(centralColor, W);\n" +
            "\n" +
            "    highp float alpha = pow(lumance, params.r);\n" +
            "\n" +
            "    highp vec3 smoothColor = centralColor + (centralColor-vec3(highPass))*alpha*0.1;\n" +
            "\n" +
            "    smoothColor.r = clamp(pow(smoothColor.r, params.g), 0.0, 1.0);\n" +
            "    smoothColor.g = clamp(pow(smoothColor.g, params.g), 0.0, 1.0);\n" +
            "    smoothColor.b = clamp(pow(smoothColor.b, params.g), 0.0, 1.0);\n" +
            "\n" +
            "    highp vec3 lvse = vec3(1.0)-(vec3(1.0)-smoothColor)*(vec3(1.0)-centralColor);\n" +
            "    highp vec3 bianliang = max(smoothColor, centralColor);\n" +
            "    highp vec3 rouguang = 2.0*centralColor*smoothColor + centralColor*centralColor - 2.0*centralColor*centralColor*smoothColor;\n" +
            "\n" +
            "    gl_FragColor = vec4(mix(centralColor, lvse, alpha), 1.0);\n" +
            "    gl_FragColor.rgb = mix(gl_FragColor.rgb, bianliang, alpha);\n" +
            "    gl_FragColor.rgb = mix(gl_FragColor.rgb, rouguang, params.b);\n" +
            "\n" +
            "    highp vec3 satcolor = gl_FragColor.rgb * saturateMatrix;\n" +
            "    gl_FragColor.rgb = mix(gl_FragColor.rgb, satcolor, params.a);\n" +
            "    gl_FragColor.rgb = vec3(gl_FragColor.rgb + vec3(brightness));\n" +
            "}";

    private float toneLevel;
    private float beautyLevel;
    private float brightLevel;

    private int paramsLocation;
    private int brightnessLocation;
    private int singleStepOffsetLocation;

    public GPUImageBeautyFilter() {
        super(NO_FILTER_VERTEX_SHADER, BILATERAL_FRAGMENT_SHADER);
        toneLevel = -0.5f;
        beautyLevel = 0.8f;
        brightLevel = 0.3f;
    }

    @Override
    public void onInit() {
        super.onInit();

        paramsLocation = GLES20.glGetUniformLocation(getProgram(), "params");
        brightnessLocation = GLES20.glGetUniformLocation(getProgram(), "brightness");
        singleStepOffsetLocation = GLES20.glGetUniformLocation(getProgram(), "singleStepOffset");

        setParams(beautyLevel, toneLevel);
        setBrightLevel(brightLevel);
    }

    //磨皮
    public void setBeautyLevel(float beautyLevel) {
        this.beautyLevel = beautyLevel;
        setParams(beautyLevel, toneLevel);
    }

    //美白
    public void setBrightLevel(float brightLevel) {
        this.brightLevel = brightLevel;
        setFloat(brightnessLocation, 0.6f * (-0.5f + brightLevel));
    }

    //红润
    public void setToneLevel(float toneLevel) {
        this.toneLevel = toneLevel;
        setParams(beautyLevel, toneLevel);
    }

    public void setAllBeautyParams(float beauty,float bright,float tone) {
        setBeautyLevel(beauty);
        setBrightLevel(bright);
        setToneLevel(tone);
    }



    public void setParams(float beauty, float tone) {
        float[] vector = new float[4];
        vector[0] = 1.0f - 0.6f * beauty;
        vector[1] = 1.0f - 0.3f * beauty;
        vector[2] = 0.1f + 0.3f * tone;
        vector[3] = 0.1f + 0.3f * tone;
        setFloatVec4(paramsLocation, vector);
    }

    private void setTexelSize(final float w, final float h) {
        setFloatVec2(singleStepOffsetLocation, new float[] {2.0f / w, 2.0f / h});
    }

    @Override
    public void onOutputSizeChanged(final int width, final int height) {
        super.onOutputSizeChanged(width, height);
        setTexelSize(width, height);
    }
}
