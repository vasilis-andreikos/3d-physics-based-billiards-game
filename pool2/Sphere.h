#ifndef SPHERE_H
#define SPHERE_H

#include "RigidBody.h"

class Drawable;

class Sphere : public RigidBody {
public:
    Drawable* sphere;
    float r;
    float In;
    glm::vec3 rotation_angle;
    glm::mat4 modelMatrix; 
    bool isPocketed = false;
    unsigned int textureID;
    int type;
    bool hide = false;
    bool floating = false;
    bool almostFloating = false;
    float fall = 0.0f;
    int timesShot = 5;
    int timesIn = 0;

    Sphere(glm::vec3 pos, glm::vec3 vel, glm::vec3 omega , float radius, float mass, int ball_type);
    ~Sphere();

    void draw(unsigned int drawable = 0);
    void update(float t = 0, float dt = 0);
    //void chooseLocation(GLFWwindow* window = 0);
};

#endif