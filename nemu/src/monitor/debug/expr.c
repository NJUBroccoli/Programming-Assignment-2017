#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,TK_NUM

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
  {"\\(",'('},			//left_bracket
  {"\\)",')'},			//right_bracket
  {"[0-9]+",TK_NUM},		// num
  {"\\*", '*'},         // multiply
  {"/", '/'},          // divide
  {"\\+", '+'},         // plus
  {"\\-", '-'},			// subtract
  {"==", TK_EQ},         // equal
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
  char str[128];
} Token;

Token tokens[128];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i,index;
  regmatch_t pmatch;
  nr_token = 0;

  
  while (e[position] != '\0') {
    /* Try all rules one by one. */
	  bool discard_space=false;
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
		Token new_token;
        switch (rules[i].token_type) {
			case TK_NOTYPE: discard_space=true;
							break;
			case '+':		new_token.type=rules[i].token_type;
							break;
			case TK_EQ:		new_token.type=rules[i].token_type;
							break;
			case TK_NUM:	printf("substr_len:%d\n",substr_len);
							new_token.type=rules[i].token_type;
							for (index=0;index<substr_len;index++)
								new_token.str[index]=substr_start[index];
							break;
			case '-':		new_token.type=rules[i].token_type;
							break;
			case '/':		new_token.type=rules[i].token_type;
							break;
			case '*':		new_token.type=rules[i].token_type;
							break;
			case '(':		new_token.type=rules[i].token_type;
							break;
			case ')':		new_token.type=rules[i].token_type;
							break;

			default: TODO();
        }
		if (discard_space)
			break;
		tokens[nr_token++]=new_token;
		break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  int i,j;



  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for (i=0;i<nr_token;i++)
  {
	  printf("type:%d\n",tokens[i].type);
	  if (tokens[i].type==258)
	  {
		  printf("str: ");
		  int size=strlen(tokens[i].str);
		  for (j=0;j<size;j++)
			  printf("%c",tokens[i].str[j]);
		  printf("\n");
	  }
  }
  /* TODO: Insert codes to evaluate the expression. */
//  TODO();

  return 0;
}
