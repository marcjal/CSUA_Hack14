#pragma once

#include "Entity.h"
#include "Bullet.h"

class Player
{
    const int SPEED = 5;

public:
    Player(graphics::Window& window) : m_Sprite(window, "player")
    {
        m_Sprite.Init();
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
        printf("Shooting from %d,%d at %d,%d\n", m_Sprite.GetPosition().x, m_Sprite.GetPosition().y, pos.x, pos.y);
        m_Bullets.emplace_back(bullet);
    }

    bool Update()
    {
        m_Sprite.Adjust(m_velocity);
        for (auto& i : m_Bullets)
        {
            i->Update();
            i->Draw();
        }
        return m_Sprite.Draw();
    }

private:
    std::vector<Bullet*> m_Bullets;
    Entity m_Sprite;
    vector2_t m_velocity;
};