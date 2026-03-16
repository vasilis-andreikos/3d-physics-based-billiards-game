// Include C++ headers
#include <iostream>
#include <string>
#include <windows.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>
#include <common/light.h>


#include "Cube.h"
#include "Box.h"
#include "Sphere.h"
#include "Plane.h"
#include "MassSpringDamper.h"
#include "Collision.h"
#include "Pool_Table.h"
#include "Cue.h"
#include "Player.h"
#include "tableGreen.h"
#include "table_base.h"
#include "table_edges.h"
#include "table_pockets.h"

using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void free();
struct Material;
void uploadMaterial(const Material& mtl);
void uploadLight(const Light& light);
void depth_pass(mat4 viewMatrix, mat4 projectionMatrix);
void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix);
void renderDepthMap();

#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "Lab 07"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

// Global variables
GLFWwindow* window;
Camera* camera;
GLuint shaderProgram, depthProgram, miniMapProgram, diffuseColorSampler, specularColorSampler, depthMapSampler;
GLuint projectionMatrixLocation, viewMatrixLocation, modelMatrixLocation;
GLuint LaLocation, LdLocation, LsLocation, lightPositionLocation, lightPowerLocation, lightVPLocation;
GLuint KdLocation, KsLocation, KaLocation, NsLocation;
GLuint shadowViewProjectionLocation, shadowModelLocation;
GLuint depthFBO, depthTexture;
GLuint table_texture;
GLuint surf_texture;
GLuint floor_texture;
GLuint base_texture;
GLuint edges_texture;
GLuint pockets_texture;
GLuint ball_texture[16];
GLuint textureSampler;
GLuint useTextureLocation;

// locations for miniMapProgram
GLuint quadTextureSamplerLocation;

Drawable* quad;

Box* test;
Box* TableSurf;
Box* wall_up;
Box* wall_up2;
Box* wall_down;
Box* wall_down2;
Box* wall_left;
Box* wall_right;
Box* surface;
Table* pool_table;
Table_Green* table_green;
Table_Base* table_base;
Table_Edges* table_edges;
Table_Pockets* table_pockets;


Player* player1;
Player* player2;
Player* player_shooting;
Player* player_waiting;
Player* temp;

Light* light1;

struct Material {
    glm::vec4 Ka;
    glm::vec4 Kd;
    glm::vec4 Ks;
    float Ns;
};

const Material defaultMaterial{
    vec4{ 0.1, 0.1, 0.1, 1 },
    vec4{ 1.0, 1.0, 1.0, 1 },
    vec4{ 0.3, 0.3, 0.3, 1 },
    20.0f
};

Cube* cube;
Plane* plane;
Sphere* cue_ball;
Sphere* holetest;
std::vector<Sphere*> balls;
std::vector<Sphere*> ballsStripes, ballsSolids;
std::vector<Sphere*> holes;
Cue* cue;

// Standard acceleration due to gravity
const float g = 9.80665f;

int change_view = 0;

float radius = 0.04f;


float anglerad;
vec3 shot_dir;
float shot_power;
vec3 shot_force;

bool making_Shot = false;
bool draw_cue = true;
bool aiming = false;
bool wasClickPressed = false;
bool wasSpacePressed = false;
bool canGoBack = false;
bool choosePower = false;
bool canShoot = false;
bool clickEnable = true;
bool gameState = true;
bool printType = true;

float max_radius = 1.44f;  // Max distance the cue can pull back
float max_force = 4000.0f;  // Adjust this to balance shot power

