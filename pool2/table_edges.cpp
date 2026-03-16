#include "table_edges.h"
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include <iostream>
#include <tiny_obj_loader.h>
#include "stb_image.h"

using namespace glm;

Table_Edges::Table_Edges(vec3 pos)
    : RigidBody() {
    table_edges = new Drawable("models/table_edges.obj");
    //table = new ogl::Model("models/table.obj");
    x = pos;
    }
    


Table_Edges::~Table_Edges() {
    delete table_edges;
}

void Table_Edges::draw(unsigned int drawable) {
    table_edges->bind();
    table_edges->draw();

}

void Table_Edges::update(float t, float dt) {
    // numerical integration
    advanceState(t, dt);

    // compute model matrix
    mat4 scale = glm::scale(mat4(), vec3(l, l, l));
    mat4 tranlation = translate(mat4(), vec3(x.x, x.y, x.z));
#ifdef USE_QUATERNIONS
    mat4 rotation = mat4_cast(q);
#else
    mat4 rotation = mat4(R);
#endif
    modelMatrix = tranlation;
}