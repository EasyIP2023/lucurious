#
# The MIT License (MIT)
#
# Copyright (c) 2019 EasyIP2023
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

vulkan = dependency('vulkan')
cglm = cc.find_library('cglm')
math = cc.find_library('m')

gp_files = [
	'gp/bind.c', 'gp/cmd.c', 'gp/create.c', 'gp/exec.c',
	'gp/set.c', 'gp/update.c'
]

cglm_files = ['matrix/print.c', 'matrix/set.c']

vkcomp_files = [
	'vkcomp/create.c', 'vkcomp/device.c', 'vkcomp/display.c',
	'vkcomp/exec.c', 'vkcomp/setup.c', 'vkcomp/utils.c',
	'vkcomp/vlayer.c', 'vkcomp/copy.c', 'vkcomp/set.c',
	'vkcomp/values.c'
]

lib_vulkan = static_library(
	'lvulkan',
	files(vkcomp_files, cglm_files, gp_files),
	include_directories: lucur_inc,
	dependencies: [vulkan,cglm,math]
)
