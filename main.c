#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dir.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

int file_exists(char *);
void copyfile(char *, char *);
void clip_string(char *, char *, int, int);
int createfile(char *);
void handle_createfile();
int insert(char *, char *,int, int, int);
int insert_by_index(char *, char *, int, int);
void handle_insert();
void cat(char *);
void handle_cat();
int removestr(char *, int, int, int, char);
int removestr_by_index(char *, int, int);
void handle_remove();
int copy(char *,int,int,int, char);
void handle_copy();
int cut(char *,int,int,int, char);
void handle_cut();
int paste(char *,int,int);
void handle_paste();
int file_lines(char *);
int grep(char *, char *, char, int*);
void handle_grep();
int* find(char *, char *, char);
void handle_find();
void replace();
void handle_replace();
void get_command();

int main(){
  while(1){
    get_command();
  }
}

/*
Hello
Fucking
World
*/

int file_exists(char *path){
  return access(path + 1, F_OK) == -1 ? 0 : 1;
}

void copyfile(char *src, char *tg){
  
  if(!file_exists){
    printf("target doesn't exist in the path\n");
    return;
  }
  
  FILE *source = fopen(src, "r");
  FILE *target = fopen(tg, "w");

  char ch;

  while(1){
    ch = fgetc(source);

    if(ch==EOF)
      break;

    fputc(ch, target);
  }

  fclose(source);
  fclose(target);

  // printf("copy done successfully\n");
}

void clip_string(char *src, char *result, int start, int end){

  for (int i = 0; i < end - start + 1;i++)
    result[i] = src[start + i];
}

int input_path(char *path){
  
  char sub_command[10];
  int counter = 0;
  scanf("%s", sub_command);

  if(strcmp(sub_command, "--file") != 0){
    printf("invalid flag\n");
    return -1;
  }

  getchar();

  char ch = getchar();

  if(ch == '\"'){
    while(1){
      ch = getchar();
      if(ch == '\"')
        break;
      
      path[counter] = ch;
      counter++;
    }
  }
  
  else if(ch=='/'){
    *path = '/';
    scanf("%s", path+1);
    for (int i = 0; path[i] != '\0'; i++)
      counter++;
  }

  else{
    printf("invalid path format\n");
    return -1;
  }

  return 1;
}

int createfile(char *path){

  int slash_count = 0, counter=0, length = strlen(path) -1;

  for (int i = 0; i < length; i++)
    if(path[i]=='/')
      slash_count++;

  int slash_index[slash_count];

  for (int i = 0; i < length; i++)
    if(path[i]=='/'){
      slash_index[counter] = i;
      counter++;
    }

  if(file_exists(path)){
    return -1;
  }

  else  
    for (int i = 0; i < slash_count; i++){

      if(i==slash_count-1){
        FILE *new_file = fopen(path+1, "w");
        fclose(new_file);
      }

      else{    
        char folder[slash_index[i + 1] - slash_index[0]];

        for (int i = 0; i<=slash_index[i + 1] - slash_index[0]; i++)
          folder[i] = '\0';

        clip_string(path, folder, slash_index[0] + 1, slash_index[i + 1] -1);
        DIR* dir = opendir(folder);
        if (dir) {
          closedir(dir);
        } 
        else if (ENOENT == errno) {
          mkdir(folder);
          printf("folder %s created\n", folder);
        }
      }
    }
  return 1;
}

void handle_createfile(){

  char path[100] = {'\0'};

  int input_flag = input_path(path);

  if(input_flag == -1)
    return;

   createfile(path) == 1
  ? printf("file %s created\n", path+1)
  : printf("file already exists in the path\n");;
}

void handle_insert(){
  
  char path[100] = {'\0'}, sub_command[10] = {'\0'}, ch,
  *string = (char*)malloc(1000 * sizeof(char));
  int counter = 0;

  int input_flag = input_path(path);

  if(input_flag == -1)
    return;

  getchar();

  for (int i = 0; i < 10; i++)
    sub_command[i] = '\0';

  scanf("%s", sub_command);

  if(strcmp(sub_command, "--str") != 0){
    printf("invalid flag\n");
    return;
  }

  getchar();

  ch=getchar();
  counter = 0;
  int is_pre_backslash = 0, flag=0, 
  space_included = ch=='\"' ? 1 : 0;
  
  if(space_included)
    ch = getchar();

  while(1){
    if(flag)
      ch = getchar();

    flag = 1;

    if(ch=='\\'){
      if(is_pre_backslash == 0){
        is_pre_backslash = 1;
        continue;
      }
      else {
        is_pre_backslash = 0;
      }
    }
    else{
      
      if(ch=='\"' && is_pre_backslash){
        string[counter] = '\"';
        counter++;
        is_pre_backslash = 0;
        continue;;
      }
      else if(ch == '\"' && space_included)
        break;
      else if(ch == 32 && !space_included)
        break;
      if(is_pre_backslash)
        if(ch=='n'){
          string[counter] = '\n';
          counter++;
          is_pre_backslash = 0;
          continue;
        }
        
      is_pre_backslash = 0;
    }
    string[counter] = ch;
    counter++;
  }

  if(space_included)
   getchar();

  for (int i = 0; i < 10; i++)
    sub_command[i] = '\0';

  scanf("%s", sub_command);

  if(strcmp(sub_command, "--pos") != 0){
    printf("invalid flag\n");
    return;
  }

  getchar();

  int line, index;

  scanf("%d", &line);
  ch = getchar();
  if(ch!=':'){
    printf("invalid flag format\n");
    return;
  }
  scanf("%d", &index);

  int insert_flag = insert(path, string, counter, line, index);

  if(insert_flag == 1)
    printf("insert done\n");
}

