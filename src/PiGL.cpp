#include "PiGL.h"
#include "bcm_host.h"

#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#define	ERROR_IF(C, M)	if (C) { errorstring=M"\n"; return 1; }

uint32_t raspiGL_screen_width;
uint32_t raspiGL_screen_height;
EGLDisplay raspiGL_display;
EGLSurface raspiGL_surface;
EGLContext raspiGL_context;

const char *errorstring;

// Initialize OpenGL
int OGL_Init() {
	bcm_host_init();

	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };

   EGLConfig config;

   // get an EGL display connection
   raspiGL_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	ERROR_IF(EGL_NO_DISPLAY == raspiGL_display, "Error getting EGL display.");

   // initialize the EGL display connection
   result = eglInitialize(raspiGL_display, NULL, NULL);
	ERROR_IF(EGL_FALSE == result, "Error initializing EGL display.");

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(raspiGL_display, attribute_list, &config, 1, &num_config);
	ERROR_IF(EGL_FALSE == result, "Error choosing EGL framebuffer config.");

   // create an EGL rendering context
   raspiGL_context = eglCreateContext(raspiGL_display, config, EGL_NO_CONTEXT, NULL);
	ERROR_IF(EGL_NO_CONTEXT == raspiGL_context, "Error creating EGL context.");

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &raspiGL_screen_width, &raspiGL_screen_height);
	ERROR_IF(success < 0, "Error creating an EGL window.");

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = raspiGL_screen_width;
   dst_rect.height = raspiGL_screen_height;

   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = raspiGL_screen_width << 16;
   src_rect.height = raspiGL_screen_height << 16;

   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );

   dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0 /*layer*/, &dst_rect, 0 /*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0 /*clamp*/, DISPMANX_NO_ROTATE);

   nativewindow.element = dispman_element;
   nativewindow.width = raspiGL_screen_width;
   nativewindow.height = raspiGL_screen_height;
   vc_dispmanx_update_submit_sync( dispman_update );

   raspiGL_surface = eglCreateWindowSurface( raspiGL_display, config, &nativewindow, NULL );
   ERROR_IF(raspiGL_surface == EGL_NO_SURFACE, "Error creating EGL surface.");

   result = eglMakeCurrent(raspiGL_display, raspiGL_surface, raspiGL_surface, raspiGL_context);
   ERROR_IF(EGL_FALSE == result, "Error connecting context to surface.");

	return 0;
}

// Return the error string that's created if something goes wrong
const char* OGL_GetError() {
	return errorstring;
}

void OGL_Quit() {
	// Get rid of the last frame
	glClear( GL_COLOR_BUFFER_BIT );
	eglSwapBuffers(raspiGL_display, raspiGL_surface);

	// Uninitialize all the EGL components
	eglMakeCurrent( raspiGL_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
	eglDestroySurface( raspiGL_display, raspiGL_surface );
	eglDestroyContext( raspiGL_display, raspiGL_context );
	eglTerminate( raspiGL_display );
}

void OGL_SwapBuffers() {
	eglSwapBuffers(raspiGL_display, raspiGL_surface);
}
