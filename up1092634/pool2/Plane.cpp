#include "Plane.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>

using namespace std;
using namespace glm;


Plane::Plane(float s, float height) {
    size = s;
    y = height; // offset to move the place up/down across the y axis
    // plane vertices
    
    vector<vec3> floorVertices = {
        vec3(-20.0f, y, -20.0f),
        vec3(-20.0f, y,  20.0f),
        vec3(20.0f,  y,  20.0f),
        vec3(20.0f,  y,  20.0f),
        vec3(20.0f,  y, -20.0f),
        vec3(-20.0f, y, -20.0f),

    };

    // plane normals
    vector<vec3> floorNormals = {
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f)
    };

    // plane uvs
    vector<vec2> floorUVs = {
        vec2(0.0f, 0.0f),
        vec2(0.0f, 1.0f),
        vec2(1.0f, 1.0f),
        vec2(1.0f, 1.0f),
        vec2(1.0f, 0.0f),
        vec2(0.0f, 0.0f),
    };


    plane = new Drawable(floorVertices, floorUVs, floorNormals);

    //plane = new Drawable("models/cube.obj");
}

Plane::~Plane() {
    delete plane;
}

void Plane::draw(unsigned int drawable) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);
    plane->bind();
    plane->draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
}

void Plane::update(float t, float dt) {
    mat4 translate = glm::translate(mat4(), vec3(size / 2, size / 2, size / 2));
    mat4 scale = glm::scale(mat4(), vec3(size, size, size));
    modelMatrix = translate * scale;
}
