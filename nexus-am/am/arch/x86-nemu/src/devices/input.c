#include <am.h>
#include <x86.h>
#include <amdev.h>

#define I8042_DATA_PORT 0x60   //添加键盘设备的信息


#define KEYDOWN_MASK 0x8000
              
size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
      int key = inl(I8042_DATA_PORT);
      kbd->keydown = ((key & KEYDOWN_MASK) == KEYDOWN_MASK) ? 1:0 ;
      kbd->keycode = key;
      return sizeof(_KbdReg);
    }
  }
  return 0;
}
