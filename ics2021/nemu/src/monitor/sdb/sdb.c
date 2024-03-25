#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
word_t paddr_read(paddr_t addr, int len) ;
/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
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
word_t expr(const char *e, bool *success);

#define TEST_CMD_P_PATH "./tools/gen-expr/input"
static int  test_cmd_p(){
  int counter = 0;
  char buffer[65535];
  char *expression;
  FILE *fp = fopen(TEST_CMD_P_PATH, "r");
  assert(fp != NULL);

  char* input = fgets(buffer, ARRLEN(buffer), fp);
  while (input != NULL){
    input[strlen(input) - 1] = '\0'; // 删去末尾'\n'
    uint32_t ans = 0;
    bool success = false;
    char* ans_text = strtok(input, " ");
    sscanf(ans_text, "%u", &ans);
    expression = input + strlen(ans_text) + 1;
    IFDEF(CONFIG_DEBUG, Log("Testing %u %s ...\n", ans, expression));
    uint32_t result = expr(expression, &success);
    assert(result == ans);
    input = fgets(buffer, ARRLEN(buffer), fp);
    ++counter;
  }

  Log("通过%d个测试样例", counter);
    return 0;
}
static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_p(char *args){
  bool success;
  u_int32_t v = expr(args, &success);
  if (success)
        printf("the res = %u ,WELL DONE\n",v);
      return 0;    
}

uint8_t* guest_to_host(paddr_t paddr);

static int cmd_xe(char *args){
    char *arg = strtok(NULL," ");
    int n = -1;
    bool success = true;
    paddr_t base = 0x80000000;
    sscanf(arg, "%d",&n);
    arg = args + strlen(arg) + 1;
    base = expr(arg,&success);
    if(!success)
        return 1;
    for (int i = 0; i < n; ++i){
        if (i % 4 == 0){
            printf ("\n\e[1;36m%#x: \e[0m\t", base + i * 4);
        }
        for (int j = 0; j < 4; ++j){
            uint8_t* pos = guest_to_host(base + i * 4 + j);
            printf("%.2x ", *pos);
        }
        printf("\t");
        }       
    printf("\n");
    return 0;
}
static int cmd_px(char *args){
    bool success;
    uint32_t v = expr(args,&success);
    if(success)
        printf("%s = \e[1;36m%#.8x\e[0m\n",args,v);
    else
        printf("ERROR in expr\n");
    return 0;
}

static int cmd_q(char *args) {
    nemu_state.state = NEMU_QUIT;
    return -1;
}

static int cmd_w(char *args){
    bool success = true;
    WP* point = new_wp(args,&success);
    if(!success)
        printf("ERROR in the F new_wp");
    else
        printf("Created a \e[1;36mWatchPoint(NO.%d)\e[0m: %s \n",
            point->NO,point->w_expr);
    return 0;
}

static int cmd_si(char *args){
    int step = 0;
    if(args == NULL)
        step = 1;
    else 
        sscanf(args,"%d",&step);
    cpu_exec(step);
    return 0;
}

static int cmd_d(char *args){
    int NO;
    sscanf(args,"%d",&NO);
    free_wp(NO);
    return 0;

}
static int cmd_info(char *args){
    if(args == NULL)
        printf("No args\n");
    else if (strcmp(args,"r") == 0)
        isa_reg_display();
    else if(strcmp(args,"w") == 0)
        watchpoint_display();
    else 
        printf("不明参数 <%s>\n",args);
    return 0;
}

static int cmd_x(char *args){
    char *n = strtok(args," ");
    char *baseaddr = strtok(NULL," ");

    int len = 0;
    paddr_t addr = 0;
    sscanf(n,"%d",&len);
    sscanf(baseaddr,"%x",&addr);

    for(int i = 0;i<len;i++,addr+=4)
        printf("%x\n",paddr_read(addr,4));

    return 0;
}
static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "x", "Scan Memory", cmd_x },
  { "si", "si N 程序单步执行，不给N默认1", cmd_si },
  { "info", "Print reginfo or WatchPoint", cmd_info },
  { "p", "p expr 求出表达式expr的值并打印 ", cmd_p},  
  { "T", "test the expr",test_cmd_p },
  { "xe", "x N expr求出表达式expr，将结果作为起始内存地址，以十六进制输出的N个4字节"
      ,cmd_xe },
  { "w", "w expr 当表达式expr发生变化，暂停程序执行",cmd_w },
  { "d", "d N删除序号为N的监视点" , cmd_d},
  { "px", "与p一致，但以16y进制输出结果", cmd_px },
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

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

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
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

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
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

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();
    
  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
