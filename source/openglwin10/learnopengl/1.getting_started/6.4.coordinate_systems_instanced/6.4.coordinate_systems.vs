#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
// 实例化矩阵属性 (mat4 需要4个location)
layout (location = 2) in vec4 aInstanceMatrix0;
layout (location = 3) in vec4 aInstanceMatrix1;
layout (location = 4) in vec4 aInstanceMatrix2;
layout (location = 5) in vec4 aInstanceMatrix3;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 重构实例化模型矩阵
    mat4 instanceMatrix = mat4(
        aInstanceMatrix0,
        aInstanceMatrix1,
        aInstanceMatrix2,
        aInstanceMatrix3
    );
    
    // 应用变换：投影 * 视图 * 实例化模型矩阵 * 顶点位置
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
    TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}