int insert(char *path, char *text, int length, int line, int index){

  if(!file_exists(path)){
    printf("file doesn't exist\n");
    return -1;
  }

  char *temp_path = "/.temp/.temp.txt", ch;

  createfile(temp_path);

  copyfile(path+1, temp_path+1);
  
  FILE *file = fopen(path+1, "w");
  FILE *temp = fopen(temp_path+1, "r");

  int line_counter = 1, index_counter = 0;

  while(1){
    if(line == line_counter){
      while(1){
        if(index == index_counter)
          break;

        ch = fgetc(temp);
        if(ch==EOF){
          printf("position is out of range\n");
          return -1;
        }
        fputc(ch, file);
        index_counter++;
      }
      break;
    }

    ch = fgetc(temp);
    if(ch==EOF){
      printf("position is out of range\n");
      return -1;
    }
    fputc(ch,file);
    if(ch=='\n')
      line_counter++;
  }

  for (int i = 0; i < length; i++)
    fputc(text[i], file);

  while(1){
    
    ch = fgetc(temp);
    
    if(ch==EOF)
      break;

    fputc(ch, file);
  }

  fclose(file);
  fclose(temp);

  remove(temp_path+1);

  return 1;
}

int insert_by_index(char *path, char *text, int length, int index){

  if(!file_exists(path)){
    printf("file doesn't exist\n");
    return -1;
  }

  char *temp_path = "/.temp/.temp.txt", ch;

  createfile(temp_path);

  copyfile(path+1, temp_path+1);
  
  FILE *file = fopen(path+1, "w");
  FILE *temp = fopen(temp_path+1, "r");
  
  int index_counter = 0;

  while(1){
    if(index_counter == index)
      break;

    fputc(fgetc(temp), file);
    index_counter++;
  }

  for (int i = 0; i < length; i++)
    fputc(text[i], file);

   while(1){
    ch = fgetc(temp);

    if(ch == EOF)
      break;

    fputc(ch, file);
  }

  fclose(file);
  fclose(temp);

  remove(temp_path+1);

  return 1;
}

void cat(char *path){

  if(!file_exists(path)){
    printf("file doesn't exist\n");
    return;
  }

  FILE *file = fopen(path+1, "r");

  char ch = fgetc(file);

  for (int i = 0; ch != EOF; i++){
    printf("%c", ch);
    ch = fgetc(file);
  }

  fclose(file);

  printf("\n");
}

void handle_cat(){

  char path[100] = {'\0'};

  int input_flag = input_path(path);

  if(input_flag == -1)
    return;

  cat(path);
}

