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

#ifndef WLU_UTILS_TYPES_H
#define WLU_UTILS_TYPES_H

/* Used to help determine which ANSI Escape Codes to use */
typedef enum _wlu_log_type {
  WLU_NONE    = 0x0000,
  WLU_SUCCESS = 0x0001,
  WLU_DANGER  = 0x0002,
  WLU_INFO    = 0x0003,
  WLU_WARNING = 0x00004,
  WLU_RESET   = 0x0005,
  WLU_MAX_LOG_ENUM = 0xFFFF
} wlu_log_type;

typedef enum _wlu_block_type {
  WLU_LARGE_BLOCK_PRIV = 0x0000,
  WLU_SMALL_BLOCK_PRIV = 0x0001,
  WLU_LARGE_BLOCK_SHARED = 0x0002,
  WLU_SMALL_BLOCK_SHARED = 0x0004
} wlu_block_type;

typedef enum _wlu_data_type {
  WLU_SC_DATA = 0x0000,
  WLU_GP_DATA = 0x0001,
  WLU_CMD_DATA = 0x0002,
  WLU_BUFFS_DATA = 0x0003,
  WLU_DESC_DATA = 0x0004,
  WLU_TEXT_DATA = 0x0005,
  WLU_SC_DATA_MEMS = 0x0006,
  WLU_DESC_DATA_MEMS = 0x0007,
  WLU_GP_DATA_MEMS = 0x0008
} wlu_data_type;

typedef enum _wlu_err_msg_type {
  WLU_DR_INSTANCE_PROC_ADDR_ERR = 0x0001,
  WLU_DR_DEVICE_PROC_ADDR_ERR = 0x0002,
  WLU_VK_ENUM_ERR = 0x0003,
  WLU_VK_CREATE_ERR = 0x0004,
  WLU_VK_ALLOC_ERR = 0x0005,
  WLU_VK_BEGIN_ERR = 0x0006,
  WLU_VK_END_ERR = 0x0007,
  WLU_VK_QUEUE_ERR = 0x0008,
  WLU_VK_GET_ERR = 0x0009,
  WLU_VK_BIND_ERR = 0x000A,
  WLU_VK_MAP_ERR = 0x000B,
  WLU_VK_FLUSH_ERR = 0x000C,
  WLU_VK_RESET_ERR = 0x000D,
  WLU_VK_WAIT_ERR = 0x000E,
  WLU_VKCOMP_INSTANCE = 0x0100,
  WLU_VKCOMP_DEVICE = 0x0101,
  WLU_VKCOMP_RENDER_PASS = 0x0102,
  WLU_VKCOMP_PIPELINE_LAYOUT = 0x0103,
  WLU_VKCOMP_DESC_POOL = 0x0104,
  WLU_VKCOMP_DESC_LAYOUT = 0x0105,
  WLU_VKCOMP_PHYS_DEV = 0x0106,
  WLU_VKCOMP_INDICES = 0x0107,
  WLU_VKCOMP_SURFACE = 0x0108,
  WLU_VKCOMP_SC = 0x0109,
  WLU_VKCOMP_SC_IC = 0x010A,
  WLU_VKCOMP_SC_SEMS = 0x010B,
  WLU_VKCOMP_BUFF_MEM = 0x010C,
  WLU_VKCOMP_CMD_POOL = 0x010D,
  WLU_VKCOMP_CMD_BUFFS = 0x010E,
  WLU_BUFF_NOT_ALLOC = 0x0FFC,
  WLU_OP_NOT_PERMITED = 0x0FFD,
  WLU_ALLOC_FAILED = 0x0FFE,
  WLU_ALREADY_ALLOC = 0x0FFF
} wlu_err_msg_type;

typedef struct _wlu_otma_mems {
  uint32_t inta_cnt;  /* int array count */
  uint32_t cha_cnt;   /* char array count */
  uint32_t fla_cnt;    /* float array count */
  uint32_t dba_cnt;   /* double array count */
  uint32_t wclient_cnt;
  uint32_t vkcomp_cnt;
  uint32_t vkext_props_cnt;
  uint32_t vkval_layer_cnt;
  uint32_t desc_cnt;  /* descriptor count */
  uint32_t gp_cnt;    /* Graphics pipelines count */
  uint32_t si_cnt;    /* swap chain image count */
  uint32_t scd_cnt;   /* swap chain data count */
  uint32_t gpd_cnt;   /* graphics pipeline data count */
  uint32_t cmdd_cnt;  /* command data count */
  uint32_t bd_cnt;    /* buffer data count */
  uint32_t dd_cnt;    /* descriptor data count */
  uint32_t td_cnt;    /* texture data count */
} wlu_otma_mems;

#endif
