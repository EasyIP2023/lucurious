
glslang_validator = find_program('glslangValidator', required : true)

tests_dir = meson.current_source_dir()
build_dir = meson.current_build_dir()

shaders = [
	[tests_dir, 'shader.frag'],
	[tests_dir, 'shader.vert'],
]

foreach s : shaders
	shade = join_paths(s)
	run_command(glslang_validator, '-V', shade)
endforeach

run_command('bash', 'mv.sh', build_dir)
