
test_src = ['vlucur-test.c']

testexe = executable('ltest',
                    test_src,
                    include_directories : lucur_inc,
                    link_with : lib_lucur)

test('lucurious test', testexe)
