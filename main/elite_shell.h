//mr_shell header only library
#include "string.h"
#pragma once



typedef struct{
  char* var_name;
  char* value;
}named_env_var_t;

typedef struct{
  char* cmd_str;
  int (*cmd_function_ptr)(void*,void*,void*);
}intrinsic_cmd_t;


typedef struct {
  char* in_buf;
  named_env_var_t *named_env_vars;
  uint8_t num_named_env_vars;
  intrinsic_cmd_t *intrinsic_cmds;
  uint8_t num_intrinsic_cmds;
  //out_buf should be a file/stream but i have no vfs file driver atm. just a socket. and no pipes and stuff
  //ill go for a hackish adapter byte banging function for now
  char* out_buf;
  int outfd;
}elite_shell_t;


elite_shell_t* elite_shell_construct(){
  elite_shell_t *self=(elite_shell_t*)malloc(sizeof(elite_shell_t));
/*
  named_env_var_t prompt={"prompt","$ "};
  named_env_var_t pwd={"pwd","/"};
  named_env_var_t username={"username","user"};
  named_env_var_t hostname={"hostname","esp32matrix"};
  self->num_named_env_vars=4;
  self->named_env_vars=(named_env_var_t*)malloc(sizeof(named_env_var_t)*self->num_named_env_vars);
  self->named_env_vars[0]=username;
  self->named_env_vars[1]=hostname;
  self->named_env_vars[2]=pwd;
  self->named_env_vars[3]=prompt;
*/
  return self;
};

bool elite_shell_handle_input(int outfd,const char* buf, size_t len,int flag){
  char* response="lalala";
  int cmd=0;
  if (strlen(buf)==strlen("rain\n")) {
      if (strcmp(buf,"rain\n")) {response="rain#newline";cmd=1;};
    };
  if (strlen(buf)==strlen("rain\0")) {
        if (!strcmp(buf,"rain\0")) {response="rain#null";cmd=1;};
      };
  if (strlen(buf)==strlen("rain\n\0")) {
            if (!strcmp(buf,"rain\n\0")) {response="rain#newline#null";cmd=1;};
  };
  if (strlen(buf)==strlen("reboot\n\0")) {
            if (!strcmp(buf,"reboot\n\0")) {response="reboot#newline#null";cmd=2;};
  };
  if (strlen(buf)==strlen("stop\n\0")) {
            if (!strcmp(buf,"stop\n\0")) {response="stop#newline#null";cmd=3;};
  };
  if (strlen(buf)==strlen("kill\n\0")) {
            if (!strcmp(buf,"kill\n\0")) {response="kill#newline#null";cmd=3;};
  };
  if (strlen(buf)==strlen("exit\n\0")) {
            if (!strcmp(buf,"exit\n\0")) {response="exit#newline#null";cmd=3;};
  };
  if (strlen(buf)==strlen("rain")) {
            if (!strcmp(buf,"rain")) {response="rain";cmd=1;};
  };
  send(outfd,response,strlen(response),flag);
  if (cmd==1){if(main_theres_a_pixel_game_running==false&&main_kill_pixel_game==false){main_start_pixel_game_task();};};
  if (cmd==2){main_reboot();};
  if (cmd==3){if(main_theres_a_pixel_game_running==true&&main_kill_pixel_game==false){main_kill_pixel_game=true;};};
  return true;
};
