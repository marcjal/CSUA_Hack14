#pragma once

#include "Enemy.h"

class BossEnemy : public Enemy
{
public:
    BossEnemy(const graphics::Window& window,
              const Player& player,
              const string_t& type) : Enemy(window, player, type)
    {}

    void SetSwarm(const std::list<Enemy*>& enemies)
    {
        m_Swarm = &enemies;
    }

    virtual void Update()
    {
        if (!m_enabled) return;

        bool done = false;
        
        if (abs(this->GetPosition().x - 400) <= m_SPEED)
        {
            this->Move(vector2_t(400, this->GetPosition().y));
            done = true;
        }

        if (abs(this->GetPosition().y - 300) <= m_SPEED)
        {
            this->Move(vector2_t(this->GetPosition().x, 300));
            done = true;
        }

        if (done) return;
        Enemy::Update();
    }

private:
    const std::list<Enemy*>* m_Swarm;
};