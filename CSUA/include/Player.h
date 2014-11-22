#pragma once

#include <list>
#include <algorithm>

#include "Entity.h"
#include "Bullet.h"

enum class PowerUp
{
    MULTI_SHOT = 0,
    BULLET_HELL,
    BOMB,
    NONE
};

class Enemy;
class Player
{
    int SPEED = 10;
    unsigned m_SHOT_DELAY = 1;
    unsigned m_DAMAGE = 25;

    enum class PlayerStates
    {
        e_MOVING = 0,
        e_SHOOTING = 1
    };

public:
    Player(graphics::Window& window) : m_Sprite(window, "player")
    {
        m_Sprite.Init();
        m_state = PlayerStates::e_MOVING;
        m_Sprite.Move(vector2_t(375, 275));
    }

    ~Player()
    {
        for (auto& i : m_Bullets)
        {
            delete i;
        }
    }

    int HandleEvent(const SDL_Event& evt)
    {
        int retval = 0;

        switch (evt.type)
        {
        case SDL_KEYDOWN:
            switch (evt.key.keysym.sym)
            {
            case SDLK_w:
            case SDLK_UP:
                m_velocity.y = -Player::SPEED;
                break;

            case SDLK_s:
            case SDLK_DOWN:
                m_velocity.y = Player::SPEED;
                break;
            
            case SDLK_a:
            case SDLK_LEFT:
                m_velocity.x = -Player::SPEED;
                break;
            
            case SDLK_d:
            case SDLK_RIGHT:
                m_velocity.x = Player::SPEED;
                break;
            }
            break;

        case SDL_KEYUP:
            switch (evt.key.keysym.sym)
            {
            case SDLK_w:
            case SDLK_UP:
            case SDLK_s:
            case SDLK_DOWN:
                m_velocity.y = 0;
                break;

            case SDLK_a:
            case SDLK_LEFT:
            case SDLK_d:
            case SDLK_RIGHT:
                m_velocity.x = 0;
                break;

            case SDLK_b:
                if (this->HasPowerup(PowerUp::BOMB))
                {
                    this->RemovePowerup(PowerUp::BOMB);
                    retval = 1;
                }

                break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (evt.button.button == SDL_BUTTON_LEFT)
            {
                m_state = PlayerStates::e_SHOOTING;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (evt.button.button == SDL_BUTTON_LEFT)
            {
                m_state = PlayerStates::e_MOVING;
            }
            break;

        default:
            break;
        }

        return retval;
    }

    void Move(const vector2_t& pos)
    {
        m_Sprite.Move(pos);
    }

    void Shoot(const vector2_t& pos)
    {
        if (this->HasPowerup(PowerUp::MULTI_SHOT))
        {
            vector2_t target = pos;
            Bullet* left = new Bullet(m_Sprite.GetWindow());
            Bullet* rite = new Bullet(m_Sprite.GetWindow());

            target.Rotate(5);
            left->ShootAt(m_Sprite.GetCenter(), target);
            target.Rotate(-10);
            rite->ShootAt(m_Sprite.GetCenter(), target);

            m_Bullets.emplace_back(left);
            m_Bullets.emplace_back(rite);
        }

        Bullet* bullet = new Bullet(m_Sprite.GetWindow());
        bullet->ShootAt(m_Sprite.GetCenter(), pos);
        m_Bullets.emplace_back(bullet);
    }

    bool Update()
    {
        if (m_state == PlayerStates::e_SHOOTING && m_delay++ > m_SHOT_DELAY)
        {
            int x = 0, y = 0;
            SDL_GetMouseState(&x, &y);
            this->Shoot({ x, y });
            m_delay = 0;
        }

        m_Sprite.Adjust(m_velocity);

        for (auto it  = m_Bullets.begin();
                  it != m_Bullets.end(); )
        {
            Bullet* bPtr = *it;
            Bullet& bullet = *bPtr;

            if (bullet.GetPosition().x < -10 ||
                bullet.GetPosition().x > 810 ||
                bullet.GetPosition().y < -10 ||
                bullet.GetPosition().y > 610)
            {
                delete bPtr;
                it = m_Bullets.erase(it);
            }
            else
            {
                ++it;
            }
        }

        for (auto& i : m_Bullets)
        {
            i->Update();
            i->Draw();
        }
        return m_Sprite.Draw();
    }

    void AddPowerUp(const PowerUp& p)
    {
        m_Powerups.emplace_back(p);
    }

    rect_t GetRect() const
    {
        return rect_t(this->GetX(), this->GetY(),
                      m_Sprite.GetWidth(), m_Sprite.GetHeight());
    }

    std::list<Bullet*>& GetBullets()
    {
        return m_Bullets;
    }

    unsigned GetDamage() const
    {
        return m_DAMAGE;
    }

    bool HasPowerup(const PowerUp& p) const
    {
        return m_Powerups.end() != std::find(m_Powerups.begin(), m_Powerups.end(), p);
    }

    void RemovePowerup(const PowerUp& p)
    {
        for (auto it  = m_Powerups.begin();
                  it != m_Powerups.end(); ++it)
        {
            if (*it == p)
            {
                m_Powerups.erase(it);
                break;
            }
        }
    }

    const int GetX() const { return m_Sprite.GetPosition().x; }
    const int GetY() const { return m_Sprite.GetPosition().y; }

    friend class Enemy;

private:
    std::vector<PowerUp> m_Powerups;
    std::list<Bullet*> m_Bullets;
    Entity m_Sprite;
    vector2_t m_velocity;
    PlayerStates m_state;
    int m_delay;
};