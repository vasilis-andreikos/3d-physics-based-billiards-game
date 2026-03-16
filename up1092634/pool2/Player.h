#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"


class Player {
public:
    std::vector<Sphere*> balls;
    int turn;
    int ball_type = NULL;
    bool correct;
    bool firstOfRound;
    Player(int turn, const std::vector<Sphere*>& _balls);

    void Get_ball_type(Sphere& ball);
    void removeBall(Sphere& ball, bool gameState);
};

#endif
