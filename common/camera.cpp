#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include <iostream>
#include <string>

using namespace glm;

int pressed = 0;
vec3 prev_pos;
vec3 prev_dir;
vec3 prev_right;
vec3 prev_up;
float prev_hor;
float prev_ver;

vec3 static_position = vec3(0.1f, 4.0f, 0.0f);
vec3 static_direction = vec3(0, -1, 0);
vec3 static_right = vec3(sin(-3.14f / 2.0f), 0, cos(-3.14f / 2.0f));
vec3 static_up = -cross(static_right, static_direction);

Camera::Camera(GLFWwindow* window) : window(window) {
    position = vec3(0, 1.3f, 5);
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;
    FoV = 45.0f;
    speed = 3.0f;
    mouseSpeed = 0.001f;
    fovSpeed = 2.0f;
    direction = vec3(0, 0, 0);
    up = vec3(0, 0, 0);
    right = vec3(0, 0, 0);
}

void Camera::update(int change_view) {
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);



    //std::cout << "\n prev x here: " << prev_pos.x;
    //std::cout << "\n prev y here: " << prev_pos.y;
    //std::cout << "\n prev z here: " << prev_pos.z;

    // Task 5.1: simple camera movement that moves in +-z and +-x axes
    /*/
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position -= vec3(0, 0, 1) * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position += vec3(0, 0, 1) * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += vec3(1, 0, 0) * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= vec3(1, 0, 0) * deltaTime * speed;
    }

    // Task 5.2: update view matrix so it always looks at the origin
    projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
    viewMatrix = lookAt(
        position,
        vec3(0, 0, 0),
        vec3(0, 1, 0)
    );
    //*/

    // Task 5.3: Compute new horizontal and vertical angles, given windows size
    //*/
    // and cursor position
    if (change_view == 2) {
        horizontalAngle = prev_hor;
        verticalAngle = prev_ver;
        position = prev_pos;
    }
    else if(change_view==0){
        horizontalAngle += mouseSpeed * float(width / 2 - xPos);
        verticalAngle += mouseSpeed * float(height / 2 - yPos);
    }


    if (change_view == 0) {    
        // Reset mouse position for next frame
        glfwSetCursorPos(window, width / 2, height / 2);
        // Task 5.4: right and up vectors of the camera coordinate system
        // use spherical coordinates
        vec3 direction(
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
        );
        // Right vector
        vec3 right(
            sin(horizontalAngle - 3.14f / 2.0f),
            0,
            cos(horizontalAngle - 3.14f / 2.0f)
        );

        // Up vector
        vec3 up = cross(right, direction);

        // Task 5.5: update camera position using the direction/right vectors
        // Move forward
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            position += direction * deltaTime * speed;
        }
        // Move backward
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            position -= direction * deltaTime * speed;
        }
        // Strafe right
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            position += right * deltaTime * speed;
        }
        // Strafe left
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            position -= right * deltaTime * speed;
        }

        // Task 5.6: handle zoom in/out effects
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            FoV -= fovSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            FoV += fovSpeed;
        }

        // Task 5.7: construct projection and view matrices
        projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
        viewMatrix = lookAt(
            position,
            position + direction,
            up
        );

        vec3 tableMin(-1.7f, 1.0f, -1.0f);
        vec3 tableMax(1.7f, 1.6f, 1.0f);

        if (position.x > tableMin.x && position.x < tableMax.x &&
            position.z > tableMin.z && position.z < tableMax.z &&
            position.y < tableMax.y + 0.2f) { // Allow looking slightly over the table

            // Find the closest edge and push the camera out
            float distLeft = fabs(position.x - tableMin.x);
            float distRight = fabs(position.x - tableMax.x);
            float distFront = fabs(position.z - tableMin.z);
            float distBack = fabs(position.z - tableMax.z);

            // Push the camera in the direction of least penetration
            float minDist = std::min({ distLeft, distRight, distFront, distBack });
            if (minDist == distLeft) position.x = tableMin.x;
            else if (minDist == distRight) position.x = tableMax.x;
            else if (minDist == distFront) position.z = tableMin.z;
            else if (minDist == distBack) position.z = tableMax.z;
        }
    

        if (pressed == 0) {
            position.y = 1.3f;
        }

        if (position.y < 0.8f) {
            position.y = 0.8f;
        }

        if (position.y > 1.3f) {
            position.y = 1.3f;
        }

        if (position.y == 1.3f) {
            pressed = 0;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            pressed = 1;
            if (position.y > 0.8f) {
                position.y -= 0.002f;
            }
        }
        else {
            if (position.y < 1.3f) {
                position.y += 0.002f;
            }
        }
    }
    if (change_view == 1) {
        prev_pos = position;
        prev_dir = direction;
        prev_up = up;
        prev_right = right;
        prev_hor = horizontalAngle;
        prev_ver = verticalAngle;

        projectionMatrix = ortho(-1.5f, 1.5f, -1.5f, 1.5f, 0.1f, 100.0f);
        viewMatrix = lookAt(
            static_position,
            static_position + static_direction,
            static_up
        );
    }



    //*/
    // Homework XX: perform orthographic projection

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}

