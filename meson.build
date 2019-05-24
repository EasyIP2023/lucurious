project(
	'lucurious',
	'c',
	version: '1.0.0',
	license: 'MIT',
	meson_version: '>=0.49.2',
	default_options: [
		'c_std=c11',
		'warning_level=2',
		'werror=true',
	],
)

so_version = ['1']

cc = meson.get_compiler('c')

add_global_arguments('-D_POSIX_C_SOURCE=200809L', language : 'c')

glfw3 = dependency('glfw3')
vulkan = dependency('vulkan')
cglm = dependency('cglm')

executable(
 	'lucur',
 	[ 'src/main.c' ],
  dependencies: [glfw3, vulkan, cglm]
)
