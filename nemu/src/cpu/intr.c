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

       
    GateDesc gate_desc;
    gate_desc.val1 = vaddr_read(cpu.idtr.base + NO * 8, 4);
    gate_desc.val2 = vaddr_read(cpu.idtr.base + NO * 8 + 4, 4);
    if(gate_desc.present == 0){
        panic("Invalid!");
        assert(0);
    }
    vaddr_t aim = (gate_desc.val1 & 0x0000ffff) + (gate_desc.val2 & 0xffff0000);
    rtl_j(aim);
    // TODO();
}

void dev_raise_intr() {
}
