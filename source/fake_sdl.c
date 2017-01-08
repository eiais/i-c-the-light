/*
* Simple DirectMedia Layer
* Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* So... the original SDL source says I have to include this (and I'm happy to)
* but honestly, except for SDL_CreateRGBSurface this file is entirely stub
* functions, and even that is... minimal, at 22 lines.
*/

#include "fake_sdl.h"

int SDL_Quit()
{
	return 0;
}

int SDL_UpdateWindowSurface()
{
	return 0;
}

int SDL_Init(int flags)
{
	return 0;
}

int SDL_DestroyWindow(SDL_Window *window)
{
	return 0;
}

char *SDL_GetError()
{
	return 0;
}

SDL_Surface *SDL_CreateRGBSurface(
	unsigned int flags,
	int width,
	int height,
	int depth,
	unsigned int rmask,
	unsigned int gmask,
	unsigned int bmask,
	unsigned int amask
)
{
	SDL_Surface *surface;

	(void)flags;

	surface = (SDL_Surface *)calloc(1, sizeof(*surface));
	if (surface == NULL) {
		printf("Surface allocation failed! Out of memory.\n");
		return NULL;
	}

	surface->format = 0;
	surface->w = width;
	surface->h = height;
	surface->pitch = surface->w * sizeof(unsigned int);

	if(surface->w && surface->h) {
		surface->pixels = malloc(surface->h * surface->pitch);
		if(surface->pixels == NULL) {
			printf("Pixel allocation failed! Out of memory.\n");
			return NULL;
		}
	}
	surface->map = NULL;
	surface->refcount = 1;
	return surface;
}

void *SDL_CreateWindow(
	char *title,
	unsigned int xwindowpos,
	unsigned int ywindowpos,
	unsigned int width,
	unsigned int height,
	unsigned int flags
)
{
	printf("Fake SDL library used, window NOT being created\n");
	unsigned int rmask, gmask, bmask, amask;
#define SCREEN_BIT_DEPTH 32
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
	return SDL_CreateRGBSurface(
		0,
		width, height,
		SCREEN_BIT_DEPTH,
		rmask, gmask, bmask, amask
	);
}

void *SDL_GetWindowSurface(SDL_Window window)
{
	//it was a surface all along!
	return (SDL_Surface *)window;
}

int SDL_FillRect(
	SDL_Surface *surface,
	SDL_Rect *rect,
	unsigned int color
)
{
	//start x, y, end x, y, indicies
	int sx, sy, ex, ey, i, j;
	if(rect == NULL) {
		sx = sy = 0;
		ex = surface->w;
		ey = surface->h;
	}
	for(i = sy; i < ey; i++) {
	for(j = sx; j < ex; j++) {
		((unsigned int *)surface->pixels)[j + i * surface->w] = color;
	}
	}
	return 0;
}

void SDL_Delay(unsigned int ms)
{
#ifdef Sleep
	Sleep(ms);
#elif defined(nanosleep)
	int s = ms / 1000;
	int ns = (ms % 1000) * 1000000;
	struct timespec spec = {
		tv_sec = (time_t)s;
		tv_nsec = (long)ns;
	};
	nanosleep(spec, NULL);
#endif
	return;
}

void *SDL_PollEvent(void *event)
{
	return NULL;
}
