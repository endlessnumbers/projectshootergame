#pragma once
class Player
{
public:
	Player();
	void damage();
	void increaseScore();
	int getHealth();
	int getScore();
private:
	int health = 100;
	int score = 0;
};

