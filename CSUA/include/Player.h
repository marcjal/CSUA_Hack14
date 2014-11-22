#pragma once

#include <list>

#include "Entity.h"
#include "Bullet.h"

class Player
{
    int SPEED = 5;
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
    }

    ~Player()
    {
        for (auto& i : m_Bullets)
        {
            delete i;
        }
    }

    bool HandleEvent(const SDL_Event& evt)
    {
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

        return true;
    }

    void Move(const vector2_t& pos)
    {
        m_Sprite.Move(pos);
    }

    void Shoot(const vector2_t& pos)
    {
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

    std::list<Bullet*>& GetBullets()
    {
        return m_Bullets;
    }

    unsigned GetDamage() const
    {
        return m_DAMAGE;
    }

    const int GetX() const { return m_Sprite.GetPosition().x; }
    const int GetY() const { return m_Sprite.GetPosition().y; }

private:
    std::list<Bullet*> m_Bullets;
    Entity m_Sprite;
    vector2_t m_velocity;
    PlayerStates m_state;
    int m_delay;
};