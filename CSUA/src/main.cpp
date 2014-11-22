#include <vector>
#include <string>
#include <map>
#include <ctime>

#include "SDL2/SDL_ttf.h"
#pragma comment(lib, "SDL2_ttf.lib")

#include "SDL2/SDL_mixer.h"
#pragma comment(lib, "SDL2_mixer.lib")

#include "Window.h"
#include "Texture.h"
#include "Entity.h"
#include "Player.h"
#include "BossEnemy.h"
#include "Emitter.h"

std::map<string_t, string_t> Entity::s_ENTITY_TYPES {
    { "john",   "textures/john.png"     },
    { "dan",    "textures/dan.png"      },
    { "hilf", "textures/hilf.png"       },
    { "oski",   "textures/oski.png"     },
    { "player", "textures/player.png"   },
    { "bullet", "textures/bullet.png"   },
    { "bg",     "textures/bg.png"       }  
};

void spawn_enemy(const graphics::Window& window,
                 const Player& player,
                 std::list<Enemy*>& enemies, const string_t& type, int x, int y)
{
    Enemy* newest = new Enemy(window, player, type);
    newest->Move(vector2_t(x, y));
    newest->SetHealth(newest->m_HEALTH);
    enemies.push_back(newest);
}

struct Level
{
    unsigned to_spawn;
};

