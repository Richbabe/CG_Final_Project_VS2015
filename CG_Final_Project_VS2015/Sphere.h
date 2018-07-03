#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader_s.h>
#include <iostream>
#include <vector>

#define pi 3.1415926

class Sphere {
public:
	/* Sphere Data */
	std::vector<float> vertices;
	unsigned int sphereVAO;
	unsigned int sphereTexture;

	/* Functions */
	Sphere() {

	}

	Sphere(int divide, float radius, std::string picturePath) {
		createSphere(divide, radius, picturePath);
	}

	void Draw(Shader shader, const glm::mat4& transform, const glm::mat4& view, const glm::mat4& projection) {
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sphereTexture);
		/*
		if (depthCubemap != 0) {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		}
		*/
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("transform", transform);
		glBindVertexArray(sphereVAO);//��VAO
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 8);//����ͼԪ
	}
private:
	/* Functions */
	void createSphere(int divide, float radius, std::string picturePath) {
		float altitude;//γ��
		float altitudeDelta;//��һ��γ��
		float azimuth;//����
		float ex;//���x����
		float ey;//���y����
		float ez;//���z����

		//��ά�ȵȷֳ�divide�ݣ��������ܼ����ÿһ�ȷݵ�γ��ֵ
		for (int i = 0; i <= divide; i++) {
			altitude = (float)(pi / 2.0 - i * pi / divide);//��ȡ��ǰ�ȷݵ�γ��ֵ
			altitudeDelta = (float)(pi / 2.0 - (i + 1) * pi / divide);//��ȡ��һ�ȷݵ�γ��ֵ
																	  //�����ȵȷֳ�divide�ݣ��������ܵõ���ǰγ��ֵ����һγ��ֵ��ÿһ�ݾ���ֵ
			for (int j = 0; j <= divide; j++) {
				azimuth = (float)(j * (pi * 2) / divide);//���㾭��ֵ
				ex = (float)(cos(altitude) * cos(azimuth));
				ey = (float)sin(altitude);
				ez = (float)-(cos(altitude) * sin(azimuth));
				//����azimuth������γ��Ϊaltitude�ĵ�����
				vertices.push_back(radius * ex);
				vertices.push_back(radius * ey);
				vertices.push_back(radius * ez);
				//����azimuth������γ��Ϊaltitude�����������
				vertices.push_back(j / (float)divide);
				vertices.push_back(i / (float)divide);
				//����azimuth������γ��Ϊaltitude�ĵ��Ӧ������
				vertices.push_back(radius * ex - vertices[0]);
				vertices.push_back(radius * ey - vertices[1]);
				vertices.push_back(radius * ez - vertices[2]);

				ex = (float)(cos(altitudeDelta) * cos(azimuth));
				ey = (float)sin(altitudeDelta);
				ez = (float)-(cos(altitudeDelta) * sin(azimuth));
				//����azimuth������γ��ΪaltitudeDelta�ĵ�����
				vertices.push_back(radius * ex);
				vertices.push_back(radius * ey);
				vertices.push_back(radius * ez);
				//����azimuth������γ��ΪaltitudeDelta�����������
				vertices.push_back(j / (float)divide);
				vertices.push_back((i + 1) / (float)divide);
				//����azimuth������γ��ΪaltitudeDelta�ĵ��Ӧ������
				vertices.push_back(radius * ex - vertices[0]);
				vertices.push_back(radius * ey - vertices[1]);
				vertices.push_back(radius * ez - vertices[2]);
			}
		}
		//����
		unsigned int sphereVBO;//���㻺�����
		glGenVertexArrays(1, &sphereVAO);//����һ��VAO����
		glGenBuffers(1, &sphereVBO);//����һ��VBO����
		glBindVertexArray(sphereVAO);//��VAO
									 //�Ѷ������鸴�Ƶ������й�OpengGLʹ��
		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);//���´����Ļ���VBO�󶨵�GL_ARRAY_BUFFERĿ����
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);//��֮ǰ����Ķ�������points_vertices���Ƶ�������ڴ���
		//���Ӷ�������
		//λ�����ԣ�ֵΪ0
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));//������������
		glEnableVertexAttribArray(0);
		//�������ԣ�ֵΪ1
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));//������������
		glEnableVertexAttribArray(1);
		//���������ԣ�ֵΪ2
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));//������������
		glEnableVertexAttribArray(2);

		//*******����1*******
		glGenTextures(1, &sphereTexture);//��һ��������ʾ�����������������
		glBindTexture(GL_TEXTURE_2D, sphereTexture);//������
		//Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
		//���û��Ʒ�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//���ù��˷�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//��������
		int width;//ͼ����
		int    height;//ͼ��߶�
		int    nrChannels;//��ɫͨ���ĸ���
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(picturePath.c_str(), &width, &height, &nrChannels, 0);
		//��������
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture,please check the path!" << std::endl;
		}
		stbi_image_free(data);//�ͷ��ڴ�
	}
};

#endif