#pragma once

#include "Defines.h"

struct Player
{
	float LegsStartX() { return x + 24; }

	float LegsEndX() { return x + 64; }

	float LegsY() { return y + 73; }

	float x, y; // virsutinio kairiojo kampo koordinates
};
struct Bullet
{
    float x, y;
    bool active = false;

    void update()
    {
        if (active)
        {
            x += BULLET_SPEED;
            if (x > WINDOW_WIDTH) active = false;  // Deactivate bullet when it moves off screen
        }
    }

    void shoot(float startX, float startY)
    {
        if (!active)
        {
            x = startX;
            y = startY;
            active = true;
        }
    }
};

struct Enemy
{
    float x, y;
    bool active = true;
    float speed;
    int health = 3; // Enemy starts with 3 health points

    void update()
    {
        if (active)
        {
            x -= 3.0f;  // Move enemies from right to left
            if (x < 0) active = false;  // Deactivate enemy when it moves off screen
        }
    }
};
