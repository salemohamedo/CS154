#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>

typedef struct{
    int redirect;
    char* formatted;
}formatted_object;

char* semi_c = ";";
char* spaces = "\v\f\r\n\t ";
void error(){//standard error message
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
}

void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}
//checks if a line only consists of some kind of white space
int only_spaces(char* cmd){
    while(*cmd != '\0'){
        if(!isspace((unsigned char)*cmd)) return 0;
        cmd++;
    }
    return 1;
}

int file_exists(char* file){
    FILE* fp;
    int ret;
    fp = fopen(file,"r");
    if(fp){
        ret = 1;
        fclose(fp);
    }
    else ret = 0;
    return ret;
}

//given a delim and a command, creates a new string array after seperating that command by the delim
char** separate(char* cmd,char* delim, int size){
    char** commands = malloc(sizeof(char*)*size);
    char* command_i = NULL;
    int i = 0;

    command_i = strtok(cmd,delim);
    commands[i] = command_i;

    for(i = 1; i < 512; i++){
        command_i = strtok(NULL,delim);
        commands[i] = command_i;
        if(commands[i] == NULL) break;

    }
    commands[i+1] = NULL;
    return commands;
}
//adds spacing so that when seperate function is called on
//a command, the > or >+ are distinguished from the rest of the string
formatted_object format_redirection(char* cmd){

    int tally = 0;
    char* formatted = malloc(sizeof(char)*514);
    int redirects_present = -1;
    int j = 0;
    for(int i = 0; cmd[i] != '\0'; i++, j++){
        if(!(cmd[i] == '>')){
            formatted[j] = cmd[i];
        }
        else{
            tally++;
            if(cmd[i+1] == '+'){
                formatted[i] = ' ';
                formatted[j+1] = '>';
                formatted[j+2] = '+';
                formatted[j+3] = ' ';
                i++;
                j+=3;
                redirects_present = 2;
            }
            else{
                formatted[j] = ' ';
                formatted[j+1] = '>';
                formatted[j+2] = ' ';
                j+=2;
                redirects_present = 1;
            }
        }
    }
    formatted_object ret_val;
    if(tally > 1){
        ret_val.redirect = -2;
    }
    else ret_val.redirect = redirects_present;
    ret_val.formatted = formatted;
    return ret_val;
}


//opens up files for redirection
void redirection(char** command, char* adv_red_file, int adv_file_exists){
	int i;
    int norm_red = 0;
    int adv_red = 0;
    for(i = 0; command[i]; i++){
        if(!(strcmp(command[i],">"))){
            norm_red = 1;
            break;
        }
      if(!(strcmp(command[i],">+"))){
            adv_red = 1;
            break;
        }
    }

    if(command[i+2]){
        return;
    }
    //if the advanced redirection file does not exist, treats it as normal redirection
    if(norm_red||!adv_file_exists){
        int file_exists2 = file_exists(command[i+1]);        
        if(file_exists2){
            error();
            exit(0);
		return;
        }
    int fd = open(command[i+1],O_RDWR|O_CREAT|O_EXCL,0777);
	if(fd < 0){
            error();
            exit(0); 
        }
        else{
            close(STDOUT_FILENO);
            if(dup2(fd,STDOUT_FILENO) < 0) error();
            command[i] = NULL;
            return;

        }}
        //I handle adv_redirection by opening up a temp file to place output of commands into
    if(adv_red){
	    int new_fd = open("tmp.txt",O_RDWR|O_CREAT|O_EXCL,0777);

    if(new_fd < 0){
            error();
            return; 
        }
        else{
            close(STDOUT_FILENO);
	
            if(dup2(new_fd,STDOUT_FILENO) < 0) error();
            adv_red_file = command[i] + 1;
            command[i] = NULL;
            return;


    }}

    return; 
    }

int built_cd(char** cmd_line){
    if(cmd_line[2]) error();
    else{
        if(!cmd_line[1]){
            if(chdir(getenv("HOME"))) error();               
            }
        else{
            if(chdir(cmd_line[1])) error();
        }
        }
    return 1;
}

int built_exit(char** cmd_line){
    if(cmd_line[1]) error();
    else exit(0);
    return 1;
}

