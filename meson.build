project('triangle-wayland-opengl', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

waylandc = dependency('wayland-client', required: true)
egl = dependency('egl', required: true)
wl_egl = dependency('wayland-egl', required: true)
opengl = dependency('opengl', required: true)

subdir('src/glad')

sources = [
  'src/egl-thing.c',
  'src/triangle_wayland_opengl.c', 
  'src/wl-callback.c',
  'src/wl-kbconfig.c',
  'src/wl-registry.c',
  'src/wl-seat.c',
  'src/xdg-config.c',
  'src/xdg-protocol.c'
]

exe = executable('triangle-wayland-opengl', sources,
  dependencies: [waylandc, egl, wl_egl, opengl],
  link_with: [glad],
  c_args: ['-Wno-pedantic'])

