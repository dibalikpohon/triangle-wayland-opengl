#include <EGL/egl.h>
#include <stdio.h>
#include <stdlib.h>
#include <wayland-egl.h>

#include "state.h"

extern struct state g;
extern const int32_t surf_width;   // ehhe
extern const int32_t surf_height;  // ehhe

#define EGL_PRINT_ERROR(what)                                           \
    do {                                                                \
        EGLint error = eglGetError();                                   \
        if (error != EGL_SUCCESS) {                                     \
            (void) fprintf(stderr,                                      \
                           "%s failed with code 0x%04hX (just look up " \
                           "the code im lazy)\n",                       \
                           (what),                                      \
                           error);                                      \
        }                                                               \
    } while (0)

void init_egl(void) {
    EGLint major, minor, count, n, size;
    EGLConfig *configs;
    // clang-format off
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE,     EGL_WINDOW_BIT,
        EGL_RED_SIZE,         8,
        EGL_GREEN_SIZE,       8,
        EGL_BLUE_SIZE,        8,
        EGL_ALPHA_SIZE,       8,
        EGL_CONFORMANT,       EGL_OPENGL_BIT,
        EGL_RENDERABLE_TYPE,  EGL_OPENGL_BIT,
        EGL_NONE
    };
    // clang-format on

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

    g.egl_display = eglGetDisplay((EGLNativeDisplayType) g.display);
    if (g.egl_display == EGL_NO_DISPLAY) {
        (void) fprintf(stderr, "Can't create egl display\n");
        exit(1);
    } else {
        (void) fprintf(stderr, "Created egl display\n");
    }

    if (eglInitialize(g.egl_display, &major, &minor) != EGL_TRUE) {
        (void) fprintf(stderr, "Can't initialise egl display\n");
        exit(1);
    }
    printf("EGL major: %d, minor %d\n", major, minor);

    eglBindAPI(EGL_OPENGL_API);
    eglGetConfigs(g.egl_display, NULL, 0, &count);
    printf("EGL has %d configs\n", count);

    configs = calloc(count, sizeof(EGLConfig));
    eglChooseConfig(g.egl_display, config_attribs, configs, count, &n);
    g.egl_conf = configs[0];

    g.egl_context = eglCreateContext(
        g.egl_display, g.egl_conf, EGL_NO_CONTEXT, context_attribs);
    EGL_PRINT_ERROR("Create context");

    free(configs);
}

void create_egl_surface(void) {
    g.egl_window = wl_egl_window_create(g.surface, surf_width, surf_height);
    if (g.egl_window == EGL_NO_SURFACE) {
        (void) fprintf(stderr, "Can't create egl window\n");
        exit(1);
    } else {
        (void) fprintf(stderr, "Created egl window\n");
    }

    g.egl_surface =
        eglCreateWindowSurface(g.egl_display, g.egl_conf, g.egl_window, NULL);
    EGL_PRINT_ERROR("Create window surface");

    if (eglMakeCurrent(
            g.egl_display, g.egl_surface, g.egl_surface, g.egl_context))
    {
        (void) fprintf(stderr, "Made current\n");
    } else {
        (void) fprintf(stderr, "Made current failed\n");
    }
}
