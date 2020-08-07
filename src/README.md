# Development

Lucurious uses [valgrind](http://valgrind.org/) for memory leak checks

**To produce valgrind generic output**
```bash
valgrind ./build/tests/${UNIT_TEST_NAME}
```

**To be sure that there are zero memory leaks**
```bash
export CK_FORK=no
valgrind ./build/tests/${UNIT_TEST_NAME}
```

**To produce detailed valgrind error report**
```bash
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./build/tests/${UNIT_TEST_NAME}
```

**To change the level of compiler checkings for warnings**
```bash
meson --warnlevel=2 build/
```