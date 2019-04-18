#include "nemu.h"
#include "string.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include "stdlib.h"
enum {
  TK_NOTYPE = 256, TK_EQ =257, TK_NUM10 = 258 , TK_LEFT = 259 , TK_RIGHT = 260,
  TK_OR = 261 , TK_NOT_EQ = 262 , TK_SM_OR_EQ = 263 , TK_BG_OR_EQ = 264 , 
  TK_LSHIFT = 265 , TK_RSHIFT = 266 , TK_BG = 267 , TK_SM = 268 , 
  TK_NEG = 269 ,TK_POINT = 270 , TK_AND = 271 , TK_NUM16 = 272 , TK_REG = 273

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
    //剩下的操作符
    {"\\|\\|", TK_OR},
    {"&&",  TK_AND},
    {"==",  TK_EQ},       // equal
    {"!=",  TK_NOT_EQ},       // not equal
    {"<=",  TK_SM_OR_EQ},
    {">=",  TK_BG_OR_EQ},
    {"<<",  TK_LSHIFT},
    {">>",  TK_RSHIFT},
    {">",   TK_BG},
    {"<",   TK_SM},
    //base
    {" +", TK_NOTYPE},    // spaces
    {"\\+", '+'},         // plus
    {"\\-", '-'},
    {"\\*", '*'},
    {"\\/", '/'},
    {"0x[0-9a-fA-F]{1,8}", TK_NUM16},
    {"-?[0-9]+", TK_NUM10},
    {"\\(", TK_LEFT},
    {"\\)",TK_RIGHT},
    {"==", TK_EQ} ,        // equal
    {"\\$e(ax|cx|dx|bx|sp|bp|si|di|ip)",TK_REG} //reg 
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */

void init_regex() {
    int i;
    char error_msg[128];
    int ret;
    for (i = 0; i < NR_REGEX; i ++) {
        ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if (ret != 0) {
            regerror(ret, &re[i], error_msg, 128);
            panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
                                  
        }
                    
    }
          
}

typedef struct token {
      int type;
      char str[32];
        
} Token;
Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  nr_token = 0;
 
  while (e[position] != '\0') {
    /* Try all rules one by one. */
      for (i = 0; i < NR_REGEX; i ++) {
        if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. or certain types
         * of tokens, some extra actions should be performed.
         */
        /*
        printf("%d \n",  rules[0].token_type);  
        printf("%d \n",  rules[1].token_type);
        printf("%d \n",  rules[2].token_type); 
        printf("%d \n",  rules[3].token_type);  
        printf("%d \n",  rules[4].token_type);  
        printf("%d \n",  rules[5].token_type);  
        printf("%d \n",  rules[6].token_type);  
        printf("%d \n",  rules[7].token_type);  
        */
        switch (rules[i].token_type) {
        case  TK_NOTYPE : break;
        case  TK_BG :tokens[nr_token].type = TK_BG ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;
        case  TK_EQ :tokens[nr_token].type = TK_EQ ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;      
        case  TK_OR :tokens[nr_token].type = TK_OR ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;      
        case  TK_SM :tokens[nr_token].type = TK_SM ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;      
        case  TK_AND :tokens[nr_token].type = TK_AND ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;      
        case  TK_LSHIFT : tokens[nr_token].type = TK_LSHIFT ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;      
        case  TK_RSHIFT : tokens[nr_token].type = TK_RSHIFT ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;      
        case  TK_BG_OR_EQ : tokens[nr_token].type = TK_BG_OR_EQ ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;      
        case  TK_SM_OR_EQ : tokens[nr_token].type = TK_SM_OR_EQ ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;      
        case  TK_NOT_EQ : tokens[nr_token].type = TK_NOT_EQ ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;      
        case '+' :tokens[nr_token].type = '+' ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;
        case '-' :tokens[nr_token].type = '-' ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;
        case '*' :tokens[nr_token].type = '*' ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;       
        case '/' :tokens[nr_token].type = '/' ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;
        case TK_LEFT :tokens[nr_token].type = TK_LEFT ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;
        case TK_RIGHT :tokens[nr_token].type = TK_RIGHT ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;
        case TK_NUM10 :tokens[nr_token].type = TK_NUM10 ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;
        case TK_NUM16 :tokens[nr_token].type = TK_NUM16 ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;
        case TK_REG :tokens[nr_token].type = TK_REG ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);tokens[nr_token].str[substr_len] = '\0';break;
        default: TODO();
        }
        nr_token+=1;
        break;
        }
       // printf("%d",nr_token);
    }
    
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


