#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

#include "types.h"

/*
 *
 * FlatDawg: A simple (flat) ray-casting engine
 * Credits:
 *      https://lodev.org/cgtutor/raycasting.html // Lode Vandevenne - Raycasting tutorial (C)
 *      https://github.com/jdah/doomenstein-3d // jdah - Doomenstein 3D (C)
 *
 */


#define MAP_SIZE 8
static u8 MAPDATA[MAP_SIZE * MAP_SIZE] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 2, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1
};

struct {
    SDL_Window *window;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
    u32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    bool quit;

    v2 pos, dir, plane;
} state;

static void verline(int x, int y0, int y1, u32 color) {
    for (int y = y0; y <= y1; y++) {
        state.pixels[(y * SCREEN_WIDTH) + x] = color;
    }
}

static void render() {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        f32 cameraX = 2 * x / (f32) SCREEN_WIDTH - 1;
        v2 rayDir = (v2) {state.dir.x + state.plane.x * cameraX, state.dir.y + state.plane.y * cameraX};

        deltaTime time = {0, 0};

        v2 pos = state.pos;
        v2i ipos = (v2i) { (i32) sign(pos.x), (i32) sign(pos.y) };

        const v2 deltaDist = (v2) {
                fabsf(rayDir.x) < 1e-20 ? 1e30 : fabsf(1.0f / rayDir.x),
                fabsf(rayDir.y) < 1e-20 ? 1e30 : fabsf(1.0f / rayDir.y)
        };

        v2 sideDist = (v2) {
                rayDir.x < 0 ? (pos.x - ipos.x) * deltaDist.x : (ipos.x + 1.0f - pos.x) * deltaDist.x,
                rayDir.y < 0 ? (pos.y - ipos.y) * deltaDist.y : (ipos.y + 1.0f - pos.y) * deltaDist.y
        };

        const v2i step = (v2i) {
                rayDir.x < 0 ? -1 : 1,
                rayDir.y < 0 ? -1 : 1
        };

        f32 perpWallDist;

        struct {
            int val, side; v2 pos;
        } hit = {0, 0, {0.0f, 0.0f}};
        int side;

        // DDA
        while (!hit.val) {
            if (sideDist.x < sideDist.y) {
                sideDist.x += deltaDist.x;
                ipos.x += step.x;
                hit.side = 0;
            } else {
                sideDist.y += deltaDist.y;
                ipos.y += step.y;
                hit.side = 1;
            }
            hit.val = MAPDATA[ipos.y * MAP_SIZE + ipos.x];
        }
        if (hit.side == 0) {
            perpWallDist = (sideDist.x - deltaDist.x);
        } else {
            perpWallDist = (sideDist.y - deltaDist.y);
        }

        int lineHeight = (int) (SCREEN_HEIGHT / perpWallDist);

        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) { drawStart = 0; }
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd >= SCREEN_HEIGHT) { drawEnd = SCREEN_HEIGHT - 1; }

        u32 color = 0;
        switch (hit.val) {
            case 1: color = 0x00FF0000; break;
            case 2: color = 0x0000FF00; break;
            case 3: color = 0x000000FF; break;
            case 4: color = 0x00FFFF00; break;
            default:color = 0x00FFFFFF; break;
        }

        hit.pos = (v2) { pos.x + sideDist.x, pos.y + sideDist.y };

        verline(x, 0, drawEnd, 0xFF202020);
        verline(x, drawStart, SCREEN_HEIGHT - 1, 0xFF505050);
        verline(x, drawStart, drawEnd, color);
    }
}

static void rotate(f32 rot) {
    const v2 d = state.dir, p = state.plane;
    state.dir.x = d.x * cos(rot) - d.y * sin(rot);
    state.dir.y = d.x * sin(rot) + d.y * cos(rot);
    state.plane.x = p.x * cos(rot) - p.y * sin(rot);
    state.plane.y = p.x * sin(rot) + p.y * cos(rot);
}

int main(int argc, char *argv[]) {

    state.window =
            SDL_CreateWindow(
                    "DEMO",
                    SDL_WINDOWPOS_CENTERED_DISPLAY(0),
                    SDL_WINDOWPOS_CENTERED_DISPLAY(0),
                    1280,
                    720,
                    SDL_WINDOW_ALLOW_HIGHDPI);

    state.renderer =
            SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);

    state.texture =
            SDL_CreateTexture(
                    state.renderer,
                    SDL_PIXELFORMAT_ABGR8888,
                    SDL_TEXTUREACCESS_STREAMING,
                    SCREEN_WIDTH,
                    SCREEN_HEIGHT);

    state.pos = (v2) { 2, 2 };
    state.dir = normalize(((v2) { -1.0f, 0.1f }));
    state.plane = (v2) { 0.0f, 0.66f };

    v2 delta = (v2) { 0.0f, 0.0f };

    while (!state.quit) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    state.quit = true;
                    break;
            }
        }

        delta.x = delta.y;
        delta.y = SDL_GetTicks();
        f32 frameTime = (delta.y - delta.x) / 1000.0f;
        // FPS
        printf("FPS: %f\n", (1.0f / frameTime));

        const f32 rotspeed = frameTime * 5.0f, movespeed = frameTime * 5.0f;

        const u8 *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_LEFT]) {
            rotate(+rotspeed);
        }

        if (keystate[SDL_SCANCODE_RIGHT]) {
            rotate(-rotspeed);
        }

        if (keystate[SDL_SCANCODE_UP]) {
            if(MAPDATA[(i32) (state.pos.y + state.dir.y * movespeed) * MAP_SIZE + (i32) (state.pos.x + state.dir.x * movespeed)] == 0) {
                state.pos.x += state.dir.x * movespeed;
                state.pos.y += state.dir.y * movespeed;
            }
        }

        if (keystate[SDL_SCANCODE_DOWN]) {
            if(MAPDATA[(i32) (state.pos.y - state.dir.y * movespeed) * MAP_SIZE + (i32) (state.pos.x - state.dir.x * movespeed)] == 0) {
                state.pos.x -= state.dir.x * movespeed;
                state.pos.y -= state.dir.y * movespeed;
            }
            printf("pos: %f, %f\n", state.pos.x, state.pos.y);
        }

        memset(state.pixels, 0, sizeof(state.pixels));
        render();

        SDL_UpdateTexture(state.texture, NULL, state.pixels, SCREEN_WIDTH * 4);
        SDL_RenderCopyEx(
                state.renderer,
                state.texture,
                NULL,
                NULL,
                0.0,
                NULL,
                SDL_FLIP_VERTICAL);
        SDL_RenderPresent(state.renderer);
    }

    SDL_DestroyTexture(state.texture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    return 0;
}