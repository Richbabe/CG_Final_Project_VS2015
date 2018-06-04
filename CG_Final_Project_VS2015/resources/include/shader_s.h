#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> ;//包含glad来获取所有的必须OpenGL头文件

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader{
	public:
		// 程序ID
		unsigned int ID;

		//构造器读取并构建着色器
		Shader(const char* vertexPath, const char* fragmentPath)
		{
			// 1. 从文件路径中获取顶点/片段着色器
			std::string vertexCode;
			std::string fragmentCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
			// 保证ifstream对象可以抛出异常：
			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				// 打开文件
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				std::stringstream vShaderStream, fShaderStream;
				// 读取文件的缓冲内容到数据流中
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();
				// 关闭文件处理器
				vShaderFile.close();
				fShaderFile.close();
				// 转换数据流到string
				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			}
			catch (std::ifstream::failure e)
			{
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			}
			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();

			// 2. 编译顶点/片段着色器并链接成着色器程序
			unsigned int vertex, fragment;
			int success;
			char infoLog[512];
			// 顶点着色器
			vertex = glCreateShader(GL_VERTEX_SHADER);//顶点着色器
			glShaderSource(vertex, 1, &vShaderCode, NULL);//将顶点着色器源码附加到顶点着色器对象上
			glCompileShader(vertex);//编译顶点着色器
			// 打印编译错误（如果有的话）
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);//编译成功success为1，否则为0
			if (!success)
			{
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);//获得错误信息
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			};
			// 片段着色器
			fragment = glCreateShader(GL_FRAGMENT_SHADER);//片段着色器
			glShaderSource(fragment, 1, &fShaderCode, NULL);//将片段着色器源码附加到片段着色器对象上
			glCompileShader(fragment);//编译片段着色器
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);//编译成功success为1，否则为0
			if (!success)
			{
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);//获得错误信息
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			};
			// 着色器程序
			ID = glCreateProgram();//创建一个程序，并返回新创建程序对象的ID引用
			glAttachShader(ID, vertex);//将顶点着色器附加到程序对象上
			glAttachShader(ID, fragment);//将片段着色器附加到程序对象上
			glLinkProgram(ID);//链接顶点着色器和片段着色器生成着色器程序
			// 打印连接错误（如果有的话）
			glGetProgramiv(ID, GL_LINK_STATUS, &success);//链接成功success为1，否则为0
			if (!success)
			{
				glGetProgramInfoLog(ID, 512, NULL, infoLog);//获得错误信息
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			}
			// 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
			glDeleteShader(vertex);
			glDeleteShader(fragment);
		};
		//使用/激活程序
		void use() {
			glUseProgram(ID);
		};
		//uniform工具函数
		void setBool(const std::string &name, bool value) const {
			glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
		};
		void setInt(const std::string &name, int value) const {
			glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
		};
		void setFloat(const std::string &name, float value) const {
			glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
		};
		void setVec2(const std::string &name, const glm::vec2 &value) const
		{
			glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		}
		void setVec2(const std::string &name, float x, float y) const
		{
			glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
		}
		// ------------------------------------------------------------------------
		void setVec3(const std::string &name, const glm::vec3 &value) const
		{
			glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		}
		void setVec3(const std::string &name, float x, float y, float z) const
		{
			glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
		}
		// ------------------------------------------------------------------------
		void setVec4(const std::string &name, const glm::vec4 &value) const
		{
			glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
		}
		void setVec4(const std::string &name, float x, float y, float z, float w)
		{
			glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
		}
		// ------------------------------------------------------------------------
		void setMat2(const std::string &name, const glm::mat2 &mat) const
		{
			glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		// ------------------------------------------------------------------------
		void setMat3(const std::string &name, const glm::mat3 &mat) const
		{
			glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
		// ------------------------------------------------------------------------
		void setMat4(const std::string &name, const glm::mat4 &mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}
};

#endif