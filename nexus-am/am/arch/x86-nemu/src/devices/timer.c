#include <am.h>
#include <x86.h>
#include <amdev.h>

#define RTC_PORT 0x48

uint32_t boot_time;

size_t timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
      _UptimeReg *uptime = (_UptimeReg *)buf;
      uint64_t temptime = inl(RTC_PORT) - boot_time;
      uptime->hi = (uint32_t) (temptime >> 32);
      uptime->lo = (uint32_t) temptime;
      //uptime->hi = 0;
      //uptime->lo = 0;
      return sizeof(_UptimeReg);
    }
    case _DEVREG_TIMER_DATE: {
      _RTCReg *rtc = (_RTCReg *)buf;
      rtc->second = 0;
      rtc->minute = 0;
      rtc->hour   = 0;
      rtc->day    = 0;
      rtc->month  = 0;
      rtc->year   = 2018;
      return sizeof(_RTCReg);
    }
  }
  return 0;
}

void timer_init() {
   // boot_time = inl(RTC_PORT);
    //gettimeofday(boot_time);    
}
