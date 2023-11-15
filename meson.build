project(
  'titac',
  'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

deps = [ dependency('raylib') ]

exe = executable(
  'titac',
  'titac.c',
  dependencies : deps,
  install : false
)

test('basic', exe)
