shaderc = dependency('shaderc')

lib_shade = static_library(
	'lshade',
	files( 'shade.c' ),
	include_directories: lucur_inc,
	dependencies: [shaderc]
)
