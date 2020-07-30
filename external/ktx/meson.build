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

# Install libktx headers
ktx_dir = '/usr/include/'
install_headers('include/ktx.h', install_dir: ktx_dir)
install_headers('include/ktxvulkan.h', install_dir: ktx_dir)

fs = [
  'lib/checkheader.c',
  'lib/filestream.c',
  'lib/filestream.h',
  'lib/hashlist.c',
  'lib/ktxint.h',
  'lib/memstream.c',
  'lib/memstream.h',
  'lib/stream.h',
  'lib/swap.c',
  'lib/texture.c',
  'lib/uthash.h'
]

lib_ktx = library(
  'ktx', files(fs), version: '2.0',
  include_directories: ['include', 'other_include'],
  dependencies: [libmath], install_dir: '/usr/lib',
  install: true
)

# Unused Files
#  'lib/errstr.c',
#  'lib/etcdec.cxx',
#  'lib/etcunpack.cxx',
#  'lib/gl_format.h',
#  'lib/gl_funcptrs.h',
#  'lib/gles1_funcptrs.h',
#  'lib/gles2_funcptrs.h',
#  'lib/gles3_funcptrs.h',
#  'lib/glloader.c',
#  'lib/hashtable.c',
#  'lib/ktxgl.h',
#  'lib/writer.c',
#  'lib/writer_v1.c',
#  'lib/vk_format.h',
#  'lib/vkloader.c',
#  'lib/vk_funcs.c',
#  'lib/vk_funcs.h'
#  'lib/libktx.gypi'
#  'lib/vk_funclist.inl'