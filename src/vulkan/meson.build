vulkan = dependency('vulkan')
cglm = dependency('cglm')

lib_vulkan = static_library(
	'lvulkan',
	files(
		'vkall.c',
	),
	include_directories: lucur_inc,
	dependencies: [vulkan,cglm]
)
