project(
	'lucurious',
	'c',
	version: '1.0.0',
	license: 'MIT',
	meson_version: '>=0.50.1',
	default_options: [
		'c_std=c11',
		'warning_level=2',
		'werror=true',
	],
)

so_version = ['1']

cc = meson.get_compiler('c')

add_global_arguments('-D_POSIX_C_SOURCE=200809L', language : 'c')
add_global_link_arguments('-lrt', language : 'c')
add_global_link_arguments(['-g','-ggdb3'], language : 'c')

lucur_inc = include_directories('.','include')

subdir('include')
subdir('src')

lucur_parts = [
	lib_vulkan,
	lib_lwayland,
	lib_wserver,
	lib_wclient,
]

lib_lucur = shared_library(
	meson.project_name(),
	version: '.'.join(so_version),
	link_whole: lucur_parts,
	include_directories: lucur_inc,
	install: true
)

lucur = declare_dependency(
	link_with: lib_lucur,
	include_directories: lucur_inc,
)

subdir('test')

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries: lib_lucur,
                 version: meson.project_version(),
                 name: meson.project_name(),
                 filebase:  meson.project_name(),
                 description: 'Library for building and styling 2D/3D Wayland Compositors')
