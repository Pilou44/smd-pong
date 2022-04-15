#include "level.h"
#include "level_res.h"

static void handleInput();
static void joyEvent(u16 joy, u16 changed, u16 state);

static void moveP1(s16 increment);
static void moveP2(s16 increment);
static void movebar(s16 increment, Sprite *bar, u16 height);

static void manageBall();
static void checkCollision(Sprite *bar, u16 width, u16 height, s16 *ballX, s16 *ballY);

bool paused;

Sprite *player1;
Sprite *player2;
Sprite *ball;

u16 p1Height;
u16 p1Width;

u16 p2Height;
u16 p2Width;

u16 ballSize;
s8 ballSpeedX;
s8 ballSpeedY;

s16 minHeight;
s16 maxHeight;
s16 minWidth;
s16 maxWidth;

void showLevel(u8 players)
{
    u16 palette[64];

    VDP_setScreenWidth320();
    SPR_init();
    JOY_init();

    paused = FALSE;

    minHeight = 0;
    maxHeight = screenHeight;
    minWidth = 0;
    maxWidth = screenWidth;

    p1Height = sprite_barre.h;
    p1Width = sprite_barre.w;
    p2Height = sprite_barre.h;
    p2Width = sprite_barre.w;

    s16 startX1 = 20;
    s16 startX2 = screenWidth - startX1;
    s16 startY1 = (screenHeight - p1Height) / 2;
    s16 startY2 = (screenHeight - p2Height) / 2;
    player1 = SPR_addSprite(&sprite_barre, startX1, startY1, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    player2 = SPR_addSprite(&sprite_barre, startX2, startY2, TILE_ATTR(PAL1, TRUE, FALSE, TRUE));

    ballSize = sprite_ball.w;
    s16 ballX = (screenWidth - ballSize) / 2;
    s16 ballY = (screenHeight - ballSize) / 2;
    ball = SPR_addSprite(&sprite_ball, ballX, ballY, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));

    SPR_update();

    // prepare palettes (BGB image contains the 4 palettes data)
    memcpy(&palette[0], palette_all.data, 64 * 2);
    //            memcpy(&palette[16], palette_sprites.data, 16 * 2);

    JOY_setEventHandler(joyEvent);

    // fade in
    PAL_fadeIn(0, (4 * 16) - 1, palette, 20, FALSE);

    s8 xSpeedSign = (random() % 3) - 1;
    while (xSpeedSign == 0)
        xSpeedSign = (random() % 3) - 1;

    ballSpeedX = xSpeedSign * (random() % 4);
    while (ballSpeedX == 0)
        ballSpeedX = xSpeedSign * (random() % 4);

    s8 ySpeedSign = (random() % 3) - 1;
    while (ySpeedSign == 0)
        ySpeedSign = (random() % 3) - 1;

    ballSpeedY = ySpeedSign * (random() % 4);
    while (ballSpeedY == 0)
        ballSpeedY = ySpeedSign * (random() % 4);

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
    s16 ballX = SPR_getPositionX(ball);
    s16 ballY = SPR_getPositionY(ball);
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

    checkCollision(player1, p1Width, p1Height, &ballX, &ballY);
    checkCollision(player2, p2Width, p2Height, &ballX, &ballY);

    SPR_setPosition(ball, ballX, ballY);
}

static void checkCollision(Sprite *bar, u16 width, u16 height, s16 *ballX, s16 *ballY)
{
    s16 barLeft = SPR_getPositionX(bar);
    s16 barRight = barLeft + width;
    s16 barTop = SPR_getPositionY(bar);
    s16 barBottom = barTop + height;

    s16 ballLeft = SPR_getPositionX(ball);
    s16 ballRight = ballLeft + ballSize;
    s16 ballTop = SPR_getPositionY(ball);
    s16 ballBottom = ballTop + ballSize;

    bool isGoodY = ballBottom >= barTop && ballTop <= barBottom;
    if (isGoodY)
    {
        bool contactOnRight = ballLeft <= barRight && ballLeft - ballSpeedX > barRight && ballSpeedX < 0;
        if (contactOnRight)
        {
            *ballX = barRight;
            ballSpeedX = -ballSpeedX;
        }
        else
        {
            bool contactOnLeft = ballRight >= barLeft && ballRight - ballSpeedX < barLeft && ballSpeedX > 0;
            if (contactOnLeft)
            {
                *ballX = barLeft - ballSize;
                ballSpeedX = -ballSpeedX;
            }
        }
    }

    bool isGoodX = ballRight >= barLeft && ballLeft <= barRight;
    if (isGoodX)
    {
        bool contactOnBottom = ballTop <= barBottom && ballTop - ballSpeedY > barBottom && ballSpeedY < 0;
        if (contactOnBottom)
        {
            *ballY = barBottom;
            ballSpeedY = -ballSpeedY;
        }
        else
        {
            bool contactOnTop = ballBottom >= barTop && ballBottom - ballSpeedY < barTop && ballSpeedY > 0;
            if (contactOnTop)
            {
                *ballY = barTop - ballSize;
                ballSpeedY = -ballSpeedY;
            }
        }
    }
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
    movebar(increment, player1, p1Height);
}

static void moveP2(s16 increment)
{
    movebar(increment, player2, p2Height);
}

static void movebar(s16 increment, Sprite *bar, u16 height)
{
    s16 x = SPR_getPositionX(bar);
    s16 y = SPR_getPositionY(bar) + 3 * increment;
    if (y < minHeight)
        y = minHeight;
    if (y + height > maxHeight)
        y = maxHeight - height;
    SPR_setPosition(bar, x, y);
}

static void joyEvent(u16 joy, u16 changed, u16 state)
{
    // START button state changed --> pause / unpause
    if (changed & state & BUTTON_START)
    {
        paused = !paused;
    }
}
