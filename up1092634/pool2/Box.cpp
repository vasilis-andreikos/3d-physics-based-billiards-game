#include "Box.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>

using namespace glm;

Box::Box(vec3(pos), vec3(scale)){
    x = pos;
    s = scale;
    cube = new Drawable("models/cube.obj");
    fill = false;
}

Box::~Box() {
    delete cube;
}

void Box::draw(unsigned int drawable) {
    if (fill) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glDisable(GL_CULL_FACE);
    cube->bind();
    cube->draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
}

void Box::update(float t, float dt) {
    mat4 translate = glm::translate(mat4(), x);
    mat4 scale = glm::scale(mat4(), s);
    modelMatrix = translate * scale;
}