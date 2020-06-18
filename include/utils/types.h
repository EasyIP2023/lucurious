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

#ifndef DLU_UTILS_TYPES_H
#define DLU_UTILS_TYPES_H

/* Used to help determine which ANSI Escape Codes to use */
typedef enum _dlu_log_type {
  DLU_NONE    = 0x0000,
  DLU_SUCCESS = 0x0001,
  DLU_DANGER  = 0x0002,
  DLU_INFO    = 0x0003,
  DLU_WARNING = 0x00004,
  DLU_RESET   = 0x0005,
  DLU_MAX_LOG_ENUM = 0xFFFF
} dlu_log_type;

typedef enum _dlu_block_type {
  DLU_LARGE_BLOCK_PRIV = 0x0000,
  DLU_SMALL_BLOCK_PRIV = 0x0001,
  DLU_LARGE_BLOCK_SHARED = 0x0002,
  DLU_SMALL_BLOCK_SHARED = 0x0004
} dlu_block_type;

typedef enum _dlu_data_type {
  DLU_SC_DATA = 0x0000,
  DLU_GP_DATA = 0x0001,
  DLU_CMD_DATA = 0x0002,
  DLU_BUFF_DATA = 0x0003,
  DLU_DESC_DATA = 0x0004,
  DLU_TEXT_DATA = 0x0005,
  DLU_DIS_DATA = 0x0006,
  DLU_PD_DATA = 0x0007,
  DLU_LD_DATA = 0x0008,
  DLU_SC_DATA_MEMS = 0x0F01,
  DLU_DESC_DATA_MEMS = 0x0F02,
  DLU_GP_DATA_MEMS = 0x0F03,
  DLU_DEVICE_OUTPUT_DATA = 0x0F04
} dlu_data_type;

typedef struct _dlu_otma_mems {
  uint32_t inta_cnt;    /* int array count */
  uint32_t cha_cnt;     /* char array count */
  uint32_t fla_cnt;      /* float array count */
  uint32_t dba_cnt;     /* double array count */
  uint32_t vkcomp_cnt;  /* Vkcomp struct count */
  uint32_t vkext_props_cnt;
  uint32_t vkval_layer_cnt;
  uint32_t desc_cnt;    /* descriptor count */
  uint32_t gp_cnt;      /* Graphics pipelines count */
  uint32_t si_cnt;      /* swap chain image count */
  uint32_t scd_cnt;     /* swap chain data count */
  uint32_t gpd_cnt;     /* graphics pipeline data count */
  uint32_t cmdd_cnt;    /* command data count */
  uint32_t bd_cnt;      /* buffer data count */
  uint32_t dd_cnt;      /* descriptor data count */
  uint32_t td_cnt;      /* texture data count */
  uint32_t dis_cnt;     /* display data count */
  uint32_t pd_cnt;      /* physical device data count */
  uint32_t ld_cnt;      /* logical device data count */
  uint32_t drmc_cnt;    /* dlu_drm_core struct count */
  uint32_t dod_cnt;     /* Device output_data struct count */
} dlu_otma_mems;

#ifdef INAPI_CALLS
typedef enum _dlu_err_msg_type {
  DLU_DR_INSTANCE_PROC_ADDR_ERR = 0x0001,
  DLU_DR_DEVICE_PROC_ADDR_ERR = 0x0002,
  DLU_VK_FUNC_ERR  = 0x0003,
  DLU_MEM_TYPE_ERR = 0x0004,
  DLU_VKCOMP_INSTANCE = 0x0100,
  DLU_VKCOMP_DEVICE = 0x0101,
  DLU_VKCOMP_RENDER_PASS = 0x0102,
  DLU_VKCOMP_PIPELINE_LAYOUT = 0x0103,
  DLU_VKCOMP_DESC_POOL = 0x0104,
  DLU_VKCOMP_DESC_LAYOUT = 0x0105,
  DLU_VKCOMP_PHYS_DEV = 0x0106,
  DLU_VKCOMP_INDICES = 0x0107,
  DLU_VKCOMP_SURFACE = 0x0108,
  DLU_VKCOMP_SC = 0x0109,
  DLU_VKCOMP_SC_IC = 0x010A,
  DLU_VKCOMP_SC_SYNCS = 0x010B,
  DLU_VKCOMP_BUFF_MEM = 0x010C,
  DLU_VKCOMP_CMD_POOL = 0x010D,
  DLU_VKCOMP_CMD_BUFFS = 0x010E,
  DLU_VKCOMP_DEVICE_NOT_ASSOC = 0x010F,
  DLU_BUFF_NOT_ALLOC = 0x0FFC,
  DLU_OP_NOT_PERMITED = 0x0FFD,
  DLU_ALLOC_FAILED = 0x0FFE,
  DLU_ALREADY_ALLOC = 0x0FFF
} dlu_err_msg_type;
#endif

#endif