//p = head q = end

bool check_parentheses(int p ,int q){
   // printf("--------------\n");
    int i,tag = 0;
    if(tokens[p].type != TK_LEFT || tokens[q].type != TK_RIGHT) return false; //首尾没有()则为false
    for(i = p ; i <= q ; i ++){
        if(tokens[i].type == TK_LEFT) tag++;
        else if(tokens[i].type == TK_RIGHT) tag--;
        if(tag == 0 && i < q) return false ;  //(3+4)*(5+3) 返回false
    }
    if( tag != 0 ) return false;
    return true;
}
//判断运算符优先级，数字越小，优先级越高。
int pir(int tpye){
    int re = -1;
    switch(tpye){
    case '+': re = 4 ; break;
    case '-': re = 4 ; break;
    case '*': re = 3 ; break;
    case '/': re = 3 ; break;
    case TK_NEG :re  = 2 ; break;
    case TK_POINT : re  = 2 ; break;
    case TK_LSHIFT : re = 5 ; break;
    case TK_RSHIFT : re = 5 ; break;
    case TK_BG : re = 6 ; break;
    case TK_SM : re = 6 ; break;
    case TK_BG_OR_EQ : re = 6 ; break;
    case TK_SM_OR_EQ : re = 6 ; break;
    case TK_EQ : re = 7 ; break;
    case TK_NOT_EQ : re = 7 ; break;
    case TK_OR : re = 12 ; break;
    default : break;
    }
    return re;
}

int dominant_operator(int p , int q){

    int i ,dom = p, left_n = 0;
    int pr = -1 ;
    for(i = p ; i <= q ; i++){
        if(tokens[i].type == TK_LEFT){
            left_n += 1;
            i++;
            while(1){
                if(tokens[i].type == TK_LEFT) left_n += 1;
                else if(tokens[i].type == TK_RIGHT) left_n --;
                i++;
                if(left_n == 0)
                    break;
            }
            if(i > q)break;
        }
        else if(tokens[i].type == TK_NUM10) continue;
        else if(pir(tokens[i].type ) > pr){
            pr = pir(tokens[i].type);
            dom = i;
        }
    }
   // printf("%d\n",left_n);
    return dom;
}

