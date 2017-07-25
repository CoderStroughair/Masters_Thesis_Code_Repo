#include "Camera.h"

// Initialize view, projection and position
void Camera::Init(int screenWidth, int screenHeight)
{
	position =  glm::vec3(0.0f, 0.0f, -2.0f);
	focus = glm::vec3(0.0f);
	distFromFocus = glm::distance(position, focus);
	xPixels = screenWidth;
	yPixels = screenHeight;

	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	FoV = 67.0f;
	float aspect = (float)xPixels / (float)yPixels;

	projMat = glm::perspective(FoV, aspect, nearPlane, farPlane);

	viewMat = glm::lookAt(position, focus, glm::vec3(0.0f,1.0f,0.0f));
}


// Update View Matrix
void Camera::Update()
{
	viewMat = glm::lookAt(position, focus, glm::vec3(0.0f,1.0f,0.0f));
}


// Zoom an amount
void Camera::Zoom(float zoomAmount)
{
	distFromFocus += zoomAmount;

	position = focus + (glm::normalize(position - focus) * distFromFocus);
}


// Translate by a vec3
void Camera::Translate(glm::vec3 translateAmount)
{
	position += translateAmount;

	position = focus + (glm::normalize(position - focus) * distFromFocus);
}


// Rotate by degrees
void Camera::Rotate(float rotateAmount)
{
	position = glm::rotateY(position, rotateAmount);
}

// Get current view direction
glm::vec3 Camera::GetViewDirection()
{
	return glm::normalize(focus - position);
}