#include "splash.h"
#include "splash_res.h"

void splash()
{
    s16 wait = 0;
    u16 palette[64];
    u16 ind;

    u16 bgBaseTileIndex[2];
    Map *bgb;

    VDP_setScreenWidth320();
    VDP_setPaletteColors(0, (u16 *)palette_black, 64);

    // load background tilesets in VRAM
    ind = TILE_USERINDEX;
    bgBaseTileIndex[0] = ind;
    VDP_loadTileSet(&bgb_tileset, ind, DMA);
    ind += bgb_tileset.numTile;

    bgb = MAP_create(&bgb_map, BG_B, TILE_ATTR_FULL(0, FALSE, FALSE, FALSE, bgBaseTileIndex[0]));
    s16 camX = 0;
    MAP_scrollTo(bgb, camX, 0);

    memcpy(&palette[0], palette_splash.data, 16 * 2);
    memcpy(&palette[16], palette_splash.data, 16 * 2);

    VDP_setTextPalette(1);
    VDP_drawText("Wechant Loup presents", 9, 22);
    VDP_setHorizontalScroll(BG_A, 4);

    PAL_fadeIn(0, (1 * 16) - 1, palette, 20, FALSE);

    while (camX < 320)
    {
        camX += 4;
        MAP_scrollTo(bgb, camX, 0);
        SYS_doVBlankProcess();
    }
    PAL_fadeIn(16, (2 * 16) - 1, palette, 20, FALSE);
    while (wait < 180)
    {
        wait++;
        SYS_doVBlankProcess();
    }
    PAL_fadeOut(16, (2 * 16) - 1, 20, FALSE);
    while (camX < 640)
    {
        camX += 4;
        MAP_scrollTo(bgb, camX, 0);
        SYS_doVBlankProcess();
    }
    VDP_clearText(9, 22, 21);

    MEM_free(bgb);
}
