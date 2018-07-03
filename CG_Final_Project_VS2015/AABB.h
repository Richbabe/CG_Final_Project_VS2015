#pragma once
#ifndef AABB_H
#define AABB_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_s.h>
#include <Camera.h>
#include "Skybox.h"
#include "model.h"
#include "Sphere.h"
#include "mesh.h"
#include <math.h>

#include <iostream>
#include <vector>

class AABB {
public:
	Model model;//ģ��
	Sphere ball;//����
	float xMin;//x����Сֵ
	float yMin;//y����Сֵ
	float zMin;//z����Сֵ
	float xMax;//x�����ֵ
	float yMax;//y�����ֵ
	float zMax;//z�����ֵ
	unsigned int AABB_VAO;//�����������
	glm::vec3 AABB_vertices[8];//AABB��ײ�е�8������
	glm::vec3 aabb_vertices[8];//����ÿ����ײ�г�ʼ8�������λ��

	AABB() {

	}

	~AABB() {

	}

	AABB(Model m) {
		model = m;
		InitializeModelAABB();
		createAABB();
	};

	AABB(Sphere s) {
		ball = s;
		InitializeSphereAABB();
		createAABB();
	}

	//����AABB��8������
	void InitializeAABBvertices() {
		//��һ������
		AABB_vertices[0].x = xMin;
		AABB_vertices[0].y = yMax;
		AABB_vertices[0].z = zMin;

		aabb_vertices[0].x = xMin;
		aabb_vertices[0].y = yMax;
		aabb_vertices[0].z = zMin;

		//�ڶ�������
		AABB_vertices[1].x = xMin;
		AABB_vertices[1].y = yMax;
		AABB_vertices[1].z = zMax;

		aabb_vertices[1].x = xMin;
		aabb_vertices[1].y = yMax;
		aabb_vertices[1].z = zMax;

		//����������
		AABB_vertices[2].x = xMax;
		AABB_vertices[2].y = yMax;
		AABB_vertices[2].z = zMin;

		aabb_vertices[2].x = xMax;
		aabb_vertices[2].y = yMax;
		aabb_vertices[2].z = zMin;

		//���ĸ�����
		AABB_vertices[3].x = xMax;
		AABB_vertices[3].y = yMax;
		AABB_vertices[3].z = zMax;

		aabb_vertices[3].x = xMax;
		aabb_vertices[3].y = yMax;
		aabb_vertices[3].z = zMax;

		//���������
		AABB_vertices[4].x = xMin;
		AABB_vertices[4].y = yMin;
		AABB_vertices[4].z = zMin;

		aabb_vertices[4].x = xMin;
		aabb_vertices[4].y = yMin;
		aabb_vertices[4].z = zMin;

		//����������
		AABB_vertices[5].x = xMin;
		AABB_vertices[5].y = yMin;
		AABB_vertices[5].z = zMax;

		aabb_vertices[5].x = xMin;
		aabb_vertices[5].y = yMin;
		aabb_vertices[5].z = zMax;

		//���߸�����
		AABB_vertices[6].x = xMax;
		AABB_vertices[6].y = yMin;
		AABB_vertices[6].z = zMin;

		aabb_vertices[6].x = xMax;
		aabb_vertices[6].y = yMin;
		aabb_vertices[6].z = zMin;

		//�ڰ˸�����
		AABB_vertices[7].x = xMax;
		AABB_vertices[7].y = yMin;
		AABB_vertices[7].z = zMax;

		aabb_vertices[7].x = xMax;
		aabb_vertices[7].y = yMin;
		aabb_vertices[7].z = zMax;
	}

	//��ʼ��ģ��AABB��
	void InitializeModelAABB() {
		xMin = 999999.0f;
		xMax = -999999.0f;
		yMin = 999999.0f;
		yMax = -999999.0f;
		zMin = 999999.0f;
		zMax = -999999.0f;
		vector<Mesh> mesh = model.meshes;
		for (int i = 0; i < mesh.size(); i++) {
			vector<Vertex> vertice = mesh[i].vertices;
			for (int j = 0; j < vertice.size(); j++) {
				//����x����Сֵ
				xMin = min(xMin, vertice[j].Position.x);
				//����y����Сֵ
				yMin = min(yMin, vertice[j].Position.y);
				//����z����Сֵ
				zMin = min(zMin, vertice[j].Position.z);

				//����x�����ֵ
				xMax = max(xMax, vertice[j].Position.x);
				//����y�����ֵ
				yMax = max(yMax, vertice[j].Position.y);
				//����z�����ֵ
				zMax = max(zMax, vertice[j].Position.z);
			}
		}
		InitializeAABBvertices();
	}