int built_pwd(char** cmd_line){
	if(cmd_line[1]){
		error();
    }
        else {
            char cwd[512];
            if(!getcwd(cwd,512)){
		error();
    }
            else{
                myPrint(cwd);
                myPrint("\n");}
            }
        return 1;
}

int built_in(char** cmd_line){
    char* cmd = cmd_line[0];
    if(cmd){
        if(!strcmp(cmd,"cd")){
            return built_cd(cmd_line);
        }
        else if(!strcmp(cmd,"exit")){
            return built_exit(cmd_line);

        }
        else if(!strcmp(cmd,"pwd")){
            return built_pwd(cmd_line);

        }
    }
    else return 0;
    return 0;
}



void run_cmd(char* cmd_line){
    pid_t pid;
    int exec_error;
    int proc_status;
    char** commands = separate(cmd_line,semi_c,256); 
    if(!commands){
        error();
        exit(0);
    }

    char** command = NULL;
    
    char* adv_red_file = malloc(sizeof(char)*512);
    int redirect_check; 
    formatted_object f_o;      
    for(int i = 0; commands[i]; i++){
	f_o = format_redirection(commands[i]); //format >+ and > so they can be seperated
    redirect_check = f_o.redirect;
    if(redirect_check == -2) error();
    else{
        char* formatted = f_o.formatted;
        command = separate(formatted,spaces,256);
    	if(!command) error();

        int exists = 0;
            if(redirect_check == 2){
                for(int x = 0; command[x]; x++){
                    if(!strcmp(command[x],">+")&&command[x+1]){
                        adv_red_file = command[x+1];
                        exists = file_exists(adv_red_file);}

                }
            }
            if(!built_in(command)){
            pid = fork();
                if(!command){
                    error();
                    exit(0);
                }
                if(pid < 0){
                    error();
                    exit(0);
                }
                else if(pid == 0){//child
                    if(redirect_check > -1){

                        redirection(command,adv_red_file,exists);

                    }
                    exec_error = execvp(command[0],command);

    		        
                    if(exec_error){
                        error();
                        exit(0);
                    }

                    }
                else{
                    do{
                        waitpid(pid,&proc_status,WUNTRACED);}
                    while(!WIFEXITED(proc_status) && !WIFSIGNALED(proc_status));

                    if((redirect_check == 2)&&exists){
                        //if adv_redirection file exists, read tmp file and original file and write into new file, then rename the new file
                        //to adv_redirect file and delete adv_redirect file and tmp file
                        FILE* tmp = fopen("tmp.txt","r");
                        FILE* adv_file_pt = fopen(adv_red_file,"r");
                        FILE* new_file = fopen("newfile.txt","w+");
            			if(!tmp) error();
            			if(!adv_file_pt) error();
            			if(!new_file) error();

                        if(!tmp||!adv_file_pt||!new_file){
                            error();
                        }

                        char c;
                        while((c = fgetc(tmp))!=EOF) fputc(c,new_file);
                        while((c = fgetc(adv_file_pt))!=EOF) fputc(c,new_file);
                        if(fclose(tmp)||fclose(adv_file_pt)||fclose(new_file)) error();
                        if(remove(adv_red_file)) error();
                        if(remove("tmp.txt")) error();
                        if(rename("newfile.txt",adv_red_file)) error();
                        }

                }
    }
}
}
 
    return;
}

void batch_mode(char* bfile){
    char* cmdline = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* fp;
    fp = fopen(bfile, "r");
    if(!fp){
        error();
        exit(0);
    }
    while((read = getline(&cmdline, &len, fp)) != -1){
        if(read >= 513){
            myPrint(cmdline);
            error();
        }
        else if(!only_spaces(cmdline)){
            myPrint(cmdline);
            run_cmd(cmdline);
        }
    }

    fclose(fp);
    return;
}

int main(int argc, char *argv[]) 
{
    char cmd_buff[100];
    char *pinput;

    if(argc == 1){
        while(1) {
            myPrint("myshell> ");
            pinput = fgets(cmd_buff, 100, stdin);
            if (!pinput) {
                exit(0);
            }
            else run_cmd(pinput);
        }}
    else batch_mode(argv[1]);
    
    return 1;
}
