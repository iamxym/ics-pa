#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
    size_t i = 0;
    while(*(s+i) != '\0') i++;
    return i;
}

char *strcpy(char* dst,const char* src) {
    size_t i = 0;
    while(*(src+i) != '\0'){
        *(dst+i) = *(src+i);
        i++;
    }
    //注意此处不需要'\0'
    return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
    size_t i;
    for(i = 0; i < n && src[i] != '\0'; ++i)    dst[i] = src[i];                  
    for( ; i < n; ++i )  dst[i] = '\0';
    return dst;
}

char* strcat(char* dst, const char* src) {
    size_t dst_len = strlen(dst);
    size_t i = 0;
    for(i = 0; src[i] != '\0'; ++i) dst[dst_len + i] = src[i];
    dst[dst_len + i] = '\0';
    return dst;
}

int strcmp(const char* s1, const char* s2) {
    int l1 = strlen(s1);
    int l2 = strlen(s2);
    int l = 0;
    if(l1 < l2) l = l1;
    else l = l2;
    for(int i = 0; i <= l; ++i){
        if(s1[i] < s2[i]) return -1;
        else if(s1[i] > s2[i]) return 1;
    }
    return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    for(size_t i = 0; i < n; ++i){
        if(s1[i] < s2[i]) return -1;
        else if(s1[i] > s2[i]) return 1;                      
    }
    return 0;
}

void* memset(void* v,int c,size_t n) {
    if(v == NULL || n < 0) return NULL;
    char* temp = (char*) v;
    for(int i = 0; i < n; ++i){
        temp[i] = (char) c;
    }
    return v;
}

void* memcpy(void* out, const void* in, size_t n) {
    if(out == NULL || n < 0) return NULL;
    char* charout = (char*) out;
    char* charin = (char*) in;
    for(int i = 0; i < n; ++i)    charout[i] = charin[i];
    return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
    char* c1 = (char*) s1;
    char* c2 = (char*) s2;
    for(int i = 0; i < n; ++i){
        if(c1[i] < c2[i]) return -1;
        else if(c1[i] > c2[i]) return 1;
    }
    return 0;
}

#endif
