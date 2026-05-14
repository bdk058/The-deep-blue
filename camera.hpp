#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
public:

    glm::vec3 ballPos;
    float ballRadius;

    float ballYaw;
    float ballPitch;

    float cctvRadius;
    float cctvTheta;
    float cctvPhi;

    bool followMode;  

    Camera(glm::vec3 startPos = glm::vec3(0.0f, 0.2f, 0.0f), float radius = 0.2f);

    void toggleMode();

    void processMouse(float xoffset, float yoffset);

    void processScroll(float yoffset);

    void setBallPosition(const glm::vec3& newPos);

    glm::vec3 getBallForward() const;

    glm::vec3 getPosition() const;

    glm::vec3 getTarget() const;
};

#endif