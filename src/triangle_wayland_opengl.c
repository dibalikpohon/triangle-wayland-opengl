#include <EGL/egl.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-egl.h>

#include "egl-thing.h"
#include "glad/glad/glad.h"
#include "state.h"
#include "wl-callback.h"
#include "wl-registry.h"
#include "xdg-config.h"
#include "xdg-protocol.h"

struct state g = {0};

const int32_t surf_width = 800;
const int32_t surf_height = 600;
const int32_t bit_depth = 4;
const int32_t stride = surf_width * bit_depth;

GLuint g_shader_program = -0;
GLuint g_vao, g_vbo;

static GLuint load_shader(GLuint shader_type, const char *shader_source) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_source, NULL);
    glCompileShader(shader);

    GLint success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        (void) fprintf(
            stderr, "Error: Shader compilation failed.\n%s\n", infoLog);
        return 0;
    }

    return shader;
}

static GLuint create_program(uint64_t shader_count, ...) {
    GLuint program = glCreateProgram();

    va_list args;
    va_start(args, shader_count);
    for (uint64_t i = 0; i < shader_count; ++i) {
        GLuint shader = va_arg(args, GLuint);
        glAttachShader(program, shader);
    }
    va_end(args);

    glLinkProgram(program);
    GLint success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        (void) fprintf(
            stderr, "Error: Shader program linking failed.\n%s\n", infoLog);
        return 0;
    }

    return program;
}

void prepare_triangle(void) {
    // clang-format off
const float tri_vertices[] = {
    // positions            // colors
    -0.5F, -0.5F, 0.0F,     1.0F, 0.0F, 0.0F,
     0.5F, -0.5F, 0.0F,     0.0F, 1.0F, 0.0F,
     0.0F, 0.5F, 0.0F,      0.0F, 0.0F, 1.0F,
};

const char *vertex_shader_source = 
    "#version 330 core\n\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n\n"
    "out vec3 ourColor;\n\n"
    "void main() {\n"
    "gl_Position = vec4(aPos, 1.0);\n"
    "ourColor = aColor;\n"
    "}";

const char *fragment_shader_source =
    "#version 330 core\n\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n\n"
    "void main() {\n"
    "FragColor = vec4(ourColor, 1.0);\n"
    "}";
    // clang-format on

    GLuint vertex_shader = load_shader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fragment_shader =
        load_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
    g_shader_program = create_program(2, vertex_shader, fragment_shader);

    // Delete shader because it is not used anymore (it's already compiled on
    // the program)
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glGenVertexArrays(1, &g_vao);
    glGenBuffers(1, &g_vbo);

    glBindVertexArray(g_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof tri_vertices, tri_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(float),
                          (void *) (0 * sizeof(float)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(float),
                          (void *) (3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void draw_triangle(void) {
    glClearColor(0.302F, 0.244F, 0.482F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_shader_program);

    glBindVertexArray(g_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    eglSwapBuffers(g.egl_display, g.egl_surface);
    EGL_PRINT_ERROR("Swap buffers");
}

int main(int argc, char **argv) {
    g.display = wl_display_connect(NULL);

    if (g.display == NULL) {
        (void) fputs("Cannot connect to display!", stderr);
        return 1;
    }

    struct wl_registry *registry = wl_display_get_registry(g.display);
    wl_registry_add_listener(registry, &g_wl_registry_listener, &g);
    // wl_display_dispatch(g.display);
    wl_display_roundtrip(g.display);

    g.surface = wl_compositor_create_surface(g.compositor);
    struct wl_callback *callback = wl_surface_frame(g.surface);
    wl_callback_add_listener(callback, &g_wl_callback_listener, &g);

    struct xdg_surface *xdg_surface =
        xdg_wm_base_get_xdg_surface(g.xdg_base, g.surface);
    xdg_surface_add_listener(xdg_surface, &g_xdg_surface_listener, &g);

    g.xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
    xdg_toplevel_add_listener(g.xdg_toplevel, &g_xdg_toplevel_listener, &g);
    xdg_toplevel_set_title(g.xdg_toplevel, "Wayland OpenGL Triangle");

    // create_opaque_region();
    init_egl();
    create_egl_surface();

    if (!gladLoadGLLoader((GLADloadproc) eglGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    prepare_triangle();

    puts("Press ESC to quit...");
    while (wl_display_dispatch(g.display)) {
        draw_triangle();
        if (g.closed) break;
    }

    glDeleteVertexArrays(1, &g_vao);
    glDeleteBuffers(1, &g_vbo);
    glDeleteProgram(g_shader_program);

    if (g.buffer) {
        wl_buffer_destroy(g.buffer);
    }

    // don't forget to disconnect
    if (g.keyboard) {
        wl_keyboard_destroy(g.keyboard);
    }
    wl_seat_release(g.seat);
    xdg_toplevel_destroy(g.xdg_toplevel);
    xdg_surface_destroy(xdg_surface);
    wl_surface_destroy(g.surface);
    wl_display_disconnect(g.display);
}
