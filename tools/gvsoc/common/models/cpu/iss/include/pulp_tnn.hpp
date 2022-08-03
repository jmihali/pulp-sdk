/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Authors: Joan Mihali <jmihali@student.ethz.ch>
 */

#ifndef __CPU_ISS_PULP_TNN_HPP
#define __CPU_ISS_PULP_TNN_HPP

#define WSPR_UPDATE_MASK 0x10
#define WSPR_UPDATE_POS 4
#define ASPR_UPDATE_MASK 0x08
#define ASPR_UPDATE_POS 3
#define WSPR_ADDR_MASK 0x06
#define WSPR_ADDR_POS 1
#define ASPR_ADDR_MASK 0x01
#define ASPR_ADDR_POS 0


#include "iss.hpp"
#include "ternary_hashmaps.hpp"

static inline uint32_t lib_VEC_DOTP_TERNARY(iss_cpu_state_t *s, uint32_t A, uint32_t B) {
  uint8_t A0 = (A & 0xff);
  uint8_t A1 = (A & 0xff00) >> 8;
  uint8_t A2 = (A & 0xff0000) >> 16;
  uint8_t A3 = (A & 0xff000000) >> 24;

  uint16_t A0_dec = decoder_map[A0];
  uint16_t A1_dec = decoder_map[A1];
  uint16_t A2_dec = decoder_map[A2];
  uint16_t A3_dec = decoder_map[A3];

  uint8_t B0 = (B & 0xff);
  uint8_t B1 = (B & 0xff00) >> 8;
  uint8_t B2 = (B & 0xff0000) >> 16;
  uint8_t B3 = (B & 0xff000000) >> 24;

  uint16_t B0_dec = decoder_map[B0];
  uint16_t B1_dec = decoder_map[B1];
  uint16_t B2_dec = decoder_map[B2];
  uint16_t B3_dec = decoder_map[B3];

  int8_t a0, a1, a2, a3, b0, b1, b2, b3;
  uint32_t out = 0;

  for (int i=0; i<5; i++) {
    a0 = (A0_dec & (0x3 << 2*i)) >> 2*i;
    a1 = (A1_dec & (0x3 << 2*i)) >> 2*i;
    a2 = (A2_dec & (0x3 << 2*i)) >> 2*i;
    a3 = (A3_dec & (0x3 << 2*i)) >> 2*i;

    b0 = (B0_dec & (0x3 << 2*i)) >> 2*i;
    b1 = (B1_dec & (0x3 << 2*i)) >> 2*i;
    b2 = (B2_dec & (0x3 << 2*i)) >> 2*i;
    b3 = (B3_dec & (0x3 << 2*i)) >> 2*i;

    a0 = (a0 & 0x2) ? (a0 | 0xfe) : a0;
    a1 = (a1 & 0x2) ? (a1 | 0xfe) : a1;
    a2 = (a2 & 0x2) ? (a2 | 0xfe) : a2;
    a3 = (a3 & 0x2) ? (a3 | 0xfe) : a3;

    b0 = (b0 & 0x2) ? (b0 | 0xfe) : b0;
    b1 = (b1 & 0x2) ? (b1 | 0xfe) : b1;
    b2 = (b2 & 0x2) ? (b2 | 0xfe) : b2;
    b3 = (b3 & 0x2) ? (b3 | 0xfe) : b3;

    out += a0 * b0 + a1 * b1 + a2 * b2 + a3 * b3;
  }
  return out;
}

static inline void pv_mlsdotp_t_resume(iss_t *iss)
{
}

static inline iss_insn_t *pv_mlsdotp_t_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_uim_t ctl_imm = UIM_GET(0);

  int ac_addr = ((ctl_imm & ASPR_ADDR_MASK) >> ASPR_ADDR_POS);
  int wt_addr = ((ctl_imm & WSPR_ADDR_MASK) >> WSPR_ADDR_POS) + 0x2;
  bool ac_update = ((ctl_imm & ASPR_UPDATE_MASK) >> ASPR_UPDATE_POS);
  bool wt_update = ((ctl_imm & WSPR_UPDATE_MASK) >> WSPR_UPDATE_POS);

  REG_SET(0, LIB_CALL3(lib_VEC_DOTP_TERNARY, REG_GET(0), SPR_GET(wt_addr), SPR_GET(ac_addr)));

  if(ac_update)
  {
    iss_reg_t addr = REG_GET(1);
    if (!iss->data_req(addr, (uint8_t *)&iss->cpu.pulp_nn.spr_ml[ac_addr], 4, false))
    {
      iss_msg(iss, "Loaded new value (spr_loc: 0x%x, value: 0x%x)\n", ac_addr, SPR_GET(ac_addr));
    }
    else
    {
      iss->cpu.state.stall_callback = pv_##insn_name##_c_resume;
      iss->cpu.pulp_nn.ml_insn = insn;
      iss_exec_insn_stall(iss);
    }
    iss_msg(iss, "Address updating (addr: 0x%x)\n", addr + 4);
    IN_REG_SET(1, addr + 4);
  }
  else if (wt_update)
  {
    iss_reg_t addr = REG_GET(1);
    if (!iss->data_req(addr, (uint8_t *)&iss->cpu.pulp_nn.spr_ml[wt_addr], 4, false))
    {
      iss_msg(iss, "Loaded new value (spr_loc: 0x%x, value: 0x%x)\n", wt_addr, SPR_GET(wt_addr));
    }
    else
    {
      iss->cpu.state.stall_callback = pv_##insn_name##_c_resume;
      iss->cpu.pulp_nn.ml_insn = insn;
      iss_exec_insn_stall(iss);
    }
    iss_msg(iss, "Address updating (addr: 0x%x)\n", addr + 4);
    IN_REG_SET(1, addr + 4);
  }
  else
  {
    iss_msg(iss, "No address updating\n");
  }
  return insn->next;
}

static inline void iss_pulp_tnn_init(iss_t *iss)
{
  iss->cpu.pulp_tnn.qnt_step = 0;
}

#endif
