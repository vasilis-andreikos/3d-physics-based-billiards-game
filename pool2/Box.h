#ifndef BOX_H
#define BOX_H

#include <glm/glm.hpp>

class Drawable;

/**
 * Represents the bounding box
 */
class Box {
public:
    Drawable* cube;
    glm::vec3 x, s;
    glm::mat4 modelMatrix;
    bool fill;

    Box(glm::vec3(pos), glm::vec3(scale));
    ~Box();

    void draw(unsigned int drawable = 0);
    void update(float t = 0, float dt = 0);
};

#endif
