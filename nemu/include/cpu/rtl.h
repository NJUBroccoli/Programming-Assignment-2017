#ifndef __RTL_H__
#define __RTL_H__

#include "nemu.h"
#include "reg.h"
#include "memory.h"

extern rtlreg_t t0, t1, t2, t3;
extern const rtlreg_t tzero;

/* RTL basic instructions */

static inline void rtl_li(rtlreg_t* dest, uint32_t imm) {
  *dest = imm;
}

#define c_add(a, b) ((a) + (b))
#define c_sub(a, b) ((a) - (b))
#define c_and(a, b) ((a) & (b))
#define c_or(a, b)  ((a) | (b))
#define c_xor(a, b) ((a) ^ (b))
#define c_shl(a, b) ((a) << (b))
#define c_shr(a, b) ((a) >> (b))
#define c_sar(a, b) ((int32_t)(a) >> (b))
#define c_slt(a, b) ((int32_t)(a) < (int32_t)(b))
#define c_sltu(a, b) ((a) < (b))

#define make_rtl_arith_logic(name) \
  static inline void concat(rtl_, name) (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = concat(c_, name) (*src1, *src2); \
  } \
  static inline void concat3(rtl_, name, i) (rtlreg_t* dest, const rtlreg_t* src1, int imm) { \
    *dest = concat(c_, name) (*src1, imm); \
  }


make_rtl_arith_logic(add)
make_rtl_arith_logic(sub)
make_rtl_arith_logic(and)
make_rtl_arith_logic(or)
make_rtl_arith_logic(xor)
make_rtl_arith_logic(shl)
make_rtl_arith_logic(shr)
make_rtl_arith_logic(sar)
make_rtl_arith_logic(slt)
make_rtl_arith_logic(sltu)

static inline void rtl_mul(rtlreg_t* dest_hi, rtlreg_t* dest_lo, const rtlreg_t* src1, const rtlreg_t* src2) {
  asm volatile("mul %3" : "=d"(*dest_hi), "=a"(*dest_lo) : "a"(*src1), "r"(*src2));
}

static inline void rtl_imul(rtlreg_t* dest_hi, rtlreg_t* dest_lo, const rtlreg_t* src1, const rtlreg_t* src2) {
  asm volatile("imul %3" : "=d"(*dest_hi), "=a"(*dest_lo) : "a"(*src1), "r"(*src2));
}

static inline void rtl_div(rtlreg_t* q, rtlreg_t* r, const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  asm volatile("div %4" : "=a"(*q), "=d"(*r) : "d"(*src1_hi), "a"(*src1_lo), "r"(*src2));
}

static inline void rtl_idiv(rtlreg_t* q, rtlreg_t* r, const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  asm volatile("idiv %4" : "=a"(*q), "=d"(*r) : "d"(*src1_hi), "a"(*src1_lo), "r"(*src2));
}

static inline void rtl_lm(rtlreg_t *dest, const rtlreg_t* addr, int len) {
  *dest = vaddr_read(*addr, len);
}

static inline void rtl_sm(rtlreg_t* addr, int len, const rtlreg_t* src1) {
  vaddr_write(*addr, len, *src1);
}

static inline void rtl_lr_b(rtlreg_t* dest, int r) {
  *dest = reg_b(r);
}

static inline void rtl_lr_w(rtlreg_t* dest, int r) {
  *dest = reg_w(r);
}

static inline void rtl_lr_l(rtlreg_t* dest, int r) {
  *dest = reg_l(r);
}

static inline void rtl_sr_b(int r, const rtlreg_t* src1) {
  reg_b(r) = *src1;
}

static inline void rtl_sr_w(int r, const rtlreg_t* src1) {
  reg_w(r) = *src1;
}

static inline void rtl_sr_l(int r, const rtlreg_t* src1) {
  reg_l(r) = *src1;
}