int removestr(char *path, int line,int index, int size, char direction){

  if(!file_exists(path)){
    printf("file doesn't exist\n");
    return -1;
  }

  char *temp_path = "/.temp/.temp.txt", ch;

  int line_counter = 1, index_counter = 0, total_lines = 0, line_chars = 0,
  line_number = 1, is_pre_enter = 1, temp_size =size;

  createfile(temp_path);

  copyfile(path+1, temp_path+1);

  FILE *file = fopen(path + 1, "w");
  FILE *temp = fopen(temp_path + 1, "r");

  if(direction == -1 && size>index){

    line_counter = 0;
    // specify total lines.

    while(1){
      ch = fgetc(temp);
      if(ch == EOF)
        break;
      else if(ch=='\n')
        is_pre_enter = 1;
      else if(is_pre_enter){
        total_lines++;
        is_pre_enter = 0;
      }
    }

    fclose(temp);
    temp = fopen(temp_path + 1, "r");

    int line_width[total_lines];

    for (int i = 0; i < total_lines; i++)
      line_width[i] = 0;

    // specify width of every line.

    while (1){
      line_chars = 0;
      while (1){
        ch = fgetc(temp);
        if (ch == EOF)
          break;
        line_width[line_number - 1]++;
        if (ch == '\n')
          break;
      }
      if (ch == EOF)
        break;
      line_number++;
    }

    temp_size -= index;
    
    // one line before target.
    line_number = line - 2;
    
    while(1){
      if(abs(temp_size) <= line_width[line_number])
        break;
      temp_size -= line_width[line_number];
      line_number--;
    }

    fclose(temp);
    temp = fopen(temp_path + 1, "r");

    while(1){
      if(line_counter == line_number){
        while(1){      
          if(line_width[line_number]-temp_size <= index_counter){
            for (int i = 0; i < size; i++)
              fgetc(temp);
            break;
          }
          ch = fgetc(temp);
          if(ch==EOF){
            printf("position is out of range\n");
            return -1;
          }
          fputc(ch, file);
          index_counter++;
        }
        break;
      }
    
      ch = fgetc(temp);
      if(ch==EOF){
        printf("position is out of range\n");
        return -1;
      }
      fputc(ch,file);
      if(ch=='\n')
        line_counter++;
    }
  }
 
  else {
    while(1){
      if(line == line_counter){
        while(1){
          if(direction == 1){
            if(index == index_counter){
              for (int i = 0; i < size; i++)
                fgetc(temp);
              break;
            }
            ch = fgetc(temp);
            if(ch==EOF){
              printf("position is out of range\n");
              return -1;
            }
            fputc(ch, file);
            index_counter++;
          }
          else {
            if(index-size <= index_counter){
              for (int i = 0; i < size; i++)
                fgetc(temp);
              break;
            }
            ch = fgetc(temp);
            if(ch==EOF){
              printf("position is out of range\n");
              return -1;
            }
            fputc(ch, file);
            index_counter++;
          }
        }
        break;
      }

      ch = fgetc(temp);
      if(ch==EOF){
        printf("position is out of range\n");
        return -1;
      }
      fputc(ch,file);
      if(ch=='\n')
        line_counter++;
    }
  }
 
  while(1){
    ch = getc(temp);
    if(ch == EOF)
      break;
    putc(ch, file);
  }

  fclose(file);
  fclose(temp);

  remove(temp_path +1);
  return 1;
}

int removestr_by_index(char *path,int index, int size){
   if(!file_exists(path)){
    printf("file doesn't exist\n");
    return -1;
  }

  char *temp_path = "/.temp/.temp.txt", ch;

  int line_counter = 1, index_counter = 0, total_lines = 0, line_chars = 0,
  line_number = 1, is_pre_enter = 1, temp_size =size;

  createfile(temp_path);

  copyfile(path+1, temp_path+1);

  FILE *file = fopen(path + 1, "w");
  FILE *temp = fopen(temp_path + 1, "r");

  for (int i = 0; i < index; i++){
    fputc(fgetc(temp), file);
  }

  for (int i = 0; i < size; i++)
    fgetc(temp);

  while (1){
    ch = getc(temp);
    if (ch == EOF)
      break;
    putc(ch, file);
  }

  fclose(file);
  fclose(temp);

  remove(temp_path +1);
  return 1;
}

void handle_remove(){
  
  char path[100] = {'\0'}, sub_command[10], ch, direction;

  int input_flag = input_path(path),
  line,index, size;

  if(input_flag == -1)
    return;

  getchar();

  for (int i = 0; i < 10; i++)
    sub_command[i] = '\0';

  scanf("%s", sub_command);

  if(strcmp(sub_command, "--pos") != 0){
    printf("invalid flag\n");
    return;
  }

  getchar();

  scanf("%d", &line);
  ch=getchar();
  if(ch!=':'){
    printf("invalid flag\n");
    return;
  }
  scanf("%d", &index);

  getchar();

  for (int i = 0; i < 10; i++)
    sub_command[i] = '\0';

  scanf("%s", sub_command);

  if(strcmp(sub_command, "--size") != 0){
    printf("invalid flag\n");
    return;
  }

  getchar();
  scanf("%d", &size);
  getchar();
  ch = getchar();

  if(ch!= '-'){
    printf("invalid flag\n");
    return;
  }

  direction = getchar();

  if(direction != 'f' && direction != 'b'){
    printf("invalid flag\n");
    return;
  }

  direction = direction == 'f' ? 1 : -1;

  int remove_flag =removestr(path, line, index, size, direction);

  if(remove_flag == 1)
    printf("remove done\n");
}

