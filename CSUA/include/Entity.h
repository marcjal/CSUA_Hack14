#pragma once

#include <map>
#include "Window.h"
#include "Texture.h"

class Entity
{
    static std::map<string_t, string_t> s_ENTITY_TYPES;

public:
    Entity(const graphics::Window& window,
           const string_t& type) :
           m_type(type), m_Texture(window) {}

    virtual bool Init()
    {
        if (m_type == "null") return false;
        return m_Texture.Init(s_ENTITY_TYPES[m_type]);
    }

    template<typename T>
    void Adjust(const Vector2<T>& velocity)
    {
        m_Position.x += velocity.x;
        m_Position.y += velocity.y;
    }

    virtual void Move(const vector2_t& pos)
    {
        m_Position = pos;
    }

    virtual void Resize(const vector2_t& size)
    {
        m_Texture.Resize(size);
    }

    bool Draw() const
    {
        return m_Texture.DrawAt(m_Position);
    }

    bool Collides(const vector2_t& pos) const
    {
        return this->Collides(rect_t(pos.x, pos.y, 1, 1));
    }

    bool Collides(const rect_t& pos) const
    {
        return detect_collision(pos, rect_t(
            m_Position.x, m_Position.y,
            m_Texture.GetWidth(), m_Texture.GetHeight())
        );
    }

    void SetTexture(SDL_Texture* texture)
    {
        m_Texture.SetTexture(texture);
    }

    vector2_t GetCenter() const
    {
        return vector2_t(
            m_Position.x + m_Texture.GetWidth() / 2,
            m_Position.y + m_Texture.GetHeight() / 2
        );
    }
    
    unsigned GetWidth()  const { return m_Texture.GetWidth();  }
    unsigned GetHeight() const { return m_Texture.GetHeight(); }

    const vector2_t& GetPosition() const { return m_Position; }
    const int GetX() const { return this->GetPosition().x; }
    const int GetY() const { return this->GetPosition().y; }
    const graphics::Window& GetWindow() const { return m_Texture.GetWindow(); }
    SDL_Texture* GetRawTexture() const
    {
        return m_Texture.GetTexture();
    }

protected:
    graphics::Texture m_Texture;
    string_t m_type;
    vector2_t m_Position;
};