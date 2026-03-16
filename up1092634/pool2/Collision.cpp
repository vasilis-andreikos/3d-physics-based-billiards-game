#include "Collision.h"
#include "Box.h"
#include "Sphere.h"
#include "Plane.h"
#include "Player.h"
#include <iostream>

using namespace glm;
using namespace std;

float hole_size = 0.1f;

bool first_ball = true;

void handlePlaneSphereCollision(Plane& plane, Sphere& sphere);
bool checkForPlaneSphereCollision(glm::vec3& pos, const float& r,
    float size, float height,glm::vec3& n);

void handleBoxSphereCollision(Box& box, Sphere& sphere, Player* playerShot, Player* playerOther, Sphere& cue_ball, bool gameState);
bool checkForBoxSphereCollision(Sphere& sphere,
    glm::vec3 size, glm::vec3& n, vec3& boxpos, Player* playerShot, Player* playerOther, Sphere& cue_ball, bool gameState);

void handleSphereSphereCollision(Sphere& sphere, Sphere& sphere2, Player* playerShot);
bool checkForSphereSphereCollision(glm::vec3& pos, const float& r,
    glm::vec3& pos2, const float& r2, glm::vec3& n);

struct Hole {
    glm::vec3 position;  // Center of the hole
    float radius;        // Radius of the hole
};

std::vector<Hole> holes = {
    {{-0.85f, 1.0f, -0.5f}, hole_size},  // Bottom-left corner
    {{1.1f, 1.0f, -0.5f}, hole_size},   // Bottom-right corner
    {{-0.85f, 1.0f, 0.5f}, hole_size},   // Top-left corner
    {{1.1f, 1.0f, 0.5f}, hole_size},    // Top-right corner
    {{0.13f, 1.0f, 0.5f}, hole_size},   // Middle-bottom
    {{0.13f, 1.0f, -0.55f}, hole_size},    // Middle-top
};

void handlePlaneSphereCollision(Plane& plane, Sphere& sphere) {
    vec3 n;
    if (checkForPlaneSphereCollision(sphere.x, sphere.r, plane.size, plane.y, n)) {
        // Task 2b: define the velocity of the sphere after the collision
        sphere.v = sphere.v - n * glm::dot(sphere.v, n) * 2.0f; // 1.96
        sphere.P = sphere.m * sphere.v;
    }
}

void handleBoxSphereCollision(Box& box, Sphere& sphere, Player* playerShot, Player* playerOther, Sphere& cue_ball, bool gameState) {
    vec3 n;
    float COR = 0.6f;
    if (checkForBoxSphereCollision(sphere, box.s, n, box.x, playerShot, playerOther, cue_ball, gameState)) {
        // Task 2b: define the velocity of the sphere after the collision
        //sphere.v = sphere.v - n * glm::dot(sphere.v, n) * (2.0f); // 1.96
        sphere.v = reflect(sphere.v, n);
        sphere.v *= COR;
        sphere.P = sphere.m * sphere.v;
    }
}