void createContext() {
    //shaderProgram = loadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
    shaderProgram = loadShaders("ShadowMapping.vertexshader", "ShadowMapping.fragmentshader");

    depthProgram = loadShaders("Depth.vertexshader", "Depth.fragmentshader");

    miniMapProgram = loadShaders("SimpleTexture.vertexshader", "SimpleTexture.fragmentshader");

    textureSampler = glGetUniformLocation(shaderProgram, "textureSampler");

    projectionMatrixLocation = glGetUniformLocation(shaderProgram, "P");
    viewMatrixLocation = glGetUniformLocation(shaderProgram, "V");
    modelMatrixLocation = glGetUniformLocation(shaderProgram, "M");
    KaLocation = glGetUniformLocation(shaderProgram, "mtl.Ka");
    KdLocation = glGetUniformLocation(shaderProgram, "mtl.Kd");
    KsLocation = glGetUniformLocation(shaderProgram, "mtl.Ks");
    NsLocation = glGetUniformLocation(shaderProgram, "mtl.Ns");
    LaLocation = glGetUniformLocation(shaderProgram, "light.La");
    LdLocation = glGetUniformLocation(shaderProgram, "light.Ld");
    LsLocation = glGetUniformLocation(shaderProgram, "light.Ls");
    lightPositionLocation = glGetUniformLocation(shaderProgram, "light.lightPosition_worldspace");
    lightPowerLocation = glGetUniformLocation(shaderProgram, "light.power");
    diffuseColorSampler = glGetUniformLocation(shaderProgram, "diffuseColorSampler");
    specularColorSampler = glGetUniformLocation(shaderProgram, "specularColorSampler");
    useTextureLocation = glGetUniformLocation(shaderProgram, "useTexture");
    depthMapSampler = glGetUniformLocation(shaderProgram, "shadowMapSampler");
    lightVPLocation = glGetUniformLocation(shaderProgram, "lightVP");

    // --- depthProgram ---
    shadowViewProjectionLocation = glGetUniformLocation(depthProgram, "VP");
    shadowModelLocation = glGetUniformLocation(depthProgram, "M");

    // --- miniMapProgram ---
    quadTextureSamplerLocation = glGetUniformLocation(miniMapProgram, "textureSampler");
    plane = new Plane(0.5, -0.5);

    test = new Box(vec3(0.0f, 3.0f, 0.0f), vec3(2.0f));

    pool_table = new Table(vec3(0,0.7f,0));

    TableSurf = new Box(vec3(0.0f,0.95f,0.0f), vec3(2.6f,0.5f,1.3f));
    surface = new Box(vec3(0.125f, 0.98f, 0.0f), vec3(1.9f, 0.05f, 0.955f));
    wall_down2 = new Box(vec3(0.6f, 1.0f, 0.49f), vec3(0.83f, 0.05f, 0.05f));
    wall_down = new Box(vec3(-0.34f, 1.0f, 0.49f), vec3(0.83f, 0.05f, 0.05f));
    wall_up2 = new Box(vec3(0.6f, 1.0f, -0.515f), vec3(0.83f, 0.05f, 0.05f));
    wall_up = new Box(vec3(-0.34f, 1.0f, -0.515f), vec3(0.83f, 0.05f, 0.05f));
    wall_right = new Box(vec3(1.11f, 1.0f, 0.0f), vec3(0.05f, 0.05f, 0.82f));
    wall_left = new Box(vec3(-0.85f, 1.0f, 0.0f), vec3(0.05f, 0.05f, 0.82f));

    table_texture = loadSOIL("textures/wood.png");
    surf_texture = loadSOIL("textures/table_surf.png");
    floor_texture = loadSOIL("textures/floor.png");
    base_texture = loadSOIL("textures/base.png");
    edges_texture = loadSOIL("textures/wood_table.png");
    pockets_texture = loadSOIL("textures/corners.png");
    float offsetpos = 0.0f;
    // Task 2a: change the initial position and velocity, radius and mass
    //0.7f, 0.7f, 0
    cue_ball = new Sphere(vec3(0.7f, 0.7f, 0.0f), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f, 0);
    cue_ball->timesShot = 0;
    //-0.4f, 0.7f, 0
    balls.push_back(new Sphere(vec3(-0.45f, 0.7f, 0.0f), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f, 1)); // ball1

    balls.push_back(new Sphere(vec3(-0.5f , 0.7f, radius + offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f, 2)); // ball2
    balls.push_back(new Sphere(vec3(-0.5f , 0.7f, -radius - offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,2)); // ball3

    balls.push_back(new Sphere(vec3(-0.55f , 0.7f, 2*radius + offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,1)); // ball4
    balls.push_back(new Sphere(vec3(-0.55f , 0.7f, 0.0f), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,8)); // ball5
    balls.push_back(new Sphere(vec3(-0.6f , 0.7f, radius + offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f, 1)); // ball6
    balls.push_back(new Sphere(vec3(-0.55f , 0.7f, -2*radius - offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,1)); // ball7

    balls.push_back(new Sphere(vec3(-0.6f , 0.7f, 3 * radius + offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,2)); // ball8
    
    balls.push_back(new Sphere(vec3(-0.6f , 0.7f, -radius - offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,1)); // ball9
    balls.push_back(new Sphere(vec3(-0.6f , 0.7f, -3 * radius - offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,2)); // ball10

    balls.push_back(new Sphere(vec3(-0.65f , 0.7f, 4 * radius + offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,1)); // ball11
    balls.push_back(new Sphere(vec3(-0.65f , 0.7f, 2 * radius + offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,2)); // ball12
    balls.push_back(new Sphere(vec3(-0.65f , 0.7f, 0.0f), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,1)); // ball13
    balls.push_back(new Sphere(vec3(-0.65f , 0.7f, -2 * radius - offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,2)); // ball14
    balls.push_back(new Sphere(vec3(-0.65f , 0.7f, -4 * radius - offsetpos), vec3(0, 0, 0), vec3(0, 0, 0), radius, 1.0f,1)); // ball15

    table_green = new Table_Green(vec3(0.0f, 0.70f, 0.0f));

    table_base = new Table_Base(vec3(0.0f, 0.70f, 0.0f));
    table_edges = new Table_Edges(vec3(0.0f, 0.7f, 0.0f));
    table_pockets = new Table_Pockets(vec3(0.0f, 0.7f, 0.0f));

    //holetest = new Sphere(vec3(1.1f, 1.0f, 0.5f), vec3(0.0f), vec3(0.0f), 0.06f, 1.0f);

    cue = new Cue(cue_ball->x);
    
    vector<vec3> quadVertices = {
      vec3(0.5, 0.5, -1.0),
      vec3(1.0, 0.5, -1.0),
      vec3(1.0, 1.0, -1.0),
      vec3(1.0, 1.0, -1.0),
      vec3(0.5, 1.0, -1.0),
      vec3(0.5, 0.5, -1.0)
    };

    vector<vec2> quadUVs = {
      vec2(0.0, 0.0),
      vec2(1.0, 0.0),
      vec2(1.0, 1.0),
      vec2(1.0, 1.0),
      vec2(0.0, 1.0),
      vec2(0.0, 0.0)
    };

    quad = new Drawable(quadVertices, quadUVs);
    
    for (Sphere* ball : balls) {
        if (ball->type == 1) {
            ballsSolids.push_back(ball);
        }
        else if (ball->type == 2) {
            ballsStripes.push_back(ball);
        }
    }

    player1 = new Player(0, balls);
    player2 = new Player(1, balls);

    player_shooting = player1;
    player_waiting = player2;


    for (int i = 1; i <= 15; i++) {
        string path = "textures/Balls/temp/" + to_string(i) + ".jpg";
        const char* cstr = path.c_str();
        ball_texture[i] = loadSOIL(cstr);
    }
  
    balls[0]->textureID = ball_texture[1];  // Assign texture to each ball
    balls[4]->textureID = ball_texture[8];

    balls[12]->textureID = ball_texture[5];

    balls[13]->textureID = ball_texture[11];

    balls[1]->textureID = ball_texture[14];

    balls[2]->textureID = ball_texture[13];

    balls[3]->textureID = ball_texture[7];

    balls[5]->textureID = ball_texture[10];

    balls[6]->textureID = ball_texture[2];

    balls[7]->textureID = ball_texture[9];

    balls[8]->textureID = ball_texture[6];

    balls[9]->textureID = ball_texture[15];

    balls[10]->textureID = ball_texture[3];

    balls[11]->textureID = ball_texture[12];

    balls[14]->textureID = ball_texture[4];
    

    ball_texture[0] = loadSOIL("textures/Balls/temp/BallCue.jpg");

    glGenFramebuffers(1, &depthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // Set color to set out of border 
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Finally, we have to always check that our frame buffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glfwTerminate();
        throw runtime_error("Frame buffer not initialized correctly");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void free() {
    delete cube;
    delete cue_ball;
    delete plane;
    glDeleteProgram(shaderProgram);
    glDeleteProgram(depthProgram);
    glfwTerminate();
}

void checkOpenGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    while (err != GL_NO_ERROR) {
        cout << "OpenGL error " << err << " at " << fname << ":" << line << " - " << stmt << endl;
        err = glGetError();
    }
}
#define GL_CHECK(stmt) do { \
    stmt; \
    checkOpenGLError(#stmt, __FILE__, __LINE__); \
} while (0)


void mainLoop() {
    float t = static_cast<float>(glfwGetTime());
    camera->update(change_view);
    do {

        float totalSpeed = 0.0f;
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        //cout << light1->power << "\n";
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glUseProgram(shaderProgram);
        //if (lastMouseX < 0) {
        //    lastMouseX = xPos;
        //}
        float normalizedX = (float(xPos) / width) * 2.0f - 1.0f;
        float normalizedY = (float(yPos) / height) * 2.0f - 1.0f;
        // calculate dt
        float currentTime = static_cast<float>(glfwGetTime());
        float dt = currentTime - t;
        light1->update();
        mat4 light_proj = light1->projectionMatrix;
        mat4 light_view = light1->viewMatrix;

        depth_pass(light_view, light_proj);

        mat4 projectionMatrix = camera->projectionMatrix;
        mat4 viewMatrix = camera->viewMatrix;

        //lighting_pass(viewMatrix, projectionMatrix);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, W_WIDTH, W_HEIGHT);

        // Task 2e: change dt to 0.001f and observe the total energy, then change
        // the numerical integration method to Runge - Kutta 4th order (in RigidBody.cpp)

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        uploadMaterial(defaultMaterial);

        // camera
        camera->update(change_view);
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

        uploadLight(*light1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glUniform1i(depthMapSampler, 0);

        mat4 lightVP = light1->lightVP();
        glUniformMatrix4fv(lightVPLocation, 1, GL_FALSE, &lightVP[0][0]);

        glUniform1i(useTextureLocation, 1);

        glActiveTexture(GL_TEXTURE0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, floor_texture);

        plane->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &plane->modelMatrix[0][0]);
        plane->draw();

        glBindTexture(GL_TEXTURE_2D, surf_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        table_green->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &table_green->modelMatrix[0][0]);
        table_green->draw();

        glBindTexture(GL_TEXTURE_2D, base_texture);

        table_base->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &table_base->modelMatrix[0][0]);
        table_base->draw();

        glBindTexture(GL_TEXTURE_2D, edges_texture);

        table_edges->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &table_edges->modelMatrix[0][0]);
        table_edges->draw();

        glBindTexture(GL_TEXTURE_2D, pockets_texture);

        table_pockets->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &table_pockets->modelMatrix[0][0]);
        table_pockets->draw();


        glBindTexture(GL_TEXTURE_2D, table_texture);

        glUniform1i(textureSampler, 0);

        test->update(t, dt);

        pool_table->update(t,dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &pool_table->modelMatrix[0][0]);
        pool_table->draw();





        glUniform1i(useTextureLocation, 0);

        //TableSurf->update(t, dt);
        //glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &TableSurf->modelMatrix[0][0]);
        //TableSurf->draw();



        surface->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &surface->modelMatrix[0][0]);
        //surface->draw();

        wall_down->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &wall_down->modelMatrix[0][0]);
        //wall_down->draw();

        wall_down2->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &wall_down2->modelMatrix[0][0]);
        //wall_down2->draw();

        wall_up->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &wall_up->modelMatrix[0][0]);
        //wall_up->draw();

        wall_up2->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &wall_up2->modelMatrix[0][0]);
        //wall_up2->draw();

        wall_right->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &wall_right->modelMatrix[0][0]);
        //wall_right->draw();

        wall_left->update(t, dt);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &wall_left->modelMatrix[0][0]);
        //wall_left->draw();

        glUniform1i(useTextureLocation, 1);

        glBindTexture(GL_TEXTURE_2D, ball_texture[10]);

        glUniform1i(textureSampler, 0);

        if (player1->ball_type == 1) {
            player1->balls = ballsSolids;
            player2->balls = ballsStripes;
            if (printType == true) {
                cout << "Player 1 has SOLIDS \n Player 2 has STRIPES";
                printType = false;
            }
        }
        else if (player1->ball_type == 2) {
            player1->balls = ballsStripes;
            player2->balls = ballsSolids;
            if (printType == true) {
                cout << "Player 1 has STRIPES \n Player 2 has SOLIDS";
                printType = false;
            }
        }

        if (change_view == 2) { change_view = 0; }

        bool isSpacePressed = GetAsyncKeyState(GLFW_KEY_SPACE);
        bool isClickPressed, isClickPressedCheck;

        isClickPressedCheck = GetAsyncKeyState(VK_LBUTTON);
       
        if (clickEnable) { 
            isClickPressed = isClickPressedCheck;
        }

        if (isSpacePressed && change_view == 0 && !wasSpacePressed) {
            change_view = 1;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            aiming = true;
        }

        if (!isSpacePressed && change_view == 1 && wasSpacePressed) {
            canGoBack = true;
        }

        if (isSpacePressed && change_view == 1 && wasSpacePressed && canGoBack == true) {
            change_view = 2;
            canGoBack = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        
        if (change_view == 1 ) {
            cue->update(t, dt, window, cue_ball, making_Shot);
            //make cue texture different based on player
            if (player_shooting == player1) {
                //cout << "player1\n";
                glBindTexture(GL_TEXTURE_2D, ball_texture[2]);
                glUniform1i(textureSampler, 0);
            }
            else if (player_shooting == player2) {
                glBindTexture(GL_TEXTURE_2D, ball_texture[7]);
                glUniform1i(textureSampler, 0);
            }

            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &cue->modelMatrix[0][0]);

            if (draw_cue && gameState) {
                if (cue_ball->almostFloating == true) { cue_ball->floating = true; cue_ball->almostFloating = false; }
                //if (cue_ball->timesIn == 0) { cue_ball->floating = false; }
                if (cue_ball->floating == true) {
                    cue_ball->hide = false;
                    cue_ball->x.x = normalizedX;
                    cue_ball->x.z = normalizedY;
                    draw_cue = false;
                    cue_ball->fall = 0.0f;
                    //clickEnable = false;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                    //Den kserw giati alla xwris auth th grammh ta paizei
                    cout << (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS);
                    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        cue_ball->floating = false;
                        draw_cue = true;
                        cue_ball->x.y = 0.74f;
                        //clickEnable = true;
                    }
                }

                else if (player_shooting->correct == true) {
                    player_shooting = player_shooting;
                    player_waiting = player_waiting;
                }

                else if (player_waiting->correct == true) {
                    temp = player_shooting;
                    player_shooting = player_waiting;
                    player_waiting = temp;
                    //cout << "changing player\n";
                    if (player_shooting == player1) {
                        cout << "Player 1 playing\n";
                    }
                    else if (player_shooting == player2) {
                        cout << "Player 2 playing\n";
                    }
                }

                if (draw_cue == true) {
                    cue->draw();
                    clickEnable = true;
                    player_shooting->firstOfRound = true;
                }
            }
        }
        // Task 2c: model the force due to gravity and assign to cue_ball->f
        float velocityThreshold = 0.001f;  // Minimum velocity to stop        
        float rolling_friction_coefficient = 0.01f; // Coefficient of rolling friction
        float frictionCoefficient = 0.0005f;  // Coefficient of friction
            cue_ball->x.y = 0.74 - cue_ball->fall * dt;
            if (change_view == 1) {
                //cue_ball->r = radius - cue_ball->fall * dt;
            }

            if (glm::length(cue_ball->v) > velocityThreshold) {
                // Apply friction
                
                glm::vec3 frictionForce = -frictionCoefficient * cue_ball->m * glm::normalize(cue_ball->v);  // Apply opposing friction force
                cue_ball->v += (frictionForce / cue_ball->m)*dt;


                cue_ball->P = cue_ball->m * cue_ball->v;
            }

            if (glm::length(cue_ball->v) < velocityThreshold && glm::length(cue_ball->w) > velocityThreshold) {
                cue_ball->v += glm::cross(cue_ball->w, glm::vec3(0, cue_ball->r, 0)) * dt;
                cue_ball->P = cue_ball->m * cue_ball->v;
            }

            if (glm::length(cue_ball->v) > velocityThreshold) {
                glm::vec3 rolling_friction = -rolling_friction_coefficient * cue_ball->m * 9.81f * glm::normalize(cue_ball->v);
                cue_ball->v += (rolling_friction / cue_ball->m) * dt;  // Apply rolling friction to velocity
                vec3 contact_point = cue_ball->x - vec3(0, cue_ball->r, 0); // Point of contact on the ground
                vec3 torque = cross(contact_point, rolling_friction);

                vec3 angular_acceleration = torque / cue_ball->In;
                cue_ball->w += glm::cross(glm::normalize(cue_ball->v), glm::vec3(0, 1, 0)) * glm::length(angular_acceleration) * dt * 15.0f;  // Apply angular acceleration
                cue_ball->P = cue_ball->v * cue_ball->m;
                vec3 velocity_dir = normalize(cue_ball->v);
                vec3 rotation_axis = glm::cross(velocity_dir, glm::vec3(0, 1, 0));
                cue_ball->w = glm::length(cue_ball->w) * rotation_axis;
            }
            
            cue_ball->rotation_angle.x += cue_ball->w.x * dt;
            cue_ball->rotation_angle.y += cue_ball->w.y * dt;
            cue_ball->rotation_angle.z += cue_ball->w.z * dt;    

            if ((isClickPressed || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) && change_view == 1 && !wasClickPressed && aiming == true) {
                making_Shot = true;
                aiming = false;
                choosePower = true;
                isClickPressed = true;
                //cout << "Chose angle \n";
            }

            if (!isClickPressed && change_view == 1 && wasClickPressed && choosePower == true) {
                canShoot = true;
                choosePower = false;
                //cout << "Choosing power \n";
            }
        cue_ball->forcing = [&](float t, const vector<float>& y)->vector<float> {
            vector<float> f(6, 0.0f);
            if (isClickPressed && canShoot == true && change_view == 1 && wasClickPressed) {
                anglerad = cue->shot_angle;
                //cout << "Shot Angle (Degrees): " << cue->shot_angle << endl;
                //cout << "Shot Angle (Radians): " << anglerad << endl;
                //cout << "SHOOTING!\n";
                shot_dir = normalize(vec3(cos(anglerad), 0.0f, sin(anglerad)));
                //cout << "Shot Direction: X=" << shot_dir.x << " Z=" << shot_dir.z << endl;
                shot_power = pow(cue->radius / max_radius, 6) * max_force;
                shot_force = shot_dir * shot_power;
                making_Shot = false;
                draw_cue = false;
                canShoot = false;
                aiming = true;
                player_shooting->correct = false;
                player_waiting->correct = true;
                cue->radius = 0.9f;
                cue_ball->timesShot += 1;
                //change_view = 2;        
                f[0] = -shot_force.x;
                f[2] = shot_force.z;
                //cout << f[0];
            }
            //if (!isClickPressed && !wasClickPressed) {
              //  cout << "reset\n";
             //   aiming == true;
            //}
            // Update the momentum based on new velocity
            //cue_ball->P = cue_ball->m * cue_ball->v;    
            //std::cout << "\nspeed on x:" << cue_ball->v.x;
            //std::cout << "\nspeed on y:" << cue_ball->v.y;
            //std::cout << "\nspeed on z:" << cue_ball->v.z;


            //cout << "\n speed is :" << glm::length(cue_ball->v);
            //cout << "\n z is :" << cue_ball->v.y;
            return f;
        };

        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
            cue_ball->v = vec3(0.0f);
            cue_ball->w = vec3(0.0f);
            cue_ball->P = vec3(0.0f);
            velocityThreshold = 100;
        }
        if (glm::length(cue_ball->v) < velocityThreshold) {
            cue_ball->v *= 0.99f;  // Set velocity to zero to stop the ball
            cue_ball->P = cue_ball->m * cue_ball->v;
        }
        if (glm::length(cue_ball->v) < velocityThreshold && glm::length(cue_ball->w) < velocityThreshold) {
            cue_ball->w *= 0.99f;  // Stop angular velocity as well
            cue_ball->P = cue_ball->m * cue_ball->v;
            //draw_cue = true;
        }

        handleBoxSphereCollision(*TableSurf, *cue_ball, player_shooting, player_waiting, *cue_ball, gameState);
        //handleBoxSphereCollision(*wall_down, *cue_ball);
        //handleBoxSphereCollision(*wall_down2, *cue_ball);
        //handleBoxSphereCollision(*wall_left, *cue_ball);
        //handleBoxSphereCollision(*wall_right, *cue_ball);
        //handleBoxSphereCollision(*wall_up, *cue_ball);
        //handleBoxSphereCollision(*wall_up2, *cue_ball);
        //handlePlaneSphereCollision(*plane, *cue_ball);

        cue_ball->update(t, dt);
        glBindTexture(GL_TEXTURE_2D, ball_texture[0]);
        glUniform1i(textureSampler, 0);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &cue_ball->modelMatrix[0][0]);
        cue_ball->draw();

        // First, update all balls' states

        for (Sphere* ball : balls) {
            ball->x.y = 0.74-ball->fall*dt;

            //ball->forcing = [&](float t, const vector<float>& y) -> vector<float> {
            //    vector<float> f(6, 0.0f);

            //    // Gravity force, always in the negative Y direction
            //    if (ball->v.y > 0 || ball->x.y > 0.8f) {
            //        f[1] = -ball->m * g;
            //    }

                    // Friction (note, the ball might be stuck if friction is applied too aggressively)
                    if (glm::length(ball->v) > velocityThreshold) {
                        glm::vec3 frictionForce = -frictionCoefficient * ball->m * glm::normalize(ball->v);
                        ball->v += (frictionForce / ball->m) * dt;  // Apply friction to velocity

                        ball->P = ball->v * ball->m;
                    }

                    if (glm::length(ball->v) < velocityThreshold && glm::length(ball->w) > velocityThreshold) {
                        ball->v += glm::cross(ball->w, glm::vec3(0, ball->r, 0)) * dt;
                        ball->P = ball->v * ball->m;
                    }

                    // Rolling friction if the ball is rolling
                    if (glm::length(ball->v) > velocityThreshold) {
                        glm::vec3 rolling_friction = -rolling_friction_coefficient * ball->m * 9.81f * glm::normalize(ball->v);
                        ball->v += (rolling_friction / ball->m) * dt;  // Apply rolling friction to velocity
                        vec3 contact_point = ball->x - vec3(0, ball->r, 0);
                        vec3 torque = cross(contact_point, rolling_friction);

                        vec3 angular_acceleration = torque / ball->In;
                        ball->w += glm::cross(glm::normalize(ball->v), glm::vec3(0, 1, 0)) * glm::length(angular_acceleration) * dt * 15.0f;  // Apply angular acceleration
                        ball->P = ball->v * ball->m;                    
                        vec3 velocity_dir = normalize(ball->v);
                        vec3 rotation_axis = glm::cross(velocity_dir, glm::vec3(0, 1, 0));
                        ball->w = glm::length(ball->w) * rotation_axis;
                    }



                    
                    // Update rotation angles
                    ball->rotation_angle.x += ball->w.x * dt;
                    ball->rotation_angle.y += ball->w.y * dt;
                    ball->rotation_angle.z += ball->w.z * dt;
                    

     /*       return f;
            };*/
                if (glm::length(ball->v) < velocityThreshold) {
                    ball->v *= 0.99f;  // Set velocity to zero to stop the ball
                    ball->P = ball->m * ball->v;
                }
                if (glm::length(ball->v) < velocityThreshold && glm::length(ball->w) < 0.1f) {
                    ball->w *= 0.99f;  // Stop angular velocity as well
                    ball->P = ball->m * ball->v;
                }
                
        }
        

        // First, loop through the balls and check for collisions
        for (int i = 0; i < balls.size(); ++i) {
            // Update ball position and velocity
            balls[i]->update(t, dt);  // Update positions and velocities of the balls

            // Check for sphere-box collisions (Table)
            //handleBoxSphereCollision(*wall_down, *balls[i]);
            //handleBoxSphereCollision(*wall_down2, *balls[i]);
            //handleBoxSphereCollision(*wall_left, *balls[i]);
            //handleBoxSphereCollision(*wall_right, *balls[i]);
            //handleBoxSphereCollision(*wall_up, *balls[i]);
            //handleBoxSphereCollision(*wall_up2, *balls[i]);
            //handlePlaneSphereCollision(*plane, *balls[i]);
            totalSpeed += length(balls[i]->v);
            handleBoxSphereCollision(*TableSurf, *balls[i], player_shooting, player_waiting, *cue_ball, gameState);
            if (cue_ball->floating == false) {
                handleSphereSphereCollision(*cue_ball, *balls[i], player_shooting);
            }


            if (length(balls[i]->v) < velocityThreshold) {
                balls[i]->v = vec3(0.0f);
            }

            for (int i = 0; i < balls.size(); ++i) {
                glBindTexture(GL_TEXTURE_2D, balls[i]->textureID);
                glUniform1i(textureSampler, 0);
                glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &balls[i]->modelMatrix[0][0]);
                balls[i]->draw();
            }

            // Check for sphere-to-sphere collisions (between balls)
            for (int j = i + 1; j < balls.size(); ++j) {
                handleSphereSphereCollision(*balls[i], *balls[j], player_shooting);
            }
        }
        totalSpeed += length(cue_ball->v);
        //cout << totalSpeed << "\n";
        if (totalSpeed < velocityThreshold) {
            draw_cue = true;
        }
        // After the update, apply any other necessary logic
        

        balls.erase(std::remove_if(balls.begin(), balls.end(),
            [](Sphere* ball) { return ball->x.y < 0.65f; }), balls.end());

        //if (int(t)%5 == 0) {
        //    std::cout << "\n x here: " << camera->position.x;
        //    std::cout << "\n y here: " << camera->position.y;
        //    std::cout << "\n z here: " << camera->position.z;
        //}

        //holetest->update(t, dt);
        //glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &holetest->modelMatrix[0][0]);
        //holetest->draw();
        //

        // Task 2d: calculate the total energy and comment on the previous
        // results
        
        //float KE = sphere->calcKinecticEnergy(); // given
        //float PE = sphere->m * g * sphere->x.y; // not given
        //float T = KE + PE;
        //if (T > maxEnergy) {
        //    cout << "Total Energy: " << T << endl;
        //    maxEnergy = T;
        //}
        //

        // Task 3: model a mass-spring-damper system
        
        // Task 3b: model the forces due to gravity, damper and spring
        //msd->forcing = [&](float t, const vector<float>& y)->vector<float> {
        //    vector<float> f(6, 0.0f);
        //    f[1] =
        //        -msd->m * g
        //        + msd->k * (distance(msd->a, msd->x) - msd->l0)
        //        - 1.0f * msd->b * msd->v.y;
        //    return f;
        //};
        //msd->update(t, dt);
        //glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &msd->springModelMatrix[0][0]);
        //msd->draw(0); // draw spring = 0
        //glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &msd->blobModelMatrix[0][0]);
        //msd->draw(1); // draw blob = 0
        //

        t += dt;

        wasSpacePressed = isSpacePressed;
        wasClickPressed = isClickPressed;

        //renderDepthMap();
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0 && gameState == true);
}


void initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        throw runtime_error(string(string("Failed to open GLFW window.") +
                            " If you have an Intel GPU, they are not 3.3 compatible." +
                            "Try the 2.1 version.\n"));
    }
    glfwMakeContextCurrent(window);

    // Start GLEW extension handler
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW\n");
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

    // Gray background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    glfwSetScrollCallback(window, scroll_callback);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CW);
    // glFrontFace(GL_CCW);

    // enable point size when drawing points
    glEnable(GL_PROGRAM_POINT_SIZE);

    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthFunc(GL_LESS));
    GL_CHECK(glEnable(GL_CULL_FACE));


    // Log
    logGLParameters();

    // Create camera
    camera = new Camera(window);

    light1 = new Light(window, 
        vec4{ 1, 1, 1, 1 },
        vec4{ 1, 1, 1, 1 },
        vec4{ 1, 1, 1, 1 },
        vec3{ 0, 5, 5 },
        25.0f
    );
}

void uploadMaterial(const Material& mtl) {
    glUniform4f(KaLocation, mtl.Ka.r, mtl.Ka.g, mtl.Ka.b, mtl.Ka.a);
    glUniform4f(KdLocation, mtl.Kd.r, mtl.Kd.g, mtl.Kd.b, mtl.Kd.a);
    glUniform4f(KsLocation, mtl.Ks.r, mtl.Ks.g, mtl.Ks.b, mtl.Ks.a);
    glUniform1f(NsLocation, mtl.Ns);
}

