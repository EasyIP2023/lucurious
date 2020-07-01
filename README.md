# Lucurious
[//]: <> (GIF taken from https://gmunk.com/OBLIVION-GFX)
<p align="center"><img src="https://github.com/EasyIP2023/lucurious/blob/development/OBLVN_GFX_CHAN_01.gif" alt="SyFy User Interfaces"></p
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](#license)

**Note: This library is meant to be purely experimental and for educational purposes only. Yes, I am still learning all of these APIs**

Inspired by the t.v series the expanse and other syfy t.v shows/movies.

Lucurious (L) Lowkey (U) unsure and curious if this is even possible, but why stop now!!! Essentially it will be an easy to use library for building highly
sophisticated and advanced [Vulkan](https://www.khronos.org/vulkan/) DRM renderers. These renderers allow for easy creation of interactable single application
compositors that appear to be straight out of science fiction.

## Needed Implementations
* [Implement VK_EXT_image_drm_format_modifier in mesa](https://gitlab.freedesktop.org/mesa/mesa/-/merge_requests/1466)

## Dependencies
* vulkan (Install your graphics card driver)
* vulkan-icd-loader
* vulkan-headers
* [CGLM (for linear algebra)](https://github.com/recp/cglm)
* [libshaderc](https://github.com/google/shaderc)
* [SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools) (Required by glslang and libshaderc)
* [libdrm](https://gitlab.freedesktop.org/mesa/drm)
* [libgbm](https://gitlab.freedesktop.org/mesa/mesa/-/tree/master/src/gbm)
* [libinput](https://gitlab.freedesktop.org/libinput/libinput)
* libsystemd
* libudev

### [Installation/Usage/Examples](https://github.com/EasyIP2023/lucurious-examples/)
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