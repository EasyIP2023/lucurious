# Lucurious
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](#license)

Inspired by the t.v series expanse and other syfy t.v shows/movies.

Lucurious (L) Lowkey (U) unsure and curious if this is even possible, but why stop now!!! It\'s an library for building and styling 2D/3D Wayland Compositors. Will be using [Vulkan](https://vulkan.lunarg.com) and the Wayland protocol to turn the nonexistent UI into reality.

## Dependencies
* wayland-client
* wayland-server
* wayland-protocols
* wayland-scanner
* vulkan (Install your graphics card driver)
* vulkan-icd-loader
* vulkan-headers
* vulkan-sdk
* [CGLM (for linear algebra)](https://github.com/recp/cglm)
* libinput

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
