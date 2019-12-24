project('ft_vox', 'cpp',
	version: '0.1',
	default_options: ['warning_level=3', 'cpp_std=c++17'])

srcs = [
	'src/main.cpp',
	'src/core/rendering/RenderingEngine.cpp',
	'src/core/rendering/RenderingPipeline.cpp',
	'src/core/rendering/DeferredRenderer.cpp',
	'src/core/rendering/lights/Light.cpp',
	'src/core/rendering/lights/PointLight.cpp',
	'src/core/rendering/lights/DirectionalLight.cpp',
	'src/core/rendering/primitives/CubePrimitive.cpp',
	'src/core/rendering/primitives/QuadPrimitive.cpp',
	'src/Scene.cpp'
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

executable('ft_vox',
	srcs,
	include_directories : include_directories('src/'),
	dependencies: [ dep_lazyGL, pthread, glew, glfw, x11, xi, xrandr, dl, m ])
