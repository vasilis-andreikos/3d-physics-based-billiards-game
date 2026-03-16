#ifndef TABLE_POCKETS_H
#define TABLE_POCKETS_H

#include "RigidBody.h"
#include <common/model.h>

class Drawable;

class Table_Pockets : public RigidBody {
public:
    Drawable* table_pockets;
    float l;
    glm::mat4 modelMatrix;

    Table_Pockets(glm::vec3 pos);
    ~Table_Pockets();

    void draw(unsigned int drawable = 0);
    void update(float t = 0, float dt = 0);
};

#endif