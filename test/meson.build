
test_src = ['test-runner.c', 'vlucur-test.c', 'client-test.c']

testexe = executable('ltest',
                    test_src,
                    include_directories : lucur_inc,
                    link_with : lib_lucur)

test('lucurious test', testexe)
