/**
* The MIT License (MIT)
*
* Copyright (c) 2019-2020 Vincent Davis Jr.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#ifndef DLU_VKCOMP_ALL_H
#define DLU_VKCOMP_ALL_H

#include "create.h"
#include "exec.h"
#include "set.h"
#include "bind.h"
#include "cmd.h"
#include "update.h"
#include "display.h"
#include "setup.h"
#include "utils.h"
#include "vlayer.h"
#include "vk_calls.h"

#ifdef INAPI_CALLS
#include "device.h"

/**
* Dynamically retrieve or acquire the address of a VkInstance func.
* Via token concatenation and String-izing Tokens
*/
#define DLU_DR_INSTANCE_PROC_ADDR(inst, var, func) \
  do { \
    var = (PFN_vk##func) vkGetInstanceProcAddr(inst, "vk" #func); \
    if (!var) PERR(DLU_DR_INSTANCE_PROC_ADDR_ERR, 0, #func); \
  } while(0);

/**
* Dynamically retrieve or acquire the address of a VkDevice (logical device) func.
* Via token concatenation and String-izing Tokens
*/
#define DLU_DR_DEVICE_PROC_ADDR(dev, var, func) \
  do { \
    var = (PFN_vk##func) vkGetDeviceProcAddr(dev, "vk" #func); \
    if (!var) PERR(DLU_DR_DEVICE_PROC_ADDR_ERR, 0, #func); \
  } while(0);
#endif

#endif
