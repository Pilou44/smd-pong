/*
 * main.c
 *
 *  Created on: 7 avr. 2022
 *      Author: guill
 */

#include <genesis.h>
#include "resources.h"

#define MAP_HEIGHT          1280
#define MAX_POSY            FIX32(MAP_HEIGHT - 356)

static void handleInput();
static void joyEvent(u16 joy, u16 changed, u16 state);
static void moveP1(s16 increment);

bool paused;

Sprite* player1;
Sprite* ball;
s16 p1x;
s16 p1y;
u16 p1Height;
s16 ballX;
s16 ballY;
u16 ballSize;

s16 minHeight;
s16 maxHeight;

int main(u16 hard)
{
    u16 palette[64];

	SPR_init();
	JOY_init();

	paused = FALSE;

	minHeight = 0;
	maxHeight = screenHeight;

	player1 = SPR_addSprite(&sprite_barre, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
	ball = SPR_addSprite(&sprite_ball, 0, 0, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

	p1Height = sprite_barre.h;
	p1x = 20;
	p1y = (screenHeight - p1Height) / 2;
	SPR_setPosition(player1, p1x, p1y);

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

    while(1)
    {
        handleInput();
	    SPR_update();
    	SYS_doVBlankProcess();
    }

    return 0;
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
        if (value & BUTTON_UP) moveP1(-1);
        else if (value & BUTTON_DOWN) moveP1(+1);
    }
}

static void moveP1(s16 increment) {
	p1y += 3 * increment;
	if (p1y < 0) p1y = 0;
	if (p1y + p1Height > maxHeight) p1y = maxHeight - p1Height;
    KLog_U1("moveP1 - ", p1y);
	SPR_setPosition(player1, p1x, p1y);
}

static void joyEvent(u16 joy, u16 changed, u16 state)
{
		KLog("joyEvent");

    // START button state changed --> pause / unpause
    if (changed & state & BUTTON_START)
    {
        paused = !paused;
    }
}
