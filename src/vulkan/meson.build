vulkan = dependency('vulkan')
cglm = dependency('cglm')
math = cc.find_library('m')

lib_vulkan = static_library(
	'lvulkan',
	files(
		'setup.c', 'display.c', 'devices.c',
		'graphics_pipeline.c', 'utils.c',
		'matrix.c', 'vlayer.c'
	),
	include_directories: lucur_inc,
	dependencies: [vulkan,cglm,math]
)
