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

check = dependency('check')

extra_args = ['-g', '-ggdb3']

# Testing vulkan suite
lucur_vulkan_test = executable('lucur-vulkan-test',
  'test-vulkan.c',
  include_directories : lucur_inc,
  dependencies: [check],
  link_with : lib_lucur,
  link_args : extra_args,
  install : false
)

# Testing Wayland suite
lucur_wclient_test = executable('lucur-wclient-test',
  'test-wclient.c',
  include_directories : lucur_inc,
  dependencies: [check],
  link_with : lib_lucur,
  link_args: extra_args,
  install : false
)

# Testing shade suite
lucur_shade_test = executable('lucur-shade-test',
  'test-shade.c',
  include_directories : lucur_inc,
  dependencies: [check,shaderc],
  link_with : lib_lucur,
  link_args : extra_args,
  install : false
)

lucur_triangle_test = executable('lucur-triangle-test',
  'test-triangle.c',
  include_directories : lucur_inc,
  dependencies : [check,shaderc],
  link_with : lib_lucur,
  link_args : extra_args,
  install : false
)

lucur_square_test = executable('lucur-square-test',
  'test-square.c',
  include_directories : lucur_inc,
  dependencies : [check,shaderc],
  link_with : lib_lucur,
  link_args : extra_args,
  install : false
)

lucur_cube_test = executable('lucur-cube-test',
  'test-cube.c',
  include_directories : lucur_inc,
  dependencies : [check,shaderc],
  link_with : lib_lucur,
  link_args : extra_args,
  install : false
)

lucur_spin_square_test = executable('lucur-spin-square-test',
  'test-spin-square.c',
  include_directories : lucur_inc,
  dependencies : [check,shaderc],
  link_with : lib_lucur,
  link_args : extra_args,
  install : false
)

test('lucur-vulkan-test', lucur_vulkan_test, suite: ['all'])
test('lucur-wclient-test', lucur_wclient_test, suite: ['all'])
test('lucur-shade-test', lucur_shade_test, suite: ['all'])
test('lucur-triangle-test', lucur_triangle_test, suite: ['images'])
test('lucur-square-test', lucur_square_test, suite: ['images'])
test('lucur-cube-test', lucur_cube_test, suite: ['images'])
# test('lucur-spin-square-test', lucur_spin_square_test, suite: ['images'])
