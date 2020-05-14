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

wayland_protos = dependency('wayland-protocols', required : false)
wayland_scanner = find_program('wayland-scanner', required : false)
wayland_client = dependency('wayland-client', required : false)
wayland_protos_dir = wayland_protos.get_pkgconfig_variable('pkgdatadir')

wayland_protos = [
  [wayland_protos_dir, 'stable/xdg-shell/xdg-shell.xml'],
]

wayland_scanner_code = generator(
  wayland_scanner,
  output: '@BASENAME@-protocol.c',
  arguments: ['private-code', '@INPUT@', '@OUTPUT@'],
)

wayland_scanner_client = generator(
  wayland_scanner,
  output: '@BASENAME@-client-protocol.h',
  arguments: ['client-header', '@INPUT@', '@OUTPUT@'],
)

# Create corresponding C glue code from wayland xml protocls extension files
wayland_cglue = []
foreach p : wayland_protos
  xml = join_paths(p)
  wayland_cglue += wayland_scanner_client.process(xml)
  wayland_cglue += wayland_scanner_code.process(xml)
endforeach

lib_lwayland = static_library(
  'lwayland',
  wayland_cglue,
  files('create.c', 'setup.c'),
  include_directories: lucur_inc,
  dependencies: [wayland_client]
)