void uploadLight(const Light& light) {
    glUniform4f(LaLocation, light.La.r, light.La.g, light.La.b, light.La.a);
    glUniform4f(LdLocation, light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a);
    glUniform4f(LsLocation, light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a);
    glUniform3f(lightPositionLocation, light.lightPosition_worldspace.x,
                light.lightPosition_worldspace.y, light.lightPosition_worldspace.z);
    glUniform1f(lightPowerLocation, light.power);
}

int main(void) {
    try {
        initialize();
        createContext();
        mainLoop();
        free();
    } catch (exception& ex) {
        cout << ex.what() << endl;
        getchar();
        free();
        return -1;
    }

    return 0;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cue->power += (float)yoffset;
    cue->power = glm::clamp(cue->power, 2.0f, 19.0f);
}

void depth_pass(mat4 viewMatrix, mat4 projectionMatrix) {

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(depthProgram);

    mat4 view_projection = projectionMatrix * viewMatrix;
    glUniformMatrix4fv(shadowViewProjectionLocation, 1, GL_FALSE, &view_projection[0][0]);

    test->fill = true;


    glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &pool_table->modelMatrix[0][0]);
    pool_table->draw();

    glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &plane->modelMatrix[0][0]);
    plane->draw();

    glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &TableSurf->modelMatrix[0][0]);
    TableSurf->draw();

    glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &cue->modelMatrix[0][0]);
    cue->draw();

    glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &cue_ball->modelMatrix[0][0]);
    cue_ball->draw();

    glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &test->modelMatrix[0][0]);
    test->draw();

    for (Sphere* ball : balls) {
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &ball->modelMatrix[0][0]);
        ball->draw();
    }

    //cout << "Depth pass complete" << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix) {
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glViewport(0, 0, W_WIDTH, W_HEIGHT);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    glUseProgram(shaderProgram);
//
//    // Draw a simple triangle to test rendering
//    glBegin(GL_TRIANGLES);
//    glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -1.0f);
//    glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(0.5f, -0.5f, -1.0f);
//    glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(0.0f, 0.5f, -1.0f);
//    glEnd();
//
//    cout << "Lighting pass executed." << endl;
//}


