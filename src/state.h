#include <EGL/egl.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-egl-core.h>

#ifndef _STATE_H_
#define _STATE_H_

struct state {
    struct wl_compositor *compositor;
    struct wl_display *display;
    struct wl_surface *surface;
    struct wl_buffer *buffer;
    struct wl_shm *shm;
    struct wl_seat *seat;
    struct wl_keyboard *keyboard;

    struct xdg_wm_base *xdg_base;
    struct xdg_toplevel *xdg_toplevel;

    struct wl_egl_window *egl_window;
    struct wl_region *region;
    EGLDisplay egl_display;
    EGLConfig egl_conf;
    EGLSurface egl_surface;
    EGLContext egl_context;

    int closed;
};
#endif
