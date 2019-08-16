# Lucurious
![Example 1](http://hellodave.co/wp-content/uploads/2017/08/livtablet_render_002a_2K.jpg)
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](#license)

Inspired by the t.v series expanse and other syfy t.v shows/movies.

Lucurious (L) Lowkey (U) unsure and curious if this is even possible, but why stop now!!! It\'s currently a library for building and styling 2D/3D Wayland Compositors, like for example [wayfire](https://wayfire.org/)/[wayfire github](https://github.com/WayfireWM/wayfire). It'll later be transformed into a Desktop Engine. It uses [Vulkan](https://www.khronos.org/vulkan/) and the Wayland protocol to turn the science fiction UI into reality.

I'm not using [wlroots](https://github.com/swaywm/wlroots) because I wanted to learn more about the inner workings of the wayland protocol and how the linux graphics stack works, plus I'm working on this for my master thesis. [Wlroots](https://github.com/swaywm/wlroots) could be used in this project as it is an extremely good wayland compositing library and I recommend usage if you want to build your own compositor without dealing with the underlying wayland compositing features.

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
* [SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools)
* [Spirv-Headers](https://github.com/KhronosGroup/SPIRV-Headers)

### [Documentation](https://github.com/lucurious-labs/lucurious-docs)
### [Installation/Usage/Examples](https://github.com/lucurious-labs/lucurious/tree/development/examples)
### [Development](https://github.com/lucurious-labs/lucurious/tree/development/src)
### [Testing](https://github.com/lucurious-labs/lucurious/tree/development/tests)

## References
* [Wayland freedesktop](https://wayland.freedesktop.org/)
* [An introduction to Wayland](https://drewdevault.com/2017/06/10/Introduction-to-Wayland.html)
* [Weston](https://github.com/wayland-project/weston)
* [Programming Wayland Clients](https://jan.newmarch.name/Wayland/index.html)
* [Swaywm](https://github.com/swaywm)
* [Emersion Hello Wayland](https://github.com/emersion/hello-wayland)
* [Direct Rendering Manager](https://dri.freedesktop.org/wiki/DRM/)
* [Vulkan](https://vulkan.lunarg.com)
* [Vulkan Tutorial](https://vulkan-tutorial.com/)
* [Vulkan Api Without Secrets](https://software.intel.com/en-us/articles/api-without-secrets-introduction-to-vulkan-part-2)
* [Vulkan Validation Layers](https://gpuopen.com/using-the-vulkan-validation-layers/)
* [MVP](https://jsantell.com/model-view-projection)
