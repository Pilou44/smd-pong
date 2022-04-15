/*
 * main.c
 *
 *  Created on: 7 avr. 2022
 *      Author: guill
 */

#include <genesis.h>
#include "resources.h"
#include "splash.h"
#include "title.h"

static void handleInput();
static void joyEvent(u16 joy, u16 changed, u16 state);
static void moveP1(s16 increment);
static void manageBall();
static void game(u8 players);

bool paused;

Sprite *player1;
Sprite *player2;
Sprite *ball;

s16 p1x;
s16 p1y;
u16 p1Height;
u16 p1Width;

s16 p2x;
s16 p2y;
u16 p2Height;
u16 p2Width;

s16 ballX;
s16 ballY;
u16 ballSize;
s8 ballSpeedX;
s8 ballSpeedY;

s16 minHeight;
s16 maxHeight;
s16 minWidth;
s16 maxWidth;

int main(u16 hard)
{
    showSplash();

    while (TRUE)
    {
        u8 players = showTitle();
        game(players);
    }

    return 0;
}

void game(u8 players)
{
    u16 palette[64];

    SPR_init();
    JOY_init();

    paused = FALSE;

    minHeight = 0;
    maxHeight = screenHeight;
    minWidth = 0;
    maxWidth = screenWidth;

    player1 = SPR_addSprite(&sprite_barre, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    player2 = SPR_addSprite(&sprite_barre, 0, 0, TILE_ATTR(PAL1, TRUE, FALSE, TRUE));
    ball = SPR_addSprite(&sprite_ball, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

    p1Height = sprite_barre.h;
    p1Width = sprite_barre.w;
    p2Height = sprite_barre.h;
    p2Width = sprite_barre.w;
    p1x = 20;
    p1y = (screenHeight - p1Height) / 2;
    SPR_setPosition(player1, p1x, p1y);
    p2x = screenWidth - 20;
    p2y = (screenHeight - p2Height) / 2;
    SPR_setPosition(player2, p2x, p2y);

    ballSize = sprite_ball.w;
    ballX = (screenWidth - ballSize) / 2;
    ballY = (screenHeight - ballSize) / 2;
    SPR_setPosition(ball, ballX, ballY);

    SPR_update();

    // prepare palettes (BGB image contains the 4 palettes data)
    memcpy(&palette[0], palette_all.data, 64 * 2);
    //            memcpy(&palette[16], palette_sprites.data, 16 * 2);

    JOY_setEventHandler(joyEvent);

    KLog("Launch");

    // fade in
    PAL_fadeIn(0, (4 * 16) - 1, palette, 20, FALSE);

    s8 xSpeedSign = (random() % 3) - 1;
    while (xSpeedSign == 0)
        xSpeedSign = (random() % 3) - 1;
    KLog_S1("x speed sign: ", xSpeedSign);

    ballSpeedX = xSpeedSign * (random() % 4);
    while (ballSpeedX == 0)
        ballSpeedX = xSpeedSign * (random() % 4);
    KLog_S1("x speed: ", ballSpeedX);

    s8 ySpeedSign = (random() % 3) - 1;
    while (ySpeedSign == 0)
        ySpeedSign = (random() % 3) - 1;
    KLog_S1("y speed sign: ", ySpeedSign);

    ballSpeedY = ySpeedSign * (random() % 4);
    while (ballSpeedY == 0)
        ballSpeedY = ySpeedSign * (random() % 4);
    KLog_S1("x speed: ", ballSpeedY);

    while (1)
    {
        manageBall();
        handleInput();
        SPR_update();
        SYS_doVBlankProcess();
    }
}

static void manageBall()
{
    ballX += ballSpeedX;
    ballY += ballSpeedY;
    if (ballY <= minHeight)
    {
        ballY = minHeight;
        ballSpeedY = -ballSpeedY;
    }
    else if (ballY + ballSize > maxHeight)
    {
        ballY = maxHeight - ballSize;
        ballSpeedY = -ballSpeedY;
    }

    // ToDo Temp for test
    if (ballX <= minWidth)
    {
        ballX = minWidth;
        ballSpeedX = -ballSpeedX;
    }
    else if (ballX + ballSize > maxWidth)
    {
        ballX = maxWidth - ballSize;
        ballSpeedX = -ballSpeedX;
    }

    u16 p1R = p1x + p1Width;
    u16 p1T = p1y;
    u16 p1B = p1y + p1Height;
    u16 ballCenterY = ballY + ballSize / 2;

    bool contactOnP1X = ballX <= p1R && ballX - ballSpeedX > p1R && ballSpeedX < 0;
    bool contactOnP1Y = ballCenterY > p1T && ballCenterY < p1B;
    if (contactOnP1X && contactOnP1Y)
    {
        ballX = p1R;
        ballSpeedX = -ballSpeedX;
    }
    SPR_setPosition(ball, ballX, ballY);
}

static void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    // game is paused ? adjust physics settings
    if (paused)
    {
        // ToDo
    }
    // can affect gameplay
    else
    {
        //        KLog_U1("handleInput - ", value);
        if (value & BUTTON_UP)
            moveP1(-1);
        else if (value & BUTTON_DOWN)
            moveP1(+1);
    }
}

static void moveP1(s16 increment)
{
    p1y += 3 * increment;
    if (p1y < minHeight)
        p1y = minHeight;
    if (p1y + p1Height > maxHeight)
        p1y = maxHeight - p1Height;
    SPR_setPosition(player1, p1x, p1y);
}

static void joyEvent(u16 joy, u16 changed, u16 state)
{
    // START button state changed --> pause / unpause
    if (changed & state & BUTTON_START)
    {
        paused = !paused;
    }
}
