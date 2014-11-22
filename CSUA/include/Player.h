#pragma once

#include "Entity.h"

class Player
{
    const int SPEED = 5;

public:
    Player(graphics::Window& window) : m_Sprite(window, "player")
    {
        m_Sprite.Init();
    }

    ~Player()
    {}

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

    bool Update()
    {
        m_Sprite.Adjust(m_velocity);
        return m_Sprite.Draw();
    }

private:
    Entity m_Sprite;
    vector2_t m_velocity;
};