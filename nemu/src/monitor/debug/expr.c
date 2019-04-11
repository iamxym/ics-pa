#include "nemu.h"
#include "string.h"
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ =257, TK_NUM10 = 258 , TK_LEFT = 259 , TK_RIGHT = 260

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},
  {"\\*", '*'},
  {"\\/", '/'},
  {"-?[1-9]\\d*", TK_NUM10},
  {"\\(", TK_LEFT},
  {"\\)",TK_RIGHT},
  {"==", TK_EQ}         // equal
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
         * to record the token in the array `tokens'. For certain types
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
        
        case 42 :tokens[nr_token].type = 1 ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);break;
        case 43 :tokens[nr_token].type = 2 ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);break;
        case 45 :tokens[nr_token].type = 3 ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);break;       
        case 47 :tokens[nr_token].type = 4 ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);break;
        case 258 :tokens[nr_token].type = 258 ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);break;
        case 259 :tokens[nr_token].type = 6 ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);break;
        case 260 :tokens[nr_token].type = 7 ; strncpy(tokens[nr_token].str,e + position - substr_len , substr_len);break;
        
        default: TODO();
        }
      
        break;
        }
        nr_token+=1;
     // printf("%d",nr_token);
    }
    
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
 
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for(int i = 0 ;i<nr_token;i++)
  {
    printf("%s\n",tokens[i].str);
  }
  /* TODO: Insert cides ti evaluate the expression. */
  TODO();

  return 0;
}
/*
uint32_t expr(char *e){
    make_token(e);
    return 0;
}
*/
