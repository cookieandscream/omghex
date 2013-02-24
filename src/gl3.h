#ifndef TRAINS_GL3_H
#define TRAINS_GL3_H

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include "SDL.h"

#if defined(__MACOSX__)
#include <OpenGL/gl3.h>
//#include <OpenGL/gl3ext.h>
#else
#include "SDL_opengl.h"
#endif

#endif
