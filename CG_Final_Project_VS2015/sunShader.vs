//顶点着色器源代码
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 transform;//变换矩阵
uniform mat4 view;//观察矩阵
uniform mat4 projection;//投影矩阵

out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * transform * vec4(aPos, 1.0f);//获得变换后的位置
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}