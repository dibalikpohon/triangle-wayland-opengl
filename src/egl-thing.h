
#ifndef _EGLTHING_H_
#define _EGLTHING_H_

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

extern void init_egl(void);
extern void create_egl_surface(void);
#endif
