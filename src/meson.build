srcs = [
  'main.cpp',
  'SimplexNoise.cpp',
  'Chunk.cpp',
  'ChunkRenderer.cpp',
  'ChunkFactory.cpp',
  'TextRenderer.cpp',
  'FPSCounter.cpp',
  'World.cpp',
  'WorldGenerator.cpp',
  'stb_image.cpp',
  'Settings.cpp',
  'Texture.cpp',
  'TextureManager.cpp',
  'PlayerHUD.cpp',
  'Cubemap.cpp',
  'Game.cpp',
  'Anchor.cpp',
  'ui/Button.cpp',
  'ui/UI.cpp',
  'ui/SceneComponent.cpp',
  'ui/UIScene.cpp',
  'ui/MainMenuBackground.cpp',
  'ecs/ECSEngine.cpp',
]

cmake = import('cmake')
lazyGL = cmake.subproject('LazyGL')

deps = [
  lazyGL.dependency('LazyGL'),
  dependency('glew', required : true),
  dependency('glfw3', required : true),
  dependency('threads', required : true),
  dependency('GL', required : true),
  meson.get_compiler('cpp').find_library('dl'),
  meson.get_compiler('cpp').find_library('m'),
]

incdirs += [ include_directories('.') ]

if build_machine.system() == 'linux'
  deps_linux = [
	dependency('x11', required : true),
	dependency('xi', required : true),
	dependency('xrandr', required : true),
	meson.get_compiler('cpp').find_library('freetype'),
  ]
  deps += deps_linux
  incdirs += include_directories('/usr/include/freetype2/')
endif
if build_machine.system() == 'darwin'
  deps_macos = [
	meson.get_compiler('cpp')
	  .find_library('freetype', dirs: '~/.brew/lib')
  ]
  deps += deps_macos
  incdirs += include_directories('/Users/oyagci/.brew/include/freetype2') 
endif

executable('ft_vox',
  srcs,
  include_directories : incdirs,
  dependencies: deps
)
