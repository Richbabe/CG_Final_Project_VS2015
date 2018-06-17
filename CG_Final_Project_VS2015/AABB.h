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
	Model model;//模型
	Sphere ball;//星球
	float xMin;//x的最小值
	float yMin;//y的最小值
	float zMin;//z的最小值
	float xMax;//x的最大值
	float yMax;//y的最大值
	float zMax;//z的最大值
	unsigned int AABB_VAO;//顶点数组对象
	glm::vec3 AABB_vertices[8];//AABB碰撞盒的8个顶点
	glm::vec3 aabb_vertices[8];//保存每个碰撞盒初始8个顶点的位置

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

	//构建AABB盒8个顶点
	void InitializeAABBvertices(){
		//第一个顶点
		AABB_vertices[0].x = xMin;
		AABB_vertices[0].y = yMax;
		AABB_vertices[0].z = zMin;

		aabb_vertices[0].x = xMin;
		aabb_vertices[0].y = yMax;
		aabb_vertices[0].z = zMin;

		//第二个顶点
		AABB_vertices[1].x = xMin;
		AABB_vertices[1].y = yMax;
		AABB_vertices[1].z = zMax;

		aabb_vertices[1].x = xMin;
		aabb_vertices[1].y = yMax;
		aabb_vertices[1].z = zMax;

		//第三个顶点
		AABB_vertices[2].x = xMax;
		AABB_vertices[2].y = yMax;
		AABB_vertices[2].z = zMin;

		aabb_vertices[2].x = xMax;
		aabb_vertices[2].y = yMax;
		aabb_vertices[2].z = zMin;

		//第四个顶点
		AABB_vertices[3].x = xMax;
		AABB_vertices[3].y = yMax;
		AABB_vertices[3].z = zMax;

		aabb_vertices[3].x = xMax;
		aabb_vertices[3].y = yMax;
		aabb_vertices[3].z = zMax;

		//第五个顶点
		AABB_vertices[4].x = xMin;
		AABB_vertices[4].y = yMin;
		AABB_vertices[4].z = zMin;

		aabb_vertices[4].x = xMin;
		aabb_vertices[4].y = yMin;
		aabb_vertices[4].z = zMin;

		//第六个顶点
		AABB_vertices[5].x = xMin;
		AABB_vertices[5].y = yMin;
		AABB_vertices[5].z = zMax;

		aabb_vertices[5].x = xMin;
		aabb_vertices[5].y = yMin;
		aabb_vertices[5].z = zMax;

		//第七个顶点
		AABB_vertices[6].x = xMax;
		AABB_vertices[6].y = yMin;
		AABB_vertices[6].z = zMin;

		aabb_vertices[6].x = xMax;
		aabb_vertices[6].y = yMin;
		aabb_vertices[6].z = zMin;

		//第八个顶点
		AABB_vertices[7].x = xMax;
		AABB_vertices[7].y = yMin;
		AABB_vertices[7].z = zMax;

		aabb_vertices[7].x = xMax;
		aabb_vertices[7].y = yMin;
		aabb_vertices[7].z = zMax;
	}

	//初始化模型AABB盒
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
				//更新x的最小值
				xMin = min(xMin, vertice[j].Position.x);
				//更新y的最小值
				yMin = min(yMin, vertice[j].Position.y);
				//更新z的最小值
				zMin = min(zMin, vertice[j].Position.z);

				//更新x的最大值
				xMax = max(xMax, vertice[j].Position.x);
				//更新y的最大值
				yMax = max(yMax, vertice[j].Position.y);
				//更新z的最大值
				zMax = max(zMax, vertice[j].Position.z);
			}
		}
		InitializeAABBvertices();
	}

	//初始化星球AABB盒
	void InitializeSphereAABB() {
		xMin = 999999.0f;
		xMax = -999999.0f;
		yMin = 999999.0f;
		yMax = -999999.0f;
		zMin = 999999.0f;
		zMax = -999999.0f;
		vector<float> vertice = ball.vertices;
		for (int i = 0; i < vertice.size(); i += 8) {
			//更新x的最小值
			xMin = min(xMin, vertice[i]);
			//更新y的最小值
			yMin = min(yMin, vertice[i + 1]);
			//更新z的最小值
			zMin = min(zMin, vertice[i + 2]);

			//更新x的最大值
			xMax = max(xMax, vertice[i]);
			//更新y的最大值
			yMax = max(yMax, vertice[i + 1]);
			//更新z的最大值
			zMax = max(zMax, vertice[i + 2]);
		}
		InitializeAABBvertices();
	}

	//创建AABB碰撞盒并绑定VAO
	void createAABB() {
		float AABB_vertices[] = {
			//位置             //颜色
			//------第一根线------
			xMin, yMin, zMin,   1.0f, 0.0f, 0.0f,
			xMax, yMin, zMin,   1.0f, 0.0f, 0.0f,

			//------第二根线------
			xMin, yMin, zMin,   1.0f, 0.0f, 0.0f,
			xMin, yMin, zMax,   1.0f, 0.0f, 0.0f,

			//------第三根线------
			xMin, yMin, zMin,   1.0f, 0.0f, 0.0f,
			xMin, yMax, zMin,   1.0f, 0.0f, 0.0f,

			//------第四根线------
			xMin, yMin, zMax,   1.0f, 0.0f, 0.0f,
			xMax, yMin, zMax,   1.0f, 0.0f, 0.0f,

			//------第五根线------
			xMax, yMin, zMin,   1.0f, 0.0f, 0.0f,
			xMax, yMin, zMax,   1.0f, 0.0f, 0.0f,

			//------第六根线------
			xMax, yMin, zMin,   1.0f, 0.0f, 0.0f,
			xMax, yMax, zMin,   1.0f, 0.0f, 0.0f,

			//------第七根线------
			xMax, yMax, zMin,   1.0f, 0.0f, 0.0f,
			xMin, yMax, zMin,   1.0f, 0.0f, 0.0f,

			//------第八根线------
			xMin, yMax, zMin,   1.0f, 0.0f, 0.0f,
			xMin, yMax, zMax,   1.0f, 0.0f, 0.0f,

			//------第九根线------
			xMax, yMax, zMin,   1.0f, 0.0f, 0.0f,
			xMax, yMax, zMax,   1.0f, 0.0f, 0.0f,

			//------第十根线------
			xMin, yMin, zMax,   1.0f, 0.0f, 0.0f,
			xMin, yMax, zMax,   1.0f, 0.0f, 0.0f,

			//------第十一根线------
			xMax, yMin, zMax,   1.0f, 0.0f, 0.0f,
			xMax, yMax, zMax,   1.0f, 0.0f, 0.0f,

			//------第十二根线------
			xMin, yMax, zMax,   1.0f, 0.0f, 0.0f,
			xMax, yMax, zMax,   1.0f, 0.0f, 0.0f,
		};
		unsigned int AABB_VBO;//顶点缓冲对象
		glGenVertexArrays(1, &AABB_VAO);//生成一个VAO对象
		glGenBuffers(1, &AABB_VBO);//生成一个VBO对象
		glBindVertexArray(AABB_VAO);//绑定VAO
		//把顶点数组复制到缓冲中供OpengGL使用
		glBindBuffer(GL_ARRAY_BUFFER, AABB_VBO);//把新创建的缓冲VBO绑定到GL_ARRAY_BUFFER目标上
		glBufferData(GL_ARRAY_BUFFER, sizeof(AABB_vertices), AABB_vertices, GL_STATIC_DRAW);//把之前定义的顶点数据vertices复制到缓冲的内存中
		//链接顶点属性
		//位置属性，值为0
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);//解析顶点数据
		glEnableVertexAttribArray(0);
		//颜色属性，值为1
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));//解析顶点数据
		glEnableVertexAttribArray(1);
	}

	//判断两个AABB盒是否相交
	bool IsIntersection(const AABB &a) {
		//即各轴是否相互包含，(a包含当前包围盒) || （当前包围盒包含a）
		/*
		//如果a包围盒最小的x坐标大于b包围盒的最大x坐标，a包围盒在x方向上在b的右边
		if (a.xMin > xMax) {
			return FALSE;
		}
		//如果a包围盒最大的x坐标小于b包围盒的最小x坐标，a包围盒在x方向上在b的左边
		else if (a.xMax < xMin) {
			return FALSE;
		}
		//如果a包围盒最小的Y坐标大于b包围盒的最大Y坐标，a包围盒在Y方向上在b的右边
		else if (a.yMin > yMax)  
		{
			return FALSE;
		}
		//如果a包围盒最大的Y坐标小于b包围盒的最小的Y坐标,a包围盒在Y方向上在b的左边 
		else if (a.yMax < yMin) 
		{
			return FALSE;
		}
		//如果a包围盒最小的Z坐标大于b包围盒的最大Z坐标，a包围盒在Z方向上在b的上方
		else if (a.zMin > zMax)  
		{
			return FALSE;
		}
		//如果a包围盒最大的Z坐标小于b包围盒的最小的Z坐标,a包围盒在Z方向上在b的下方
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

	//实时更新AABB碰撞盒的顶点
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
		//更新8个顶点和xMin,yMin,zMin,xMax,yMax,zMax;
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

		glm::vec4 test = transform * glm::vec4(0.0f,0.0f,0.0f,1.0f);
		/*
		cout << "result----------------" << endl;
		for (int i = 0; i < 4; i++)
			cout << test[i] << ' ';
		cout << endl;
		*/
		//更新xMin,yMin,zMin,xMax,yMax,zMax
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

	//绘制AABB碰撞盒
	void drawAABB(Shader shader, const glm::mat4& transform, const glm::mat4& view, const glm::mat4& projection,bool& ShowAABB) {
		//更新AABB碰撞盒的顶点
		upDateAABBvertices(transform);

		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("transform", transform);

		glBindVertexArray(AABB_VAO);//绑定VAO
		if (ShowAABB) {
			glDrawArrays(GL_LINES, 0, 24);//绘制图元
		}
		glBindVertexArray(0);
	}
};


#endif
