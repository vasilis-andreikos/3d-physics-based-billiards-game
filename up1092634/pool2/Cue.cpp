#include <GL/glew.h>
#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include <iostream>
#include "Cue.h"
#include "Sphere.h"

using namespace glm;
using namespace std;


Cue::Cue(vec3 pos)
    : RigidBody() {
    cue = new Drawable("models/cube.obj");

    x = pos;
    float angle;
    float radius;
    float lastMouseX;
    float shot_angle;
    float power;
}

Cue::~Cue() {
    delete cue;
}

void Cue::draw(unsigned int drawable) {
    if (!hide) {
        cue->bind();
        cue->draw();
    }
}



void Cue::update(float t, float dt, GLFWwindow* window, Sphere* cue_ball , bool making_Shot) {
    // numerical integration
    advanceState(t, dt);
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    if (lastMouseX < 0) {
        lastMouseX = xPos;
    }
    float sensitivity = 0.5f;
    float normalizedX = (float(xPos) / width) * 2.0f - 1.0f;
    float normalizedY = (float(yPos) / height) * 2.0f - 1.0f;

    float dx = normalizedX - cue_ball->x.x;
    float dz = normalizedY - cue_ball->x.z;

    float DX = xPos - cue_ball->x.x;
    float DZ = yPos - cue_ball->x.z;

    angle = -atan2(dz, dx);


    //float deltaX = xPos - lastMouseX;
    //lastMouseX = xPos;


    //angle += deltaX * sensitivity;

    //angle = fmod(angle, two_pi<float>());

    if (making_Shot == false) {

        x.x = DX + radius * cos(angle);
        x.z = DZ + radius * sin(angle);


        // compute model matrix
        mat4 scale = glm::scale(mat4(), vec3(1.5f, 0.01f, 0.03f));
        //mat4 tranlation = translate(mat4(), vec3(x.x, x.y, x.z));

        modelMatrix = mat4(1.0f);
        modelMatrix = translate(modelMatrix, vec3(cue_ball->x.x, x.y, cue_ball->x.z));
        modelMatrix = rotate(modelMatrix, angle, vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = translate(modelMatrix, vec3(radius, 1.2f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, vec3(1.5f, 0.01f, 0.03f));
        
        shot_angle = angle;
}
    else {
 /*       glfwSetScrollCallback(window, [this](GLFWwindow* window, double xoffset, double yoffset) {

            this->scroll_callback(window, xoffset, yoffset);
            });*/
       // radius = sqrt(dx * dx + dz * dz);
        radius = power * 0.04f;
        //cout << radius;
        float minRadius = 0.04f + 0.05f;  // Minimum distance from the ball
        float maxRadius = 0.04f + 0.7f;  // Maximum distance for full power
        radius = glm::clamp(radius, minRadius, maxRadius)+0.7f;

        //std::cout << "\n" << radius;
        modelMatrix = mat4(1.0f);
        modelMatrix = translate(modelMatrix, vec3(cue_ball->x.x, x.y, cue_ball->x.z));
        modelMatrix = rotate(modelMatrix, shot_angle, vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = translate(modelMatrix, vec3(radius, 1.2f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, vec3(1.5f, 0.01f, 0.03f));
    }

}
#ifdef USE_QUATERNIONS
    //mat4 rotation = mat4_cast(q);
#else

#endif

