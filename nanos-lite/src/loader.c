#include "proc.h"

#define DEFAULT_ENTRY 0x4000000

typedef size_t ssize_t;
typedef size_t off_t;
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
int fs_open(const char *pathname, int flags, int mode);
size_t fs_filesz(int fd);
ssize_t fs_read(int fd, void* buf, size_t len);
ssize_t fs_close(int fd);
ssize_t fs_write(int fd, const void* buf, size_t len);
off_t fs_lseek(int fd, off_t offset, int whence);

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
    
 int fd = fs_open(filename, 0, 0);
 uint32_t filesz = fs_filesz(fd);
 int size_left = filesz;
 void* va = (void*)DEFAULT_ENTRY;
 while(size_left > 0) {
     void* pa = new_page(1);
     _map(&(pcb->as), va, pa, 1);
     va += PGSIZE;
     int size_to_read = PGSIZE;
     if(size_left < PGSIZE) size_to_read = size_left;
     fs_read(fd, pa, size_to_read);
     size_left -= PGSIZE;
 }
 
 pcb->max_brk = DEFAULT_ENTRY + filesz;
 
 //Log("before loader return");
 
 //  fs_close(fd);return DEFAULT_ENTRY;
  return DEFAULT_ENTRY;
}



void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->tf = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
