#
# The MIT License (MIT)
#
# Copyright (c) 2019 Vincent Davis Jr.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

project(
  'lucurious',
  'c',
  version: '0.0.2',
  license: 'MIT',
  meson_version: '>=0.54.0',
  default_options: [
    'c_std=gnu18',
    'warning_level=2',
    'werror=false'
  ],
)

so_version = ['1']
cc = meson.get_compiler('c')

add_project_arguments(
  [
    '-DLUCUR_VERSION="@0@"'.format(meson.project_version()),
    '-DDLU_DIR_SRC="@0@"'.format(meson.current_source_dir())
  ],
  language : 'c'
)

lucur_inc = include_directories('include', '/usr/include/libdrm')

subdir('include')
subdir('src')
subdir('external')

lucur_parts = [
  lib_utils, lib_shade,
  lib_math, lib_vkcomp,
  lib_drm
]

lib_lucur = library(
  meson.project_name(),
  version: '.'.join(so_version),
  link_whole: lucur_parts,
  include_directories: lucur_inc,
  install_dir: '/usr/lib',
  install: true
)

executable('lucur',
  exec_files,
  link_with: lib_lucur,
  include_directories: lucur_inc,
  install_dir: '/usr/bin',
  install: true
)

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  libraries: lib_lucur,
  version: meson.project_version(),
  name: meson.project_name(),
  filebase:  meson.project_name(),
  install_dir: '/usr/lib/pkgconfig',
  description: 'Library for building advanced DRM Vulkan renderers that work directly with KMS'
)

# Test Vulkan render examples
check = dependency('check', required : false)
wayland_client = dependency('wayland-client', required : false)
if wayland_client.found() and check.found()
  subdir('tests')
endif