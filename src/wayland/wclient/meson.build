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
