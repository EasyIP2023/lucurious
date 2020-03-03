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

#ifndef WLU_VKCOMP_ALL_H
#define WLU_VKCOMP_ALL_H

#include "funcs/create.h"
#include "funcs/device.h"
#include "funcs/display.h"
#include "funcs/exec.h"
#include "funcs/set.h"
#include "funcs/setup.h"
#include "funcs/utils.h"
#include "funcs/vlayer.h"

#include "gp/bind.h"
#include "gp/cmd.h"
#include "gp/create.h"
#include "gp/exec.h"
#include "gp/set.h"
#include "gp/update.h"

#ifdef LUCUR_VKCOMP_MATRIX_API
#include "matrix/print.h"
#include "matrix/set.h"
#endif

#ifdef INAPI_CALLS
/* Dynamically retrieve a VkInstance function */
#define WLU_DR_INSTANCE_PROC_ADDR(var, inst, func) \
  do { \
    var = (PFN_vk##func) vkGetInstanceProcAddr(inst, "vk" #func); \
    if (!var) PERR(WLU_DR_INSTANCE_PROC_ADDR_ERR, 0, #func); \
  } while(0);

/* Dynamically retrieve a VkDevice (logical device) function */
#define WLU_DR_DEVICE_PROC_ADDR(var, dev, func) \
  do { \
    var = (PFN_vk##func) vkGetDeviceProcAddr(dev, "vk" #func); \
    if (!var) PERR(WLU_DR_DEVICE_PROC_ADDR_ERR, 0, #func); \
  } while(0);
#endif

#endif
