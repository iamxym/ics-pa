#include <dlfcn.h>

#include "nemu.h"
#include "monitor/monitor.h"
#include "diff-test.h"

static void (*ref_difftest_memcpy_from_dut)(paddr_t dest, void *src, size_t n);
static void (*ref_difftest_getregs)(void *c);
static void (*ref_difftest_setregs)(const void *c);
static void (*ref_difftest_exec)(uint64_t n);

static bool is_skip_ref;
static bool is_skip_dut;

void difftest_skip_ref() { is_skip_ref = true; }
void difftest_skip_dut() { is_skip_dut = true; }

void reg_num_to_name(int i,char *name){
    switch(i){
    case 0: strncpy(name,"eax",4); break;
    case 1: strncpy(name,"ecx",4); break;
    case 2: strncpy(name,"edx",4); break;
    case 3: strncpy(name,"ebx",4); break;
    case 4: strncpy(name,"esp",4); break;                                                                                            case 5: strncpy(name,"ebp",4); break;
    case 6: strncpy(name,"esi",4); break;
    case 7: strncpy(name,"edi",4); break;
    default: Assert(0,"Undefined register number.\n");                                                                                                                                                        
    } 
    
}


void init_difftest(char *ref_so_file, long img_size) {
#ifndef DIFF_TEST
  return;
#endif

  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY | RTLD_DEEPBIND);
  assert(handle);

  ref_difftest_memcpy_from_dut = dlsym(handle, "difftest_memcpy_from_dut");
  assert(ref_difftest_memcpy_from_dut);

  ref_difftest_getregs = dlsym(handle, "difftest_getregs");
  assert(ref_difftest_getregs);

  ref_difftest_setregs = dlsym(handle, "difftest_setregs");
  assert(ref_difftest_setregs);

  ref_difftest_exec = dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  void (*ref_difftest_init)(void) = dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: \33[1;32m%s\33[0m", "ON");
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in include/common.h.", ref_so_file);

  ref_difftest_init();
  ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), img_size);
  ref_difftest_setregs(&cpu);
}

void difftest_step(uint32_t eip) {
  CPU_state ref_r;

  if (is_skip_dut) {
    is_skip_dut = false;
    return;
  }

  if (is_skip_ref) {
    // to skip the checking of an instruction, just copy the reg state to reference design
    ref_difftest_setregs(&cpu);
    is_skip_ref = false;
    return;
  }

  ref_difftest_exec(1);
  ref_difftest_getregs(&ref_r);

  // TODO: Check the registers state with the reference design.
  // Set `nemu_state` to `NEMU_ABORT` if they are not the same.
  // TODO();
  for(int i = 0; i < 8; ++i){
      if(cpu.gpr[i]._32 != ref_r.gpr[i]._32){
               char reg_name[7]="";
               reg_num_to_name(i,reg_name);
               printf("Different value of %s\nDUT:0x%08x\nShould be:0x%08x\n",reg_name,cpu.gpr[i]._32,ref_r.gpr[i]._32);
               nemu_state = NEMU_ABORT;
             }
  }
  if(cpu.eip != ref_r.eip){
      printf("Different value of eip\nDUT:0x%8x\nShould be:0x%8x\n",cpu.eip,ref_r.eip);
      nemu_state = NEMU_ABORT;
  }                  
  if(ref_r.eflags.CF != cpu.eflags.CF){
      printf("Different value of CF\nDUT:%d\nShould be:%d\n",cpu.eflags.CF,ref_r.eflags.CF);
      nemu_state = NEMU_ABORT;                                      
  }   
  if(ref_r.eflags.OF != cpu.eflags.OF){
      printf("Different value of OF\nDUT:%d\nShould be:%d\n",cpu.eflags.OF,ref_r.eflags.OF);
      nemu_state = NEMU_ABORT;
  }   
  if(ref_r.eflags.SF != cpu.eflags.SF){
      printf("Different value of SF\nDUT:%d\nShould be:%d\n",cpu.eflags.SF,ref_r.eflags.SF);
      nemu_state = NEMU_ABORT;                                      
  }   
  if(ref_r.eflags.ZF != cpu.eflags.ZF){
      printf("Different value of ZF\nDUT:%d\nShould be:%d\n",cpu.eflags.ZF,ref_r.eflags.ZF);
      nemu_state = NEMU_ABORT;                                                  
  }
}
