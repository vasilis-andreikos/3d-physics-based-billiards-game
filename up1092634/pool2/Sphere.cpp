#include "Sphere.h"
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include <glfw3.h>
#include <iostream>

using namespace glm;

Sphere::Sphere(vec3 pos, vec3 vel, vec3 omega, float radius, float mass, int ball_type)
    : RigidBody() {
    sphere = new Drawable("models/ball.obj");

    r = radius;
    m = mass;
    x = pos;
    v = vel;
    w = omega;
    P = m * v;
    type = ball_type;
    if (radius == 0) throw std::logic_error("Sphere: radius != 0");
    mat3 I = mat3(
        2.0f / 5 * mass*radius*radius, 0, 0,
        0, 2.0f / 5 * mass*radius*radius, 0,
        0, 0, 2.0f / 5 * mass*radius*radius);
    omega = vec3(0.0f);
    In = 2.0f / 5.0f * m * r * r;
    rotation_angle = vec3(0.0f);
    I_inv = inverse(I);
}

Sphere::~Sphere() {
    delete sphere;
}

void Sphere::draw(unsigned int drawable) {
    if (this->hide == false) {
        sphere->bind();
        sphere->draw();
    }
}

void Sphere::update(float t, float dt) {
    //integration
    advanceState(t, dt);
   
    // compute model matrix
    mat4 scale = glm::scale(mat4(), vec3(r, r, r));
    mat4 tranlation = translate(mat4(), vec3(x.x, x.y, x.z));
    mat4 rotation = rotate(mat4(1.0f), -rotation_angle.x, vec3(1.0f, 0.0f, 0.0f));
    rotation = rotate(rotation, -rotation_angle.y, vec3(0.0f, 1.0f, 0.0f));
    rotation = rotate(rotation, -rotation_angle.z, vec3(0.0f, 0.0f, 1.0f));
#ifdef USE_QUATERNIONS
    //mat4 rotation = mat4_cast(q);
#else
    //mat4 rotation = mat4(R);
#endif
    modelMatrix = tranlation * rotation * scale;
}

//void Sphere::chooseLocation(GLFWwindow* window) {
//    double xPos, yPos;
//    glfwGetCursorPos(window, &xPos, &yPos);
//
//    int width, height;
//    glfwGetWindowSize(window, &width, &height);
//
//    //if (lastMouseX < 0) {
//    //    lastMouseX = xPos;
//    //}
//    float sensitivity = 0.5f;
//    float normalizedX = (float(xPos) / width) * 2.0f - 1.0f;
//    float normalizedY = (float(yPos) / height) * 2.0f - 1.0f;
//
//    x.x = normalizedX;
//    x.z = normalizedY;
//}