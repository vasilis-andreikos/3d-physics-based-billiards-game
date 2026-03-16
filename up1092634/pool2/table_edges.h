#ifndef TABLE_EDGES_H
#define TABLE_EDGES_H

#include "RigidBody.h"
#include <common/model.h>

class Drawable;

class Table_Edges : public RigidBody {
public:
    Drawable* table_edges;
    float l;
    glm::mat4 modelMatrix;

    Table_Edges(glm::vec3 pos);
    ~Table_Edges();

    void draw(unsigned int drawable = 0);
    void update(float t = 0, float dt = 0);
};

#endif