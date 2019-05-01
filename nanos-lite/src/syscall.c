#include "common.h"
#include "syscall.h"
#include "proc.h"

extern char _end;

typedef size_t ssize_t;
typedef size_t off_t;
int fs_open(const char* pathname, int flags, int mode);
ssize_t fs_read(int fd, void* buf, size_t len);
ssize_t fs_close(int fd);
ssize_t fs_lseek(int fd, off_t offset, int whence);
ssize_t fs_write(int fd, const void* buf, size_t len);

void naive_uload(PCB *pcb, const char *filename);


_Context* do_syscall(_Context *c) {
  	uintptr_t a[4];
  	a[0] = c->GPR1;
	a[1] = c->GPR2;	//parameter1:status
	a[2] = c->GPR3;
	a[3] = c->GPR4;

  switch (a[0]) {
  case SYS_yield : _yield();c->GPRx = 0 ;break;
  case SYS_exit : _halt(a[2]);break;
  case SYS_write: c->GPRx = (ssize_t)fs_write((int)a[1], (const void*)a[2], (size_t)a[3]);break;
  case SYS_brk : _heap.end = (void *)a[1]; c->GPRx = 0 ;break;
  default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
