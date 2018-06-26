#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
/*
//������������˶�
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
*/
//�������������
const float YAW = -90.0f;//ƫ����,��Ϊƫ����Ϊ0ʱ�ᵼ�·�������ָ���ұߣ����������Ҫ�������תһ��
const float PITCH = 0.0f;//������
const float SPEED = 2.0f;//����������ƶ��ٶ�
const float SENSITIVITY = 0.1f;//����������
const float ZOOM = 45.0f;//�����������Ұ��
						 
//�����������
class Camera {
public:
	//���������
	glm::vec3 Position;//�����λ��
	glm::vec3 Front;//����������λ��
	glm::vec3 Up;//�������������
	glm::vec3 Right;//�������������
	glm::vec3 WorldUp;
	//ŷ����
	float Yaw;//ƫ����
	float Pitch;//������
	//��������
	float MoveSpeed;//������ƶ��ٶ�
	float MouseSensitivity;//���������
	float Zoom;//�������Ұ�Ƕ�

	//������Ϊ�����Ĺ��캯��
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MoveSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	//������Ϊ�����Ĺ��캯��
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MoveSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	//ͨ��ŷ���Ǻ�LookAt���������۲����view
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	//����ɴ���view
	glm::mat4 GetShipViewMatrix(glm::vec3 shipPos)
	{
		return glm::lookAt(glm::vec3(shipPos.x,shipPos.y + 0.1,shipPos.z + 0.05) - 0.5f * Front, shipPos, Up);
	}

	/*
	//�����������
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
	//�����������
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		//���㸩���Ǻ�ƫ����
		Yaw += xoffset;//����ƫ����
		Pitch += yoffset;//���㸩����

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		//����������ĸ����ǲ��ܸ���90�Ⱥ͵���-90��
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
		//ͨ�������Ǻ�ƫ�����������Եõ������ķ�������
		updateCameraVectors();
	}

	//������껬��
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
	//���ݸ��µ�ŷ���Ǽ�������ĳ���λ��
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