	//��ʼ������AABB��
	void InitializeSphereAABB() {
		xMin = 999999.0f;
		xMax = -999999.0f;
		yMin = 999999.0f;
		yMax = -999999.0f;
		zMin = 999999.0f;
		zMax = -999999.0f;
		vector<float> vertice = ball.vertices;
		for (int i = 0; i < vertice.size(); i += 8) {
			//����x����Сֵ
			xMin = min(xMin, vertice[i]);
			//����y����Сֵ
			yMin = min(yMin, vertice[i + 1]);
			//����z����Сֵ
			zMin = min(zMin, vertice[i + 2]);

			//����x�����ֵ
			xMax = max(xMax, vertice[i]);
			//����y�����ֵ
			yMax = max(yMax, vertice[i + 1]);
			//����z�����ֵ
			zMax = max(zMax, vertice[i + 2]);
		}
		InitializeAABBvertices();
	}

	//����AABB��ײ�в���VAO
	void createAABB() {
		float AABB_vertices[] = {
			//λ��             //��ɫ
			//------��һ����------
			xMin, yMin, zMin,   1.0f, 0.0f, 0.0f,
			xMax, yMin, zMin,   1.0f, 0.0f, 0.0f,

			//------�ڶ�����------
			xMin, yMin, zMin,   1.0f, 0.0f, 0.0f,
			xMin, yMin, zMax,   1.0f, 0.0f, 0.0f,

			//------��������------
			xMin, yMin, zMin,   1.0f, 0.0f, 0.0f,
			xMin, yMax, zMin,   1.0f, 0.0f, 0.0f,

			//------���ĸ���------
			xMin, yMin, zMax,   1.0f, 0.0f, 0.0f,
			xMax, yMin, zMax,   1.0f, 0.0f, 0.0f,

			//------�������------
			xMax, yMin, zMin,   1.0f, 0.0f, 0.0f,
			xMax, yMin, zMax,   1.0f, 0.0f, 0.0f,

			//------��������------
			xMax, yMin, zMin,   1.0f, 0.0f, 0.0f,
			xMax, yMax, zMin,   1.0f, 0.0f, 0.0f,

			//------���߸���------
			xMax, yMax, zMin,   1.0f, 0.0f, 0.0f,
			xMin, yMax, zMin,   1.0f, 0.0f, 0.0f,

			//------�ڰ˸���------
			xMin, yMax, zMin,   1.0f, 0.0f, 0.0f,
			xMin, yMax, zMax,   1.0f, 0.0f, 0.0f,

			//------�ھŸ���------
			xMax, yMax, zMin,   1.0f, 0.0f, 0.0f,
			xMax, yMax, zMax,   1.0f, 0.0f, 0.0f,

			//------��ʮ����------
			xMin, yMin, zMax,   1.0f, 0.0f, 0.0f,
			xMin, yMax, zMax,   1.0f, 0.0f, 0.0f,

			//------��ʮһ����------
			xMax, yMin, zMax,   1.0f, 0.0f, 0.0f,
			xMax, yMax, zMax,   1.0f, 0.0f, 0.0f,

			//------��ʮ������------
			xMin, yMax, zMax,   1.0f, 0.0f, 0.0f,
			xMax, yMax, zMax,   1.0f, 0.0f, 0.0f,
		};
		unsigned int AABB_VBO;//���㻺�����
		glGenVertexArrays(1, &AABB_VAO);//����һ��VAO����
		glGenBuffers(1, &AABB_VBO);//����һ��VBO����
		glBindVertexArray(AABB_VAO);//��VAO
									//�Ѷ������鸴�Ƶ������й�OpengGLʹ��
		glBindBuffer(GL_ARRAY_BUFFER, AABB_VBO);//���´����Ļ���VBO�󶨵�GL_ARRAY_BUFFERĿ����
		glBufferData(GL_ARRAY_BUFFER, sizeof(AABB_vertices), AABB_vertices, GL_STATIC_DRAW);//��֮ǰ����Ķ�������vertices���Ƶ�������ڴ���
																							//���Ӷ�������
																							//λ�����ԣ�ֵΪ0
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);//������������
		glEnableVertexAttribArray(0);
		//��ɫ���ԣ�ֵΪ1
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));//������������
		glEnableVertexAttribArray(1);
	}

	//�ж�����AABB���Ƿ��ཻ
	bool IsIntersection(const AABB &a) {
		//�������Ƿ��໥������(a������ǰ��Χ��) || ����ǰ��Χ�а���a��
		/*
		//���a��Χ����С��x�������b��Χ�е����x���꣬a��Χ����x��������b���ұ�
		if (a.xMin > xMax) {
		return FALSE;
		}
		//���a��Χ������x����С��b��Χ�е���Сx���꣬a��Χ����x��������b�����
		else if (a.xMax < xMin) {
		return FALSE;
		}
		//���a��Χ����С��Y�������b��Χ�е����Y���꣬a��Χ����Y��������b���ұ�
		else if (a.yMin > yMax)
		{
		return FALSE;
		}
		//���a��Χ������Y����С��b��Χ�е���С��Y����,a��Χ����Y��������b�����
		else if (a.yMax < yMin)
		{
		return FALSE;
		}
		//���a��Χ����С��Z�������b��Χ�е����Z���꣬a��Χ����Z��������b���Ϸ�
		else if (a.zMin > zMax)
		{
		return FALSE;
		}
		//���a��Χ������Z����С��b��Χ�е���С��Z����,a��Χ����Z��������b���·�
		else if (a.zMax < zMin)
		{
		return FALSE;
		}
		else
		{
		return TRUE;
		}
		*/

		return ((xMin >= a.xMin && xMin <= a.xMax) || (a.xMin >= xMin && a.xMin <= xMax)) &&
			((yMin >= a.yMin && yMin <= a.yMax) || (a.yMin >= yMin && a.yMin <= yMax)) &&
			((zMin >= a.zMin && zMin <= a.zMax) || (a.zMin >= zMin && a.zMin <= zMax));
	}

	bool IsIntersection(const Sphere& sphere) {
		float closest_x = max(xMin, sphere.center.x), closest_y = max(yMin, sphere.center.y), closest_z = max(zMin, sphere.center.z)
		closest_x = min(sphere.center.x, xMax);
		closest_y = min(sphere.center.y, yMax);
		closest_z = min(sphere.center.z, zMax);

		return ((pow(sphere.center.x - closest_x,2) + pow(sphere.center.y - closest_y,2) + pow(sphere.center.z - closest_z,2))
			<= sphere.radius * sphere.radius);
	}

	//ʵʱ����AABB��ײ�еĶ���
	void upDateAABBvertices(const glm::mat4& transform) {
		//cout << zMin << " " << zMax << endl;;
		//cout << "--------" << endl;
		/*
		cout << "transform----------------" << endl;
		for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
		cout << transform[j][i] << ' ';
		cout << endl;
		}
		*/
		//����8�������xMin,yMin,zMin,xMax,yMax,zMax;
		for (int i = 0; i < 8; i++) {
			glm::vec4 temp = glm::vec4(aabb_vertices[i].x, aabb_vertices[i].y, aabb_vertices[i].z, 1.0f);
			glm::vec4 result = transform * temp;

			AABB_vertices[i].x = result.x;
			//xMin = min(result.x, xMin);
			//xMax = max(result.x, xMax);

			AABB_vertices[i].y = result.y;
			//yMin = min(result.y, yMin);
			//yMax = max(result.y, yMax);

			AABB_vertices[i].z = result.z;
			//zMin = min(result.z, zMin);
			//zMax = max(result.z, zMax);
		}

		glm::vec4 test = transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		/*
		cout << "result----------------" << endl;
		for (int i = 0; i < 4; i++)
		cout << test[i] << ' ';
		cout << endl;
		*/
		//����xMin,yMin,zMin,xMax,yMax,zMax
		xMin = 999999.0f;
		xMax = -999999.0f;
		yMin = 999999.0f;
		yMax = -999999.0f;
		zMin = 999999.0f;
		zMax = -999999.0f;
		for (int i = 0; i < 8; i++) {
			xMin = min(AABB_vertices[i].x, xMin);
			xMax = max(AABB_vertices[i].x, xMax);
			yMin = min(AABB_vertices[i].y, yMin);
			yMax = max(AABB_vertices[i].y, yMax);
			zMin = min(AABB_vertices[i].z, zMin);
			zMax = max(AABB_vertices[i].z, zMax);
		}
	}

	//����AABB��ײ��
	void drawAABB(Shader shader, const glm::mat4& transform, const glm::mat4& view, const glm::mat4& projection, bool& ShowAABB) {
		//����AABB��ײ�еĶ���
		upDateAABBvertices(transform);

		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("transform", transform);

		glBindVertexArray(AABB_VAO);//��VAO
		if (ShowAABB) {
			glDrawArrays(GL_LINES, 0, 24);//����ͼԪ
		}
		glBindVertexArray(0);
	}
};


#endif