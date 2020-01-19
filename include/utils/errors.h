/**
* The MIT License (MIT)
*
* Copyright (c) 2019 Vincent Davis Jr.
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

#ifndef WLU_UTILS_ERRORS_H
#define WLU_UTILS_ERRORS_H

typedef enum _wlu_err_msg_type {
  WLU_DR_INSTANCE_PROC_ADDR_ERR = 0x00000002,
  WLU_DR_DEVICE_PROC_ADDR_ERR = 0x00000003,
  WLU_VK_ENUM_ERR = 0x00000004,
  WLU_VK_CREATE_ERR = 0x00000005,
  WLU_VK_ALLOC_ERR = 0x00000006,
  WLU_VK_BEGIN_ERR = 0x00000007,
  WLU_VK_END_ERR = 0x00000008,
  WLU_VK_QUEUE_ERR = 0x00000009,
  WLU_VK_GET_ERR = 0x0000000A,
  WLU_VK_BIND_ERR = 0x0000000B,
  WLU_VK_MAP_ERR = 0x0000000C,
  WLU_VK_FLUSH_ERR = 0x0000000D,
  WLU_VK_RESET_ERR = 0x0000000E,
  WLU_VK_WAIT_ERR = 0x0000000F,
  WLU_VKCOMP_INSTANCE = 0x00000100,
  WLU_VKCOMP_DEVICE = 0x00000101,
  WLU_VKCOMP_RENDER_PASS = 0x00000102,
  WLU_VKCOMP_PIPELINE_LAYOUT = 0x00000103,
  WLU_VKCOMP_DESC_POOL = 0x000000104,
  WLU_VKCOMP_DESC_LAYOUT = 0x00000105,
  WLU_VKCOMP_FRAMEBUFFER = 0x00000106,
  WLU_VKCOMP_PHYSICAL_DEV = 0x00000107,
  WLU_VKCOMP_INDICES = 0x00000108,
  WLU_VKCOMP_SURFACE = 0x00000109,
  WLU_VKCOMP_SC = 0x0000010A,
  WLU_VKCOMP_SC_BUFFS = 0x0000010B,
  WLU_VKCOMP_SC_IC = 0x00000010C,
  WLU_VKCOMP_SC_SEMS = 0x0000010D,
  WLU_VKCOMP_BUFF_MEM = 0x0000010E,
  WLU_VKCOMP_CMD_POOL = 0x00000110,
  WLU_VKCOMP_CMD_BUFFS = 0x00000111,
  WLU_BUFF_NOT_ALLOC = 0x0000FFFE,
  WLU_ALLOC_FAILED = 0x0000FFFF
} wlu_err_msg_type;

void _show_err_msg(uint32_t wlu_err, int vkerr, const char *wlu_msg);

#define PERR(wlu_err, vkerr, wlu_msg) \
  _show_err_msg(wlu_err, vkerr, wlu_msg);

#endif
