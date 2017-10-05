#include "cpu/exec.h"

make_EHelper(mov);
make_EHelper(movsx);
make_EHelper(movzx);
make_EHelper(leave);

make_EHelper(call);
make_EHelper(jmp);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(xor);
make_EHelper(ret);
make_EHelper(sub);
make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