void handleSphereSphereCollision(Sphere& sphere, Sphere& sphere2, Player* playerShot) {
    vec3 n;
    if (checkForSphereSphereCollision(sphere.x, sphere.r, sphere2.x, sphere2.r, n)) {
        // Ensure the normal is correctly computed
        vec3 normal = normalize(sphere.x - sphere2.x);

        // Position correction to avoid sinking into each other
        float depth = (sphere.r + sphere2.r) - glm::length(sphere.x - sphere2.x);
        vec3 correction = normal * (depth * 0.5f);
        sphere.x += correction;
        sphere2.x -= correction;

        vec3 r1 = sphere.x - sphere2.x;
        vec3 r2 = sphere2.x - sphere.x;

        // Apply torque based on the collision's point of contact
        vec3 torque1 = glm::cross(r1, (sphere.v - sphere2.v));  // Simplified torque calculation
        vec3 torque2 = glm::cross(r2, (sphere2.v - sphere.v));

        // Update angular velocities (w) based on the torques
        sphere.w += torque1 / sphere.In;  // I = inertia tensor, may need adjustment for your use
        sphere2.w += torque2 / sphere2.In;


        // Decompose velocities into parallel and perpendicular components
        vec3 v1_par = dot(sphere.v, normal) * normal;
        vec3 v1_per = sphere.v - v1_par;
        vec3 v2_par = dot(sphere2.v, normal) * normal;
        vec3 v2_per = sphere2.v - v2_par;

        // Save old parallel velocities before modifying them
        vec3 v1_par_old = v1_par;
        vec3 v2_par_old = v2_par;

        // Elastic collision formula (momentum conservation)
        v1_par = ((sphere.m - sphere2.m) * v1_par_old + 2 * sphere2.m * v2_par_old) / (sphere.m + sphere2.m);
        v2_par = ((sphere2.m - sphere.m) * v2_par_old + 2 * sphere.m * v1_par_old) / (sphere.m + sphere2.m);

        // Update velocities
        sphere.v = v1_par + v1_per;
        sphere2.v = v2_par + v2_per;

        // Update momentum
        sphere.P = sphere.m * sphere.v;
        sphere2.P = sphere2.m * sphere2.v;
        if (first_ball == false) {
            if (playerShot->firstOfRound == true) {
                if (sphere.type == 0) {
                    if (playerShot->ball_type != sphere2.type && playerShot->ball_type != NULL) {
                        sphere.almostFloating = true;
                        cout << "Hit Wrong Ball Type\n";
                    }
                }
                playerShot->firstOfRound = false;
            }
        }


    }
}


bool checkForPlaneSphereCollision(glm::vec3& pos, const float& r,
    float size, float height, glm::vec3& n) {
    if (pos.y - r <= height) {
        n = vec3(0, -1, 0);
        return true;
    }
}

bool checkForSphereSphereCollision(vec3& pos, const float& r,
    vec3& pos2, const float& r2, vec3& n) {

    if (sqrt(pow(pos2.x - pos.x,2)+ pow(pos2.y - pos.y, 2)+pow(pos2.z - pos.z, 2)) <= r + r2) {
        return true;
    }
    else {
        return false;
    }
}


