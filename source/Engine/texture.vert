#version 440

// Uniforms for the matrix transformations
layout(std140, binding = 0) uniform MatrixBuffer{
    mat4 worldMatrix;      // 世界矩阵
    mat4 viewMatrix;       // 视图矩阵
    mat4 projectionMatrix;  // 投影矩阵
}UBO;

// Input structure for vertex shader
layout(location = 0) in vec4 position; // 输入位置
layout(location = 1) in vec2 tex;    // TEXCOORD0

// Output structure for fragment shader
//layout(location = 0) out vec4 fragPosition; // SV_POSITION
layout(location = 0) out vec2 fragTex; // TEXCOORD0
out gl_PerVertex { 
    vec4 gl_Position;                // 输出位置
};


void main() {
    // Change the position vector to be 4 units for proper matrix calculations.
    vec4 pos = position;
    pos.w = 1.0; // 设置 w 分量为 1.0

    // Calculate the position of the vertex against the world, view, and projection matrices.
    gl_Position = UBO.projectionMatrix * UBO.viewMatrix * UBO.worldMatrix * pos;

    // Store the texture coordinates for the pixel shader.
    fragTex = tex;
}