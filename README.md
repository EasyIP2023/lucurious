# Lucurious
[//]: <> (GIF taken from https://gmunk.com/OBLIVION-GFX)
<p align="center"><img src="https://github.com/EasyIP2023/lucurious/blob/development/OBLVN_GFX_CHAN_01.gif" alt="SyFy User Interfaces"></p
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](#license)

**Note: This library is meant to be purely experimental and for educational purposes only**

**tested on**
  * Ubuntu 20.04
  * Arch Linux

Inspired by the t.v series the expanse and other syfy t.v shows/movies.

Lucurious (L) Lowkey (U) unsure and curious if this is even possible, but why stop now!!! Essentially it will be an easy to use library for building highly
sophisticated and advanced [Vulkan](https://www.khronos.org/vulkan/) DRM renderers. These renderers allow for easy creation of interactable single application
compositors that appear to be straight out of science fiction.

## What I am currently thinking
* [ISSUE: Remove VkSurfaceKHR and SwapChain Extension code](https://github.com/EasyIP2023/lucurious/issues/40)
* [VK_EXT_image_drm_format_modifier](https://gitlab.freedesktop.org/mesa/mesa/-/merge_requests/1466) extension in mesa (Almost implemented)

## Known Issues
* kms-novulkan crashes on (Arch Linux, Ubuntu 20.04)
* rotate-rectangle example crashes on Ubuntu 20.04

## Build/Install

**arch linux**
```
$ pacman -S lucurious-git
```

**Debian**
```
$ sudo apt install -y python3-pip pkg-config libdrm-dev libinput-dev libgbm-dev libudev-dev libsystemd-dev vulkan-validationlayers ninja-build
$ sudo python3 -m pip install meson

# Install google libshaderc
$ git clone --branch v2021.2 https://github.com/google/shaderc ; cd shaderc
$ ./utils/git-sync-deps
$ mkdir build ; cd build
$ cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
$ ninja
$ sudo ninja install

# Install libcglm
$ git clone -b v0.8.4 https://github.com/recp/cglm ; cd cglm
$ mkdir build
$ meson build/
$ sudo ninja -C build/ install

# Optional Testing
$ git clone -b 0.15.2 https://github.com/libcheck/check ; cd check
$ mkdir build ; cd build
$ cmake ..
$ make -j$(nproc)
$ sudo make install
```

### [Usage/Examples](https://github.com/EasyIP2023/lucurious-examples/)
### [Documentation](https://github.com/EasyIP2023/lucurious-docs/)
### [Development](https://github.com/EasyIP2023/lucurious/tree/development/src)
### [Testing](https://github.com/EasyIP2023/lucurious/tree/development/tests)

## Useful Links
* [kms-cube](https://gitlab.freedesktop.org/mesa/kmscube)
* [kms-quads](https://gitlab.freedesktop.org/daniels/kms-quads)
* [Writing an efficient Vulkan renderer](https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer/)
* [Direct Rendering Manager](https://dri.freedesktop.org/wiki/DRM/)
* [Vulkan](https://vulkan.lunarg.com)
* [Vulkan Tutorial](https://vulkan-tutorial.com/)
* [Vulkan LunarG Tutorial](https://vulkan.lunarg.com/doc/sdk/1.2.141.2/linux/tutorial/html/index.html)
* [Vulkan Api Without Secrets](https://software.intel.com/en-us/articles/api-without-secrets-introduction-to-vulkan-part-1)
* [Vulkan Validation Layers](https://gpuopen.com/using-the-vulkan-validation-layers/)
* [I am graphics and so can you](https://www.fasterthan.life/blog/2017/7/12/i-am-graphics-and-so-can-you-part-3-breaking-ground)
* [MVP](https://jsantell.com/model-view-projection)
* [OpenGL MVP](https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/)
