#version 440

// 输入结构体
layout(location = 0) in vec4 vColor;               // 从顶点着色器传来的颜色

// 输出颜色
layout(location = 0) out vec4 fragColor; // 片段着色器的输出颜色

void main() {
	fragColor = vColor; // 返回输入的颜色
}