//顶点着色器源代码
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;//颜色变量的属性位置值为1

uniform mat4 transform;//变换矩阵
uniform mat4 view;//观察矩阵
uniform mat4 projection;//投影矩阵

out vec3 ourColor;//向片段着色器输出一个颜色



void main()
{
    gl_Position = projection * view * transform * vec4(aPos, 1.0f);//获得变换后的位置
    ourColor = aColor;//将ourColor设置为我们从顶点数据那里得到的输入颜色
}