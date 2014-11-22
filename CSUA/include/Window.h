#pragma once

#include "SDL2/SDL.h"
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define rmask            0xff000000
#define gmask            0x00ff0000
#define bmask            0x0000ff00
#define amask            0x000000ff
#else
#define rmask            0x000000ff
#define gmask            0x0000ff00
#define bmask            0x00ff0000
#define amask            0xff000000
#endif

template<typename T>
struct Vector2
{
    T x, y;
    Vector2() : x((T)0), y((T)0) {}
    Vector2(const T& x, const T& y) : x(x), y(y) {}
};

struct rect_t
{
    rect_t(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
    rect_t() : x(0), y(0), w(0), h(0) {}

    int x, y, w, h;
};

typedef Vector2<int> vector2_t;
typedef Vector2<float> vector2f_t;
typedef std::string string_t;

static int g_OFFSET_X = 0;
static int g_OFFSET_Y = 0;

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

        SDL_Window* GetWindow() const
        {
            return m_Window;
        }

    private:
        SDL_Window* m_Window;
        SDL_Renderer* m_Renderer;
    };
}

static bool detect_collision(const rect_t& a, const rect_t& b)
{
    int top1, bottom1, right1, left1;
    int top2, bottom2, right2, left2;

    left1 = a.x;
    right1 = a.x + a.w;
    top1 = a.y;
    bottom1 = a.y + a.h;

    left2 = b.x;
    right2 = b.x + b.w;
    top2 = b.y;
    bottom2 = b.y + b.h;

    if (bottom1 <= top2)
        return false;
    if (top1 >= bottom2)
        return false;
    if (right1 <= left2)
        return false;
    if (left1 >= right2)
        return false;

    return true;
}

static SDL_Texture* SurfaceToTexture(const graphics::Window& Window,
                                     SDL_Surface* surf)
{
    SDL_Texture* textPtr = SDL_CreateTextureFromSurface(
        Window.GetRenderer(), surf
    );
    if (!textPtr) printf("Error: %s\n", SDL_GetError());
    SDL_FreeSurface(surf);
    return textPtr;
}