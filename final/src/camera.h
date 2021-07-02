#ifndef CAMERA_H
#define CAMERA_H

#include <glad4.3/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/string_cast.hpp>

class Camera {
public:
	Camera(glm::vec3 position, glm::vec3 worldup) : position(position), worldup(worldup), 
		front(glm::vec3(0.0, 0.0, -1.0)), up(worldup), keyboardVelocity(10.0), mouseVelocity(0.1) 
	{
		this->right = glm::cross(this->front, this->up);
		this->pitch = 0;
		this->yaw = -90;
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
		yaw += dyaw * this->mouseVelocity;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		updateCameraVectors();
	}
	void invertPitch() {
		this->pitch = -pitch;
		updateCameraVectors();
	}

	glm::vec3 position;
private:
	glm::vec3	worldup;
	glm::vec3	front, up, right;
	GLfloat		keyboardVelocity, mouseVelocity;
	GLfloat     pitch, yaw;
	//GLfloat pfov, pratio, pnear, pfar;

	void updateCameraVectors()
	{
		this->front.y = sin(glm::radians(pitch));
		this->front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		this->front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		this->front = glm::normalize(this->front);

		this->right = glm::cross(this->front, this->worldup);
		this->up = glm::cross(this->right, this->front);
	}
};

#endif