# Lucurious
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](#license)

Inspired by the t.v series expanse and other syfy t.v shows/movies.

Lucurious (L) Lowkey (U) unsure and curious if this is even possible, but why stop now!!! It\'s a library for building and styling 2D/3D Wayland Compositors, like for example [wayfire](https://wayfire.org/)/[wayfire github](https://github.com/WayfireWM/wayfire). Will be using [Vulkan](https://vulkan.lunarg.com) and the Wayland protocol to turn the nonexistent UI into reality.

I'm not using [wlroots](https://github.com/swaywm/wlroots) because I wanted to learn more about the inner workings of the wayland protocol and how the linux graphics stack works, plus I'm working on this for my master thesis. [Wlroots](https://github.com/swaywm/wlroots) could be used in this project as it is an extremely good wayland compositing library and I recommend usage if you want to build your own compositor without dealing with the underlying wayland compositor features.

## Dependencies
* wayland-client
* wayland-server
* wayland-protocols
* wayland-scanner
* vulkan (Install your graphics card driver)
* vulkan-icd-loader
* vulkan-headers
* vulkan-sdk
* libinput
* [CGLM (for linear algebra)](https://github.com/recp/cglm)
* [SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools)
* [Spirv-Headers](https://github.com/KhronosGroup/SPIRV-Headers)

**To install**
```bash
mkdir -v build
meson build
ninja install -C build

# Encase of PolicyKit daemon errors
pkttyagent -p $(echo $$) | pkexec ninja install -C $(pwd)/build/
```

**To Uninstall**
```bash
ninja uninstall -C build

# Encase of PolicyKit daemon errors
pkttyagent -p $(echo $$) | pkexec ninja uninstall -C $(pwd)/build/
```

**If need to reconfigure build**
```bash
meson --reconfigure build
```

**If need to wipe build**
```bash
meson --wipe build
```

**Running test**
```bash
ninja test -C build
cat build/meson-logs/testlog.txt
```

**Usage**
```bash
# This is just an extra step
export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH
cc -L /usr/local/lib/ -l lucurious test.c -o test
```

## Development
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

## References
* [Wayland freedesktop](https://wayland.freedesktop.org/)
* [An introduction to Wayland](https://drewdevault.com/2017/06/10/Introduction-to-Wayland.html)
* [Weston](https://github.com/wayland-project/weston)
* [Programming Wayland Clients](https://jan.newmarch.name/Wayland/index.html)
* [Swaywm](https://github.com/swaywm)
* [Emersion Hello Wayland](https://github.com/emersion/hello-wayland)
* [Direct Rendering Manager](https://dri.freedesktop.org/wiki/DRM/)
* [vulkan](https://vulkan.lunarg.com)
* [Vulkan Tutorial](https://vulkan-tutorial.com/)
* [Vulkan Api Without Secrets](https://software.intel.com/en-us/articles/api-without-secrets-introduction-to-vulkan-part-2)
