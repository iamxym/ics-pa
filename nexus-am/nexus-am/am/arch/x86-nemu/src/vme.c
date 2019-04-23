#include <x86.h>
#include "klib.h"

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*pgalloc_usr)(size_t);
static void (*pgfree_usr)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);

  return 0;
}

int _protect(_Protect *p) {
  PDE *updir = (PDE*)(pgalloc_usr(1));
  p->pgsize = 4096;
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
  return 0;
}

void _unprotect(_Protect *p) {
}

static _Protect *cur_as = NULL;
void get_cur_as(_Context *c) {
  c->prot = cur_as;
}

void _switch(_Context *c) {
	//printf("c->prot->ptr = %p\n",c->prot->ptr);
  set_cr3(c->prot->ptr);
  cur_as = c->prot;
}

int _map(_Protect *p, void *va, void *pa, int mode) {
	//printf("**starting map va=%p to pa=%p**\n", va ,pa);
	uint32_t paddr = (uint32_t)pa;
	uint32_t dir = PDX(va);
	PDE* map_pde_ptr = p->ptr + 4 * dir;
	//find its page directory
	
	if((*map_pde_ptr & PTE_P) == 0) {
		PDE new_pt_addr = (PDE)pgalloc_usr(1);
		//allocate one page as page table
		*map_pde_ptr = (new_pt_addr >> 12) << 12;
		//fill in the page dir with the addr of new page table
	 	*map_pde_ptr |= PTE_P;	
		//create a new page
	}

	uint32_t page = PTX(va);
	void* pt_addr = (void*)(((*map_pde_ptr) >> 12) << 12); //get page table addr
	PTE* map_pte_ptr = (PTE*)(pt_addr + page * 4);
	*map_pte_ptr = (paddr >> 12) << 12; //fill in physical page number
	*map_pte_ptr |= PTE_P; // set present
	if((mode & PTE_P) == 0) *map_pte_ptr = 0; // make mapping invalid
	//printf("**starting map va=%p to pa=%p**\n", va ,pa);
  return 0;
}

_Context *_ucontext(_Protect *p, _Area ustack, _Area kstack, void *entry, void *args) {
	//printf("call _ucontext!\n");
	//_protect(p);
	void* start_frame = ustack.end - 3 * sizeof(void*);
	memset(start_frame, 0, sizeof(void*) * 3);
	_Context *cont = (_Context*)(start_frame - sizeof(_Context));
	cont->cs = 8;
	cont->eip = (uintptr_t)(entry);
	cont->esp = cont->ebp = (uintptr_t)(ustack.end);
	cont->edi = cont->esi = cont->ebx = cont->ecx = cont->eax = 0;
	cont->irq = 0x81;
	cont->err = cont->eflags = 0;
	cont->eflags |= (1 << 9); //set IF
	cont->prot = p;
  return cont;
}
