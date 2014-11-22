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
        int incr = 0;
        vector2_t last_john;
        bool vert = false;

        for (auto& i : *m_Swarm)
        {
            Enemy& e = *i;
            if (e.m_enemy_type == "john")
            {
                if (last_john.x == 0)
                {
                    last_john.x = m_player.GetX() - 100;
                    last_john.y = m_player.GetY() - 100;

                    e.SetTarget(last_john);
                }
                else
                {
                    last_john.x += 100;
                    if (last_john.x > 700)
                    {
                        vert = true;
                    }

                    if (vert)
                    {
                        last_john.y += 100;
                        if (last_john.y >= m_player.GetY() + 100)
                        {
                            vert = false;
                        }
                    }

                    e.SetTarget(last_john);
                }
            }
        }
        
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