#pragma once

#include "SDL2/SDL.h"
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

template<typename T>
struct Vector2
{
    T x, y;
    Vector2() : x((T)0), y((T)0) {}
    Vector2(const T& x, const T& y) : x(x), y(y) {}
};

typedef Vector2<int> vector2_t;
typedef Vector2<float> vector2f_t;
typedef std::string string_t;

namespace graphics
{
    class Window
    {
    public:
        Window() {}
        ~Window() {}

        bool Init(const vector2_t& size)
        {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

            m_Window = SDL_CreateWindow("BerklyWarz", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, size.x, size.y,
                                        SDL_WINDOW_SHOWN);
            if (!m_Window) return false;
            
            m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED);
            if (!m_Renderer) return false;

            return true;
        }

        void Clear(const unsigned r = 0, const unsigned g = 0,
                   const unsigned b = 0, const unsigned a = 0)
        {
            SDL_SetRenderDrawColor(m_Renderer, r, g, b, a);
            SDL_RenderClear(m_Renderer);
        }

        void Display()
        {
            SDL_RenderPresent(m_Renderer);
        }

        SDL_Renderer* GetRenderer() const
        {
            return m_Renderer;
        }

    private:
        SDL_Window* m_Window;
        SDL_Renderer* m_Renderer;
    };
}