#pragma once

#include "Entity.h"
#include "Player.h"

class Enemy : public Entity
{
    int m_SPEED = 3;
    int m_HEALTH = 100;

public:
    Enemy(const graphics::Window& win,
          const Player& player,
          const string_t& type) :
          Entity(win, type), m_HealthBar(win, "null"), m_player(player)
    {
        int roll = rand() % 4;
        int x = -20, y = -20;

        // left side
        if (roll == 1)
        {
            x = -40 + (rand() % 20);
            y = rand() % 600;
        }

        // top
        else if (roll == 2)
        {
            x = rand() % 800;
            y = -40 + (rand() % 20);
        }

        // bottom 
        else if (roll == 2)
        {
            x = rand() % 800;
            y = 640 - (rand() % 20);
        }

        // right side
        else if (roll == 2)
        {
            x = 840 - (rand() % 20);
            y = rand() % 600;
        }

        this->Move(vector2_t(x, y));
        this->Init();

        this->SetHealth(m_HEALTH);
        m_HealthBar.Move(vector2_t(x, y - 6));
    }

    bool Hit(const int damage)
    {
        this->SetHealth(m_health - damage);
        return this->IsDead();
    }

    bool IsDead() const
    {
        return m_health <= 0;
    }

    virtual void Update()
    {
        int dx = 0, dy = 0;

        if (m_player.GetX() < this->GetX())
            dx = -m_SPEED;
        else if (m_player.GetX() > this->GetX())
            dx = +m_SPEED;
        
        if (m_player.GetY() < this->GetY())
            dy = -m_SPEED;
        else if (m_player.GetY() > this->GetY())
            dy = +m_SPEED;

        this->Adjust(vector2_t(dx, dy));
        m_HealthBar.Adjust(vector2_t(dx, dy));
        this->Draw();
        m_HealthBar.Draw();
    }

    void SetHealth(int health)
    {
        m_health = health;

        SDL_Rect fill = {
            0, 0, 32 * (m_health / (float)m_HEALTH), 4
        };

        SDL_Surface* surf = SDL_CreateRGBSurface(SDL_SWSURFACE, m_Texture.GetWidth(), 4, 32,
                                                 rmask, gmask, bmask, amask);
        SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, 255, 0, 0));
        SDL_FillRect(surf, &fill, SDL_MapRGB(surf->format, 0, 255, 0));
        SDL_Texture* textPtr = SDL_CreateTextureFromSurface(
            m_Texture.GetWindow().GetRenderer(), surf
        );
        SDL_FreeSurface(surf);
        m_HealthBar.SetTexture(textPtr);
    }

private:
    const Player& m_player;
    Entity m_HealthBar;
    int m_health;
};
