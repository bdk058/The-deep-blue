#include "camera.hpp"
#include <cmath>

Camera::Camera(glm::vec3 startPos, float radius)
    : ballPos(startPos), ballRadius(radius),
      ballYaw(-90.0f), ballPitch(0.0f),
      cctvRadius(5.0f), cctvTheta(0.0f), cctvPhi(45.0f),
      followMode(true) {}

void Camera::toggleMode() {
    followMode = !followMode;
}

void Camera::processMouse(float xoffset, float yoffset) {
    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    if (followMode) {
        ballYaw += xoffset;
        ballPitch += yoffset;
        if (ballPitch > 89.0f) ballPitch = 89.0f;
        if (ballPitch < -89.0f) ballPitch = -89.0f;
    } else {
        cctvTheta += xoffset;
        cctvPhi += yoffset;
        if (cctvPhi > 85.0f) cctvPhi = 85.0f;
        if (cctvPhi < 5.0f) cctvPhi = 5.0f;
    }
}

void Camera::processScroll(float yoffset) {
    if (!followMode) {
        cctvRadius -= yoffset;
        if (cctvRadius < 2.0f) cctvRadius = 2.0f;
        if (cctvRadius > 15.0f) cctvRadius = 15.0f;
    }
}

void Camera::setBallPosition(const glm::vec3& newPos) {
    ballPos = newPos;
}

glm::vec3 Camera::getBallForward() const {
    if (followMode) {
        
        glm::vec3 front;
        front.x = cos(glm::radians(ballYaw)) * cos(glm::radians(ballPitch));
        front.y = sin(glm::radians(ballPitch));
        front.z = sin(glm::radians(ballYaw)) * cos(glm::radians(ballPitch));
        return glm::normalize(front);
    } else {
    
        glm::vec3 camDir = glm::normalize(getTarget() - getPosition());
        return glm::normalize(glm::vec3(camDir.x, 0.0f, camDir.z));
    }
}

glm::vec3 Camera::getPosition() const {
    if (followMode) {
        
        glm::vec3 front;
        front.x = cos(glm::radians(ballYaw)) * cos(glm::radians(ballPitch));
        front.y = sin(glm::radians(ballPitch));
        front.z = sin(glm::radians(ballYaw)) * cos(glm::radians(ballPitch));
        front = glm::normalize(front);
        return ballPos - front * 2.5f + glm::vec3(0.0f, 0.5f, 0.0f);
    } else {
        float x = cctvRadius * sin(glm::radians(cctvTheta)) * cos(glm::radians(cctvPhi));
        float y = cctvRadius * sin(glm::radians(cctvPhi));
        float z = cctvRadius * cos(glm::radians(cctvTheta)) * cos(glm::radians(cctvPhi));
        return ballPos + glm::vec3(x, y + 1.5f, z);
    }
}

glm::vec3 Camera::getTarget() const {
    return ballPos + glm::vec3(0.0f, ballRadius, 0.0f);
}