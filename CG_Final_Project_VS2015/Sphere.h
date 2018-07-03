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
		glBindVertexArray(sphereVAO);//绑定VAO
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 8);//绘制图元
	}
private:
	/* Functions */
	void createSphere(int divide, float radius, std::string picturePath) {
		float altitude;//纬度
		float altitudeDelta;//下一层纬度
		float azimuth;//经度
		float ex;//点的x坐标
		float ey;//点的y坐标
		float ez;//点的z坐标

		//将维度等分成divide份，这样就能计算出每一等份的纬度值
		for (int i = 0; i <= divide; i++) {
			altitude = (float)(pi / 2.0 - i * pi / divide);//获取当前等份的纬度值
			altitudeDelta = (float)(pi / 2.0 - (i + 1) * pi / divide);//获取下一等份的纬度值
																	  //将经度等分成divide份，这样就能得到当前纬度值和下一纬度值的每一份经度值
			for (int j = 0; j <= divide; j++) {
				azimuth = (float)(j * (pi * 2) / divide);//计算经度值
				ex = (float)(cos(altitude) * cos(azimuth));
				ey = (float)sin(altitude);
				ez = (float)-(cos(altitude) * sin(azimuth));
				//计算azimuth经度下纬度为altitude的点坐标
				vertices.push_back(radius * ex);
				vertices.push_back(radius * ey);
				vertices.push_back(radius * ez);
				//计算azimuth经度下纬度为altitude的纹理点坐标
				vertices.push_back(j / (float)divide);
				vertices.push_back(i / (float)divide);
				//计算azimuth经度下纬度为altitude的点对应法向量
				vertices.push_back(radius * ex - vertices[0]);
				vertices.push_back(radius * ey - vertices[1]);
				vertices.push_back(radius * ez - vertices[2]);

				ex = (float)(cos(altitudeDelta) * cos(azimuth));
				ey = (float)sin(altitudeDelta);
				ez = (float)-(cos(altitudeDelta) * sin(azimuth));
				//计算azimuth经度下纬度为altitudeDelta的点坐标
				vertices.push_back(radius * ex);
				vertices.push_back(radius * ey);
				vertices.push_back(radius * ez);
				//计算azimuth经度下纬度为altitudeDelta的纹理点坐标
				vertices.push_back(j / (float)divide);
				vertices.push_back((i + 1) / (float)divide);
				//计算azimuth经度下纬度为altitudeDelta的点对应法向量
				vertices.push_back(radius * ex - vertices[0]);
				vertices.push_back(radius * ey - vertices[1]);
				vertices.push_back(radius * ez - vertices[2]);
			}
		}
		//球体
		unsigned int sphereVBO;//顶点缓冲对象
		glGenVertexArrays(1, &sphereVAO);//生成一个VAO对象
		glGenBuffers(1, &sphereVBO);//生成一个VBO对象
		glBindVertexArray(sphereVAO);//绑定VAO
									 //把顶点数组复制到缓冲中供OpengGL使用
		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);//把新创建的缓冲VBO绑定到GL_ARRAY_BUFFER目标上
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);//把之前定义的顶点数据points_vertices复制到缓冲的内存中
		//链接顶点属性
		//位置属性，值为0
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0 * sizeof(float)));//解析顶点数据
		glEnableVertexAttribArray(0);
		//纹理属性，值为1
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));//解析顶点数据
		glEnableVertexAttribArray(1);
		//法向量属性，值为2
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));//解析顶点数据
		glEnableVertexAttribArray(2);

		//*******纹理1*******
		glGenTextures(1, &sphereTexture);//第一个参数表示输入生成纹理的数量
		glBindTexture(GL_TEXTURE_2D, sphereTexture);//绑定纹理
		//为当前绑定的纹理对象设置环绕、过滤方式
		//设置环绕方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//设置过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//加载纹理
		int width;//图像宽度
		int    height;//图像高度
		int    nrChannels;//颜色通道的个数
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(picturePath.c_str(), &width, &height, &nrChannels, 0);
		//生成纹理
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture,please check the path!" << std::endl;
		}
		stbi_image_free(data);//释放内存
	}
};

#endif