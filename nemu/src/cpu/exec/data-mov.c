#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_lr_l(&t0,R_ESP);
  rtl_lr_l(&t1,R_EBP);
  rtl_mv(&t0,&t1);
  rtl_sr_l(R_ESP,&t0);
  rtl_pop(&reg_l(R_EBP));
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    rtl_lr_w(&t0,R_AX);
	rtl_li(&t1,0);
	rtl_slt(&t2,&t0,&t1);
	if (t2 == 1) {
		rtl_li(&t0,0xffff);
		rtl_sr_w(R_DX,&t0);
	}
	else{
		rtl_sr_w(R_DX,&tzero);
	}
  }
  else {
    rtl_lr_l(&t0,R_EAX);
	rtl_li(&t1,0);
	rtl_slt(&t2,&t0,&t1);
	if (t2 == 1){
		rtl_li(&t0,0xffffffff);
		rtl_sr_l(R_EDX,&t0);
	}
	else{
		rtl_sr_l(R_EDX,&tzero);
	}
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
