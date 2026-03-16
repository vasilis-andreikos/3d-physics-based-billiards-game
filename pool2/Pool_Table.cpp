#include "Pool_Table.h"
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include <iostream>
#include <tiny_obj_loader.h>
#include "stb_image.h"

using namespace glm;


Table::Table(vec3 pos)
    : RigidBody() {
    table = new Drawable("models/table_shell.obj");
    x = pos;

}


Table::~Table() {
    delete table;
}

void Table::draw(unsigned int drawable) {
    table->bind();
    table->draw();
}

void Table::update(float t, float dt) {
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