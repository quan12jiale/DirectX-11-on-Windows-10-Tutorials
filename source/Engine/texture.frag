#version 440

layout(set=0, binding=1) uniform sampler2D shaderTexture; // 在描述符集 0 的绑定点 1 上，定义了一个名为 shaderTexture 的 uniform 变量，它是一个 2D 纹理采样器。

// 输入结构体
//layout(location = 0) in vec4 fragPosition;               // SV_POSITION
layout(location = 0) in vec2 fragTex;               // TEXCOORD0

// 输出颜色
layout(location = 0) out vec4 fragColor; // 片段着色器的输出颜色

void main() {
	fragColor = texture(shaderTexture, fragTex); // 返回输入的颜色
}