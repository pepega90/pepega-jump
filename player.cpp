#include "raylib.h"
#include <iostream>
#include <vector>
#include "platform.h"

struct Player
{
    Texture2D img;
    float scale;
    Vector2 pos{};
    Vector2 vel{};
    Vector2 acc{};
    Vector2 dir{};
    Rectangle frameRec{};
    Rectangle rect{};
    Sound jumpSound;

    float speed{500.f};

    float gravity = 50.f;

    // idle
    int currentFrame = 0;
    int framesCounter = 0;
    int framesSpeed = 8;
    int jumpFrame = 8;

    // run
    int runCur = 4;
    int runCounter = 0;
    int runSpeed = 8;

    // anim control
    bool idle = false;
    bool run = false;
    bool flip = false;
    bool jump = false;
    bool onGround = false;

    // came
    float bgY = -800;

    void Animation()
    {
        /*
        animation frame index

        idle = 0 - 3
        run = 4 - 7
        jump = 8

        */

        // animation
        if (idle)
        {
            framesCounter++;

            if (framesCounter >= (60 / framesSpeed))
            {
                framesCounter = 0;
                currentFrame++;

                if (currentFrame > 3)
                    currentFrame = 0;

                frameRec.x = (float)currentFrame * (float)img.width / 19;
            }
        }

        if (run)
        {
            runCounter++;

            if (runCounter >= (60 / runSpeed))
            {
                runCounter = 0;
                runCur++;

                if (runCur > 7)
                    runCur = 4;

                frameRec.x = (float)runCur * (float)img.width / 19;
            }
        }

        if (!onGround)
        {
            frameRec.x = (float)jumpFrame * (float)img.width / 19;
        }
    }

    void Update(std::vector<Platform> &platformList, float &dt)
    {
        Animation();

        if (IsKeyPressed(KEY_R))
        {
            rect.x = 50;
            rect.y = GetScreenHeight() - 300;
        }

        // player control
        if (IsKeyDown(KEY_RIGHT))
        {
            idle = false;
            run = true;
            dir.x = 1;
            flip = false;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            idle = false;
            run = true;
            dir.x = -1;
            flip = true;
        }
        else
        {
            dir.x = 0;
            idle = true;
            run = false;
        }

        // apply acc
        vel.x += dir.x * speed * dt;
        acc.x = -vel.x * 0.4;
        vel.x += acc.x;
        rect.x += vel.x;

        // player jump
        if (IsKeyDown(KEY_SPACE) and onGround)
        {
            PlaySound(jumpSound);
            onGround = false;
            dir.y = -18.f;
        }

        // collision y
        dir.y += gravity * dt;
        rect.y += dir.y;
        for (auto &p : platformList)
        {
            if (CheckCollisionRecs(rect, p.rect))
            {
                Rectangle collideRect = GetCollisionRec(rect, p.rect);

                if (rect.y < p.rect.y && rect.y + rect.height / 2 < p.rect.y && collideRect.width > 10)
                {
                    if (dir.y > 0)
                    {
                        rect.y = p.rect.y - rect.height;
                        dir.y = 0;
                        onGround = true;
                    }
                }
            }
        }

        // batas layar
        if (rect.x < 0)
            rect.x += (float)GetScreenWidth();
        if (rect.x >= (float)GetScreenWidth())
            rect.x -= (float)GetScreenWidth();

        // update position
        pos.x = rect.x;
        pos.y = rect.y;
        rect.width = frameRec.width;
        rect.height = frameRec.height;
    }

    void Draw()
    {
        DrawTextureRec(img, Rectangle{frameRec.x, frameRec.y, flip ? -frameRec.width : frameRec.width, frameRec.height}, pos, WHITE);
    }
};