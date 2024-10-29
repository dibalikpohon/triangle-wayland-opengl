#include <stdint.h>
#include <stdio.h>
#include <wayland-client-protocol.h>

#include "state.h"

static void wl_keyboard_listener_keymap(void *data,
                                        struct wl_keyboard *wl_keyboard,
                                        uint32_t format, int32_t fd,
                                        uint32_t size) { }

static void wl_keyboard_listener_enter(void *data,
                                       struct wl_keyboard *wl_keyboard,
                                       uint32_t serial,
                                       struct wl_surface *surface,
                                       struct wl_array *keys) { }

static void wl_keyboard_listener_leave(void *data,
                                       struct wl_keyboard *wl_keyboard,
                                       uint32_t serial,
                                       struct wl_surface *surface) { }

static void wl_keyboard_listener_key(void *data,
                                     struct wl_keyboard *wl_keyboard,
                                     uint32_t serial, uint32_t time,
                                     uint32_t key, uint32_t state) {
    struct state *s = (struct state*) data;
    // s = NULL;   // Uncomment this to prevent segfaults (wink)
    if (key == 1) { // Escape Key
        puts("Quitting...");
        s->closed = 1;
    }
}

static void wl_keyboard_listener_modifiers(
    void *data, struct wl_keyboard *wl_keyboard, uint32_t serial,
    uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked,
    uint32_t group) { }

static void wl_keyboard_listener_repeat_info(void *data,
                                             struct wl_keyboard *wl_keyboard,
                                             int32_t rate, int32_t delay) { }

struct wl_keyboard_listener g_wl_keyboard_listener = {
    .keymap = wl_keyboard_listener_keymap,
    .enter = wl_keyboard_listener_enter,
    .leave = wl_keyboard_listener_leave,
    .key = wl_keyboard_listener_key,
    .modifiers = wl_keyboard_listener_modifiers,
    .repeat_info = wl_keyboard_listener_repeat_info
};
