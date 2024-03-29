#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "cpu/reg.h"
//#include "src/memory/memory.c"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
extern CPU_state cpu;


/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}
static int cmd_si(char *args){

//  first 
//    char *str = rl_gets();
//    char *arg = strtok(str," ");    
    
    char *arg = strtok(args," ");
   // printf("%s\n",arg);
    if(arg == NULL){
        printf("too few arguments.\n");
        return 1;
    }
    int num = atoi(arg);
    cpu_exec(num);
    printf("OK");
    return 0;
};
static int cmd_info(char *args){
    char *arg = strtok(args," ");
    printf("%s\n",arg);
    //cpu info
    if (strcmp(arg,"r")==0){
        printf("eax is 0x%08x\n",cpu.eax);
        printf("ecx is 0x%08x\n",cpu.ecx);
        printf("edx is 0x%08x\n",cpu.edx);
        printf("ebx is 0x%08x\n",cpu.ebx);
        printf("esp is 0x%08x\n",cpu.esp);
        printf("ebp is 0x%08x\n",cpu.ebp); 
        printf("esi is 0x%08x\n",cpu.esi);
        printf("edi is 0x%08x\n",cpu.edi);
        printf("---------------------------\n");
    }
    else if(strcmp(arg,"w")==0){
        print_wp();
    }
   
    return 0;
}
static int cmd_x(char *args){
    //获取内存起始地址和扫描长度。
    if(args == NULL){
        printf("too few parameter! \n");                                                                       
        return 1;
    }
    
    char *arg = strtok(args," ");
    if(arg == NULL){
        printf("too few parameter! \n");
        return 1;
    }
    int  n = atoi(arg);
    char *EXPR = strtok(NULL," ");
    if(EXPR == NULL){
        printf("too few parameter! \n");
        return 1;
    }
    if(strtok(NULL," ")!=NULL){
        printf("too many parameter! \n");
        return 1;
    }
    bool success = true;
    //vaddr_t addr = expr(EXPR , &success);
    if (success!=true){
        printf("ERRO!!\n");
        return 1;
    }
    char *str;
   // vaddr_t addr = atoi(EXPR);
    vaddr_t addr =  strtol( EXPR,&str,16 );
   // printf("%#lX\n",ad);
    //进行内存扫描,每次四个字节;
    for(int i = 0 ; i < n ; i++){
        uint32_t data = vaddr_read(addr + i * 4,4);
        printf("0x%08x  " , addr + i * 4 );
        for(int j =0 ; j < 4 ; j++){
            printf("0x%02x " , data & 0xff);
            data = data >> 8 ;
        }
        printf("\n");
    }

    return 0;
}

static int cmd_p(char *args){
   
    bool su = true;
    char *arg = strtok(args, " ");
   // printf("%s\n %ld\n" , arg, strlen(arg));
    printf("0x%08llx \n ",expr(arg,&su));
    return 0;
}

static int cmd_w(char *args){
    char *arg = strtok(args," ");
    if(arg == NULL){
        printf("too few parameter!\n ");
        return 1;
    }
    else{
        
        bool su;
        uint32_t res = expr(arg,&su);
      //  printf("%s %d  %d \n",arg,res,su);
        if(su){
            new_wp(arg,res);
            printf("add a watchpoint %s   0x%08x  \n!",arg,res);
        }
        else{
            printf("error!\n");
        }
        return 0;
    }
}

//删除监视点
static int cmd_d(char *args){
    char *arg = strtok(args," ");
    if(arg == NULL){
        printf("too few parameter!\n ");
        return 1;
    }
    else{
        int no = atoi(arg);
        if(no>=0&&no<=32){
            free_wp(no);
            return 0;
        }else{
            printf("输入错误！\n");
            return 1;
        }
    }
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
    { "help", "Display informations about all supported commands", cmd_help },
    { "c", "Continue the execution of the program", cmd_c },
    { "q", "Exit NEMU", cmd_q },
    {"si"," Single-step execution",cmd_si},
    {"info","Print program status",cmd_info},
    {"x","Scanning memory",cmd_x},
    {"p","表达式求值",cmd_p},
  /* TODO: Add more commands */
    {"w","设置监视点。",cmd_w},
    {"d","删除监视点。",cmd_d}
};
#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
