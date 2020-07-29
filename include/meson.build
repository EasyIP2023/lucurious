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

i_dir = '/usr/include/dluc/'
install_headers('lucurious.h', install_dir: i_dir)
install_headers('stb_image.h', install_dir: i_dir)

##########################
# Installing DRM headers #
##########################
drm_hs = ['drm/all.h', 'drm/create.h', 'drm/logind.h', 'drm/device.h', 'drm/info.h', 'drm/setup.h', 'drm/types.h']
install_headers(drm_hs, install_dir: i_dir + 'drm')

############################
# Installing utils headers #
############################
utils_hs = ['utils/all.h', 'utils/log.h', 'utils/mm.h', 'utils/types.h', 'utils/clock.h', 'utils/errors.h']
install_headers(utils_hs, install_dir: i_dir + 'utils')

#############################
# Installing spir-v headers #
#############################
spirv_hs = ['spirv/all.h', 'spirv/file.h', 'spirv/shade.h', 'spirv/types.h']
install_headers(spirv_hs, install_dir: i_dir + 'spirv')

#############################
#  Installing math headers  #
#############################
math_hs = ['math/all.h', 'math/types.h', 'math/print.h', 'math/set.h']
install_headers(math_hs, install_dir: i_dir + 'math')

#############################
# Installing vkcomp headers #
#############################
vkcomp_hs = [
  'vkcomp/all.h', 'vkcomp/types.h', 'vkcomp/set.h', 'vkcomp/create.h', 'vkcomp/exec.h',
  'vkcomp/bind.h', 'vkcomp/update.h', 'vkcomp/display.h', 'vkcomp/setup.h',
  'vkcomp/utils.h', 'vkcomp/vlayer.h', 'vkcomp/vk_calls.h'
]
install_headers(vkcomp_hs, install_dir: i_dir + 'vkcomp')
