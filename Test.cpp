#include "PiGL.h"
#include "GLES/gl.h"

volatile int terminate;

bool x;
void redraw_scene() {
	glClearColor(.1, .2, .4, x ?.9 : .93);
	x = !x;
	glClear(GL_COLOR_BUFFER_BIT);
	OGL_SwapBuffers();
}

int main () {
   OGL_Init();

   while (!terminate) {
      redraw_scene();
   }

   OGL_Quit();
   return 0;
}

