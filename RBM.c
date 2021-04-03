#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//<===============================Variables=============================>
/*Regex*/
#include <regex.h>
/*First Keyword*/
const char* ADDMET = "addMeeting";
const char* ADDPRE = "addPresentation";
const char* ADDCON = "addConference";
const char* BOKDEV = "bookDevice";
const char* ADDBAT = "addBatch";
const char* PRTBOK = "printBookings";
const char* ENDPGM = "endProgram";

char input[255];
char *str;
/*regx pattern*/
char *ptnDate = "[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])";
char *ptnTime = "(0[0-9]|1[0-9]|2[0-3]):([0-5][0-9])";
char ptnKeyWord[100];

//pipes and child
int child[3];
int pipes[6][2];

//loop variable
int i,j;
//<===============================Variables=============================>
//<===============================function==============================>
//Add keyword into array
void define_keyword(char *ptnKeyWord){
  strcpy(ptnKeyWord, "(");
  strcat(ptnKeyWord, ADDMET);
  strcat(ptnKeyWord, "|");
  strcat(ptnKeyWord, ADDPRE);
  strcat(ptnKeyWord, "|");
  strcat(ptnKeyWord, ADDCON);
  strcat(ptnKeyWord, ")");
}

//regex matching
int match(const char *string, const char *pattern)
{
  regex_t re;
  if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return 0;
  int status = regexec(&re, string, 0, NULL, 0);
  regfree(&re);
  if (status != 0) return 0;
  return 1;
}

//Split the command
char* extract_cmd(char *input){
  str = strtok (input," ");
  while (str != NULL) {
      //printf("%s\n", str);
      //check pattern date
      int m = match(str,ptnDate);
      if(m)
      {
        //printf("match date = %s\n", str);
        m = 0;
      }
      //check pattern keyword
      m = match(str,ptnKeyWord);
      
      if(m)
      {
        //printf("match keyword = %s\n", str);
        m = 0;
      }

      //Get the .dat filename
      if (strstr(str, ".dat")!=NULL){
        strcpy(&str[0], &str[0 + 3]);
        return (str);
      }

      //find next string
      str = strtok (NULL, " ");
  }    
}

//Read the batch file
void batchFileHandler(char *filename){
  FILE *infilep;
  char line[100];
  char file[100];
  strncpy(file, filename, strlen(filename)-1);
  file[strlen(filename)-1] = '\0';
  infilep = fopen(file, "r");
  if (infilep == NULL) {
    printf("Error in opening input file\n");
    exit(1);
  }
  while (fgets( line, 100, infilep ) != NULL ) {
    printf("%s",line);
  }
}
//<===============================function==============================>

int main(void) {
  char buf[80]={0};
  /*
  //Create new pipe
  for (i = 0;i < 6; i++){
    if (pipe(pipes[i]) < 0){
        printf("pipe creation error\n");
        exit(1);
    }
  }

  //Create Child
  for(i = 0;i < 3; i++){
    child[i] = fork();
    if (child[i]<0) {
    printf("Fork Failed\n");
    exit(1);
    } else if (child[i]==0) {
      //Close unused pipes in Child
      for (j = 0; j < 6; j++){
        if(j != i*2){
          close(pipes[j][0]);
        }
        if(j != (i*2+1)){
          close(pipes[j][1]);
        }
      }
      read(pipes[i*2][0],buf,80);
      break;
    }
  }

  //Close unused pipes in Parent
  for (i = 0; i < 6; i++){
    if(i%2 == 0){
      close(pipes[i][0]);
    } else if (i%2 != 0){
      close(pipes[i][1]);
    }
  }
  */

  FILE *outfilep;
  define_keyword(ptnKeyWord);
  //printf("ptnKeyWord = %s",ptnKeyWord);
  /*program start*/
  printf("~~ WELCOME TO PolySME ~~\n");
  //Open allBooking.log
  outfilep = fopen("allBooking.log", "a");
  if (outfilep == NULL) {
    printf("Error in opening output file\n");
    exit(1);
  }



  //Get user input
  while (1)
  {
    printf("Please enter booking:\n");
    // fgets(input, sizeof input, stdin);
    // scanf(" %[^;]",input);
    //get whole line
    fgets(input,255,stdin);
    if (strstr(input, "endProgram") != NULL) {
      remove("allBooking.log");
      //remove("FCFSBooking.log");
      //remove("PRIOBooking.log");
      //remove("OPTIBooking.log");
      printf("Bye!\n");
      break;
    } else if (strstr(input, "addBatch")!=NULL){
      batchFileHandler(extract_cmd(input));
    } else if (strstr(input, "printBookings")!=NULL){
      printf("Check what request\n");
    } else if (strstr(input, "addMeeting")!=NULL || 
               strstr(input, "addPresentation")!=NULL ||
               strstr(input, "addConference")!=NULL ||
               strstr(input, "bookDevice")!=NULL){
      fprintf(outfilep, "%s", input);
    } else {
      printf("Error, Please try again!\n");
    }
    //extract_cmd(input);
    
  }
  fclose(outfilep);
  for(i=0;i<3;i++){
    wait(NULL);
  }
  return 0;
}