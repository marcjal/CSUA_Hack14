#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <sstream>
#include <iomanip>

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
    { "hilf",   "textures/hilf.png"     },
    { "oski",   "textures/oski.png"     },
    { "player", "textures/player.png"   },
    { "bullet", "textures/bullet.png"   },
    { "scheme", "textures/parens.png"   },
    { "bg",     "textures/bg.png"       },  
    { "sf",     "textures/stanford.png" },
    { "multi",  "textures/gun.png"      },
    { "explode","textures/explode.png"  },
    { "bomb",   "textures/bomb.png"     },
    { "tree",   "textures/tree.png"     }
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

class FontStream
{
public:
    template<typename T>
    FontStream& operator<<(const T& data)
    {
        m_stream << data;
        return (*this);
    }

    SDL_Texture* Render(TTF_Font* font, const graphics::Window& window, SDL_Color color)
    {
        SDL_Surface* surf = TTF_RenderText_Blended(font, m_stream.str().c_str(), color);
        return SurfaceToTexture(window, surf);
    }

    void Clear()
    {
        m_stream.str(std::string());
    }

private:
    std::stringstream m_stream;
};

int main(int argc, char* argv[])
{
    srand(static_cast<unsigned>(time(nullptr)));
    static const int MAX_FPS = 60;

    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);

    graphics::Window window;
    window.Init({ 800, 600 });
    Player player(window);
    Entity background(window, "bg");
    Entity overlay(window, "null");

    background.Init();
    background.Move(vector2_t(-10, -10));
    background.Resize(vector2_t(850, 650));

    std::list<Enemy*> enemies;
    std::list<Bullet*> parentheses;

    ParticleEmitter particles(window);

    bool quit = false;
    uint32_t frame = 0;
    SDL_Event e;
    SDL_Color clear_color = { 255, 255, 255, 255 };

    uint32_t kill_time = 0;
    uint32_t kill_count = 0;
    uint32_t total_kills = 0;

    TTF_Font* font = TTF_OpenFont("comic.ttf", 20);
    TTF_Font* uiFont = TTF_OpenFont("comic.ttf", 18);

    const unsigned int UI_LINE_HEIGHT = TTF_FontHeight(uiFont);

    Entity powerupUI(window, "null");
    Entity killUI(window, "null");
    Entity scoreUI(window, "null");

    FontStream fs;

    fs << "PowerUps     0 bombs | no upgrades";
    powerupUI.SetTexture(fs.Render(uiFont, window, { 255, 255, 255 }));
    fs.Clear();

    fs << "0 kills";
    killUI.SetTexture(fs.Render(uiFont, window, { 255, 255, 255 }));
    fs.Clear();

    fs << "4.0 GPA";
    scoreUI.SetTexture(fs.Render(uiFont, window, { 255, 255, 255 }));
    fs.Clear();

    SDL_Surface* surf = TTF_RenderText_Blended(font, "DOUBLE KILL", {
        150 + (rand() % 105), 150 + (rand() % 105), 150 + (rand() % 105)
    });
    if (!surf) printf("error: %s\n", TTF_GetError());
    Entity DoubleText(window, "null");
    DoubleText.SetTexture(SurfaceToTexture(window, surf));
    DoubleText.Move(vector2_t(300, 200));

    surf = TTF_RenderText_Blended(font, "TRIPLE KILL", {
        150 + (rand() % 105), 150 + (rand() % 105), 150 + (rand() % 105)
    });
    if (!surf) printf("error: %s\n", TTF_GetError());
    Entity TripleText(window, "null");
    TripleText.SetTexture(SurfaceToTexture(window, surf));
    TripleText.Move(vector2_t(300, 200));

    surf = TTF_RenderText_Blended(font, "M-M-M-MULTI KILL", {
        150 + (rand() % 105), 150 + (rand() % 105), 150 + (rand() % 105)
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

    printf("volume is %d\n", Mix_Volume(-1, -1));

    BossEnemy* BossManPtr = new BossEnemy(window, player, "sf");
    BossEnemy& BossMan = *BossManPtr;
    BossMan.m_HEALTH *= 50;
    BossMan.m_SPEED = 1;
    BossMan.Move(vector2_t(800, 200));
    BossMan.Resize(vector2_t(200, 200));
    BossMan.SetHealth(BossMan.m_HEALTH);
    BossMan.SetSwarm(enemies);
    BossMan.Disable();

    enemies.push_back(BossManPtr);

    int time_till_build = 3869;
    int time_till_insanity = 4700;
    int count = 0;

    char* ENEMY_SEED[] = {
        "oski",
        "john",
        "hilf",
        "dan",
        "tree"
    };

    std::map<PowerUp, Entity*> POWER_UPS = {
        { PowerUp::MULTI_SHOT,  new Entity(window, "multi") },
        { PowerUp::BOMB,        new Entity(window, "bomb")  }
    };
    
    std::vector<Level> levels = {
        { 15 },
        { 5  },
        { 4  },
        { -1 }
    };

    float gpa = 4.0f;
    int stage = 0;
    int enemies_left = levels[0].to_spawn;
    int enemies_spawned = 0;

    for (auto& i : POWER_UPS)
    {
        i.second->Init();
    }

    Entity explosion(window, "explode");
    explosion.Init();
    bool exploding = false;

    Entity* currentPowerup = nullptr;
    PowerUp currentPowerupType = PowerUp::NONE;

    while (!quit)
    {
        printf("frame count: %d\n", count);
        printf("enemies left: %d\n", enemies_left);

        ++count;
        time_till_insanity--;
        time_till_build--;

        frame = SDL_GetTicks();

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                quit = true;

            int retval = player.HandleEvent(e);

            // bomb triggered
            if (retval == 1 && !exploding)
            {
                exploding = true;
                explosion.Resize(vector2_t(64, 60));
                explosion.Move(vector2_t(player.GetX(), player.GetY()));
            }
        }

        // spawn enemies
        bool trigger = (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_G] != 0);
        if (enemies_left <= 0 && stage >= 0 && stage < levels.size() && levels[stage].to_spawn != -1)
        {
            printf("upping stage\n");
            enemies_spawned = 0;
            enemies_left = levels[++stage].to_spawn;
            trigger = true;
        }
        
        switch (stage)
        {
        case 0:
            if (enemies_spawned < levels[0].to_spawn && (trigger || frame % 120 == 0))
            {
                spawn_enemy(window, player, enemies, "oski", -10, -10);
                spawn_enemy(window, player, enemies, "oski", 810, -10);
                spawn_enemy(window, player, enemies, "oski", 810, 610);
                spawn_enemy(window, player, enemies, "oski", -10, 610);
                enemies_spawned += 4;
            }
            break;
        
        // hilf rush
        case 1:
            if (enemies_spawned < levels[1].to_spawn && (trigger || frame % 180 == 0))
            {
                spawn_enemy(window, player, enemies, "hilf", 25,  -10);
                spawn_enemy(window, player, enemies, "hilf", 225, -30);
                spawn_enemy(window, player, enemies, "hilf", 425, -10);
                spawn_enemy(window, player, enemies, "hilf", 625, -20);
                enemies_spawned += 4;
            }

            break;

        // john rekt
        case 2:
            if (enemies_spawned < levels[2].to_spawn && (trigger || frame % 180 == 0))
            {
                switch (enemies_spawned)
                {
                case 0:
                    spawn_enemy(window, player, enemies, "john", -20, 275);
                    enemies_spawned++;
                    break;

                case 1:
                    spawn_enemy(window, player, enemies, "john", 400, -20);
                    spawn_enemy(window, player, enemies, "john", 400, 650);
                    enemies_spawned += 2; 
                    break;

                case 3:
                    spawn_enemy(window, player, enemies, "john", -20, 275);
                    enemies_spawned++;
                    break;

                default:
                    spawn_enemy(window, player, enemies, "john", 625, -20);
                    enemies_spawned++; 
                    break;
                }
            }

            break;

        // stan4rd
        case 69:
            for (int i = 0; i < 600; i += 50)
            {
                spawn_enemy(window, player, enemies, "tree", -10, i);
                spawn_enemy(window, player, enemies, "tree", 810, i + 25);
            }
            stage = 70;

            break;

        // hard
        case 70:
            if (rand() % 60 == 1 && enemies.size() <= 10)
            {
                char* enemy = ENEMY_SEED[rand() % 5];
                enemies.emplace_back(new Enemy(window, player, enemy));
            }
            break;

        default:
            if (rand() % 100 == 1 && enemies.size() <= 10)
            {
                char* enemy = ENEMY_SEED[rand() % 4];
                enemies.emplace_back(new Enemy(window, player, enemy));
            }
            break;
        }

        if (trigger)
        {
            char* enemy = ENEMY_SEED[rand() % 5];
            enemies.emplace_back(new Enemy(window, player, enemy));
        }

        auto& bullets = player.GetBullets();
        for (auto bit = bullets.begin(); bit != bullets.end();)
        {
            auto& b = *(*bit);
            bool bkill = false;

            for (auto it = enemies.begin(); it != enemies.end();)
            {
                auto& e = *(*it);

                vector2_t pos = b.GetPosition();
                vector2_t center = b.GetCenter();
                bool collides = e.Collides(pos);

                if (collides)
                {
                    bit = bullets.erase(bit);
                    bkill = true;
                }

                if (collides && e.Hit(player.GetDamage()))
                {
                    if (&e == BossManPtr)
                    {
                        particles.Explode(center, 500);
                        stage = 69;
                    }

                    it = enemies.erase(it);
                    particles.Explode(center);

                    total_kills++;
                    ++kill_count;
                    kill_time = 0;
                    --enemies_left;

                    if (time_till_insanity <= 0)
                    {
                        g_OFFSET_X = ((rand() % 1 == 1) ? -1 : 1) * (rand() % 5);
                        g_OFFSET_Y = ((rand() % 1 == 1) ? -1 : 1) * (rand() % 5);
                    }
                    else
                    {
                        g_OFFSET_X = 0;
                        g_OFFSET_Y = 0;
                    }
                }
                else
                {
                    ++it;
                }

                if (bkill) break;
            }

            if (!bkill) ++bit;
        }

        for (auto it = parentheses.begin();
                  it != parentheses.end(); )
        {
            Bullet* bullet = *it;
            if (bullet->Collides(player.GetRect()))
            {
                //player.damage(bullet->GetDamage());
                it = parentheses.erase(it);
            }
            else
            {
                ++it;
            }
        }

        for (auto& i : enemies)
        {
            if (i->Collides(player.GetRect()))
            {
                gpa -= static_cast<double>(i->m_DAMAGE);
                gpa = std::max(gpa, 3.0f);
            }
        }

        if (currentPowerup && currentPowerup->Collides(player.GetRect()))
        {
            player.AddPowerUp(currentPowerupType);
            currentPowerup = nullptr;
            currentPowerupType = PowerUp::NONE;
        }

        if (kill_time++ > 60)
        {
            DoubleText.Resize(vector2_t(128, 32));
            TripleText.Resize(vector2_t(128, 32));
            MultiText.Resize(vector2_t(128, 32));
            kill_count = 0;
            kill_time = 0;
        }

        if (rand() % 500 == 1 && currentPowerup == nullptr)
        {
            currentPowerupType = static_cast<PowerUp>(rand() % (static_cast<int>(PowerUp::NONE)));
            currentPowerup = POWER_UPS[currentPowerupType];
            vector2_t pos(100 + (rand() % 600), 100 + (rand() % 400));
            printf("powerup spawned: %d @ %d,%d\n",
                   static_cast<int>(currentPowerupType), pos.x, pos.y); 
            currentPowerup->Move(pos);
        }

        if (time_till_build <= 0 && count % 24 == 0)
        {
            clear_color = SDL_Color{ rand() % 255, rand() % 255, rand() % 255 };
        }

        window.Clear(clear_color.r, clear_color.g, clear_color.b);
        background.Draw();

        //printf("tti: %d\n", time_till_insanity);

        if (time_till_insanity <= 0)
        {
            printf("Bass? Dropped.\n");
            BossMan.Enable();

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
            i->Update(parentheses);

        for (auto& i : parentheses)
        {
            i->Update();
            i->Draw();
        }

        particles.Update();

        if (currentPowerup)
        {
            currentPowerup->Draw();
        }

        if (exploding)
        {
            explosion.Resize(vector2_t(explosion.GetWidth() + 10,
                                       explosion.GetHeight() + 10));
            explosion.Move(vector2_t(explosion.GetX() - 5, explosion.GetY() - 5));

            if (explosion.GetWidth() > 1500)
            {
                exploding = false;
            }
            
            for (auto it = enemies.begin(); it != enemies.end();)
            {
                Enemy* i = *it;

                if (i != BossManPtr &&
                    i->Collides(rect_t(explosion.GetX(), explosion.GetY(),
                                       explosion.GetWidth(), explosion.GetHeight())))
                {
                    particles.Explode(i->GetCenter());
                    delete i;
                    it = enemies.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            explosion.Draw();
        }

        fs << "PowerUps     " << player.PowerupCount(PowerUp::BOMB) << " bombs | ";
        if (player.HasPowerup(PowerUp::MULTI_SHOT))
            fs << "multi-shot";
        powerupUI.SetTexture(fs.Render(uiFont, window, { 255, 255, 255 }));
        fs.Clear();
        powerupUI.Draw();

        fs << total_kills << " kills";
        killUI.SetTexture(fs.Render(uiFont, window, { 255, 255, 255 }));
        fs.Clear();
        killUI.Move(vector2_t(0, UI_LINE_HEIGHT));
        killUI.Draw();

        fs << std::fixed << std::setprecision(3) << gpa << " GPA";
        scoreUI.SetTexture(fs.Render(uiFont, window, { 255, 255, 255 }));
        fs.Clear();
        scoreUI.Move(vector2_t(0, 2*UI_LINE_HEIGHT));
        scoreUI.Draw();

        window.Display();

        //clear_color = SDL_Color { 255, 255, 255 };

        uint32_t current_tick = SDL_GetTicks();
        if ((current_tick - frame) < 1000 / MAX_FPS)
        {
            uint32_t delay = (1000 / MAX_FPS) - (current_tick - frame);
            SDL_Delay(delay);
        }   
    }

    return 0;
}