int copy(char *path, int line, int index, int size, char direction){
  
  if(!file_exists(path)){
    printf("file doesn't exist\n");
    return -1;
  }

  char *clipboard_path = "/.temp/.clipboard.txt",
  *backup_clipboard_path = "/.temp/.backup-clipboard.txt", ch;
  int line_counter = 1, index_counter = 0;

  // current content of clipboard goes to backup.
  copyfile(clipboard_path+1, backup_clipboard_path+1);

  FILE *file = fopen(path + 1, "r");
  FILE *clipboard = fopen(clipboard_path + 1, "w");

  // reaching target
  while (1)
  {
    if(line == line_counter){
      while(1){
        if(index == index_counter)
          break;

        ch = fgetc(file);
        if(ch==EOF){
          printf("position is out of range\n");
          return -1;
        }
        // fputc(ch, file);
        index_counter++;
      }
      break;
    }

    ch = fgetc(file);
    if(ch==EOF){
      printf("position is out of range\n");
      return -1;
    }
    // fputc(ch,file);
    if(ch=='\n')
      line_counter++;
  }

  if(direction == -1)
    fseek(file, -size, SEEK_CUR);

  for (int i = 0; i < size; i++){
    ch = fgetc(file);
    if(ch==EOF && i != size - 1){
      printf("out of bounds\n");
      fclose(clipboard);
      copyfile(backup_clipboard_path + 1, clipboard_path + 1);
      // dont forget to close every file!
      // remove(clipboard_path+1);
      return -1;
    }
    fputc(ch, clipboard);
  }

  fclose(file);
  fclose(clipboard);

  return 1;
}

void handle_copy(){

  char path[100] = {'\0'}, sub_command[10] = {'\0'},ch, direction;

  int input_flag = input_path(path), line, index, size;

  if(input_flag == -1)
    return;

  getchar();
  scanf("%s", sub_command);

  if(strcmp(sub_command,"--pos") !=0){
    printf("invalid flag\n");
    return;
  }

  getchar();
  scanf("%d", &line);
  ch = getchar();
  if (ch != ':'){
    printf("invalid flag\n");
    return;
  }
  scanf("%d", &index);
  getchar();

  for (int i = 0; i < 10; i++)
    sub_command[i] = '\0';

  scanf("%s", sub_command);

  if(strcmp(sub_command, "--size") != 0){
    printf("invalid flag\n");
    return;
  }
  getchar();
  scanf("%d", &size);
  getchar();
  ch = getchar();

  if(ch != '-'){
    printf("invalid flag\n");
    return;
  }

  direction = getchar();

  if(direction != 'f' && direction != 'b'){
    printf("invalid flag\n");
    return;
  }

  direction = direction == 'f' ? 1 : -1;

  int copy_flag = copy(path, line, index, size, direction);

  if(copy_flag == 1)
    printf("copy done\n");
}

int cut(char *path, int line, int index, int size, char direction){
  
  int copy_flag = copy(path, line, index, size, direction);
  if(copy_flag==-1)
    return -1;
  removestr(path, line, index, size, direction);
  return 1;
}

void handle_cut(){
   char path[100] = {'\0'}, sub_command[10] = {'\0'},ch, direction;

  int input_flag = input_path(path), line, index, size;

  if(input_flag == -1)
    return;

  getchar();
  scanf("%s", sub_command);

  if(strcmp(sub_command,"--pos") !=0){
    printf("invalid flag\n");
    return;
  }

  getchar();
  scanf("%d", &line);
  ch = getchar();
  if (ch != ':'){
    printf("invalid flag\n");
    return;
  }
  scanf("%d", &index);
  getchar();

  for (int i = 0; i < 10; i++)
    sub_command[i] = '\0';

  scanf("%s", sub_command);

  if(strcmp(sub_command, "--size") != 0){
    printf("invalid flag\n");
    return;
  }
  getchar();
  scanf("%d", &size);
  getchar();
  ch = getchar();

  if(ch != '-'){
    printf("invalid flag\n");
    return;
  }

  direction = getchar();

  if(direction != 'f' && direction != 'b'){
    printf("invalid flag\n");
    return;
  }

  direction = direction == 'f' ? 1 : -1;

  int cut_flag = cut(path, line, index, size, direction);

  if(cut_flag == 1)
    printf("cut done\n");
}

int paste(char *path, int line, int index){

  char ch, *clipboard_path = "/.temp/.clipboard.txt";
  int counter = 0;

  FILE *clipboard = fopen(clipboard_path+1,"r");

  while(1){
    ch = fgetc(clipboard);
    if(ch == EOF)
      break;
    counter++;
  }

  char text[counter];

  counter = 0;
  fseek(clipboard, 0, SEEK_SET);

  while(1){
    ch = fgetc(clipboard);
    text[counter] = ch;
    if(ch == EOF)
      break;
    counter++;
  }
  int insert_flag = insert(path, text, counter, line, index);
  
  if(insert_flag == -1)
    return -1;
  
  return 1;
}

void handle_paste(){
  
  char path[100] = {'\0'}, sub_command[10] = {'\0'},ch;

  int input_flag = input_path(path), line,index;

  if(input_flag == -1){
    printf("invalid path format\n");
    return;
  }

  getchar();
  scanf("%s", sub_command);

  if(strcmp(sub_command, "--pos") != 0){
    printf("invalid flag\n");
    return;
  }

  getchar();
  scanf("%d", &line);
  ch = getchar();
  if(ch!=':'){
    printf("invalid flag\n");
    return;
  }
  scanf("%d", &index);

  int paste_flag = paste(path, line, index);

  if(paste_flag == 1)
    printf("paste done\n");
}

