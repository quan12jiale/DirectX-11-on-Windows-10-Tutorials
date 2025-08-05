#version 440

// Input structure for vertex shader
layout(location = 0) in vec4 inputPosition; // POSITION
layout(location = 1) in vec2 inputTexCoord; // TEXCOORD0
layout(location = 2) in vec3 inputNormal; // NORMAL

// Output structure for fragment shader
layout(location = 0) out vec2 texCoord; // TEXCOORD0
layout(location = 1) out vec3 normal; // NORMAL

// Uniforms for the matrix transformations
layout(std140, binding = 0) uniform MatrixBuffer{
    mat4 worldMatrix;      // 世界矩阵
    mat4 viewMatrix;       // 视图矩阵
    mat4 projectionMatrix;  // 投影矩阵
}UBO;

out gl_PerVertex { 
    vec4 gl_Position;                // 输出位置
};

void main() {
    // Change the position vector to be 4 units for proper matrix calculations.
    vec4 pos = inputPosition;
    pos.w = 1.0; // 设置 w 分量为 1.0

    // Calculate the position of the vertex against the world, view, and projection matrices.
    gl_Position = UBO.projectionMatrix * UBO.viewMatrix * UBO.worldMatrix * pos;

    // Store the texture coordinates for the pixel shader.
    texCoord = inputTexCoord;
    
    // Calculate the normal vector against the world matrix only.
    normal = normalize(mat3(UBO.worldMatrix) * inputNormal);
}