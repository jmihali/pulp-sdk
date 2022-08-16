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
//#include "ternary_hashmaps.hpp"

static inline uint32_t lib_VEC_SDOTP_TERNARY(iss_cpu_state_t *s, uint32_t out, uint32_t A, uint32_t B) {

unordered_map<uint8_t, uint16_t> decoder_map({
{0,277},{127,511},{1,321},{128,789},{2,320},{129,833},{3,340},{130,832},{4,325},{131,852},{5,337},{132,837},{6,336},{133,849},{7,341},{134,848},
{8,85},{135,853},{9,64},{25,64},{136,213},{10,276},{137,192},{153,192},{11,84},{138,21},{12,261},{139,81},{13,65},{140,773},{14,20},
{142,20},{141,193},{15,343},{143,855},{16,279},{144,791},{17,323},{145,835},{18,260},{82,260},{146,772},{210,772},{19,348},{147,860},{20,327},
{148,839},{21,339},{149,851},{22,324},{150,836},{23,349},{151,861},{24,87},{152,215},{26,788},{154,23},{27,92},{155,83},{28,263},{156,775},
{29,67},{157,195},{30,28},{158,28},{159,863},{31,351},{160,797},{32,285},{161,785},{33,273},{162,784},{34,272},{163,884},{35,372},{164,845},
{36,333},{165,881},{37,369},{166,880},{38,368},{167,885},{39,373},{168,221},{40,93},{169,1},{41,256},{170,29},{42,284},{171,113},{43,116},
{172,781},{44,269},{173,769},{45,257},{175,887},{46,52},{174,52},{176,799},{47,375},{177,787},{48,287},{178,780},{242,780},{49,275},{179,892},
{50,268},{114,268},{180,847},{51,380},{181,883},{52,335},{182,844},{53,371},{183,893},{54,332},{184,223},{55,381},{185,3},{56,95},{186,31},
{57,768},{187,115},{58,796},{188,783},{59,124},{189,771},{60,271},{191,895},{61,259},{192,821},{62,60},{190,60},{193,961},{63,383},{194,960},
{64,309},{195,980},{65,449},{196,965},{66,448},{197,977},{67,468},{198,976},{68,453},{199,981},{69,465},{200,245},{70,464},{201,16},{71,469},
{202,53},{72,117},{203,209},{73,4},{89,4},{204,197},{74,308},{205,17},{75,212},{206,208},{76,69},{207,983},{77,5},{208,823},{78,80},
{209,963},{79,471},{211,988},{80,311},{212,967},{81,451},{213,979},{83,476},{214,964},{84,455},{215,989},{85,467},{216,247},{86,452},{217,0},
{249,0},{87,477},{218,55},{88,119},{219,211},{90,820},{220,199},{91,220},{221,19},{92,71},{222,196},{93,7},{223,991},{94,68},{224,829},
{95,479},{225,817},{96,317},{226,816},{97,305},{227,1012},{98,304},{228,973},{99,500},{229,1009},{100,461},{230,1008},{101,497},{231,1013},{102,496},
{232,253},{103,501},{233,48},{104,125},{234,61},{105,12},{121,12},{235,241},{106,316},{236,205},{107,244},{237,49},{108,77},{238,240},{109,13},
{239,1015},{110,112},{240,831},{111,503},{241,819},{112,319},{243,1020},{113,307},{244,975},{115,508},{245,1011},{116,463},{246,972},{117,499},{247,1021},
{118,460},{248,255},{119,509},{250,63},{120,127},{251,243},{122,828},{252,207},{123,252},{253,51},{124,79},{254,204},{125,15},{255,1023},{126,76}});

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

static inline void pv_smlsdotp_t_resume(iss_t *iss)
{
}

static inline iss_insn_t *pv_smlsdotp_t_exec(iss_t *iss, iss_insn_t *insn)
{
  iss_uim_t ctl_imm = UIM_GET(0);

  int ac_addr = ((ctl_imm & ASPR_ADDR_MASK) >> ASPR_ADDR_POS);
  int wt_addr = ((ctl_imm & WSPR_ADDR_MASK) >> WSPR_ADDR_POS) + 0x2;
  bool ac_update = ((ctl_imm & ASPR_UPDATE_MASK) >> ASPR_UPDATE_POS);
  bool wt_update = ((ctl_imm & WSPR_UPDATE_MASK) >> WSPR_UPDATE_POS);

  REG_SET(0, LIB_CALL3(lib_VEC_SDOTP_TERNARY, REG_GET(0), SPR_GET(wt_addr), SPR_GET(ac_addr)));

  if(ac_update)
  {
    iss_reg_t addr = REG_GET(1);
    if (!iss->data_req(addr, (uint8_t *)&iss->cpu.pulp_nn.spr_ml[ac_addr], 4, false))
    {
      iss_msg(iss, "Loaded new value (spr_loc: 0x%x, value: 0x%x)\n", ac_addr, SPR_GET(ac_addr));
    }
    else
    {
      iss->cpu.state.stall_callback = pv_smlsdotp_t_resume;
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
      iss->cpu.state.stall_callback = pv_smlsdotp_t_resume;
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
