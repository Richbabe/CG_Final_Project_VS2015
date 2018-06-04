#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> ;//����glad����ȡ���еı���OpenGLͷ�ļ�

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader{
	public:
		// ����ID
		unsigned int ID;

		//��������ȡ��������ɫ��
		Shader(const char* vertexPath, const char* fragmentPath)
		{
			// 1. ���ļ�·���л�ȡ����/Ƭ����ɫ��
			std::string vertexCode;
			std::string fragmentCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
			// ��֤ifstream��������׳��쳣��
			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				// ���ļ�
				vShaderFile.open(vertexPath);
				fShaderFile.open(fragmentPath);
				std::stringstream vShaderStream, fShaderStream;
				// ��ȡ�ļ��Ļ������ݵ���������
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();
				// �ر��ļ�������
				vShaderFile.close();
				fShaderFile.close();
				// ת����������string
				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			}
			catch (std::ifstream::failure e)
			{
				std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			}
			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();

			// 2. ���붥��/Ƭ����ɫ�������ӳ���ɫ������
			unsigned int vertex, fragment;
			int success;
			char infoLog[512];
			// ������ɫ��
			vertex = glCreateShader(GL_VERTEX_SHADER);//������ɫ��
			glShaderSource(vertex, 1, &vShaderCode, NULL);//��������ɫ��Դ�븽�ӵ�������ɫ��������
			glCompileShader(vertex);//���붥����ɫ��
			// ��ӡ�����������еĻ���
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);//����ɹ�successΪ1������Ϊ0
			if (!success)
			{
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);//��ô�����Ϣ
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			};
			// Ƭ����ɫ��
			fragment = glCreateShader(GL_FRAGMENT_SHADER);//Ƭ����ɫ��
			glShaderSource(fragment, 1, &fShaderCode, NULL);//��Ƭ����ɫ��Դ�븽�ӵ�Ƭ����ɫ��������
			glCompileShader(fragment);//����Ƭ����ɫ��
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);//����ɹ�successΪ1������Ϊ0
			if (!success)
			{
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);//��ô�����Ϣ
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			};
			// ��ɫ������
			ID = glCreateProgram();//����һ�����򣬲������´�����������ID����
			glAttachShader(ID, vertex);//��������ɫ�����ӵ����������
			glAttachShader(ID, fragment);//��Ƭ����ɫ�����ӵ����������
			glLinkProgram(ID);//���Ӷ�����ɫ����Ƭ����ɫ��������ɫ������
			// ��ӡ���Ӵ�������еĻ���
			glGetProgramiv(ID, GL_LINK_STATUS, &success);//���ӳɹ�successΪ1������Ϊ0
			if (!success)
			{
				glGetProgramInfoLog(ID, 512, NULL, infoLog);//��ô�����Ϣ
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			}
			// ɾ����ɫ���������Ѿ����ӵ����ǵĳ������ˣ��Ѿ�������Ҫ��
			glDeleteShader(vertex);
			glDeleteShader(fragment);
		};
		//ʹ��/�������
		void use() {
			glUseProgram(ID);
		};
		//uniform���ߺ���
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