void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, W_WIDTH, W_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    uploadLight(*light1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(depthMapSampler, 0);

    mat4 lightVP = light1->lightVP();
    glUniformMatrix4fv(lightVPLocation, 1, GL_FALSE, &lightVP[0][0]);

    glUniform1i(useTextureLocation, 1);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, table_texture);

    glUniform1i(textureSampler, 0);

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &pool_table->modelMatrix[0][0]);
    pool_table->draw();

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &plane->modelMatrix[0][0]);
    plane->draw();

    glUniform1i(useTextureLocation, 1);

    glBindTexture(GL_TEXTURE_2D, ball_texture[10]);

    glUniform1i(textureSampler, 0);

    if (player_shooting == player1) {
        glBindTexture(GL_TEXTURE_2D, ball_texture[2]);
        glUniform1i(textureSampler, 0);
    }
    else if (player_shooting == player2) {
        glBindTexture(GL_TEXTURE_2D, ball_texture[7]);
        glUniform1i(textureSampler, 0);
    }

    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &cue->modelMatrix[0][0]);

    glBindTexture(GL_TEXTURE_2D, ball_texture[0]);
    glUniform1i(textureSampler, 0);
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &cue_ball->modelMatrix[0][0]);
    cue_ball->draw();

    for (int i = 0; i < balls.size(); ++i) {
        glBindTexture(GL_TEXTURE_2D, balls[i]->textureID);
        glUniform1i(textureSampler, 0);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &balls[i]->modelMatrix[0][0]);
        balls[i]->draw();
    }

    //cout << "Lighting pass complete" << endl;
}

void renderDepthMap() {
    // using the correct shaders to visualize the depth texture on the quad
    glUseProgram(miniMapProgram);

    //enabling the texture - follow the aforementioned pipeline
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glUniform1i(quadTextureSamplerLocation, 0);

    // Drawing the quad
    quad->bind();
    quad->draw();
}