//递归函数
uint32_t eval(int p ,int  q) {
    int op,i;
    uint32_t val1,val2,result;
    if (p > q) {
        if ( tokens[p].type == '+' )
            return 0;
        else if (tokens[p].type == '-')
            return 0;
             /* Bad expression */
          printf("Bad expressopn ! \n");
          assert(0);
    }
    else if (p == q) {
            /* Single token.
             *      * For now this token should be a number.
             *           * Return the value of the number.
             *              * 2019-4-2 只考虑NUM10
             *                   */
        
        int type = tokens[p].type;
        u_int32_t t = 0;
        char *str;
        switch(type){
        case TK_NUM10 : 
            t = atoi(tokens[p].str);
            return t;
        case TK_NUM16:
            t =  strtol(tokens[p].str,&str,16 );
            return t;
        case TK_REG :
           // printf("%s \n", tokens[p].str);
            if(strcmp(tokens[p].str,"$eax") == 0) return cpu.eax;
            else if(strcmp(tokens[p].str ,"$ecx") == 0) return cpu.ecx;
            else if(strcmp(tokens[p].str ,"$edx") == 0) return cpu.edx;
            else if(strcmp(tokens[p].str ,"$ebx") == 0) return cpu.ebx;
            else if(strcmp(tokens[p].str ,"$esp") == 0) return cpu.esp;
            else if(strcmp(tokens[p].str ,"$ebp") == 0) return cpu.ebp;
            else if(strcmp(tokens[p].str ,"$esi") == 0) return cpu.esi;
            else if(strcmp(tokens[p].str ,"$edi") == 0) return cpu.edi;
            else if(strcmp(tokens[p].str ,"$eip") == 0) return cpu.eip;
            else  assert(0);
        default :   assert(0);
        }
    }
    else if (check_parentheses(p, q) == true) {
            /* The expression is surrounded by a matched pair of parentheses.
             *      * If that is the case, just throw away the parentheses.
             *           */
            return eval(p + 1, q - 1);
              
    }
    else {
            /* We should do more things here. */
        op = dominant_operator(p,q);
        // printf("op位置是 %d \n",op);
        //处理负号和指针 优先级为2 
        //第一版只处理十进制数字
        if(tokens[op].type == TK_NEG){
            for( i = op ; i<nr_token ; i++){
                if(tokens[i].type == TK_NUM10){
                    sscanf(tokens[i].str, "%x", &result);
                    //printf("%d \n",result);
                    // return -result;
                    break;
                }

            }
            for( ;i > 0 ;i --) result = -result;
            return result;
        }
        else if (tokens[op].type == TK_POINT){
            for( i = op ; i<nr_token ; i++){
                if(tokens[i].type == TK_NUM10){
                    sscanf(tokens[op+1].str, "%x", &result);
                   // result = vaddr_read(result, 4);
                   // return result;
                    break;
                }
            }
            for( ;i > 0 ;i -- ) vaddr_read(result, 4);
            return result;
            
        }
        
        // printf("%d", op);
        val1 = eval(p, op - 1);
        val2 = eval(op + 1, q);
        switch (tokens[op].type) {
        case '+': return val1 + val2;
        case '-': return val1 - val2;
        case '*': return val1 * val2;
        case '/': return val1 / val2;
        case TK_RSHIFT : return val1 >> val2;
        case TK_SM_OR_EQ : if (val1 <= val2) return 1; else return 0;
        case TK_BG_OR_EQ : if (val1 >= val2) return 1; else return 0;                                
        case TK_EQ  : if (val1 == val2) return 1; else return 0;
        case TK_NOT_EQ : if (val1 != val2) return 1; else return 0;
        case TK_SM : if (val1 < val2) return 1; else return 0;
        case TK_BG : if (val1 > val2) return 1; else return 0;
        case TK_AND: return val1 && val2;
        case TK_OR:  return val1||val2;
        default: assert(0);

        }
    }
    return -1;
}


uint32_t expr(char *e, bool *success) {
  
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  *success = true;
  /*
  else{                                                                                                                
      *success = true;                                                                                                 
      return eval( 0 , nr_token - 1 );                                                                                  
  } */ 
  // 指针和负数的区分
  // printf("%d\n",nr_token);
  for(int i = 0 ;i<nr_token;i++) {
    if(tokens[i].type=='*'&&(i==0||(tokens[i-1].type!=TK_NUM10&&tokens[i-1].type!=TK_LEFT&&tokens[i].type!=TK_NUM16)))
        tokens[i].type = TK_POINT;
    if(tokens[i].type=='-'&&(i==0||(tokens[i-1].type!=TK_NUM10&&tokens[i-1].type!=TK_LEFT&&tokens[i].type!=TK_NUM16)))
        tokens[i].type = TK_NEG;
  }
 // for (int i = 0 ; i <nr_token;i++)printf("%d\n", tokens[i].type);
  return eval(0, nr_token - 1);
  /* TODO: Insert cides ti evaluate the expression. */
  TODO();
  return 0;
}
