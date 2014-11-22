#pragma once

#include "Entity.h"
#include "Player.h"

class Enemy : public Entity
{
public:
    int m_SPEED = 3;
    int m_HEALTH = 100;
    string_t m_enemy_type;

    Enemy(const graphics::Window& win,
          const Player& player,
          const string_t& type) :
          Entity(win, type), m_HealthBar(win, "null"),
          m_player(player), m_enabled(true), m_enemy_type(type)
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

    void Resize(const vector2_t& size)
    {
        Entity::Resize(size);
        this->SetHealth(m_health);
    }

    void Move(const vector2_t& pos)
    {
        Entity::Move(pos); 
        m_HealthBar.Move(vector2_t(pos.x, pos.y - 6));
    }

    bool IsDead() const
    {
        return m_health <= 0;
    }

    virtual void Update()
    {
        if (!m_enabled) return;

        vector2_t target = (m_target.x == 0) ?
                           vector2_t(m_player.GetX(), m_player.GetY()) :
                           m_target;

        int dx = 0, dy = 0;

        if (target.x < this->GetX())
            dx = -m_SPEED;
        else if (target.x > this->GetX())
            dx = +m_SPEED;

        if (target.y < this->GetY())
            dy = -m_SPEED;
        else if (target.y > this->GetY())
            dy = +m_SPEED;

        this->Adjust(vector2_t(dx, dy));
        m_HealthBar.Adjust(vector2_t(dx, dy));
        this->Draw();
        m_HealthBar.Draw();
    }

    virtual void Update(std::list<Bullet*>& bullets)
    {
        if (!m_enabled) return;

        if (m_type == "john" && rand() % 100 == 1)
        {
            printf("throwing parenthesis.\n");
            Bullet* bullet = new Bullet(m_Texture.GetWindow(), "scheme");
            bullet->ShootAt(this->GetPosition(), vector2_t(m_player.GetX(), m_player.GetY()));
            bullets.push_back(bullet);
        }

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
            0, 0, m_Texture.GetWidth() * (m_health / (float)m_HEALTH), 4
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

    void SetTarget(const vector2_t& v)
    {
        m_target = v;
    }

    unsigned GetHealth() const
    {
        return m_health;
    }

    void Enable()  { m_enabled = true; }
    void Disable() { m_enabled = false; }

protected:
    const Player& m_player;
    Entity m_HealthBar;
    vector2_t m_target;
    int m_health;
    bool m_enabled;
};
