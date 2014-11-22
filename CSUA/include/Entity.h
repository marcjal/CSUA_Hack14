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
           m_type(type), m_Texture(window)
    {}

    bool Init()
    {
        return m_Texture.Init(s_ENTITY_TYPES[m_type]);
    }

    void Adjust(const vector2_t& velocity)
    {
        m_Position.x += velocity.x;
        m_Position.y += velocity.y;
    }

    void Move(const vector2_t& pos)
    {
        m_Position = pos;
    }

    bool Draw() const
    {
        return m_Texture.DrawAt(m_Position);
    }

private:
    graphics::Texture m_Texture;
    string_t m_type;
    vector2_t m_Position;
};