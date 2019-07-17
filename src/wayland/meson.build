wayland_client = dependency('wayland-client')
wayland_server = dependency('wayland-server')
wayland_protos = dependency('wayland-protocols')
wayland_scanner = find_program('wayland-scanner', required : true)

wayland_protos_dir = wayland_protos.get_pkgconfig_variable('pkgdatadir')

wayland_scanner_server = generator(
	wayland_scanner,
	output: '@BASENAME@-protocol.h',
	arguments: ['server-header', '@INPUT@', '@OUTPUT@'],
)

wayland_scanner_client = generator(
	wayland_scanner,
	output: '@BASENAME@-client-protocol.h',
	arguments: ['client-header', '@INPUT@', '@OUTPUT@'],
)

wayland_scanner_code = generator(
	wayland_scanner,
	output: '@BASENAME@-protocol.c',
	arguments: ['private-code', '@INPUT@', '@OUTPUT@'],
)

subdir('wserver')
subdir('wclient')

wayland_protos_src = []

foreach p : server_protocols
	xml = join_paths(p)
	wayland_protos_src += wayland_scanner_code.process(xml)
endforeach

lib_lwayland = static_library('lwayland', wayland_protos_src)
