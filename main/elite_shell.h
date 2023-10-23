//mr_shell header only library
//tcp socket based cli
//server at 9002, try netcat or sth via a terminal. "nc espmatrix.local 9002"
//"rain" worky - starting the basic rain animation
//"kill",alias "stop" alias "exit" worky - it ends the current running animation and calls chil and base destructors
//"reboot" - software reset of the esp32 host device
#include "string.h"
#include "spritegetter.h"
#include "elite_rain.h"
#include "elite_pixel_app_template.h"
#pragma once



typedef struct{
  char* name;
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

  named_env_var_t username={.name="username",.value="user"};
  named_env_var_t hostname={.name="hostname",.value="esp32matrix"};
  named_env_var_t prompt={.name="prompt",.value="$ "};
  named_env_var_t pwd={.name="pwd",.value="/"};


  self->num_named_env_vars=4;
  self->named_env_vars=(named_env_var_t*)malloc(sizeof(named_env_var_t)*self->num_named_env_vars);
  self->named_env_vars[0]=username;
  self->named_env_vars[1]=hostname;
  self->named_env_vars[2]=pwd;
  self->named_env_vars[3]=prompt;

  return self;
};
bool elite_shell_send_prompt(elite_shell_t *self,int outfd,int flags){
  char r[128]={0};
  sprintf(r,"%s@%s:%s%s",self->named_env_vars[0].value,self->named_env_vars[1].value,self->named_env_vars[2].value,self->named_env_vars[3].value);
  send(outfd,r,strlen(r),flags);
  return true;
};

bool elite_shell_handle_input(int outfd,const char* buf, size_t len,int flags){

  int cmd=0;
  if (!strcmp(buf,"rain\n\0"))cmd=1;
  if (cmd==0&&!strcmp(buf,"reboot\n\0"))cmd=2;
  if (cmd==0&&!strcmp(buf,"kill\n\0"))cmd=3;
  if (cmd==0&&!strcmp(buf,"exit\n\0"))cmd=4;
  if (cmd==0&&!strcmp(buf,"testfs\n\0"))cmd=5;
  if (cmd==0&&!strcmp(buf,"help\n\0"))cmd=6;
  if (cmd==0&&!strcmp(buf,"server\n\0"))cmd=7;
  //if (cmd==0&&!strcmp(buf,"getsprites\n\0"))cmd=8;
  if (cmd==0&&!strcmp(buf,"template\n\0"))cmd=9;


  char* wtf_str="wtf?\n";
  char* ok_str="ok\n";
  switch (cmd){
    case 1 : {
      if(elite_theres_a_pixel_game_running==false&&elite_kill_pixel_game==false)
      rain_start_task();
      break;
    };
    case 2 : {
      elite_kill_pixel_game=true;
      main_reboot();
      break;
    };
    case 3 : {
      if(elite_theres_a_pixel_game_running==true&&elite_kill_pixel_game==false){
        elite_kill_pixel_game=true;
      };
      break;
    };
    case 4 : {
      return false;
      break;
    };
    case 5 : { elite_test_little_fs();break;};
    case 6 : {
        const char *help_str="try <rain> <kill> <reboot> <server> <exit> <testfs>";
        send(outfd,help_str,strlen(help_str),flags);
        return true;
        break;
    };
    case 7 : {elite_start_file_server("/littlefs");break;};
    case 8 : {
        for (int i=0;i<NUM_FILES;i++){
          get_sprite(i);
          elog("INFO : [elite_shell_handle_input] get_sprite() returned");
          vTaskDelay(log_delay / portTICK_PERIOD_MS);
        };
        break;
    };
    case 9 : {
        if(elite_theres_a_pixel_game_running==false&&elite_kill_pixel_game==false)
        template_pixel_app_start_task();
        break;
    };

    default : {
      send(outfd,wtf_str,strlen(wtf_str),flags);
      return true;
    };
  }
  send(outfd,ok_str,strlen(ok_str),flags);
  return true;
};