bool checkForBoxSphereCollision(Sphere& sphere,
    glm::vec3 size, vec3& n, glm::vec3& boxpos, Player* playerShot, Player* playerOther, Sphere& cue_ball, bool gameState) {
    
    glm::vec3 halfSize = size * 0.5f;  // Correct half-dimensions
    glm::vec3 closestPoint;

    bool collision = false;

    //vec3 boxMin = vec3(boxpos.x - size.x, boxpos.y - size.y, boxpos.z - size.z);
    //vec3 boxMax = vec3(boxpos.x + size.x, boxpos.y + size.y, boxpos.z + size.z);

    //closestPoint.x = std::max(boxMin.x, std::min(pos.x, boxMax.x));
    //closestPoint.y = std::max(boxMin.y, std::min(pos.y, boxMax.y));
    //closestPoint.z = std::max(boxMin.z, std::min(pos.z, boxMax.z));

    //vec3 distanceVec = pos - closestPoint;
    //float squaredDist = dot(distanceVec,distanceVec);

    //if (squaredDist <= (r * r)) {
    //    cout << "collision\n";
    //    collision = true;
    //    if (pos.x < boxMin.x) { n = vec3(-1, 0, 0); }
    //    if (pos.x > boxMax.x) { n = vec3(1, 0, 0); }
    //    if (pos.y < boxMin.y) { n = vec3(0, -1, 0); }
    //    if (pos.y > boxMax.y) { n = vec3(0, 1, 0); }
    //    if (pos.z < boxMin.z) { n = vec3(0, 0, -1); }
    //    if (pos.z > boxMax.z) { n = vec3(0, 0, 1); }
    //}

    //float penetrationX = 0, penetrationY = 0, penetrationZ = 0;

    //if (pos.x < boxMin.x) {
    //    penetrationX = boxMin.x - pos.x;
    //}
    //else if (pos.x > boxMax.x) {
    //    penetrationX = pos.x - boxMax.x;
    //}

    //if (pos.y < boxMin.y) {
    //    penetrationY = boxMin.y - pos.y;
    //}
    //else if (pos.y > boxMax.y) {
    //    penetrationY = pos.y - boxMax.y;
    //}

    //if (pos.z < boxMin.z) {
    //    penetrationZ = boxMin.z - pos.z;
    //}
    //else if (pos.z > boxMax.z) {
    //    penetrationZ = pos.z - boxMax.z;
    //}


    // Find the axis with the smallest penetration (the direction to resolve)
    //if (penetrationX > penetrationY && penetrationX > penetrationZ) {
    //     Resolve on the X axis
    //    if (pos.x < boxMin.x) {
    //        pos.x = boxMin.x - r;
    //    }
    //    else {
    //        pos.x = boxMax.x + r;
    //    }
    //}
    //else if (penetrationY > penetrationX && penetrationY > penetrationZ) {
    //     Resolve on the Y axis
    //    if (pos.y < boxMin.y) {
    //        pos.y = boxMin.y - r;
    //    }
    //    else {
    //        pos.y = boxMax.y + r;
    //    }
    //}
    //else if (penetrationZ > penetrationX && penetrationZ > penetrationY) {
    //     Resolve on the Z axis
    //    if (pos.z < boxMin.z) {
    //        pos.z = boxMin.z - r;
    //    }
    //    else {
    //        pos.z = boxMax.z + r;
    //    }
    //}
    //else {
    //    std::cout << "Warning: No penetration detected despite collision!\n";
    //}
    //// Debug: Print resolved position
    //std::cout << "Resolved position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n";
    //pos.x = std::max(boxMin.x + r, std::min(pos.x, boxMax.x - r));
    //pos.y = std::max(boxMin.y + r, std::min(pos.y, boxMax.y - r));
    //pos.z = std::max(boxMin.z + r, std::min(pos.z, boxMax.z - r));

    //return squaredDist <= (r * r);

    bool bottom_left_hole = sphere.x.z < boxpos.z - halfSize.z + hole_size && sphere.x.x < boxpos.x - halfSize.x + hole_size;
    bool bottom_right_hole = sphere.x.z < boxpos.z - halfSize.z + hole_size && sphere.x.x > boxpos.x + halfSize.x - hole_size;
    bool top_left_hole = sphere.x.z > boxpos.z + halfSize.z - hole_size && sphere.x.x < boxpos.x - halfSize.x + hole_size;
    bool top_right_hole = sphere.x.z > boxpos.z + halfSize.z - hole_size && sphere.x.x > boxpos.x + halfSize.x - hole_size;

    if (sphere.x.x - sphere.r < boxpos.x - halfSize.x) {
        if (bottom_left_hole || top_left_hole) {
            ballHoleCollision(sphere, playerShot, playerOther, cue_ball, gameState);
            return false;
        }
        float dis = (boxpos.x - halfSize.x) - (sphere.x.x - sphere.r);
        sphere.x.x += dis;
        n = vec3(-1, 0, 0);
        collision = true;
    }
    else if (sphere.x.x + sphere.r > boxpos.x + halfSize.x) {
        if (bottom_right_hole || top_right_hole) {
            ballHoleCollision(sphere, playerShot, playerOther, cue_ball, gameState);
            return false;
        }
        float dis = (boxpos.x + halfSize.x) - (sphere.x.x + sphere.r);
        sphere.x.x += dis;
        n = vec3(1, 0, 0);
        collision = true;
    }        
    //if (sphere.x.y - sphere.r < boxpos.y - halfSize.y) {
    //    float dis = (boxpos.y - halfSize.y) - (sphere.x.y - sphere.r);
    //    sphere.x.y += dis;
    //    n = vec3(0, -1, 0);
    //    collision = true;
    //}
    //else if (sphere.x.y + sphere.r > boxpos.y + halfSize.y) {
    //    float dis = (boxpos.y + halfSize.y) - (sphere.x.y + sphere.r);
    //    sphere.x.y += dis;
    //    n = vec3(0, 1, 0);
    //    collision = true;
    //}
    if (cue_ball.timesShot > 1) {
        first_ball = false;
    }
    if (sphere.x.z - sphere.r < boxpos.z - halfSize.z) {
        if (sphere.x.x > boxpos.x - hole_size && sphere.x.x < boxpos.x + hole_size) {ballHoleCollision(sphere, playerShot, playerOther, cue_ball, gameState); return false; }
        float dis = (boxpos.z - halfSize.z) - (sphere.x.z - sphere.r);
        sphere.x.z += dis;
        n = vec3(0, 0, -1);
        collision = true;
    }
    else if (sphere.x.z + sphere.r > boxpos.z + halfSize.z) {
        if (sphere.x.x > boxpos.x - hole_size && sphere.x.x < boxpos.x + hole_size) {ballHoleCollision(sphere, playerShot, playerOther, cue_ball, gameState); return false; }
        float dis = (boxpos.z + halfSize.z) - (sphere.x.z + sphere.r);
        sphere.x.z += dis;
        n = vec3(0, 0, 1);
        collision = true; 
    }
    if (collision == false) {
        sphere.fall = 0.0f;
    }
    //cout << "\n" << collision;
    return collision;
}