int main(int argc, char* argv[])
{
    srand(static_cast<unsigned>(time(nullptr)));
    static const int MAX_FPS = 60;

    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44000, MIX_DEFAULT_FORMAT, 2, 1024);
    
    printf("volume is %d\n", Mix_Volume(-1, -1));

    graphics::Window window;
    window.Init({ 800, 600 });
    Player player(window);
    Entity background(window, "bg");
    Entity overlay(window, "null");

    background.Init();
    background.Move(vector2_t(-10, -10));
    background.Resize(vector2_t(850, 650));

    std::list<Enemy*> enemies;

    ParticleEmitter particles(window);

    bool quit = false;
    uint32_t frame = 0;
    SDL_Event e;
    SDL_Color clear_color = { 255, 255, 255, 255 };

    uint32_t kill_time = 0;
    uint32_t kill_count = 0;
    uint32_t total_kills = 0;

    TTF_Font* font = TTF_OpenFont("comic.ttf", 20);

    SDL_Surface* surf = TTF_RenderText_Blended(font, "DOUBLE KILL", {
        rand() % 255, rand() % 255, rand() % 255
    });
    if (!surf) printf("error: %s\n", TTF_GetError());
    Entity DoubleText(window, "null");
    DoubleText.SetTexture(SurfaceToTexture(window, surf));
    DoubleText.Move(vector2_t(300, 200));

    surf = TTF_RenderText_Blended(font, "TRIPLE KILL", {
        rand() % 255, rand() % 255, rand() % 255
    });
    if (!surf) printf("error: %s\n", TTF_GetError());
    Entity TripleText(window, "null");
    TripleText.SetTexture(SurfaceToTexture(window, surf));
    TripleText.Move(vector2_t(300, 200));

    surf = TTF_RenderText_Blended(font, "M-M-M-MULTI KILL", {
        rand() % 255, rand() % 255, rand() % 255
    });
    if (!surf) printf("error: %s\n", TTF_GetError());
    Entity MultiText(window, "null");
    MultiText.SetTexture(SurfaceToTexture(window, surf));
    MultiText.Move(vector2_t(300, 200));

    SDL_FreeSurface(surf);

    SDL_Texture* overlayText = background.GetRawTexture();
    SDL_SetTextureBlendMode(overlayText, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(overlayText, 200);

    Mix_Music* music = Mix_LoadMUS("skril.mp3");
    Mix_PlayMusic(music, -1);
    Mix_Volume(-1, 24);

    Enemy* BossManPtr = new BossEnemy(window, player, "stanford");
    Enemy& BossMan = *BossManPtr;
    BossMan.m_HEALTH *= 10; 
    BossMan.m_SPEED = 1;
    BossMan.Move(vector2_t(1000, 200));
    BossMan.Resize(vector2_t(200, 200));
    BossMan.SetHealth(BossMan.m_HEALTH);
    BossMan.Disable();

    enemies.push_back(BossManPtr);

    int time_till_build = 3745;
    int time_till_insanity = 4535;
    int count = 0;

    char* ENEMY_SEED[] = {
        "oski",
        "john",
        "hilf",
        "dan"
    };
    
    Level Level1 = { 15 };
    
    int stage = 0;
    int enemies_left = Level1.to_spawn;
    int enemies_spawned = 0;

    while (!quit)
    {
        // printf("count: %d\n", count++);

        time_till_insanity--;
        time_till_build--;

        frame = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                quit = true;

            player.HandleEvent(e);
        }

        // spawn enemies
        if (enemies_left < 0)
        {
            printf("upping stage\n");
            stage++;
            enemies_spawned = 0;
            break;
        }
        
        switch (stage)
        {
        case 0:
            if (enemies_spawned < Level1.to_spawn && frame % 180 == 0)
            {
                spawn_enemy(window, player, enemies, "oski", -10, -10);
                spawn_enemy(window, player, enemies, "oski", 810, -10);
                spawn_enemy(window, player, enemies, "oski", 810, 610);
                spawn_enemy(window, player, enemies, "oski", -10, 610);
                enemies_spawned += 4;
            }

            break;

        case 1:
            if (enemies_spawned < Level1.to_spawn && frame % 180 == 0)
            {
                spawn_enemy(window, player, enemies, "oski", -10, -10);
                spawn_enemy(window, player, enemies, "oski", 810, -10);
                spawn_enemy(window, player, enemies, "oski", 810, 610);
                spawn_enemy(window, player, enemies, "oski", -10, 610);
                enemies_spawned += 4;
            }

            break;
        }

        /*if (rand() % 100 == 1 && enemies.size() <= 10)
        {
            char* enemy = ENEMY_SEED[rand() % 4];
            printf("loading %s\n", enemy);
            enemies.emplace_back(new Enemy(window, player, enemy));
        }*/

        auto& bullets = player.GetBullets();
        for (auto bit = bullets.begin(); bit != bullets.end();)
        {
            auto& b = *(*bit);
            bool bkill = false;

            for (auto it = enemies.begin(); it != enemies.end();)
            {
                auto& e = *(*it);

                vector2_t pos = b.GetPosition();
                bool collides = e.Collides(pos);

                if (collides)
                {
                    bit = bullets.erase(bit);
                    bkill = true;
                }

                if (collides && e.Hit(player.GetDamage()))
                {
                    it = enemies.erase(it);
                    particles.Explode(pos);

                    total_kills++;
                    ++kill_count;
                    kill_time = 0;
                    --enemies_left;
                }
                else
                {
                    ++it;
                }

                if (bkill) break;
            }

            if (!bkill) ++bit;
        }

        if (kill_time++ > 60)
        {
            DoubleText.Resize(vector2_t(128, 32));
            TripleText.Resize(vector2_t(128, 32));
            MultiText.Resize(vector2_t(128, 32));
            kill_count = 0;
            kill_time = 0;
        }

        if (time_till_build <= 0 && kill_count >= 2)
        {
            clear_color = SDL_Color { rand() % 255, rand() % 255, rand() % 255 };
        }

        window.Clear(clear_color.r, clear_color.g, clear_color.b);
        background.Draw();

        //printf("tti: %d\n", time_till_insanity);

        if (time_till_insanity <= 0)
        {
            BossMan.Enable();

            if (kill_count == 1)
            {
                g_OFFSET_X = ((rand() % 1 == 1) ? -1 : 1) * (rand() % 5);
                g_OFFSET_Y = ((rand() % 1 == 1) ? -1 : 1) * (rand() % 5);
            }
            else
            {
                g_OFFSET_X = 0;
                g_OFFSET_Y = 0;
            }

            if (kill_count == 2)
            {
                vector2_t old_pos = DoubleText.GetPosition();

                DoubleText.Resize(vector2_t(
                    DoubleText.GetWidth() + 2,
                    DoubleText.GetHeight() + 2
                    ));
                DoubleText.Draw();

                DoubleText.Adjust(vector2_t(
                    100 + (rand() % 300),
                    100 + (rand() % 200)));
                DoubleText.Draw();

                DoubleText.Adjust(vector2_t(
                    -100 - (rand() % 200),
                    100 + (rand() % 200)));
                DoubleText.Draw();

                DoubleText.Adjust(vector2_t(
                    100 + (rand() % 200),
                    -100 - (rand() % 200)));
                DoubleText.Draw();

                DoubleText.Adjust(vector2_t(
                    -100 - (rand() % 200),
                    -100 - (rand() % 200)));
                DoubleText.Draw();

                DoubleText.Move(old_pos);
            }
            else if (kill_count == 3)
            {
                vector2_t old_pos = TripleText.GetPosition();

                TripleText.Resize(vector2_t(
                    TripleText.GetWidth() + 2,
                    TripleText.GetHeight() + 2
                    ));
                TripleText.Draw();

                TripleText.Adjust(vector2_t(
                    100 + (rand() % 200),
                    100 + (rand() % 200)));
                TripleText.Draw();

                TripleText.Adjust(vector2_t(
                    -100 - (rand() % 200),
                    100 + (rand() % 200)));
                TripleText.Draw();

                TripleText.Adjust(vector2_t(
                    100 + (rand() % 200),
                    -100 - (rand() % 200)));
                TripleText.Draw();

                TripleText.Adjust(vector2_t(
                    -100 - (rand() % 200),
                    -100 - (rand() % 200)));
                TripleText.Draw();

                TripleText.Move(old_pos);
            }
            else if (kill_count > 3)
            {
                vector2_t old_pos = MultiText.GetPosition();

                MultiText.Resize(vector2_t(
                    MultiText.GetWidth() + 2,
                    MultiText.GetHeight() + 2
                    ));
                MultiText.Draw();

                MultiText.Adjust(vector2_t(
                    100 + (rand() % 200),
                    100 + (rand() % 200)));
                MultiText.Draw();

                MultiText.Adjust(vector2_t(
                    -100 - (rand() % 200),
                    100 + (rand() % 200)));
                MultiText.Draw();

                MultiText.Adjust(vector2_t(
                    100 + (rand() % 200),
                    -100 - (rand() % 200)));
                MultiText.Draw();

                MultiText.Adjust(vector2_t(
                    -100 - (rand() % 200),
                    -100 - (rand() % 200)));
                MultiText.Draw();

                MultiText.Move(old_pos);
            }
        }

        player.Update();

        for (auto& i : enemies)
            i->Update();
        BossMan.Update();

        particles.Update();
        window.Display();

        clear_color = SDL_Color { 255, 255, 255 };

        uint32_t current_tick = SDL_GetTicks();
        if ((current_tick - frame) < 1000 / MAX_FPS)
        {
            uint32_t delay = (1000 / MAX_FPS) - (current_tick - frame);
            SDL_Delay(delay);
        }   
    }

    return 0;
}