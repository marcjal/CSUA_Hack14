#pragma once

#include "Window.h"
#include "SDL2/SDL_image.h"
#pragma comment(lib, "SDL2_image.lib")

namespace graphics
{
    class Texture
    {
    public:
        Texture(const Window& Window) : m_Window(Window) {}

        bool Init(const string_t& filename)
        {
            m_filename = filename;
            m_Texture = IMG_LoadTexture(m_Window.GetRenderer(), filename.c_str());
            if (m_Texture == nullptr) return false;
            SDL_QueryTexture(m_Texture, nullptr, nullptr, &m_size.x, &m_size.y);

            return true;
        }

        bool DrawAt(const vector2_t& position) const
        {
            SDL_Rect rect = {
                position.x, position.y,
                m_size.x, m_size.y
            };

            SDL_Renderer* rendererPtr = m_Window.GetRenderer();
            SDL_RenderCopy(rendererPtr, m_Texture, nullptr, &rect);
            return true;
        }

        const vector2_t& GetSize() const
        {
            return m_size;
        }

        unsigned GetWidth()  const { return m_size.x; }
        unsigned GetHeight() const { return m_size.y; }

        const graphics::Window& GetWindow() const { return m_Window; }

    private:
        const graphics::Window& m_Window;
        SDL_Texture* m_Texture;
        vector2_t m_size;
        string_t m_filename;
    };
}