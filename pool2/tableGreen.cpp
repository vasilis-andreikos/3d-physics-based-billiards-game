#include "tableGreen.h"
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include <iostream>
#include <tiny_obj_loader.h>
#include "stb_image.h"

using namespace glm;

Table_Green::Table_Green(vec3 pos)
    : RigidBody() {
    table_green = new Drawable("models/table_green.obj");
    //table = new ogl::Model("models/table.obj");
    x = pos;
    }
    


Table_Green::~Table_Green() {
    delete table_green;
}

void Table_Green::draw(unsigned int drawable) {
    table_green->bind();
    table_green->draw();

}

void Table_Green::update(float t, float dt) {
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