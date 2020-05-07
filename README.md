`# Lucurious
![Example 1](https://66.media.tumblr.com/76896cad1a8b07abc2ddd331dcc7403b/tumblr_oun56jf9mK1u2y46bo4_1280.jpg)
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](#license)

**Note: This is one mans attempt at something that could be cool. This is by no means official industry level software. This is just something I'm doing on the side for fun. IT WILL NOT BE DOCUMENTED!!! The only documentation you will find is from comments in the code. Also the API function calls parameters will be constantly changing as I continue to expand the functionality of the vulkan renderer. Yes, I am still learning all of these APIs**

Inspired by the t.v series The Expanse and other syfy t.v shows/movies.

Lucurious (L) Lowkey (U) unsure and curious if this is even possible, but why stop now!!! Essentially it will be a library (maybe a desktop engine later) for building highly sophisticated and advanced [Vulkan](https://www.khronos.org/vulkan/) DRM renderers (Vulkan Wayland Compositors) that are optimal, memory efficient, and easy to use. These renderers allow for easy creation of desktop environments that appear to be straight from science fiction.

## Purpose
I'm designing lucurious to be more focused on content. I do not want to be focused on optimizing my desktop environments or consistently working with the lower level APIs. Yes, my API will be restrictive in nature not in the sense of content creation.

**Current Goal:** Finishing the content creator [This is what I am calling it]

**Bellow are my creation steps:**
1. Finish the content creator [Vulkan DRM Renderer that gives developers control over the framebuffers they wish to create]
2. Get Vulkan DRM Renderer to display user defined content
3. Implement desktop specific features
4. Transform to a desktop engine

## Dependencies
* wayland-client
* wayland-server
* wayland-protocols
* wayland-scanner
* vulkan (Install your graphics card driver)
* vulkan-icd-loader
* vulkan-headers
* [CGLM (for linear algebra)](https://github.com/recp/cglm)
* [libshaderc](https://github.com/google/shaderc)
* [SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools) (Required by glslang and libshaderc)
* libdrm

### [Installation/Usage/Examples](https://github.com/EasyIP2023/lucurious-examples/)
### [Development](https://github.com/EasyIP2023/lucurious/tree/development/src)
### [Testing](https://github.com/EasyIP2023/lucurious/tree/development/tests)
### [Benchmarks](https://github.com/EasyIP2023/lucurious-benchmarks)

## Useful Sites
* [Writing an efficient Vulkan renderer](https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer/)
* [swvkc](https://github.com/st3r4g/swvkc/)
* [vkwayland](https://bitbucket.org/cheako/vkwayland/src/master/)
* [wayfire](https://wayfire.org/)/[wayfire github](https://github.com/WayfireWM/wayfire)
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
