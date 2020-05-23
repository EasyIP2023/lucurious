#
# The MIT License (MIT)
#
# Copyright (c) 2019-2020 Vincent Davis Jr.
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

subdir('wayland')

convert = find_program('convert', required: false)

lucur_alloc_test = executable('lucur-alloc-test',
  'test-alloc.c', include_directories: lucur_inc,
  dependencies: [check], link_with: [lib_lucur],
  c_args: '-DDEV_ENV --std=gnu18',
  install: false
)

lucur_drm_basic_test = executable('lucur-drm-basic-test',
  'test-drm-basics.c', include_directories: lucur_inc,
  dependencies: [check], link_with: [lib_lucur],
  c_args: '-DDEV_ENV --std=gnu18',
  install: false
)

# Testing vulkan suite
lucur_vulkan_test = executable('lucur-vulkan-test',
  'test-vulkan.c', include_directories: lucur_inc,
  dependencies: [check], link_with: [lib_lucur, lib_lwayland],
  c_args: '-DDEV_ENV --std=gnu18',
  install: false
)

# Testing shade suite
lucur_shade_test = executable('lucur-shade-test',
  'test-shade.c', include_directories: lucur_inc,
  dependencies: [check], link_with: [lib_lucur],
  c_args: '-DDEV_ENV --std=gnu18',
  install: false
)

lucur_triangle_test = executable('lucur-triangle-test',
  'test-triangle.c', include_directories: lucur_inc,
  dependencies: [check], link_with: [lib_lucur, lib_lwayland],
  c_args: '-DDEV_ENV --std=gnu18',
  install: false
)

lucur_square_test = executable('lucur-square-test',
  'test-square.c', include_directories: lucur_inc,
  dependencies: [check], link_with: [lib_lucur, lib_lwayland],
  c_args: '-DDEV_ENV --std=gnu18',
  install: false
)

lucur_cube_test = executable('lucur-cube-test',
  'test-cube.c', include_directories: lucur_inc,
  dependencies: [check], link_with: [lib_lucur, lib_lwayland],
  c_args: '-DDEV_ENV --std=gnu18',
  install: false
)

lucur_rotate_rect_test = executable('lucur-rotate-rect-test',
  'test-rotate-rect.c', include_directories: lucur_inc,
  dependencies: [check], link_with: [lib_lucur, lib_lwayland],
  c_args: '-DDEV_ENV --std=gnu18',
  install: false
)

test('lucur-alloc-test', lucur_alloc_test, suite: ['all', 'alloc'])
test('lucur-drm-basic-test', lucur_drm_basic_test, suite: ['all'])
test('lucur-vulkan-test', lucur_vulkan_test, suite: ['all'])
test('lucur-shade-test', lucur_shade_test, suite: ['all'])
test('lucur-triangle-test', lucur_triangle_test, suite: ['all', 'images'])
test('lucur-square-test', lucur_square_test, suite: ['all', 'images'])
test('lucur-cube-test', lucur_cube_test, suite: ['all', 'images'])
test('lucur-rotate-rect-test', lucur_rotate_rect_test, suite: ['all', 'images'])

if convert.found()
  run_command('convert', 'textures/texture.jpg', '-define', 'h:format=bgra', '-depth', '8', 'textures/texture.h')

  it_files = ['test-image-texture.c', 'helpers.c']
  lucur_img_texture_test = executable('lucur-img-texture-test', it_files, include_directories: lucur_inc,
    dependencies: [check], link_with: [lib_lucur, lib_lwayland], c_args: '-DDEV_ENV --std=gnu18', install: false)

  test('lucur-img-texture-test', lucur_img_texture_test, suite: ['all', 'images'])
endif

