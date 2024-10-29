#include <stdio.h>
#include <string.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include "state.h"
#include "xdg-protocol.h"

extern struct xdg_wm_base_listener g_xdg_wmbase_listener;
extern struct wl_seat_listener g_wl_seat_listener;

static void global_registry_handler(void *data, struct wl_registry *registry,
                                    uint32_t id, const char *interface,
                                    uint32_t version) {
    struct state *s = (struct state *) data;

    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        s->compositor =
            wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        s->xdg_base = wl_registry_bind(registry, id, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(s->xdg_base, &g_xdg_wmbase_listener, NULL);
    } else if (strcmp(interface, wl_seat_interface.name) == 0) {
        s->seat = wl_registry_bind(registry, id, &wl_seat_interface, 1);
        wl_seat_add_listener(s->seat, &g_wl_seat_listener, data);
    }
}

static void global_registry_remove(void *data, struct wl_registry *registry,
                                   uint32_t id) {
    printf("Got a registry losing event for %d\n", id);
}

struct wl_registry_listener g_wl_registry_listener = {
    .global = global_registry_handler, .global_remove = global_registry_remove};
