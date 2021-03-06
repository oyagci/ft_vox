project('ft_vox', 'cpp',
	version: '0.1',
	default_options: [
	  'warning_level=3',
	  'cpp_std=gnu++17',
	  'cpp_args=-Wno-class-memaccess',
	  'c_std=gnu17',
	  'c_args=-Wno-pedantic -Wno-unused-parameter -Wno-sign-compare -Wno-format-truncation',
	  'werror=false',
	  'optimization=2'])

conf_data = configuration_data()
conf_data.set('version', 'v0.0.2')
configure_inc = '.'

configure_file(input: 'config.h.in',
			   output: 'config.h',
			   configuration: conf_data)

srcs = [
	'src/main.cpp',
	'src/SimplexNoise.cpp',
	'src/Chunk.cpp',
	'src/ChunkRenderer.cpp',
	'src/ChunkFactory.cpp',
	'src/TextRenderer.cpp',
	'src/FPSCounter.cpp',
	'src/World.cpp',
	'src/WorldGenerator.cpp',
	'src/stb_image.cpp',
	'src/Settings.cpp',
	'src/Texture.cpp',
	'src/TextureManager.cpp',
	'src/PlayerHUD.cpp',
	'src/Cubemap.cpp',
	'src/Game.cpp',
	'src/Anchor.cpp',
	'src/ui/Button.cpp',
	'src/ui/UI.cpp',
	'src/ui/SceneComponent.cpp',
	'src/ui/UIScene.cpp',
	'src/ui/MainMenuBackground.cpp',
]

incdirs = [ 'src/' ]
incdirs += configure_inc

cmake = import('cmake')

lazyGL = cmake.subproject('LazyGL')

if build_machine.system() == 'linux'
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

	incdirs += [ '/usr/include/freetype2/' ]

	executable('ft_vox',
		srcs,
		include_directories : include_directories(incdirs),
		dependencies: [ dep_lazyGL, pthread, glew, glfw, x11, xi, xrandr, dl, m, ft ])
elif build_machine.system() == 'darwin'
	dep_lazyGL = lazyGL.dependency('LazyGL')
	glew = dependency('glew', required : true)
	glfw = meson.get_compiler('cpp').find_library('glfw', dirs: '~/.brew/lib')
	pthread = dependency('threads', required : true)
	gl = dependency('GL', required : true)
	dl = meson.get_compiler('cpp').find_library('dl')
	m = meson.get_compiler('cpp').find_library('m')
	ft = meson.get_compiler('cpp').find_library('freetype', dirs: '~/.brew/lib')

	incdirs += [ '/Users/oyagci/.brew/include/freetype2' ]

	executable('ft_vox',
		srcs,
		include_directories : include_directories(incdirs),
		dependencies: [ dep_lazyGL, pthread, glew, glfw, gl, dl, m, ft ])
endif
