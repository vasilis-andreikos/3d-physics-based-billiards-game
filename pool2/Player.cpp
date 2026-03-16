#include "Player.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include "Sphere.h"
#include <iostream>


using namespace glm;
using namespace std;

Player::Player(int turn, const std::vector<Sphere*>& _balls) 
	: turn(turn), balls(_balls) {}

void Player::Get_ball_type(Sphere& ball) {
	this->ball_type = ball.type;
}

void Player::removeBall(Sphere& ball, bool gameState) {
	if (!balls.empty()) {
		balls.erase(std::remove_if(balls.begin(), balls.end(),
			[](Sphere* ball) { return ball->isPocketed; }), balls.end());
	}
	else {
		this->ball_type = 8;
		if (ball.type == 8) {
			gameState = false;
			cout << "YOU WIN!\n";
		}
	}
}