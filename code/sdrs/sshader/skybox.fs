#version 330 core
out vec4 FragColor;

in vec3 outUV;

uniform uint render_style;
uniform float time;
uniform samplerCube  ourTexture;

// 添加一个函数用于生成伪随机数，输入vec2，是为了对应纹理坐标，方便进行处理
// 参数值是经验选择的，称为噪声函数
float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}


void change_style( )
{
    if(render_style == 0u) return;
    else if(render_style == 1u)
    {
        //1.黑白风格
        // 将彩色转为灰度，可以使用标准的灰度公式：gray = 0.2126 * r + 0.7152 * g + 0.0722 * b
        float gray = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
        // 将灰度值分配给输出颜色的所有通道，得到黑白效果
        FragColor = vec4(vec3(gray), 1.0);
    }
    else if(render_style == 2u)
    {
        //2.像素风格
        // 将彩色转为离散颜色，通过乘color_num取整得到从0到color_num这么多的类型，例如取5
        // 再除以color_num（取5），将颜色分割到1，4/5，……，0，一共6个颜色区间
        // 这样实现颜色的有限化，可以降低画面复杂度，接近于油画、像素等风格
        // 这里天空盒取多一点颜色，使得天空的精细度得以保留
        float color_num = 15.0;
        vec3 quantizedColor = round(FragColor.rgb * color_num) / color_num;
        FragColor = vec4(quantizedColor, 1.0);
        //添加扰动，增强像素效果
        float jitter = rand(vec2(outUV[0],outUV[1]) + time) * 0.05; // 调整扰动强度
        FragColor.rgb += vec3(jitter);
    }
    else if(render_style == 3u)
    {
        //3.电视花屏
        // 这个是在2的后半段基础上做的，其实就是把扰动强度提的很高
        //添加扰动，增强像素效果
        float jitter = rand(vec2(outUV[0],outUV[1]) + time) * 0.05; // 调整扰动强度
        FragColor.rgb += vec3(jitter);
    }
    else if(render_style == 4u)
    {
        // 模拟雨滴效果
        float raindrop = rand(vec2(outUV[0],outUV[1])*100 + time); // 调整扰动强度
        // 输出雨滴效果
        if(raindrop>=0.98)
        {
           FragColor = vec4(vec3(0.0, 0.0, 1.0) * raindrop, 1.0);
        }
    }
}

void main()
{
    FragColor = texture(ourTexture , outUV);
    change_style( );
};