#ifndef PIGL_H
#define PIGL_H

#include "EGL/egl.h"

extern uint32_t raspiGL_screen_width;
extern uint32_t raspiGL_screen_height;
extern EGLDisplay raspiGL_display;
extern EGLSurface raspiGL_surface;
extern EGLContext raspiGL_context;

int OGL_Init();
const char* OGL_Error();
void OGL_Quit();
void OGL_SwapBuffers();

#endif