int file_lines(char *path){

  if(!file_exists(path)){
    printf("file doesn't exist in the path\n");
    return -1;
  }

  int counter, flag = 1;;
  char ch;

  FILE *file = fopen(path + 1, "r");

  while(1){

    ch = fgetc(file);
    if(ch == EOF)
      break;
    else if(ch == '\n')
      counter++;
    else {
      if(flag)
        counter++;
      flag = 0;
    }
  }

  fclose(file);

  return counter;
}

int line_width(char *path, int line_zero_based){

  int counter = 0, line_counter = 0;;
  char ch;

  FILE *file = fopen(path + 1, "r");

  while(1){
    if(line_counter == line_zero_based){
      while(1){
        ch = fgetc(file);
        if(ch == '\n' || ch==EOF)
          return counter+1;
        else
          counter++;
      }
    }
    else{
      ch = fgetc(file);
      if(ch == '\n')
        line_counter++;
    }
  }
  fclose(file);
}

int grep(char *path, char target[], char flag, int *var){

  static int count = 0;
  int target_length = strlen(target), line_counter;
  char temp[target_length+1], 
  *content = malloc(target_length * sizeof(char)),ch;

  for (int i = 0; i < target_length +1; i++)
    temp[i] = '\0';

  FILE *file = fopen(path + 1, "r");

  int length = file_lines(path), is_first_line = 1;

  for (int i = 0; i < length; i++){

    if(!is_first_line){
      fgetc(file);
    }
    is_first_line = 0;
    fgets(content, line_width(path,i) ,file);

    for (int j = 0; j < line_width(path,i)-target_length; j++){
      clip_string(content,temp,j, j+target_length-1);

      if(strcmp(temp,target) == 0){
        if(flag == 0)
          printf("%s: %s\n",path+1, content);
        else if(flag == 'c')
          (*var)++;
        else if(flag == 'l'){
          printf("%s\n", path);
          return -1;
        }
        break;
      }
    }
  }
  
  fclose(file);
}

void handle_grep(){

  char path[20][100] = {'\0'}, sub_command[10] = {'\0'},
       string[1000] = {'\0'}, ch, grep_flag = 0, counter = 0, flag =0;

  scanf("%s", sub_command);

  if (strcmp(sub_command, "-l")==0){
    grep_flag = 'l';
    for (int i = 0; i < 10; i++)
      sub_command[i] = '\0';
    scanf("%s", sub_command);
  }
  
  else if (strcmp(sub_command, "-c")==0){
    grep_flag = 'c';
    for (int i = 0; i < 10; i++)
      sub_command[i] = '\0';
    scanf("%s", sub_command);
  }
  
  if (strcmp(sub_command, "--str") == 0){
    
    getchar();
    ch = getchar();

    counter = 0;
    
    int is_pre_backslash = 0, space_included = ch=='\"' ? 1 : 0;
    
    if(space_included)
      ch = getchar();

    while(1){
      if(flag)
        ch = getchar();

      flag = 1;

      if(ch=='\\'){
        if(is_pre_backslash == 0){
          is_pre_backslash = 1;
          continue;
        }
        else {
          is_pre_backslash = 0;
        }
      }
    
    else{
      
      if(ch=='\"' && is_pre_backslash){
        string[counter] = '\"';
        counter++;
        is_pre_backslash = 0;
        continue;;
      }
      else if(ch == '\"' && space_included)
        break;
      else if(ch == 32 && !space_included)
        break;
      if(is_pre_backslash)
        if(ch=='n'){
          string[counter] = '\n';
          counter++;
          is_pre_backslash = 0;
          continue;
        }
        
      is_pre_backslash = 0;
    }
    string[counter] = ch;
    counter++;
  }

  if(space_included)
   getchar();

  for (int i = 0; i < 10; i++)
    sub_command[i] = '\0';

    scanf("%s", sub_command);

    if(strcmp(sub_command, "--files")!=0){
      printf("invalid flag\n");
      return;
    }

    counter = 0;

    getchar();

    int inner_counter;

    while(1){
      inner_counter = 0;
      ch = getchar();

      if(ch == '\"'){
        while(1){
          ch = getchar();
          if(ch == '\"')
          break;
      
          path[counter][inner_counter] = ch;
          inner_counter++;
        }
      }
  
      else if(ch=='/'){
        path[counter][0] = '/';
        scanf("%s", path[counter]+1);

        //attention!
      }
      
      // ==============================
      ch = getchar();
      if(ch == '\n')
        break;
      counter++;
    }

    // printf("files count is %d\n", counter+1);

    int count = 0;

    for (int i = 0; i < counter + 1; i++){
      if(flag == 'c')
        grep(path[i], string, grep_flag, &count);
      else
        grep(path[i], string, grep_flag, &count);
    }
    if(flag == 'c')
      printf("%d\n", count);
  }

  else {
    printf("invalid flag\n");
  }
}

