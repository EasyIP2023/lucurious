glfw3 = dependency('glfw3')
vulkan = dependency('vulkan')
cglm = dependency('cglm')

lib_vulkan = static_library(
	'lib_vulkan',
	files(
		'hello_triangle_app.c',
		'errors.c',
	),
	include_directories: lucur_inc,
	dependencies: [glfw3,vulkan,cglm]
)
