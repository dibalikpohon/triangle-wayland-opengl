#include "wl-callback.h"

#include <stdint.h>
#include <wayland-client-protocol.h>

#include "state.h"

extern void draw_triangle(void);

void frame_new(void *data, struct wl_callback *callback, uint32_t cb_data);

void frame_new(void *data, struct wl_callback *callback, uint32_t cb_data) {
    struct state *s = (struct state *) data;
    wl_callback_destroy(callback);
    callback = wl_surface_frame(s->surface);
    wl_callback_add_listener(callback, &g_wl_callback_listener, data);

    draw_triangle();
}

struct wl_callback_listener g_wl_callback_listener = {.done = frame_new};
