#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*Regex*/
#include <regex.h>

const char* ADDMET = "addMetting";
const char* ADDPRE = "addPresentation";
const char* ADDCON = "addConference";
const char* BOKDEV = "bookDevice";
const char* ADDBAT = "addBatch";
const char* PRTBOK = "printBookings";
const char* ENDPGM = "endProgram";

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
  char *input;
  char *pt;
  char *ptnDate = "[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])";
  char *ptnTime = "(0[0-9]|1[0-9]|2[0-3]):([0-5][0-9])";
  printf("~~ WELCOME TO PolySME ~~\n");
  // while (1)
  {
    printf("Please enter booking:\n");
    // fgets(str, sizeof str, stdin);
    // scanf(" %[^;]",str);
    scanf(" %[^;\n]",input);
    //printf("str = %s\n",str);
    
    pt = strtok (input," ");
    while (pt != NULL) {
        printf("%s\n", pt);
        int m = match(pt,ptnDate);
        if(m != 0)
        {
          printf("match date = %s\n", pt);
        }

        pt = strtok (NULL, " ");
    }    
  }
  
  return 0;
}