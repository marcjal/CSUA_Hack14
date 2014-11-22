#pragma once

#include "Entity.h"

class Bullet : public Entity
{
public:
    Bullet(const graphics::Window& window) : Entity(window, "bullet")
    {

    }

    ~Bullet();
};