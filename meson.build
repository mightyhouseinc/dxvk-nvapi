project('dxvk-nvapi',
  ['cpp'],
  default_options: [
    'cpp_std=c++17',
    'warning_level=2'
  ],
  version : 'v0.0',
  meson_version : '>= 0.58.0')

cpp = meson.get_compiler('cpp')

compiler_args = ['-Wno-unused-parameter']
if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
  compiler_args += [
    '-gdwarf-4',
    '-Wa,-mbig-obj'
  ]
endif

link_args = [
    '-static',
    '-static-libgcc',
    '-static-libstdc++',
    '-Wl,--file-alignment=4096'
]

add_project_arguments(cpp.get_supported_arguments(compiler_args), language: 'cpp')
add_project_link_arguments(cpp.get_supported_link_arguments(link_args), language: 'cpp')

dxvk_cpu_family = target_machine.cpu_family()
if dxvk_cpu_family == 'x86_64'
  target_suffix = '64'
else
  target_suffix = ''
endif

vk_headers = include_directories('./external/Vulkan-Headers/include')

lib_dxgi = cpp.find_library('dxgi')
lib_d3d11 = cpp.find_library('d3d11')
lib_version = cpp.find_library('version')

dxvk_nvapi_version = vcs_tag(
  command: ['git', 'describe', '--always', '--tags', '--dirty=+'],
  input:  'version.h.in',
  output: 'version.h')

subdir('src')

enable_tests = get_option('enable_tests')
if enable_tests and dxvk_cpu_family == 'x86_64' # compiling tests for x86 fails, dunno why...
  subdir('tests')
endif
