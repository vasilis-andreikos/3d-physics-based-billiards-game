#ifndef PLANE_H
#define PLANE_H

#include <glm/glm.hpp>

class Drawable;

/**
 * Represents the bounding box
 */
class Plane {
public:
    Drawable* plane;
    float size;
    float y;
    glm::mat4 modelMatrix;

    Plane(float s, float y);
    ~Plane();

    void draw(unsigned int drawable = 0);
    void update(float t = 0, float dt = 0);
};

#endif
