#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
	//rtl_and(&id_dest->val, &id_dest->val, &id_src->val);
	rtl_and(&t0, &id_dest->val, &id_src->val);
	//dest = dest & src
	//operand_write(id_dest, &id_dest->val);

	rtl_update_ZFSF(&t0, id_dest->width);

	rtl_li(&t0,0);
	rtl_set_CF(&t0);
	rtl_set_OF(&t0);
	//CF = OF = 0
	
  print_asm_template2(test);
}

make_EHelper(and) {
	rtl_and(&id_dest->val, &id_dest->val, &id_src->val);
	//dest = dest & src	
	operand_write(id_dest, &id_dest->val);
	
	rtl_li(&t0,0);
	rtl_set_CF(&t0);
	rtl_set_OF(&t0);
	//CF = OF = 0

	rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
	rtl_xor(&id_dest->val, &id_dest->val, &id_src->val);
	operand_write(id_dest,&id_dest->val);

	rtl_li(&t0,0);
	rtl_set_CF(&t0);
	rtl_set_OF(&t0);
	//CF = OF = 0

	rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(xor);
}

make_EHelper(or) {
	rtl_or(&id_dest->val, &id_dest->val, &id_src->val);
	//dest = dest | src	
	operand_write(id_dest, &id_dest->val);
	
	rtl_li(&t0,0);
	rtl_set_CF(&t0);
	rtl_set_OF(&t0);
	//CF = OF = 0

	rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
  // unnecessary to update CF and OF in NEMU
	rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);
	rtl_sar(&id_dest->val, &id_dest->val, &id_src->val);
	operand_write(id_dest, &id_dest->val);

	rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  // unnecessary to update CF and OF in NEMU
	rtl_shl(&id_dest->val, &id_dest->val, &id_src->val);
	operand_write(id_dest, &id_dest->val);

	rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(shl);
}

make_EHelper(shr) {
  // unnecessary to update CF and OF in NEMU
	rtl_shr(&id_dest->val, &id_dest->val, &id_src->val);
	operand_write(id_dest, &id_dest->val);

	rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;

  rtl_setcc(&t2, cc);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
	rtl_not(&id_dest->val, &id_dest->val);
	operand_write(id_dest, &id_dest->val);

  print_asm_template1(not);
}


make_EHelper(rol){
	rtl_li(&t1, id_dest->val);
	for(int i = 0; i < id_src->val; ++i){
		rtl_msb(&t0, &t1, id_dest->width);
		rtl_shli(&t1, &t1, 1);
		rtl_add(&t1, &t1, &t0);
	}	
	operand_write(id_dest, &t1);
	//printf_asm_template2(rol);
}