int* find(char *path, char *target, char flag){

  int length, total_ch = 0, *results = malloc(100 * sizeof(int)),
  found =0, target_length = strlen(target), counter = 0,
  index = 0;

  for (int i = 0; i < 100; i++)
    results[i] = -1;

  char temp[target_length + 1], ch;

  FILE *file = fopen(path + 1, "r");

  for (int i = 0; i < target_length + 1; i++)
    temp[i] = '\0';

  length = file_lines(path);

  for (int k = 0; k < length; k++){ 
    if(target_length>line_width(path,k)){
      index += line_width(path,k);
      fseek(file,line_width(path,k)+1,SEEK_CUR);
      continue;
    }
    for (int i = 0; i <= line_width(path,k)-target_length; i++){
      for (int j = 0; j < target_length; j++){
        temp[j] = fgetc(file);
      }
      fseek(file, -target_length + 1, SEEK_CUR);
      if (strcmp(temp, target) == 0){
        results[found] = i + index;
        found++;
        // printf("exist = %d\n", i+index);
        //return i + index;
      }
    }
    if(k==length-1)
      break;
    index += line_width(path,k);
    fseek(file,target_length-1,SEEK_CUR);
  }

  return results;

  // return -1;
}

void handle_find(){

  char path[100] = {'\0'}, sub_command[10] = {'\0'},
      string[1000] = {'\0'}, ch, counter = 0, flag =0;
  int is_pre_backslash = 0;

  scanf("%s", sub_command);

  if(strcmp(sub_command,"--str") != 0){
    printf("invalid flag\n");
    return;
  }
    
  getchar();
  ch = getchar();
  int space_included = ch=='\"' ? 1 : 0;
  
  if(space_included)
    ch = getchar();

  while(1){
    if(flag)
      ch = getchar();

    flag = 1;

    if(ch=='\\'){
      if(is_pre_backslash == 0){
        is_pre_backslash = 1;
        continue;
      }
      else {
        is_pre_backslash = 0;
      }
    }
    
    else{
      
      if(ch=='\"' && is_pre_backslash){
        string[counter] = '\"';
        counter++;
        is_pre_backslash = 0;
        continue;;
      }
      else if(ch == '\"' && space_included)
        break;
      else if(ch == 32 && !space_included)
        break;
      if(is_pre_backslash)
        if(ch=='n'){
          string[counter] = '\n';
          counter++;
          is_pre_backslash = 0;
          continue;
        }
        
      is_pre_backslash = 0;
    }
    string[counter] = ch;
    counter++;
  }

  if(space_included)
    getchar();

  for (int i = 0; i < 10; i++)
    sub_command[i] = '\0';

  int input_flag = input_path(path);

  if(input_flag == -1){
    printf("file deosn't exist in the path\n");
    return;
  }

  ch = getchar();

  int *results=malloc(100 * sizeof(int)), count=0;
  
  if(ch == 10){
    results = find(path, string,0);
    if(results[0]==-1){
      printf("not found\n");
      return;
    }
    else{
      printf("%d\n", results[0]);
    }
  }
  else if(ch == 32){
    for (int i = 0; i < 10; i++)
      sub_command[i] = 0;

    scanf("%s", sub_command);

    if(strcmp(sub_command, "-count")==0){

      while(1){
        ch = getchar();

        if(ch == '\n')
          break;

        if(ch == ' '){
          for (int i = 0; i < 10; i++)
            sub_command[i] = '\0';
          scanf("%s", sub_command);

          if(strcmp(sub_command,"-at") == 0 || strcmp(sub_command,"-all")==0 || strcmp(sub_command,"-byword")==0){
            printf("invalid flag combination\n");
            return;
          }
          else{
            printf("invalid flag\n");
            return;
          }
        }
      }
      
      results = find(path, string,1);
      for (int i = 0; i < 100; i++){
        if(results[i]==-1)
          break;
        count++;
      }
      printf("%d\n", count);
    }
    else if(strcmp(sub_command, "-at")==0){
      int nth, byword_active = 0;
      scanf("%d", &nth);
      
      while(1){

        ch = getchar();

        if(ch == '\n')
            break;

        if(ch == ' '){
          for (int i = 0; i < 10; i++)
            sub_command[i] = '\0';
          scanf("%s", sub_command);
          if(strcmp(sub_command,"-byword")==0)
            byword_active = 1;
          else if(strcmp(sub_command, "-count") == 0 || strcmp(sub_command,"-all")==0){
            printf("invalid flag combination\n");
            return;
          }
          else{
            printf("invalid flag\n");
            return;
          }
        }
      
      }
      results = find(path, string,2);
      if(results[0]==-1){
        printf("not found\n");
        return;
      }
      for (int i = 0; i < 100; i++){
        if(results[i]==-1)
          break;
        count++;
      }
      if(nth > count)
        printf("-1\n");
      else{
        if(byword_active){

          int target = results[nth - 1];
          
          FILE *file = fopen(path + 1, "r");
          int words = 0, index=0, is_pre_seperator =1;
          while(1){
            if(index == target){
              printf("%d\n", words + 1);
              return;
            }
            ch = fgetc(file);
          
            if(ch == 32 || ch == '\n')
              is_pre_seperator = 1;
            
            else if(is_pre_seperator){
              words++;
              is_pre_seperator = 0;
            }
            index++;
          } 
        }
        else{
          printf("%d\n", results[nth - 1]);
        }
      }
    }
    else if(strcmp(sub_command, "-byword")==0){
      int at_active = 0, all_active=0, nth;
      
      while(1){
        ch = getchar();

        if(ch == '\n')
          break;

        if(ch == ' '){
          for (int i = 0; i < 10; i++)
            sub_command[i] = '\0';
          scanf("%s", sub_command);
          if(strcmp(sub_command,"-at")==0){
            at_active = 1;
            scanf("%d", &nth);
          }
          else if(strcmp(sub_command, "-all")==0)
            all_active = 1;
          else if(strcmp(sub_command, "-count") == 0){
            printf("invalid flag combination\n");
            return;
          }
          else{
            printf("invalid flag\n");
            return;
          }
      }

      }

      if(at_active){

        results = find(path, string,3);
        if(results[0]==-1){
          printf("not found\n");
          return;
        }

        int target = results[nth - 1];
          
          FILE *file = fopen(path + 1, "r");
          int words = 0, index=0, is_pre_seperator =1;
          while(1){
            if(index == target){
              printf("%d\n", words + 1);
              return;
            }
            ch = fgetc(file);
          
            if(ch == 32 || ch == '\n')
              is_pre_seperator = 1;
            
            else if(is_pre_seperator){
              words++;
              is_pre_seperator = 0;
            }
            index++;
          }
          return;
      }
      
      else if(all_active){
        results = find(path, string,4);
        if(results[0]==-1){
          printf("not found\n");
          return;
        }
        for (int i = 0; i < 100; i++){
          int target = results[i];
          
          FILE *file = fopen(path + 1, "r");
          int words = 0, index=0, is_pre_seperator =1;
          while(1){
            if(index == target){
              printf("%d", words + 1);
              break;
            }
            ch = fgetc(file);
          
            if(ch == 32 || ch == '\n')
              is_pre_seperator = 1;
            
            else if(is_pre_seperator){
              words++;
              is_pre_seperator = 0;
            }
            index++;
          }           
          if(results[i+1]==-1){
            printf("\n");
            return;
          }
          printf(", ");
          count++;
          
          fclose(file);
        }
      }

      else{
        results = find(path, string,3);
      if(results[0]==-1){
        printf("not found\n");
        return;
      }
      FILE *file = fopen(path + 1, "r");
      int words = 0, index=0, is_pre_seperator =1;
      while(1){
         if(index == results[0]){
          printf("%d\n", words + 1);
          return;
        }
        ch = fgetc(file);
       
        if(ch == 32 || ch == '\n')
          is_pre_seperator = 1;
        
        else if(is_pre_seperator){
          words++;
          is_pre_seperator = 0;
        }
        index++;
      }
      fclose(file);
      }

      
    }
    else if(strcmp(sub_command, "-all")==0){

      int byword_active = 0;

      while(1){

        ch = getchar();

        if(ch=='\n')
          break;

        if(ch == ' '){
          for (int i = 0; i < 10; i++)
            sub_command[i] = '\0';
          scanf("%s", sub_command);
          if(strcmp(sub_command,"-byword")==0)
            byword_active = 1;
          else if(strcmp(sub_command, "-count") == 0 || strcmp(sub_command,"-at")==0){
            printf("invalid flag combination\n");
            return;
          }
          else{
            printf("invalid flag\n");
            return;
          }
        }

      }

      if(byword_active){
        results = find(path, string,4);
        if(results[0]==-1){
          printf("not found\n");
          return;
        }
        for (int i = 0; i < 100; i++){
          int target = results[i];
          
          FILE *file = fopen(path + 1, "r");
          int words = 0, index=0, is_pre_seperator =1;
          while(1){
            if(index == target){
              printf("%d", words + 1);
              break;
            }
            ch = fgetc(file);
          
            if(ch == 32 || ch == '\n')
              is_pre_seperator = 1;
            
            else if(is_pre_seperator){
              words++;
              is_pre_seperator = 0;
            }
            index++;
          }           
          if(results[i+1]==-1){
            printf("\n");
            return;
          }
          printf(", ");
          count++;
          
          fclose(file);
        }
      }
      
      else{
        results = find(path, string,4);
        if(results[0]==-1){
          printf("not found\n");
          return;
        }
        for (int i = 0; i < 100; i++){
          printf("%d", results[i]);
          if(results[i+1]==-1){
            printf("\n");
            return;
          }
        printf(", ");
        count++;
        }
      }
    }
    else {
      printf("invalid flag\n");
      return;
    }
  }

}

