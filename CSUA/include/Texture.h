#pragma once

#include "Window.h"
#include "SDL2/SDL_image.h"
#pragma comment(lib, "SDL2_image.lib")

namespace graphics
{
    class Texture
    {
    public:
        Texture(const Window& Window) : m_Window(Window), m_Texture(nullptr) {}

        ~Texture()
        {
            if (m_Texture)
            {
                SDL_DestroyTexture(m_Texture);
            }
        }

        bool Init(const string_t& filename)
        {
            m_filename = filename;
            SDL_Texture* textPtr = IMG_LoadTexture(m_Window.GetRenderer(), filename.c_str());
            if (textPtr == nullptr) return false;
            this->SetTexture(textPtr);
            return true;
        }

        bool DrawAt(const vector2_t& position) const
        {
            if (!m_Texture) return false;

            SDL_Rect rect = {
                position.x + g_OFFSET_X,
                position.y + g_OFFSET_Y,
                m_size.x, m_size.y
            };

            SDL_Renderer* rendererPtr = m_Window.GetRenderer();
            if (SDL_RenderCopy(rendererPtr, m_Texture, nullptr, &rect) < 0)
            {
                printf("error: %s\n", SDL_GetError());
            }

            return true;
        }

        void Resize(const vector2_t& size)
        {
            m_size = size;
        }

        void SetTexture(SDL_Texture* texture)
        {
            if (texture)
            {
                SDL_DestroyTexture(m_Texture);
                m_Texture = nullptr;
            }

            m_Texture = texture;
            SDL_QueryTexture(m_Texture, nullptr, nullptr, &m_size.x, &m_size.y);
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