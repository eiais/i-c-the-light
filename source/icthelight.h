//repo: https://github.com/9999years/i-c-the-light
//MIT/expat license
//rebecca turner
//consult ../readme.md

//display
#ifndef USE_REAL_SDL
#define USE_REAL_SDL
#endif
#include "fake_sdl.h"
//logging, file out
#include <stdio.h>
#include <io.h>
//rand
#include <stdlib.h>
//sin functions
#include <math.h>
//timestamp for file names
#include <time.h>

//project files
#include "common.h"
#include "color.h"
#include "ppm.h"
#include "line.h"
#include "vector.h"
#include "distance.h"
#include "quaternion.h"
#include "logging.h"

#ifndef ICTHELIGHT_H
#define ICTHELIGHT_H

#define SCREEN_HEIGHT 250
#define SCREEN_WIDTH 250

#define FRAMES_IN_ROTATION 60.0F

//global
int frame;

//global distance estimator
float de(vec3 pos);

//returns a normal
vec3 getnormal(vec3 pos, float samplesize);

//takes a light vector and a position vector
//the position vector is the distance between the camera and the point on the
//surface (vec V here:
//https://en.m.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model#Description
//can usually be found with )
float blinnphong(vec3 cam, vec3 pos, vec3 rot, vec3 light);

void render(SDL_Surface *screen, const int frame);
#endif //ICTHELIGHT_H