#
# The MIT License (MIT)
#
# Copyright (c) 2019 Lucurious Labs
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
valgrind = find_program('valgrind', required : false)

if valgrind.found()
  valgrind_env = environment()
  valgrind_env.set('LTEST_JOBS', '4')
  #valgrind_suppressions_file = 'valgrind.suppressions'
  add_test_setup('valgrind',
      exe_wrapper : [valgrind,
          '--leak-check=full',
          '--show-leak-kinds=all',
          '--track-origins=yes',
          '--gen-suppressions=all',
          '--error-exitcode=3'],
          #'--suppressions=' + valgrind_suppressions_file],
      env :  valgrind_env,
      timeout_multiplier : 100)
else
  warning('valgrind not found, disabling valgrind test suite')
endif

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

# Testing main 3d wayland client later compositor build with executable
lucur_main_3d_test = executable('lucur-main-3d-test',
  'test-3D-main.c',
  include_directories : lucur_inc,
  dependencies : [check,shaderc],
  link_with : lib_lucur,
  link_args : extra_args,
  install : false
)

# Testing main wayland client later compositor build with executable
lucur_main_test = executable('lucur-main-test',
  'test-main.c',
  include_directories : lucur_inc,
  dependencies : [check,shaderc],
  link_with : lib_lucur,
  link_args : extra_args,
  install : false
)

test('lucur-vulkan-test', lucur_vulkan_test, suite: ['all'])
test('lucur-wclient-test', lucur_wclient_test, suite: ['all'])
test('lucur-shade-test', lucur_shade_test, suite: ['all'])
test('lucur-main-3d-test', lucur_main_3d_test, suite: ['all'])
test('lucur-main-test', lucur_main_test, suite: ['all'])
