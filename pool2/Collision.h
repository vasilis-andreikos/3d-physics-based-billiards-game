#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>
#include "Player.h"

using namespace std;

class Box;
class Plane;
class Sphere;
void handleBoxSphereCollision(Box& box, Sphere& sphere, Player* playerShot, Player* playerOther, Sphere& cue_ball, bool gameState);
void handlePlaneSphereCollision(Plane& plane, Sphere& sphere);
void handleSphereSphereCollision(Sphere& sphere, Sphere& sphere2, Player* playerShot);
bool ballHoleCollision(Sphere& sphere, Player* playerShot, Player* playerOther, Sphere& cue_ball, bool gameState);

#endif
