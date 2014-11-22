#pragma once

#include "Entity.h"

class Bullet : public Entity
{
    const int SPEED = 10;

public:
    Bullet(const graphics::Window& window) : Entity(window, "bullet")
    {
        this->Init();
    }

    ~Bullet() = default;

    void ShootAt(const vector2_t& start,
                 const vector2_t& end)
    {
        float magnitude = sqrt(
            (start.x - end.x) * (start.x - end.x) +
            (start.y - end.y) * (start.y - end.y)
        );

        m_velocity.x = Bullet::SPEED * (end.x - start.x) / magnitude;
        m_velocity.y = Bullet::SPEED * (end.y - start.y) / magnitude;
        this->Move(start);
    }

    void Update()
    {
        this->Adjust(m_velocity);
    }

private:
    vector2f_t m_velocity;
};