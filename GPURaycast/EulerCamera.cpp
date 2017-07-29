#include "EulerCamera.h"

EulerCamera::EulerCamera(glm::vec3 pos, int screenWidth, int screenHeight)
{
	proj = glm::perspective(67.0f, ((float)screenWidth / (float)screenHeight), 0.1f, 1000.0f);
	position = pos;
	front = glm::vec3(0.0, 0.0, 1.0);
	right = glm::vec3(1.0, 0.0, 0.0);
	yaw = 90.0;
	pitch = 0.0f;
	roll = 0.0f;
	degrees = 0;
	sensitivity = 2.0f;	
	up = glm::vec3(0.0, 1.0, 0.0);
	worldup = up;

	yawInput = 0;
	pitchInput = 0;
	rollInput = 0;
	forwardInput = 0;
	rightInput = 0;
}

void EulerCamera::setSensitivity(GLfloat value) { sensitivity = value; }

void EulerCamera::changeFront(GLfloat pi, GLfloat ya, GLfloat ro) {

	pi *= sensitivity;
	ya *= sensitivity;
	ro *= sensitivity;

	pitch += (GLfloat)pi;
	yaw += (GLfloat)ya;
	roll += (GLfloat)ro;
	if (pitch > 89)
		pitch = 89;
	if (pitch < -89)
		pitch = -89;
	if (yaw > 360)
		yaw = 0;
	else if (yaw < 0)
		yaw = 360;

	resetFront();

}

void EulerCamera::movForward(GLfloat value) { position += front * value / 10.0f; }

void EulerCamera::movRight(GLfloat value) { position += glm::normalize(glm::cross(front, up))*(value / 10.0f); }

void EulerCamera::move(GLfloat value) { position += glm::vec3(front.x * value / 10.0f, 0.0f, front.z * value / 10.0f); }

void EulerCamera::setPosition(glm::vec3 value) { position = value; }

glm::vec3 EulerCamera::getPosition() { return position; }

glm::vec3 EulerCamera::getFront() { return front; }

void EulerCamera::setFront(glm::vec3 value, GLfloat y, GLfloat p) {
	front = glm::normalize(value);
	yaw = y;
	pitch = p;
}

glm::vec3 EulerCamera::getUp() { return up; }

glm::mat4 EulerCamera::getView()
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 EulerCamera::getProj()
{
	return proj;
}

void EulerCamera::jump(bool& jumping) {
	if (jumping)
	{
		position += glm::vec3(0.0f, 0.3f*cos(glm::radians(degrees)), 0.0f);
		front += glm::vec3(0.0f, 0.3f*cos(glm::radians(degrees)), 0.0f);
		if (degrees >= 180)
		{
			jumping = false;
			degrees = 0;
		}
		degrees += 5.0f;
	}
	else
		return;
}

void EulerCamera::orbitAround(glm::vec3 point, GLfloat pi, GLfloat ya)
{
	static float totalPi = 0;
	static float totalYa = 0;
	totalPi += pi;
	totalYa += ya;

	if (totalPi > 89)
		totalPi = 89;
	if (totalPi < -89)
		totalPi = -89;
	if (totalYa > 360)
		totalYa = 0;
	else if (totalYa < 0)
		totalYa = 360;

	glm::vec3 direction = point - position;
	float displacement = glm::length(direction);

	direction.x = cos(glm::radians(totalYa)) * cos(glm::radians(totalPi));
	direction.y = sin(glm::radians(totalPi));
	direction.z = sin(glm::radians(totalYa)) * cos(glm::radians(totalPi));

	direction = glm::normalize(direction);
	direction *= displacement;
	position = point + direction;

	front = glm::normalize(point - position);
}

void EulerCamera::resetFront()
{
	GLfloat rpitch = glm::radians(pitch);
	GLfloat ryaw = glm::radians(yaw);
	GLfloat rroll = glm::radians(roll);
	GLfloat ninety = glm::radians(90.0f);

	glm::vec3 f = glm::vec3();
	f.x = cos(ryaw) * cos(rpitch);
	f.y = sin(rpitch);
	f.z = sin(ryaw) * cos(rpitch);
	//get direction to look at and normalise it to make it a unit vector
	front = glm::normalize(f);
	right = glm::normalize(glm::cross(front, worldup));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}
