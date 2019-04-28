#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
//依次将EFLAGS, CS(代码段寄存器), EIP寄存器的值压栈
    rtl_push(&cpu.eflag);
    rtl_push(&cpu.cs);
    rtl_push(&ret_addr);

       
    GateDesc gate_des;
    gate_des.val1 = vaddr_read(cpu.idtr.base + NO * 8, 4);
    gate_des.val2 = vaddr_read(cpu.idtr.base + NO * 8 + 4, 4);
    if(gate_des.present == 0){
        panic("Invalid gate descriptor!");
        assert(0);
                }
    vaddr_t aim = (gate_des.val1 & 0x0000ffff) + (gate_des.val2 & 0xffff0000);
    rtl_j(aim);
    // TODO();
}

void dev_raise_intr() {
}
