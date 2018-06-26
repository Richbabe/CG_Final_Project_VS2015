#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
/*
//定义摄像机的运动
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
*/
//定义摄像机参数
const float YAW = -90.0f;//偏航角,因为偏航角为0时会导致方向向量指向右边，因此我们需要往左边旋转一点
const float PITCH = 0.0f;//俯仰角
const float SPEED = 2.0f;//设置摄像机移动速度
const float SENSITIVITY = 0.1f;//设置灵敏度
const float ZOOM = 45.0f;//设置摄像机视野角
						 
//定义摄像机类
class Camera {
public:
	//摄像机参数
	glm::vec3 Position;//摄像机位置
	glm::vec3 Front;//摄像机朝向的位置
	glm::vec3 Up;//摄像机的上向量
	glm::vec3 Right;//摄像机的右向量
	glm::vec3 WorldUp;
	//欧拉角
	float Yaw;//偏航角
	float Pitch;//俯仰角
	//其他参数
	float MoveSpeed;//摄像机移动速度
	float MouseSensitivity;//鼠标灵敏度
	float Zoom;//摄像机视野角度

	//向量作为参数的构造函数
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MoveSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	//坐标作为参数的构造函数
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MoveSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	//通过欧拉角和LookAt矩阵计算出观察矩阵view
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	//定义飞船的view
	glm::mat4 GetShipViewMatrix(glm::vec3 shipPos)
	{
		return glm::lookAt(glm::vec3(shipPos.x,shipPos.y + 0.1,shipPos.z + 0.05) - 0.5f * Front, shipPos, Up);
	}

	/*
	//处理键盘输入
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MoveSpeed * deltaTime;
		if (direction == FORWARD) {
			Position += Front * velocity;
		}
		if (direction == BACKWARD) {
			Position -= Front * velocity;
		}
		if (direction == LEFT) {
			Position -= Right * velocity;
		}
		if (direction == RIGHT) {
			Position += Right * velocity;
		}
	}
	*/
	//处理鼠标输入
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		//计算俯仰角和偏航角
		Yaw += xoffset;//计算偏航角
		Pitch += yoffset;//计算俯仰角

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		//限制摄像机的俯仰角不能高于90度和低于-90度
		if (constrainPitch)
		{
			if (Pitch > 89.0f) {
				Pitch = 89.0f;
			}
			if (Pitch < -89.0f) {
				Pitch = -89.0f;
			}
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		//通过俯仰角和偏航角来计算以得到真正的方向向量
		updateCameraVectors();
	}

	//处理鼠标滑轮
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f) {
			Zoom -= yoffset;
		}
		if (Zoom <= 1.0f) {
			Zoom = 1.0f;
		}
		if (Zoom >= 45.0f) {
			Zoom = 45.0f;
		}
	}

	void SetPosition(glm::vec3 p) {
		Position = p;
	}

private:
	//根据更新的欧拉角计算相机的朝向位置
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

#endif