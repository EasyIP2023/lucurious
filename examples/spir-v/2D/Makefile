
CC=cc -Wall -Wextra
FLAGS=`pkgconf --cflags --libs lucurious`

all: glslang_to_spirv
	$(CC) $(FLAGS) simple_example.c -o se

glslang_to_spirv:
	glslangValidator -V ../shaders/2Dshader.frag
	glslangValidator -V ../shaders/2Dshader.vert

clean:
	$(RM) se
	$(RM) *.spv
