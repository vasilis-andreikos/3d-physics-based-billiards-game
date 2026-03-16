#ifndef POOL_TABLE_H
#define POOL_TABLE_H

#include "RigidBody.h"
#include <common/model.h>

class Drawable;

class Table : public RigidBody {
public:
    Drawable* table;
    float l;
    glm::mat4 modelMatrix;

    Table(glm::vec3 pos);
    ~Table();

    void draw(unsigned int drawable = 0);
    void update(float t = 0, float dt = 0);
};

#endif