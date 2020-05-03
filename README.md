# Lucurious
![Example 1](https://66.media.tumblr.com/76896cad1a8b07abc2ddd331dcc7403b/tumblr_oun56jf9mK1u2y46bo4_1280.jpg)
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](#license)

**Note: This is one mans attempt at something that could be cool. This is by no means official industry level software. This is just something I'm doing on the side for fun. IT WILL NOT BE DOCUMENTED!!! The only documentation you will find is from comments in the code. Also the API function calls parameters will be constantly changing as I continue to expand the functionality of the vulkan renderer. Yes, I am still learning all of these APIs**

Inspired by the t.v series The Expanse and other syfy t.v shows/movies.

Lucurious (L) Lowkey (U) unsure and curious if this is even possible, but why stop now!!! It\'s currently a library for building and styling 2D/3D Vulkan Wayland Compositors, that look similar to [wayfire](https://wayfire.org/)/[wayfire github](https://github.com/WayfireWM/wayfire). It'll later be transformed into a Desktop Engine. It uses [Vulkan](https://www.khronos.org/vulkan/) as its primary renderer and does operations over the Wayland display server protocol to assist in building optimal, memory efficient, easy to use desktop environments that may appear to be straight from science fiction.

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
* [libshaderc](https://github.com/google/shaderc)
* [SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools) (Required by glslang and libshaderc)

### [Installation/Usage/Examples](https://github.com/EasyIP2023/lucurious-examples/)
### [Development](https://github.com/EasyIP2023/lucurious/tree/development/src)
### [Testing](https://github.com/EasyIP2023/lucurious/tree/development/tests)
### [Benchmarks](https://github.com/EasyIP2023/lucurious-benchmarks)

## Useful Sites
* [Writing an efficient Vulkan renderer](https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer/)
* [Wayland freedesktop](https://wayland.freedesktop.org/)
* [An introduction to Wayland](https://drewdevault.com/2017/06/10/Introduction-to-Wayland.html)
* [Weston](https://github.com/wayland-project/weston)
* [Programming Wayland Clients](https://jan.newmarch.name/Wayland/index.html)
* [Emersion Hello Wayland](https://github.com/emersion/hello-wayland)
* [Direct Rendering Manager](https://dri.freedesktop.org/wiki/DRM/)
* [Vulkan](https://vulkan.lunarg.com)
* [Vulkan Tutorial](https://vulkan-tutorial.com/)
* [Vulkan Api Without Secrets](https://software.intel.com/en-us/articles/api-without-secrets-introduction-to-vulkan-part-1)
* [Vulkan Validation Layers](https://gpuopen.com/using-the-vulkan-validation-layers/)
* [MVP](https://jsantell.com/model-view-projection)
