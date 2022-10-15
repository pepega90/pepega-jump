#include "raylib.h"
#include "raymath.h"
#include "player.cpp"
#include "platform.h"
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>

enum Scene
{
    MENU = 1,
    PLAY,
    GAMEOVER
};

int main()
{
    const int WIDTH{400};
    const int HEIGHT{600};

    InitWindow(WIDTH, HEIGHT, "Pepega Jump");
    InitAudioDevice();
    SetTargetFPS(60);

    Image sprite = LoadImage("res/texture/player.png");
    Texture2D texture = LoadTextureFromImage(sprite);

    Player player;
    player.rect.x = 50;
    player.rect.y = 300;
    player.scale = 4.0f;
    player.img = texture;
    player.img.width *= player.scale;
    player.img.height *= player.scale;
    player.pos = {WIDTH / 2, HEIGHT / 2};
    player.frameRec = {0.0f, 0.0f, (float)sprite.width / 19 * player.scale - 13, (float)sprite.height * player.scale - 15};
    player.jumpSound = LoadSound("res/snd/Jump33.wav");

    // load background
    Texture2D bg = LoadTexture("res/texture/Background.png");

    // load backsong
    Sound themeSong = LoadSound("res/snd/happy.ogg");

    // load platform texture
    Texture2D platformTexture = LoadTexture("res/texture/0.png");
    // scaled to 0.3

    // load button
    Texture2D playBtn = LoadTexture("res/texture/playButton.png");
    Rectangle btnRect;
    btnRect.x = WIDTH / 2 - 50;
    btnRect.y = HEIGHT / 2;
    btnRect.width = playBtn.width * 0.5f;
    btnRect.height = playBtn.height * 0.5f;

    Platform initialPlat;
    initialPlat.img = platformTexture;
    initialPlat.rect.x = 30;
    initialPlat.rect.y = HEIGHT - 50;
    initialPlat.rect.width = initialPlat.img.width * 0.2f;
    initialPlat.rect.height = initialPlat.img.height * 0.2f;

    // platform vector
    int sizeList = 4;
    std::vector<Platform> platformList;

    for (int i = 1; i < sizeList; i++)
    {
        Platform p;
        p.img = platformTexture;
        p.rect.x = GetRandomValue(0, WIDTH - initialPlat.img.width * 0.2f);
        p.rect.y = initialPlat.rect.y - (i * 170);
        p.rect.width = p.img.width * 0.2f;
        p.rect.height = p.img.height * 0.2f;
        platformList.push_back(p);
    }
    platformList.push_back(initialPlat);

    // scene
    Scene currentScene = MENU;

    // game variabel
    int score = 0;
    int scrollTreshold = 250;

    while (!WindowShouldClose())
    {

        float dt = GetFrameTime();

        ClearBackground(BLACK);
        BeginDrawing();

        DrawTextureEx(bg, Vector2{0, -650}, 0.0, 3.0, WHITE);

        switch (currentScene)
        {
        case MENU:
            DrawText("Pepega\n Jump", WIDTH / 2 - 60, HEIGHT / 4, 40, WHITE);
            DrawTextureEx(playBtn, Vector2{btnRect.x, btnRect.y}, 0.0, 0.5, WHITE);
            DrawText("created by aji mustofa @pepega90", 25, HEIGHT - 30, 20, YELLOW);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), btnRect))
            {
                PlaySound(themeSong);
                currentScene = PLAY;
            }

            break;
        case PLAY:
        {

            player.Update(platformList, dt);

            // draw player
            player.Draw();

            // jika size dari platofrm kurang dari sizeList, maka push_back platofrm baru
            while (platformList.size() < sizeList)
            {
                Platform last_pos = platformList.back();
                Platform newPlat;
                newPlat.img = platformTexture;
                newPlat.rect.x = GetRandomValue(0, WIDTH - initialPlat.img.width * 0.2f);
                newPlat.rect.y = last_pos.rect.y - 230;
                newPlat.rect.width = newPlat.img.width * 0.2f;
                newPlat.rect.height = newPlat.img.height * 0.2f;
                platformList.push_back(newPlat);
            }

            // update platform
            if (player.rect.y <= scrollTreshold && !player.onGround)
            {
                score += 10;
                for (auto &p : platformList)
                {
                    p.rect.y += std::abs(player.dir.y);
                }
            }

            // Draw platform
            for (unsigned int i = 0; i < platformList.size(); i++)
            {
                DrawTextureEx(platformList[i].img, Vector2{platformList[i].rect.x, platformList[i].rect.y}, 0.0, 0.2, WHITE);
            }

            // remove platform, jika posisi y nya lebih besar dari tinggi layar
            auto platformRemove = std::remove_if(platformList.begin(), platformList.end(), [&](Platform t)
                                                 { return t.rect.y > HEIGHT; });
            if (platformRemove != platformList.end())
            {
                platformList.erase(platformRemove);
            }

            if (player.rect.y > HEIGHT)
                currentScene = GAMEOVER;
            DrawText(std::to_string(score).c_str(), WIDTH / 2 - 5, 20, 30, WHITE);
        }
        break;
        case GAMEOVER:
        {
            StopSound(themeSong);
            if (IsKeyPressed(KEY_R))
            {
                PlaySound(themeSong);
                score = 0;

                player.rect.x = 50;
                player.rect.y = HEIGHT - 300;

                platformList.clear();

                for (int i = 1; i < sizeList; i++)
                {
                    Platform p;
                    p.img = platformTexture;
                    p.rect.x = GetRandomValue(0, WIDTH - initialPlat.img.width * 0.2f);
                    p.rect.y = initialPlat.rect.y - (i * 150);
                    p.rect.width = p.img.width * 0.2f;
                    p.rect.height = p.img.height * 0.2f;
                    platformList.push_back(p);
                }
                platformList.push_back(initialPlat);

                currentScene = PLAY;
            };
            DrawText("Game Over", WIDTH / 2 - 100, HEIGHT / 4, 45, RED);
            std::string textScore = "Score: ";
            textScore.append(std::to_string(score));
            DrawText(textScore.c_str(), WIDTH / 2 - 65, HEIGHT / 3 + 50, 30, WHITE);
            DrawText("Tekan \"R\" untuk restart", WIDTH / 2 - 120, HEIGHT / 2 + 50, 20, WHITE);
        }
        break;
        }

        EndDrawing();
    }

    UnloadSound(player.jumpSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}