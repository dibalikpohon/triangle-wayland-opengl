#include <wayland-client-protocol.h>

#include "state.h"

extern struct wl_keyboard_listener g_wl_keyboard_listener;

static void wl_seat_listener_capabilities(void *data, struct wl_seat *wl_seat,
                                          uint32_t capabilities) {
    struct state *s = (struct state*) data;
    if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
        s->keyboard = wl_seat_get_keyboard(s->seat);
        wl_keyboard_add_listener(s->keyboard, &g_wl_keyboard_listener, data);
    }
}

static void wl_seat_listener_name(void *data, struct wl_seat *wl_seat,
                                  const char *name) { }

struct wl_seat_listener g_wl_seat_listener = {
    .capabilities = wl_seat_listener_capabilities,
    .name = wl_seat_listener_name};
