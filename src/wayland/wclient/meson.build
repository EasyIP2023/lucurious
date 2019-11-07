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

client_protocols = [
	[wayland_protos_dir, 'stable/xdg-shell/xdg-shell.xml'],
]

wclient_protos_headers = []

foreach p : client_protocols
	xml = join_paths(p)
	wclient_protos_headers += wayland_scanner_client.process(xml)
endforeach

lib_wclient = static_library(
	'wclient',
  wclient_protos_headers,
  files(
		'client.c',
		'shm.c',
	),
	include_directories: lucur_inc,
	dependencies: [wayland_client],
)
