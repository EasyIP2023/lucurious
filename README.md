# Lucurious
![Expanse](https://www.syfy.com/sites/syfy/files/styles/syfy_image_gallery_full_breakpoints_theme_syfy_normal_1x/public/2017/02/TheExpanse_gallery_205Recap_06.jpg?timestamp=1487633617)
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
* [vulkan](https://vulkan.lunarg.com)
* [Vulkan Tutorial](https://vulkan-tutorial.com/)
* [Vulkan Api Without Secrets](https://software.intel.com/en-us/articles/api-without-secrets-introduction-to-vulkan-part-2)
