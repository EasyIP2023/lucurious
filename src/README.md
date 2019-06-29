# Development

Install [valgrind](http://valgrind.org/) to find memory leaks or memory related issues.

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

**Creating your own cglm.pc**
```bash
sudo vim /usr/lib/pkgconfig/cglm.pc
```
Contents of file
```bash
prefix=/usr
exec_prefix=/usr
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: CGLM-Loader
Description: CGLM Loader
Version: 1.1.107
Libs: -L${libdir} -lcglm
Libs.private:  -lstdc++ -lm -lgcc_s -lgcc -lc -lgcc_s -lgcc
Cflags: -I${includedir}
```
