project('ft_vox', 'cpp',
	version: '0.1',
	default_options: [
	  'warning_level=3',
	  'cpp_std=gnu++17',
	  'cpp_args=-Wno-class-memaccess',
	  'c_std=gnu17',
	  'c_args=-Wno-pedantic -Wno-unused-parameter -Wno-sign-compare -Wno-format-truncation',
	  'werror=false'])

srcs = [
	'src/main.cpp',
	'src/SimplexNoise.cpp',
	'src/Chunk.cpp',
	'src/ChunkRenderer.cpp',
	'src/ChunkFactory.cpp',
	'src/TextRenderer.cpp',
	'src/FPSCounter.cpp',
	'src/WorldRenderer.cpp',
	'src/World.cpp',
	'src/ChunkBuilder.cpp',
	'src/WorldGenerator.cpp',
	'src/stb_image.cpp',
	'src/ChunkMesh.cpp',
	'src/Settings.cpp',
	'src/imgui/imgui.cpp',
	'src/imgui/imgui_demo.cpp',
	'src/imgui/imgui_draw.cpp',
	'src/imgui/imgui_widgets.cpp',
	'src/imgui/imgui_impl_opengl3.cpp',
	'src/imgui/imgui_impl_glfw.cpp',
]

cmake = import('cmake')

lazyGL = cmake.subproject('LazyGL')

dep_lazyGL = lazyGL.dependency('LazyGL')
glew = dependency('glew', required : true)
glfw = dependency('glfw3', required : true)
pthread = dependency('threads', required : true)
gl = dependency('GL', required : true)
x11 = dependency('x11', required : true)
xi = dependency('xi', required : true)
xrandr = dependency('xrandr', required : true)
dl = meson.get_compiler('cpp').find_library('dl')
m = meson.get_compiler('cpp').find_library('m')
ft = meson.get_compiler('cpp').find_library('freetype')

executable('ft_vox',
	srcs,
	include_directories : include_directories(['src/', '/usr/include/freetype2/']),
	dependencies: [ dep_lazyGL, pthread, glew, glfw, x11, xi, xrandr, dl, m, ft ])
