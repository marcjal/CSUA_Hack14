#include <vector>
#include <string>
#include <map>

#include "Window.h"
#include "Texture.h"
#include "Entity.h"
#include "Player.h"

std::map<string_t, string_t> Entity::s_ENTITY_TYPES {
    { "john",   "textures/john.png"     },
    { "oski",   "textures/oski.png"     },
    { "player", "textures/player.png"   }
};

int main(int argc, char* argv[])
{
    static const int MAX_FPS = 60;

    graphics::Window window;
    window.Init({ 800, 600 });
    Player player(window);

    bool quit = false;
    uint32_t frame = 0;
    SDL_Event e;
    while (!quit)
    {
        frame = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

            player.HandleEvent(e);
        }

        window.Clear(255, 255, 255);
        player.Update();
        window.Display();

        uint32_t current_tick = SDL_GetTicks();
        if ((current_tick - frame) < 1000 / MAX_FPS)
        {
            uint32_t delay = (1000 / MAX_FPS) - (current_tick - frame);
            SDL_Delay(delay);
        }
            
    }

    return 0;
}