bool ballHoleCollision(Sphere& sphere, Player* playerShot, Player* playerOther, Sphere& cue_ball, bool gameState) {
    bool inHole = false;

    if (cue_ball.timesShot == 2) {
        first_ball = false;
    }

    if (first_ball == false && sphere.type != 0 && sphere.type != 8) {
        playerShot->ball_type = sphere.type;
        if (sphere.type == 1) {
            playerOther->ball_type = 2;
            first_ball = false;
        }
        else if(sphere.type == 2){
            playerOther->ball_type = 1;
            first_ball = false;
        }    
    }
    // Check if the ball is inside any hole
    for (const auto& hole : holes) {
        //if (glm::length(sphere.x - hole.position) < hole.radius) {
            inHole = true;
            //cout << "In hole";
            break;
        //}
    }

    // If in a hole, let the ball fall (remove collision constraints)
    if (inHole) {        
        if(sphere.type == 0){
            sphere.v = vec3(0.0f);
            sphere.P = vec3(0.0f);
            sphere.hide = true;
            sphere.floating = true;
            inHole = false;
            playerShot->correct = false;
            playerOther->correct = true;
            cout << "Cue Ball pocketed\n";
            sphere.x = vec3(0.0f);
        }
        else if (sphere.type == 8 && playerShot->ball_type != 8) {
            cout << "8 Ball Pocketed Early. You Lost\n";
            gameState = false;
        }
        else if (playerShot->ball_type == NULL) {
            sphere.fall += sphere.m; // Apply gravity to make it fall
            playerShot->correct = true;
            playerOther->correct = false;
            if (sphere.isPocketed == false) {
                if (sphere.type == 1 && sphere.timesIn == 0) {
                    cout << "SOLID Ball Pocketed\n";
                    sphere.isPocketed = true;
                }
                else if (sphere.type == 2) {
                    cout << "STRIPED Ball Pocketed\n";
                    sphere.isPocketed = true;
                }
            }
        }
        else if (sphere.type == playerShot->ball_type) {
            playerShot->removeBall(sphere, gameState);
            playerShot->correct = true;
            playerOther->correct = false;
        }
        else if(sphere.type == playerOther->ball_type){
            playerOther->removeBall(sphere, gameState);
            playerShot->correct = false;
            playerOther->correct = true;
        }

        sphere.timesIn += 1;
        sphere.fall +=  sphere.m; // Apply gravity to make it fall
        if (sphere.isPocketed == false) {
            if (sphere.type == 1) {
                cout << "SOLID Ball Pocketed\n";
                sphere.isPocketed = true;
            }
            else if (sphere.type == 2) {
                cout << "STRIPED Ball Pocketed\n";
                sphere.isPocketed = true;
            }
        }

    }

    return inHole;
}
