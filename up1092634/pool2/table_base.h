#ifndef TABLE_BASE_H
#define TABLE_BASE_H

#include "RigidBody.h"
#include <common/model.h>

class Drawable;

class Table_Base : public RigidBody {
public:
    Drawable* table_base;
    float l;
    glm::mat4 modelMatrix;

    Table_Base(glm::vec3 pos);
    ~Table_Base();

    void draw(unsigned int drawable = 0);
    void update(float t = 0, float dt = 0);
};

#endif