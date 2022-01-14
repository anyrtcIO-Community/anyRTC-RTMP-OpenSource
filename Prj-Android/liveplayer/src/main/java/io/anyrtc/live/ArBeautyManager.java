package io.anyrtc.live;

public abstract class ArBeautyManager {


    public abstract void setBeautyEffect(boolean enable);

    /**
     * 设置美白级别
     * @param leave 美白级别，取值范围0 - 9；0表示关闭，9表示效果最明显。
     */
    public abstract void setWhitenessLevel(float leave);

    /**
     * 设置美颜（磨皮）级别
     * @param leave 磨皮级别，取值范围0 - 9； 0表示关闭，9表示效果最明显。
     */
    public abstract void setBeautyLevel(float leave);


    /**
     * 设置红润级别
     * @param leave 红润级别，取值范围0 - 9； 0表示关闭，9表示效果最明显。
     */
    public abstract void setToneLevel(float leave);
}
