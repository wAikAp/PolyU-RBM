#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*Regex*/
#include <regex.h>

const char* ADDMET = "addMeeting";
const char* ADDPRE = "addPresentation";
const char* ADDCON = "addConference";
const char* BOKDEV = "bookDevice";
const char* ADDBAT = "addBatch";
const char* PRTBOK = "printBookings";
const char* ENDPGM = "endProgram";

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


int main(void) {
  char input[255];
  char *str;
  /*regx pattern*/
  char *ptnDate = "[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])";
  char *ptnTime = "(0[0-9]|1[0-9]|2[0-3]):([0-5][0-9])";
  char ptnKeyWord[100];
  strcpy(ptnKeyWord, "(");
  strcat(ptnKeyWord, ADDMET);
  strcat(ptnKeyWord, "|");
  strcat(ptnKeyWord, ADDPRE);
  strcat(ptnKeyWord, "|");
  strcat(ptnKeyWord, ADDCON);
  strcat(ptnKeyWord, ")");
  //printf("ptnKeyWord = %s",ptnKeyWord);
  
  /*program start*/
  printf("~~ WELCOME TO PolySME ~~\n");
  // while (1)
  {
    printf("Please enter booking:\n");
    // fgets(input, sizeof input, stdin);
    // scanf(" %[^;]",input);
    //get whole line
    scanf(" %[^;\n]",input);
    
    str = strtok (input," ");
    while (str != NULL) {
        printf("%s\n", str);
        //check pattern date
        int m = match(str,ptnDate);
        if(m)
        {
          printf("match date = %s\n", str);
          m = 0;
        }
        //check pattern keyword
        m = match(str,ptnKeyWord);
        if(m)
        {
          printf("match keyword = %s\n", str);
          m = 0;
        }

        //find next string
        str = strtok (NULL, " ");
    }    
  }
  
  return 0;
}