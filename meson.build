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
  version: '0.0.1',
  license: 'MIT',
  meson_version: '>=0.52.0',
  default_options: [
    'c_std=gnu18',
    'warning_level=2',
    'werror=true',
  ],
)

so_version = ['1']

cc = meson.get_compiler('c')

add_project_arguments(
  [
    '-DLUCUR_VERSION="@0@"'.format(meson.project_version()),
    '-DWLU_DIR_SRC="@0@"'.format(meson.current_source_dir())
  ],
  language : 'c'
)

lucur_inc = include_directories('.', 'include')

subdir('include')
subdir('src')

lucur_parts = [
  lib_utils, lib_shade,
  lib_vkcomp, lib_lwayland
]

lib_lucur = shared_library(
  meson.project_name(),
  version: '.'.join(so_version),
  link_whole: lucur_parts,
  include_directories: lucur_inc,
  install_dir: '/usr/lib',
  install: true
)

lucur = declare_dependency(
  link_with: lib_lucur,
  include_directories: lucur_inc,
)

# Test API calls
subdir('tests')

executable(
  'lucur',
  lib_exec,
  link_with: lib_lucur,
  include_directories: lucur_inc,
  install: true,
  install_dir: '/usr/bin'
)

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  libraries: lib_lucur,
  version: meson.project_version(),
  name: meson.project_name(),
  filebase:  meson.project_name(),
  install_dir: '/usr/lib/pkgconfig',
  description: 'Library for building and styling 2D/3D Wayland Compositors'
)
