#ifndef CUE_H
#define CUE_H

#include "RigidBody.h"

class Drawable;
class Sphere;

class Cue : public RigidBody {
public:
    Drawable* cue;
    float l = 0.01f;
    float angle=0.0f;
    float radius=0.9f;
    float lastMouseX=-1.0f;
    float power=0;
    float shot_angle=0.0f;
    glm::mat4 modelMatrix;
    bool hide = false;

    Cue(glm::vec3 pos);
    ~Cue();

    void draw(unsigned int drawable = 0);
    void update(float t = 0, float dt = 0, GLFWwindow* window = 0, Sphere* cue_ball = NULL, bool making_Shot = false);
};

#endif