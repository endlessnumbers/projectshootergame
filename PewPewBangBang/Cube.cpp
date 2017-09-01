#include "Cube.h"

const glm::vec3 cubeStartPos[] = {
	glm::vec3(0.0f, 0.0f, -15.0f),
	glm::vec3(-2.0f, 0.0f, -17.0f),
	glm::vec3(4.0f, -3.0f, -20.0f)
};

Cube::Cube(glm::vec3 startingPos, glm::vec3 colour, int identity)
{
	cubePos = startingPos;
	pickingColour = colour;
	alive = true;
	timeKilled = 0.0;
	waypoint = 1;
	id = identity;
}

void Cube::calculateMovement(Player& player)
{
	//find cube by ID
	switch (id)
	{
	case 0:
		switch (waypoint)
		{
		case 1:
			//WAYPOINT 1
			if (cubePos.x > -4.0f && cubePos.z < -7.0f)
			{
				//z needs to be double x
				cubePos.x -= 0.005f;
				cubePos.z += 0.01f;
			}
			else
			{
				waypoint = 2;
				SoundEngine->play2D("media/jet.wav", GL_FALSE);
			}
			break;
		case 2:
			//WAYPOINT 2
			if (cubePos.x < -2.0f && cubePos.z < 13.0f)
			{
				//z is ten times x
				cubePos.x += 0.0025f;
				cubePos.z += 0.025f;
			}
			else
			{
				//shoot!
				waypoint = 3;
				player.damage();
				SoundEngine->play2D("media/laser.wav", GL_FALSE);
			}
			break;
		case 3:
			//waypoint 4
			if (cubePos.x < 2.0f)
			{
				//x moves +4
				cubePos.x += 0.005f;
			}
			else
			{
				waypoint = 4;
			}
			break;
		case 4:
			//waypoint 5
			if (cubePos.x < 6.0f && cubePos.z > -3.0f)
			{
				//z should be x*4
				cubePos.x += 0.01f;
				cubePos.z -= 0.04f;
			}
			else
			{
				waypoint = 5;
			}
			break;
		case 5:
			//waypoint 6
			if (cubePos.x > cubeStartPos[id].x && cubePos.z > cubeStartPos[id].z)
			{
				//z is double x
				cubePos.x -= 0.004f;
				cubePos.z -= 0.008f;
			}
			else
			{
				waypoint = 1;
				SoundEngine->play2D("media/jet_flyby1.wav", GL_FALSE);
			}
			break;
		default:
			std::cout << "CUBE ONE MOVEMENT ERROR" << std::endl;
		};
		break;

	case 1:
		switch (waypoint)
		{
		case 1:
			//WAYPOINT 1
			if (cubePos.z < -7.0f)
			{
				//z is 5 times x
				cubePos.z += 0.01f;
			}
			else
			{
				waypoint = 2;
			}
			break;
		case 2:
			//WAYPOINT 2
			//SHOOT!!
			if (cubePos.y < 4.0f && cubePos.z < 1.0f)
			{
				//y+4 z+8 :: z is double y
				cubePos.y += 0.005f;
				cubePos.z += 0.01f;
			}
			else
			{
				player.damage();
				SoundEngine->play2D("media/laser.wav", GL_FALSE);
				waypoint = 3;
			}
			break;
		case 3:
			//waypoint 3
			if (cubePos.y > 0.0f && cubePos.z < 13.0f)
			{
				//y - 4, z + 12 :: z = y*-4
				cubePos.y -= 0.004f;
				cubePos.z += 0.016f;
			}
			else
			{
				waypoint = 4;
			}
			break;
		case 4:
			//waypoint 5
			if (cubePos.y > -5.0f && cubePos.z > 3.0f)
			{
				//y - 5, z - 10 :: z = y*2
				cubePos.y -= 0.01f;
				cubePos.z -= 0.02f;
			}
			else
			{
				waypoint = 5;
			}
			break;
		case 5:
			//waypoint 6
			//BACK TO START
			if (cubePos.y < 0.0f && cubePos.z > -17.0f)
			{
				//x-2, y+5, z-20
				cubePos.y += 0.005f;
				cubePos.z -= 0.02f;
			}
			else
			{
				waypoint = 1;
				SoundEngine->play2D("media/jet_flyby1.wav", GL_FALSE);
			}
			break;
		default:
			std::cout << "CUBE 2 MOVEMENT ERROR!" << std::endl;
		}
		break;
	case 2:
		switch (waypoint)
		{
		case 1:
			//WAYPOINT 1
			if (cubePos.z < -8.0f)
			{
				cubePos.z += 0.01f;
			}
			else
			{
				waypoint = 2;
			}
			break;
		case 2:
			//WAYPOINT 2
			if (cubePos.x > 1.0f && cubePos.z < 19.0f)
			{
				//x - 3, z + 27 :: z = x * 9
				cubePos.x -= 0.001f;
				cubePos.z += 0.009f;
			}
			else
			{
				waypoint = 3;
				//SHOOT
				player.damage();
				SoundEngine->play2D("media/laser.wav", GL_FALSE);
			}
			break;
		case 3:
			//waypoint 4
			if (cubePos.x > -5.0f && cubePos.z > 3.0f)
			{
				//x+4, Z-16 :: z = x * -4
				cubePos.x -= 0.002f;
				cubePos.z -= 0.004f;
			}
			else
			{
				waypoint = 4;
			}
			break;
		case 4:
			//waypoint 5
			if (cubePos.x < 4.0f && cubePos.z > -20.0f)
			{
				//x-1, z-23
				cubePos.x += 0.0022f;
				cubePos.z -= 0.0046f;
			}
			else
			{
				waypoint = 1;
				SoundEngine->play2D("media/jet_flyby1.wav", GL_FALSE);
			}
			break;
		default:
			std::cout << "CUBE 3 MOVEMENT ERROR!" << std::endl;
			break;
		}
		break;
	default:
		std::cout << "INVALID CUBE" << std::endl;
		break;
	}

}