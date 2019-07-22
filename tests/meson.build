
subdir('shaders')

check = dependency('check')
valgrind = find_program('valgrind', required : false)

if valgrind.found()
  valgrind_env = environment()
  valgrind_env.set('LTEST_JOBS', '4')
  #valgrind_suppressions_file = 'valgrind.suppressions'
  add_test_setup('valgrind',
      exe_wrapper : [valgrind,
          '--leak-check=full',
          '--show-leak-kinds=all',
          '--track-origins=yes',
          '--gen-suppressions=all',
          '--error-exitcode=3'],
          #'--suppressions=' + valgrind_suppressions_file],
      env :  valgrind_env,
      timeout_multiplier : 100)
else
  warning('valgrind not found, disabling valgrind test suite')
endif

# Testing vulkan suite
lucur_vulkan_test = executable('lucur-vulkan-test',
  'test-vulkan.c',
  include_directories : lucur_inc,
  dependencies: [check],
  link_with : lib_lucur,
  install : false)

test('lucur-vulkan-test',
    lucur_vulkan_test,
    suite: ['all'])

# Testing Wayland suite
lucur_wclient_test = executable('lucur-wclient-test',
  'test-wclient.c',
  include_directories : lucur_inc,
  dependencies: [check],
  link_with : lib_lucur,
  install : false)

test('lucur-wclient-test',
    lucur_wclient_test,
    suite: ['all'])

# Testing shade suite
lucur_vulkan_test = executable('lucur-shade-test',
  'test-shade.c',
  include_directories : lucur_inc,
  dependencies: [check,shaderc],
  link_with : lib_lucur,
  install : false)

test('lucur-shade-test',
    lucur_vulkan_test,
    suite: ['all'])

# Testing main wayland client later compositor build with executable
lucur_main_test = executable('lucur-main-test',
                  'test-main.c',
                  include_directories : lucur_inc,
                  dependencies : [check],
                  link_with : lib_lucur,
                  install : false)

test('lucur-main-test',
    lucur_main_test,
    suite: ['all'])
