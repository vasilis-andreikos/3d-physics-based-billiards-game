#ifndef TABLE_GREEN_H
#define TABLE_GREEN_H

#include "RigidBody.h"
#include <common/model.h>

class Drawable;

class Table_Green : public RigidBody {
public:
    Drawable* table_green;
    float l;
    glm::mat4 modelMatrix;

    Table_Green(glm::vec3 pos);
    ~Table_Green();

    void draw(unsigned int drawable = 0);
    void update(float t = 0, float dt = 0);
};

#endif