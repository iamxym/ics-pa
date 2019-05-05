#include "common.h"
#include <amdev.h>
#include "klib.h"



size_t serial_write(const void *buf, size_t offset, size_t len) {
  
    Log("Used serial_write!");
    char *n_buf = (char*) buf;
    for(int i = 0 ; i < len ; i++){
       _putc(n_buf[i]); 
    }
    return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
    int key = read_key();
    char temp[256];
    //判断是键盘事件还是时间事件
    if(key != 0){
	Log("key!!!!!!");
        if((key & 0x8000) != 0){
			sprintf(temp, " %s\n", keyname[key ^ 0x8000]);
			strncpy(buf, temp, len);
		}
		else {
			sprintf(temp, " %s\n", keyname[key]);
			strncpy(buf, temp, len);
		}
    }
    else{
        Log("time !!!!");
		uint32_t n_time = uptime();
		sprintf(temp, "t %d\n", n_time);
		strncpy(buf, temp, len);
    }
	if(strlen(temp) < len) len = strlen(temp);
  return len;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
    memcpy(buf, (void*)dispinfo + offset, len);
    return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  
    int width = screen_width();
    
    int x = (offset / 4) % width;
    int y = (offset / 4) / width;
    draw_rect((uint32_t*)buf,x,y,len/4,1);
    return len;

}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
	int width = screen_width();
	int height = screen_height();
	int x = sprintf(dispinfo, "WIDTH:%d\n", width);
	sprintf(dispinfo + x, "HEIGHT:%d\n", height);

}
