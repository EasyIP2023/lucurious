server_protocols = [
	[wayland_protos_dir, 'stable/xdg-shell/xdg-shell.xml'],
]

wserver_protos_headers = []

foreach p : server_protocols
	xml = join_paths(p)
	wserver_protos_headers += wayland_scanner_server.process(xml)
endforeach

lib_wserver = static_library(
	'wserver',
  wserver_protos_headers,
	include_directories: lucur_inc,
	dependencies: [wayland_server]
)
