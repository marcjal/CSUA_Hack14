#pragma once

#include "Entity.h"

class Particle : public Entity
{
public:
    Particle(const graphics::Window& window,
             const vector2_t& start) : Entity(window, "null")
    {
        vector2_t offset(rand() % 10, rand() % 10);
        
        if (rand() % 1 == 0)
            offset.x *= -1;
        
        if (rand() % 1 == 0)
            offset.y *= -1; 
        
        this->Move(start);
    }

    ~Particle() = default;

    virtual bool Init()
    {
        m_color = SDL_Color { rand() % 255, rand() % 255, rand() % 255 };
        
        bool neg = (rand() % 2) == 1;
        if (neg)
        {
            m_velocity.x = -1 - (rand() % 10);
        }
        else
        {
            m_velocity.x = 1 + (rand() % 10);
        }

        neg = (rand() % 2) == 1;
        if (neg)
        {
            m_velocity.y = -1 - (rand() % 10);
        }
        else
        {
            m_velocity.y = 1 + (rand() % 10);
        }

        m_lifetime = (60 * 3) + (rand() % 120);

        SDL_Surface* surf = SDL_CreateRGBSurface(SDL_SWSURFACE, 4, 4, 32,
                                                 rmask, gmask, bmask, amask);
        SDL_FillRect(surf, nullptr, SDL_MapRGB(
            surf->format, m_color.r, m_color.g, m_color.b)
        );
        
        SDL_Texture* textPtr = SurfaceToTexture(m_Texture.GetWindow(), surf);
        this->SetTexture(textPtr);
        return true;
    }

    virtual void Update()
    {
        this->Adjust(m_velocity);
        --m_lifetime;
        this->Draw();
    }

    bool Dead() const
    {
        return m_lifetime <= 0;
    }

private:
    SDL_Color m_color;
    vector2f_t m_velocity;
    int m_lifetime;
};

class ParticleEmitter
{
public:
    ParticleEmitter(const graphics::Window& window) : m_Window(window) {}

    ~ParticleEmitter()
    {
        for (auto& i : m_Particles)
        {
            delete i;
        }
    }

    void Explode(const vector2_t& place, unsigned count=50)
    {
        count = count + (rand() % 100);

        for (int i = 0; i < count; ++i)
        {
            Particle* part = new Particle(m_Window, place);
            part->Init();
            m_Particles.push_back(part);
        }
    }

    void Update()
    {
        auto end = m_Particles.end();
        for (auto it = m_Particles.begin(); it != end; )
        {
            Particle* ptr = *it;
            Particle& part = *ptr;
            part.Update();

            if (part.GetX() < -10 || part.GetY() < -10  ||
                part.GetX() > 810 || part.GetY() >= 610 || part.Dead())
            {
                delete ptr;
                it = m_Particles.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

private:
    const graphics::Window& m_Window;
    std::list<Particle*> m_Particles;
};