void replace(){
  int a = 5;
  
}

void handle_replace(){
  char path[100] = {'\0'}, sub_command[10] = {'\0'},
  string1[100] = {'\0'}, string2[100]= {'\0'}, ch,
  counter = 0, flag =0, nth;
  int is_pre_backslash = 0;

  //str1

  scanf("%s", sub_command);

  if(strcmp(sub_command,"--str1") != 0){
    printf("invalid flag\n");
    return;
  }
    
  getchar();
  ch = getchar();
  int space_included = ch=='\"' ? 1 : 0;
  
  if(space_included)
    ch = getchar();

  while(1){
    if(flag)
      ch = getchar();

    flag = 1;

    if(ch=='\\'){
      if(is_pre_backslash == 0){
        is_pre_backslash = 1;
        continue;
      }
      else {
        is_pre_backslash = 0;
      }
    }
    
    else{
      
      if(ch=='\"' && is_pre_backslash){
        string1[counter] = '\"';
        counter++;
        is_pre_backslash = 0;
        continue;;
      }
      else if(ch == '\"' && space_included)
        break;
      else if(ch == 32 && !space_included)
        break;
      if(is_pre_backslash)
        if(ch=='n'){
          string1[counter] = '\n';
          counter++;
          is_pre_backslash = 0;
          continue;
        }
        
      is_pre_backslash = 0;
    }
    string1[counter] = ch;
    counter++;
  }

  if(space_included)
    getchar();

  // str2

  for (int i = 0; i < 10;i++)
    sub_command[i] = '\0';

  flag = 0;
  counter = 0;

  scanf("%s", sub_command);

  if(strcmp(sub_command,"--str2") != 0){
    printf("invalid flag\n");
    return;
  }
    
  getchar();
  ch = getchar();
  space_included = ch=='\"' ? 1 : 0;
  
  if(space_included)
    ch = getchar();

  while(1){
    if(flag)
      ch = getchar();

    flag = 1;

    if(ch=='\\'){
      if(is_pre_backslash == 0){
        is_pre_backslash = 1;
        continue;
      }
      else {
        is_pre_backslash = 0;
      }
    }
    
    else{
      
      if(ch=='\"' && is_pre_backslash){
        string2[counter] = '\"';
        counter++;
        is_pre_backslash = 0;
        continue;;
      }
      else if(ch == '\"' && space_included)
        break;
      else if(ch == 32 && !space_included)
        break;
      if(is_pre_backslash)
        if(ch=='n'){
          string2[counter] = '\n';
          counter++;
          is_pre_backslash = 0;
          continue;
        }
        
      is_pre_backslash = 0;
    }
    string2[counter] = ch;
    counter++;
  }

  if(space_included)
    getchar();

  // ======================

  int *results = malloc(100 * sizeof(int));
  int input_flag = input_path(path);

  if(input_flag == -1){
    printf("file doesn't exist in the path");
    return;
  }

  ch = getchar();

  if(ch == '\n'){

  }
  else if (ch == ' ') {
    for (int i = 0; i < 10; i++)
      sub_command[i] = '\0';

    scanf("%s", sub_command);

    if(strcmp(sub_command, "-all") == 0){
      results = find(path, string1, 0);
      if(results[0]==-1){
        printf("not found\n");
        return;
      }
      for (int i = 0; i < 100; i++){
        removestr_by_index(path, results[i], strlen(string1));
        insert_by_index(path, string2, counter + 1, results[i]);

        if(results[i+1]==-1){
          printf("replace done\n");
          return;
        }
      }
    }
    else if(strcmp(sub_command, "-at") ==0){
      scanf("%d", &nth);
    }
  }
}

void get_command(){
  
  char command[15];
  scanf("%s", command);
  getchar();

  if(strcmp(command,"createfile") == 0){
    handle_createfile();
  }
  else if(strcmp(command, "insertstr") == 0){
    handle_insert();
  }
  else if(strcmp(command, "cat") == 0){
    handle_cat();
  }
  else if(strcmp(command,"removestr") == 0){
    handle_remove();
  }
  else if(strcmp(command, "copystr")==0){
    handle_copy();
  }
  else if(strcmp(command, "cutstr") ==0){
    handle_cut();
  }
  else if(strcmp(command, "pastestr") == 0){
    handle_paste();
  }
  else if(strcmp(command,"grep") == 0){
    handle_grep();
  }
  else if(strcmp(command,"find") == 0){
    handle_find();
  }
  else if(strcmp(command,"replace") == 0){
    handle_replace();
  }
  // else {
  //   printf("invalid command\n");
  // }
}

// handle conflict of invalid flag and command later.