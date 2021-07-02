#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
//#include <glm/gtx/string_cast.hpp>

class Camera {
public:
	glm::vec3 position;
	Camera(glm::vec3 position, glm::vec3 worldup) : position(position), worldup(worldup),
		front(glm::vec3(0.0, 0.0, -1.0)), up(worldup), keyboardVelocity(10.0), mouseVelocity(0.1)
	{
		this->right = glm::cross(this->front, this->up);
		this->pitch = 0;
		this->yaw = 0;
	}
	void moveForward(GLfloat const dt)
	{
		this->position += this->front * dt * this->keyboardVelocity;
	}
	void moveBack(GLfloat const dt)
	{
		this->position -= this->front * dt * this->keyboardVelocity;
	}
	void moveRight(GLfloat const dt)
	{
		this->position += this->right * dt * this->keyboardVelocity;
	}
	void moveLeft(GLfloat const dt)
	{
		this->position -= this->right * dt * this->keyboardVelocity;
	}
	glm::mat4 getViewMatrix()
	{
		//std::cout << glm::to_string(glm::lookAt(this->position, this->position + this->front, this->up)) << std::endl;
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}
	void rotate(GLfloat dpitch, GLfloat dyaw)
	{
		pitch += dpitch * this->mouseVelocity;
		if(this->up.y > 0)
			yaw -= dyaw * this->mouseVelocity;
		else
			yaw += dyaw * this->mouseVelocity;

		//if (pitch > 89.0f)
		//	pitch = 89.0f;
		//if (pitch < -89.0f)
		//	pitch = -89.0f;

		glm::quat key_quat = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0));

		////std::cout << pitch << " " << yaw << std::endl;
		////std::cout << key_quat.x <<" "<< key_quat.y <<" "<< key_quat.z << std::endl;

		this->front = glm::rotate(key_quat, glm::vec3(0, 0, -1));
		this->front = glm::normalize(this->front);

		this->up = glm::rotate(key_quat, this->worldup);
		glm::normalize(this->up);
		this->right = glm::cross(this->front, this->up);
		glm::normalize(this->right);
	}
private:
	glm::vec3	worldup;
	glm::vec3	front, up, right;
	GLfloat		keyboardVelocity, mouseVelocity;
	GLfloat     pitch, yaw;
};

#endif