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

i_dir = '/usr/include/wlu/'
install_headers('lucurious.h', install_dir: i_dir)

############################
# Installing utils headers #
############################
utils_hs = ['utils/log.h', 'utils/mm.h', 'utils/types.h']
install_headers(utils_hs, install_dir: i_dir + 'utils')

#############################
# Installing shader headers #
#############################
shader_hs = ['shader/file.h', 'shader/shade.h', 'shader/types.h']
install_headers(shader_hs, install_dir: i_dir + 'shader')

#############################
# Installing vkcomp headers #
#############################
vkcomp_i_dir = i_dir + 'vkcomp/'
vkcomp_main_hs = ['vkcomp/all.h', 'vkcomp/types.h']
install_headers(vkcomp_main_hs, install_dir: vkcomp_i_dir)

#######################################
# Installing vkcomp functions headers #
#######################################
vkcomp_func_hs = [
  'vkcomp/funcs/copy.h', 'vkcomp/funcs/create.h', 'vkcomp/funcs/device.h',
  'vkcomp/funcs/display.h', 'vkcomp/funcs/exec.h', 'vkcomp/funcs/set.h',
  'vkcomp/funcs/setup.h', 'vkcomp/funcs/vlayer.h'
]
install_headers(vkcomp_func_hs, install_dir: vkcomp_i_dir + 'funcs')

###############################################
# Installing vkcomp graphics pipeline headers #
###############################################
vkcomp_gp_hs = [
  'vkcomp/gp/bind.h', 'vkcomp/gp/cmd.h', 'vkcomp/gp/create.h',
  'vkcomp/gp/exec.h', 'vkcomp/gp/set.h', 'vkcomp/gp/update.h'
]
install_headers(vkcomp_gp_hs, install_dir: vkcomp_i_dir + 'gp')

####################################
# Installing vkcomp matrix headers #
####################################
vkcomp_matrix_hs = ['vkcomp/matrix/print.h', 'vkcomp/matrix/set.h']
install_headers(vkcomp_matrix_hs, install_dir: vkcomp_i_dir + 'matrix')

##############################
# Installing wayland headers #
##############################
wayland_i_dir = i_dir + 'wayland/'
wayland_main_hs = ['wayland/all.h', 'wayland/types.h']
install_headers(wayland_main_hs, install_dir: wayland_i_dir)

#####################################
# Installing wayland client headers #
#####################################
wayland_client_hs = ['wayland/wclient/setup.h', 'wayland/wclient/create.h']
install_headers(wayland_client_hs, install_dir: wayland_i_dir + 'wclient')
