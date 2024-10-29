#include <wayland-client-protocol.h>
#include "state.h"
#include "xdg-protocol.h"

static void xdg_wm_base_ping(void *data, struct xdg_wm_base *shell,
                             uint32_t serial) {
    xdg_wm_base_pong(shell, serial);
}

struct xdg_wm_base_listener g_xdg_wmbase_listener = {.ping = xdg_wm_base_ping};


static void xdg_surface_configure(void *data, struct xdg_surface *surface,
                                  uint32_t serial) {
    struct state *s = (struct state *) data;
    xdg_surface_ack_configure(surface, serial);

    // wl_surface_attach(s->surface, s->buffer, 0, 0);
    wl_surface_commit(s->surface);
}

struct xdg_surface_listener g_xdg_surface_listener = {
    .configure = xdg_surface_configure};


static void xdg_toplevel_configure(void *data, struct xdg_toplevel *toplevel,
                                   int32_t width, int32_t height,
                                   struct wl_array *states) {
}

static void xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel) {
    struct state *s = (struct state*) data;
    s->closed = 1;
}

struct xdg_toplevel_listener g_xdg_toplevel_listener = {
    .configure = xdg_toplevel_configure, .close = xdg_toplevel_close};
