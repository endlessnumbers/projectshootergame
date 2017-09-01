#include "Player.h"


Player::Player()
{
	health = 100;
	score = 0;
}


void Player::damage() { health -= 5; }

void Player::increaseScore() { score += 100; }

int Player::getHealth() { return health; }

int Player::getScore() { return score; }