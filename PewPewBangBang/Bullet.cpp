#include "Bullet.h"

Bullet::Bullet()
{
	alive = false;
}

Bullet::Bullet(int bulletid, glm::vec3 cubePos)
{
	id = bulletid;
	alive = false;
	bulletPos = cubePos;
}

void Bullet::calculateMovement()
{
	if (bulletPos.z < 0.0f)
	{
		bulletPos.z += 0.05f;
		if (bulletPos.x < 0.0f)
			bulletPos.x += 0.02f;
		else if (bulletPos.x > 0.0f)
			bulletPos.x -= 0.04f;
	}
	else
	{
		alive = false;
		return;
	}
}