/* RTL psuedo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_lr_l(dest, r); return;
    case 1: rtl_lr_b(dest, r); return;
    case 2: rtl_lr_w(dest, r); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, int width, const rtlreg_t* src1) {
  switch (width) {
    case 4: rtl_sr_l(r, src1); return;
    case 1: rtl_sr_b(r, src1); return;
    case 2: rtl_sr_w(r, src1); return;
    default: assert(0);
  }
}

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    cpu.f = *src; \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest = cpu.f; \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)



static inline void rtl_mv(rtlreg_t* dest, const rtlreg_t *src1) {
  *dest = *src1;
}

static inline void rtl_not(rtlreg_t* dest) {
  *dest = !(*dest);
}

static inline void rtl_neg(rtlreg_t* dest) {
  *dest = -(*dest);
}

static inline void rtl_sext(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  *dest = (*src1) << (32 - 8 * width);
  int32_t tmp_dest = (int32_t)(*dest);
  *dest = tmp_dest >> (32 - 8 * width);
}

static inline void rtl_ext(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  *dest = (*src1) << (32 - 8 * width);
  *dest >>= (32 - 8 * width);
}

static inline void rtl_push(const rtlreg_t* src1) {
  rtl_subi(&reg_l(R_ESP), &reg_l(R_ESP), 4);
  rtl_sm(&reg_l(R_ESP), 4, src1);
}

static inline void rtl_pop(rtlreg_t* dest) {
  rtl_lm(dest, &reg_l(R_ESP), 4);
  rtl_addi(&reg_l(R_ESP), &reg_l(R_ESP), 4);
}
/*
static inline void rtl_push_w(const rtlreg_t* src1) {
  rtl_subi(&reg_w(R_SP), &reg_w(R_SP), 2);
  rtl_sm(&reg_w(R_SP), 2, src1);
}

static inline void rtl_pop_w(rtlreg_t* dest) {
  rtl_lm(dest, &reg_w(R_SP), 2);
  rtl_addi(&reg_w(R_SP), &reg_w(R_SP), 2);
}
*/
static inline void rtl_eq0(rtlreg_t* dest, const rtlreg_t* src1) {
  *dest = (*src1==0 ? 1:0);
	// dest <- (src1 == 0 ? 1 : 0)
}

static inline void rtl_eqi(rtlreg_t* dest, const rtlreg_t* src1, int imm) {
  *dest = (*src1==imm? 1:0);
	// dest <- (src1 == imm ? 1 : 0)
}


static inline void rtl_neq0(rtlreg_t* dest, const rtlreg_t* src1) {
  *dest = (*src1 != 0 ? 1 : 0);
	// dest <- (src1 != 0 ? 1 : 0)
}

static inline void rtl_msb(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  *dest = (*src1)>>(width * 8 - 1);
  printf("%d\n",*dest);
	// dest <- src1[width * 8 - 1]
}

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  cpu.ZF = (*result==0 ? 1:0);	
	// eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  rtl_msb(&t1, result, width);
  cpu.SF = (t1 == 1 ? 1 : 0);
	// eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}
/*
static inline void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  switch (subcode)
  {
	case 0x0: if (cpu.OF == 1) *dest = 1; else *dest = 0;
			  break;
	case 0x1: if (cpu.OF == 0) *dest = 1; else *dest = 0;
			  break;
	case 0x2: if (cpu.CF == 1) *dest = 1; else *dest = 0;
			  break;
	case 0x3: if (cpu.CF == 0) *dest = 1; else *dest = 0;
			  break;
	case 0x4: if (cpu.ZF == 1) *dest = 1; else *dest = 0;
			  break;
	case 0x5: if (cpu.ZF == 0) *dest = 1; else *dest = 0;
			  break;
	case 0x6: if (cpu.CF == 1 || cpu.ZF == 1) *dest = 1; else *dest = 0;
			  break;
	case 0x7: if (cpu.CF ==0 && cpu.ZF == 0) *dest = 1; else *dest = 0;
			  break;
	case 0x8: if (cpu.SF == 1) *dest = 1; else *dest = 0;
			  break;
	case 0x9: if (cpu.SF == 0) *dest = 1; else *dest = 0;
			  break;
	case 0xa: if (cpu.PF == 1) *dest = 1; else *dest = 0;
			  break;
	case 0xb: if (cpu.PF == 0) *dest = 1; else *dest = 0;
			  break;
	case 0xc: if (cpu.SF != cpu.OF) *dest = 1; else *dest = 0;
			  break;
	case 0xd: if (cpu.SF == cpu.OF) *dest = 1; else *dest = 0;
			  break;
	case 0xe: if (cpu.ZF == 1 || cpu.SF != cpu.OF) *dest = 1; else *dest = 0;
			  break;
	case 0xf: if (cpu.ZF == 0 && cpu.SF == cpu.OF) *dest = 1; else *dest = 0;
			  break;
	default:  break;
  }
}
*/

#endif
