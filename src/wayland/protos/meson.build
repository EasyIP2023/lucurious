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

wayland_protos = dependency('wayland-protocols')
wayland_scanner = find_program('wayland-scanner', required : true)
wayland_protos_dir = wayland_protos.get_pkgconfig_variable('pkgdatadir')

wayland_scanner_code = generator(
	wayland_scanner,
	output: '@BASENAME@-protocol.c',
	arguments: ['private-code', '@INPUT@', '@OUTPUT@'],
)

# Create corresponding C glue code from wayland xml protocls extension files
wayland_protos_cglue = []
wayland_protos_shared = [
	[wayland_protos_dir, 'stable/xdg-shell/xdg-shell.xml'],
]

foreach p : wayland_protos_shared
  xml = join_paths(p)
  wayland_protos_cglue += wayland_scanner_code.process(xml)
endforeach

lib_wprotos = static_library('lwayland', wayland_protos_cglue)
