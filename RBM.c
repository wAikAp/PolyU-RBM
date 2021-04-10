#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//Boolean
#include <stdbool.h>
/*Regex*/
#include <regex.h>

//<===============================Variables=============================>
/*First Keyword*/
const char* ADDMET = "addMeeting";
const char* ADDPRE = "addPresentation";
const char* ADDCON = "addConference";
const char* BOKDEV = "bookDevice";
const char* ADDBAT = "addBatch";
const char* PRTBOK = "printBookings";
const char* ENDPGM = "endProgram";

char input[255];
char input_from_file[255];
char *str;

/*regx pattern*/
char ptnKeyWord[100];
char* ptnTenant = "[-tenant]{6}";
char* ptnDate = "[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])";
char* ptnTime = "(0[0-9]|1[0-9]|2[0-3]):([0-5][0-9])";
char* ptnHour = "([0-9]*[.])[0-9]+";
char* ptnPersonNumber = "[0-9]+";
char* ptnRoom = "[room]{4}";
char* ptnDevice = "(.*)_(.*)";

//pipes and child
int child[3];
int pipes[6][2];

//loop variable
int i,j,x,k;

//Scheduling variables 
char fcfs[10];
char prio[10];
char opti[10];

//Room variables
int Room_A[168] = {0}; 
int Room_B[168] = {0}; 
int Room_C[168] = {0}; 
int room = 0; //Check which room

//Device variables
int webcam_FHD_1[168] = {0}; 
int webcam_FHD_2[168] = {0}; 
int webcam_UHD[168] = {0}; 
int monitor_50_1[168] = {0}; 
int monitor_50_2[168] = {0}; 
int monitor_75[168] = {0}; 
int projector_2K_1[168] = {0}; 
int projector_2K_2[168] = {0}; 
int projector_4K[168] = {0}; 
int screen_100_1[168] = {0}; 
int screen_100_2[168] = {0}; 
int screen_150[168] = {0};

//Tenant variables
int tenant_A_count = 0;
int tenant_B_count = 0;
int tenant_C_count = 0;
int tenant_D_count = 0;
int tenant_E_count = 0;

//Count the cmd for corresponding keyword
int conference_count = 0; 
int presentation_count = 0; 
int meeting_count = 0; 
int device_count = 0;
int conf_temp = 0; int pres_temp = 0; int meet_temp = 0; int devi_temp = 0;


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
  strcat(ptnKeyWord, "|");
  strcat(ptnKeyWord, BOKDEV);
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

//Handle Room Booking
bool roomBookingHandler(int start, int number_of_people, int hour){
  //room variable
  bool Free_roomA = true;
  bool Free_roomB = true;
  bool Free_roomC = true;
  //Check which room is available for the booking
  if(number_of_people!=0){ //Not for bookDevice
    for(i=start;i<start+hour;i++){ 
      if (number_of_people <= 10){
        if(Room_A[i] == 1){
          Free_roomA = false;
        }
        if (Room_B[i] == 1){
          Free_roomB = false;
        } 
      } else if (number_of_people > 10){
        Free_roomA = false;
        Free_roomB = false;
      }
      if (Room_C[i] == 1){
        Free_roomC = false;
      }
    }
    //Reserve the room
    if(Free_roomA){
      for(i=start;i<start+hour;i++){
        Room_A[i] = 1;
      }
      room = 1;
      return (true);
    } else if (Free_roomB){
      for(i=start;i<start+hour;i++){
        Room_B[i] = 1;
      }
      room = 2;
      return (true);
    } else if (Free_roomC){
      for(i=start;i<start+hour;i++){
        Room_C[i] = 1;
      }
      room = 3;
      return (true);
    } else {
      return(false);
    }
  }
}

//Handle Device Booking
bool deviceHandler(char* device, int start, int hour){
  //Device variables
  bool Free_webcam_FHD_1 = true;
  bool Free_webcam_FHD_2 = true;
  bool Free_webcam_UHD = true;
  bool Free_monitor_50_1 = true;
  bool Free_monitor_50_2 = true;
  bool Free_monitor_75 = true;
  bool Free_projector_2K_1 = true; 
  bool Free_projector_2K_2 = true; 
  bool Free_projector_4K = true; 
  bool Free_screen_100_1 = true;
  bool Free_screen_100_2 = true;
  bool Free_screen_150 = true;
  //Check the device if it is available for the booking
  if (strstr(device,"webcam_FHD")!=NULL){
    for(i=start;i<start+hour;i++){ 
      if(webcam_FHD_1[i] == 1){
        Free_webcam_FHD_1 = false;
      }
      if(webcam_FHD_2[i] == 1){
        Free_webcam_FHD_2 = false;
      }
    }
  } else if (strstr(device,"webcam_UHD")!=NULL){
    for(i=start;i<start+hour;i++){ 
      if(webcam_UHD[i] == 1){
        Free_webcam_UHD = false;
      }
    }
  } else if (strstr(device,"monitor_50")!=NULL){
    for(i=start;i<start+hour;i++){ 
      if(monitor_50_1[i] == 1){
        Free_monitor_50_1 = false;
      }
      if(monitor_50_2[i] == 1){
        Free_monitor_50_2 = false;
      }
    }
  } else if (strstr(device,"monitor_75")!=NULL){
    for(i=start;i<start+hour;i++){ 
      if(monitor_75[i] == 1){
        Free_monitor_75 = false;
      }
    }
  } else if (strstr(device,"projector_2K")!=NULL){
    for(i=start;i<start+hour;i++){ 
      if(projector_2K_1[i] == 1){
        Free_projector_2K_1 = false;
      }
      if(projector_2K_2[i] == 1){
        Free_projector_2K_2 = false;
      }
    }
  } else if (strstr(device, "projector_4K")!=NULL){
    for(i=start;i<start+hour;i++){ 
      if(projector_4K[i] == 1){
        Free_projector_4K = false;
      }
    }
  } else if (strstr(device,"screen_100")!=NULL){
    for(i=start;i<start+hour;i++){ 
      if(screen_100_1[i] == 1){
        Free_screen_100_1 = false;
      }
      if(screen_100_2[i] == 1){
        Free_screen_100_2 = false;
      }
    }
  } else if (strstr(device,"webcam_FHD")!=NULL){
    for(i=start;i<start+hour;i++){ 
      if(screen_150[i] == 1){
        Free_screen_150 = false;
      }
    }
  }

  //Reserve devices
  if (strstr(device,"webcam_FHD")!=NULL){
    if(Free_webcam_FHD_1){
        for(i=start;i<start+hour;i++){
            webcam_FHD_1[i] = 1;
        }
        return (true);
    } else if (Free_webcam_FHD_2){
        for(i=start;i<start+hour;i++){
            webcam_FHD_2[i] = 1;
        }
        return (true);
    } else {
        return (false);
    }
  } else if (strstr(device,"webcam_UHD")!=NULL){
    if(Free_webcam_UHD){
        for(i=start;i<start+hour;i++){
            webcam_UHD[i] = 1;
        }
        return (true);
    } else {
        return (false);
    }
  } else if (strstr(device,"monitor_50")!=NULL){
    if(Free_monitor_50_1){
        for(i=start;i<start+hour;i++){
            monitor_50_1[i] = 1;
        }
        return (true);
    } else if (Free_monitor_50_2){
        for(i=start;i<start+hour;i++){
            monitor_50_2[i] = 1;
        }
        return (true);
    } else {
        return (false);
    }
  } else if (strstr(device,"monitor_75")!=NULL){
    if(Free_monitor_75){
        for(i=start;i<start+hour;i++){
            monitor_75[i] = 1;
        }
        return (true);
    } else {
        return (false);
    }
  } else if (strstr(device,"projector_2K")!=NULL){
    if(Free_projector_2K_1){
        for(i=start;i<start+hour;i++){
            projector_2K_1[i] = 1;
        }
        return (true);
    } else if (Free_projector_2K_2){
        for(i=start;i<start+hour;i++){
            projector_2K_2[i] = 1;
        }
        return (true);
    } else {
        return (false);
    }
  } else if (strstr(device, "projector_4K")!=NULL){
    if(Free_projector_4K){
        for(i=start;i<start+hour;i++){
            projector_4K[i] = 1;
        }
      return (true);
    } else {
        return (false);
    }
  } else if (strstr(device,"screen_100")!=NULL){
    if(Free_screen_100_1){
        for(i=start;i<start+hour;i++){
            screen_100_1[i] = 1;
        }
        return (true);
    } else if (Free_screen_100_2){
        for(i=start;i<start+hour;i++){
            screen_100_2[i] = 1;
        }
        return (true);
    } else {
        return (false);
    }
  } else if (strstr(device,"screen_150")!=NULL){
    if(Free_screen_150){
        for(i=start;i<start+hour;i++){
            screen_150[i] = 1;
        }
        return (true);
    } else {
        return (false);
    }
  }
}

//Split the command
bool extract_cmd(char *input){
  int start = 0;
  int day = 0;
  int time = 0;
  int hour = 0;
  int number_of_people = 0;
  int count_device = 0;
  char device_1[30] = {0};
  char device_2[30] = {0};
  char keyword[20];

  //printf("Input is %s\n",input);
  str = strtok (input," ");
  while (str != NULL) {
      int m;
      //Check whether book device
      m = match(str,ptnKeyWord);
      if(m){
        if (strstr(str,"bookDevice")!=NULL){
          strcpy(keyword,"bookDevice");
        }
      }
      //check TENANT
      m = match(str,ptnTenant);
      if(m)
      {
        //printf("match tenant = %s\n", str);
        m = 0;
        goto NEXT;
      }
      //check DATE (1)
      m = match(str,ptnDate);
      if(m)
      {
        strcpy(&str[0], &str[0 + 8]);
        day = atoi(str);
        switch (day)
        {
        case 10: start = 24*0; break;
        case 11: start = 24*1; break;
        case 12: start = 24*2; break;
        case 13: start = 24*3; break;
        case 14: start = 24*4; break;
        case 15: start = 24*5; break;
        case 16: start = 24*6; break;
        default: start = 0; break;
        }
        //printf("match date = %d\n", day);
        //printf("start = %d\n", start);
        m = 0;
        goto NEXT;
      }
      //check TIME (2)
      m = match(str,ptnTime);
      if(m)
      {
        char time_char[2];
        strncpy(time_char, str, 2);
        time = atoi(time_char);
        start += time;
        //printf("match time = %d\n", time);
        m = 0;
        goto NEXT;
      }
      //check DURATION (3)
      m = match(str,ptnHour);
      if(m)
      {
        hour = atoi(str);
        //printf("match duration = %d\n", hour);
        m = 0;
        goto NEXT;
      }
      //check DEVICE
      m = match(str,ptnDevice);
      if(m)
      {
        if(device_1[0] == '\0'){
          strcpy(device_1,str);
        } else {
          strcpy(device_2,str);
        }
        //printf("Device = %s\n", str);
        m = 0;
        goto NEXT;
      }
      //check NUMBER OF PERSON
      m = match(str,ptnPersonNumber);
      if(m)
      {
        number_of_people = atoi(str);
        //printf("Number of people = %s\n", number_of_people);
        m = 0;
        goto NEXT;
      }
      //find next string
      NEXT:str = strtok (NULL, " ");
  }
  //printf("Start time = %d\n", start);  
  //printf("match duration = %d\n", hour);  
  //printf("Number of people = %d\n", number_of_people);
  //printf("Device 1 = %s\n", device_1);
  //printf("Device 2 = %s\n", device_2);

  //Check which room is available for the booking
  if(strstr(keyword,"bookDevice")!=NULL){
    ;
  } else {
    if(!roomBookingHandler(start,number_of_people,hour)){
      return(false);
    }
  }

  if(device_1[0] != '\0'){
    if(!deviceHandler(device_1,start,hour)){
      return(false);
    }
  }

  if(device_2[0] != '\0'){
    if(!deviceHandler(device_2,start,hour)){
      return(false);
    }
  }

  return(true);
}

//Check what scheduling algorithm is required
void schedulingChecking(char *input){
  str = strtok (input," ");
  while (str != NULL) {
    //Get fcfs request
    if (strstr(str, "fcfs")!=NULL){
      strcpy(fcfs,"fcfs");
    }
    //Check prio request
    if (strstr(str, "prio")!=NULL){
      strcpy(prio,"prio");
    }
    //Check opti request
    if (strstr(str, "opti")!=NULL){
      strcpy(opti,"opti");
    }
    //find next string
    str = strtok (NULL, " ");
  }    
}

//Check the device pair requirement
bool devicePairChecking(char *input){
  char device_1[30] = {0};
  char device_2[30] = {0};
  int m;
  str = strtok (input," ");
  while (str != NULL) {
      m = match(str,ptnKeyWord);
      if(m){
        if(strstr("bookDevice",str)!=NULL){
          return(true);
        }
      }
      m = match(str,ptnTenant);
      if(m)
      {
        //printf("match tenant = %s\n", str);
        m = 0;
        goto NEXT;
      }

      m = match(str,ptnDevice);
      if(m)
      {
        if(device_1[0] == '\0'){
          strcpy(device_1,str);
        } else {
          strcpy(device_2,str);
        }
        //printf("Device = %s\n", str);
        m = 0;
      }
    //find next string
    NEXT:str = strtok (NULL, " ");
  }
  if(strstr(device_1, "projector")!=NULL){
    if(strstr(device_2, "screen")!=NULL){
      return(true);
    } else {
      return(false);
    }
  } else if (strstr(device_1, "screen")!=NULL) {
    if(strstr(device_2, "projector")!=NULL){
      return(true);
    } else {
      return(false);
    }
  } else if (strstr(device_1, "webcam")!=NULL) {
    if(strstr(device_2, "monitor")!=NULL){
      return(true);
    } else {
      return(false);
    }
  } else if (strstr(device_1, "monitor")!=NULL){
    if(strstr(device_2, "webcam")!=NULL){
      return(true);
    } else {
      return(false);
    }
  } else if (device_1[0] == '\0'){
    return(true);
  }
}

//Get the batch filename
char* batchFileName(char *input){
  str = strtok (input," ");
  while (str != NULL) {
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
  FILE *infilep, *outfilep;
  char line[100];
  char file[100];
  strncpy(file, filename, strlen(filename)-2);
  file[strlen(filename)-2] = '\0';
  infilep = fopen(file, "r");
  outfilep = fopen("allBooking.log", "a");
  if (infilep == NULL) {
    printf("Error in opening input file\n");
    exit(1);
  }
  while (fgets( line, 100, infilep ) != NULL ) {
    fprintf(outfilep, "%s", line); //Write to allBooking.log
  }
  fprintf(outfilep, "%s", "\n");
  fclose(infilep);
  fclose(outfilep);
}

//Count tenant record
void countTenantRecord(char *input){
  str = strtok (input," ");
  while (str != NULL) {
    int m = 0;
    m = match(str,ptnTenant);
    if(m)
    {
      if (strstr(str,"tenant_A")!=NULL){
        tenant_A_count++;
      } else if (strstr(str,"tenant_B")!=NULL){
        tenant_B_count++;
      } else if (strstr(str,"tenant_C")!=NULL){
        tenant_C_count++;
      } else if (strstr(str,"tenant_D")!=NULL){
        tenant_D_count++;
      } else if (strstr(str,"tenant_E")!=NULL){
        tenant_E_count++;
      }
      m = 0;
    }
    str = strtok (NULL, " ");
  }
}

//Arrange the tenant record
int tenantRecordArrange(char *input){
  str = strtok (input," ");
  while (str != NULL) {
    int m = 0;
    m = match(str,ptnTenant);
    if(m)
    {
      if (strstr(str,"tenant_A")!=NULL){
        return(1);
      } else if (strstr(str,"tenant_B")!=NULL){
        return(2);
      } else if (strstr(str,"tenant_C")!=NULL){
        return(3);
      } else if (strstr(str,"tenant_D")!=NULL){
        return(4);
      } else if (strstr(str,"tenant_E")!=NULL){
        return(5);
      }
      m = 0;
    }
    str = strtok (NULL, " ");
  }
}

//Reconstruct print pattern
void printAppointmentSchedule(char *input, int accept_or_reject){
  //Print Date, Start, End
  int start_time = 0; int hour = 0; char end_time[5]; char column_1_to_3[100]; char keyword[20];
  char column_4_to_5[100]; char column_6[100];
  int check_exist_devices = 0; bool exist_devices = true;
  strncpy(column_1_to_3, input, strlen(input));
  column_1_to_3[strlen(input)] = '\0';
  //printf("column_1_to_3:%s\n",column_1_to_3);
  str = strtok (column_1_to_3," ");
  while (str != NULL) {
    int m = 0;
    m = match(str,ptnDate);
    if(m){
      printf("%-13s",str);
      m = 0;
    }
    m = match(str,ptnTime);
    if(m){
      char time_char[2];
      strncpy(time_char, str, 2);
      start_time = atoi(time_char);
      printf("%-8s",str);
      m = 0;
    }
    m = match(str,ptnHour);
    if(m){
      hour = atoi(str);
      sprintf(end_time,"%d",(start_time+hour));
      strcat(end_time,":00");
      printf("%-8s",end_time);
      m = 0;
    }
    if(accept_or_reject == 0){
      if(check_exist_devices==5){
        str = strtok (NULL, " ");
        if(str == NULL){
          exist_devices = false;
        }
      }
    }
    check_exist_devices++;
    str = strtok (NULL, " ");
  }
  //Print Type, Room
  strncpy(column_4_to_5, input, strlen(input));
  column_4_to_5[strlen(input)] = '\0';
  //printf("column_4_to_5:%s\n",column_4_to_5);
  str = strtok (column_4_to_5," ");
  while (str != NULL) {
    int m = 0;
    m = match(str,ptnKeyWord);
    if(m){
      if (strstr(str,"addMeeting")!=NULL){
        printf("%-15s","Meeting");
      } else if (strstr(str,"addPresentation")!=NULL){
        printf("%-15s","Presentation");
      } else if (strstr(str,"addConference")!=NULL){
        printf("%-15s","Conference");
      } else if (strstr(str,"bookDevice")!=NULL){
        strcpy(keyword,"bookDevice");
        if(accept_or_reject == 1){printf("%-15s%-9s","*","*");}
        else {printf("%-15s","*");}
      }
    }
    if (strstr(str,"room_A")!=NULL){
      printf("%-9s","room_A");
    } else if (strstr(str,"room_B")!=NULL){
      printf("%-9s","room_B");
    } else if (strstr(str,"room_C")!=NULL){
      printf("%-9s","room_C");
    }
    str = strtok (NULL, " ");
  }
  //Print Devices
  int counter = 0;
  strncpy(column_6, input, strlen(input));
  column_6[strlen(input)] = '\0';
  //printf("column 6:%s\n",column_6);
  str = strtok (column_6," ");
  while (str != NULL) {
    int m = 0;
    m = match(str,ptnTenant);
    if(m){
      m = 0;
      goto NEXT;
    }
    if(strstr(keyword,"bookDevice")!=NULL){ //Print device in book device
      if(counter==5){
         m = match(str,ptnDevice);
         if(m){
           printf("%s\n",str);
           m = 0;
         }
      }
      if(counter==6){
         m = match(str,ptnDevice);
         if(m){
          if(accept_or_reject == 0){
            printf("%54s\n",str);
          } else {
            printf("%63s\n",str);
          }
           m = 0;
         }
      }
    } else {
      if(accept_or_reject == 0){
        if (counter == 5){
          if(!exist_devices){
            printf("*\n");
          }
        }
      }
      if(counter==6){ //Print device in room booking
        m = match(str,ptnRoom);
        if(m){
          printf("*\n");
          m = 0;
        } else {
          printf("%s\n",str);
          m = 0;
        }
      }
      if(counter==7){
        m = match(str,ptnDevice);
        if(m){
          if(accept_or_reject == 1){
            printf("%63s\n",str);
            m = 0;
          } else if (accept_or_reject == 0){
            printf("%54s\n",str);
            m = 0;
          }
        }
      }
    }

    NEXT:counter++;
    str = strtok (NULL, " ");
  }
}

//Check keyword
void count_cmd_for_keyword(char *input){
  int counter = 0;
  str = strtok (input," ");
  while (str != NULL) {
    if(counter == 1){break;}
    int m = 0;
    m = match(str,ptnKeyWord);
    if(m){
      if (strstr(str, "addConference") != NULL){conference_count++;}
      else if (strstr(str, "addPresentation") != NULL){presentation_count++;}
      else if (strstr(str, "addMeeting") != NULL){meeting_count++;}
      else if (strstr(str, "bookDevice") != NULL){device_count++;}
    }
    counter++;
  }

}

//Print accepted report
void print_accepted(int algorithm){
  FILE *Accepted;
  tenant_A_count = 0; tenant_B_count = 0; tenant_C_count = 0; 
  tenant_D_count = 0; tenant_E_count = 0;

  int a = 0; int b = 0; int c = 0; int d = 0; int e = 0;
  if(algorithm == 1){
    Accepted = fopen("FCFSBookingAccepted.log", "r");
  } else if (algorithm == 2){
    Accepted = fopen("PRIOBookingAccepted.log", "r");
  }
  //Get the number of record of each tenant
  while (fscanf(Accepted, "%[^\n]\n", input_from_file) != EOF){
    countTenantRecord(input_from_file);
  }
  char tenant_A_record[tenant_A_count][255];
  char tenant_B_record[tenant_B_count][255];
  char tenant_C_record[tenant_C_count][255];
  char tenant_D_record[tenant_D_count][255];
  char tenant_E_record[tenant_E_count][255];
  fclose(Accepted);
  if(algorithm == 1){
    Accepted = fopen("FCFSBookingAccepted.log", "r");
  } else if (algorithm == 2){
    Accepted = fopen("PRIOBookingAccepted.log", "r");
  }
  //Add the record to corresponding array
  while (fscanf(Accepted, "%[^\n]\n", input_from_file) != EOF){
    char original_input[255];
    int n;
    strncpy(original_input, input_from_file, strlen(input_from_file));
    original_input[strlen(input_from_file)] = '\0';
    n = tenantRecordArrange(input_from_file);
    if(n == 1){
      strcpy(tenant_A_record[a], original_input);
      a += 1;
    } else if(n == 2){
      strcpy(tenant_B_record[b], original_input);
      b += 1;
    } else if(n == 3){
      strcpy(tenant_C_record[c], original_input);
      c += 1;
    } else if(n == 4){
      strcpy(tenant_D_record[d], original_input);
      d += 1;
    } else if(n == 5){
      strcpy(tenant_E_record[e], original_input);
      e += 1;
    }
  }
  if(algorithm == 1){
    printf("*** Room Booking – ACCEPTED / FCFS ***\n");
  } else if (algorithm == 2){
    printf("*** Room Booking – ACCEPTED / PRIO ***\n");
  }

  for(j=0;j<tenant_A_count;j++){
    if(tenant_A_count == 0){
      break;
    }
    if(j==0){
      printf("Tenant_A has the following bookings:\n");
      printf("\n");
      printf("%-13s%-8s%-8s%-15s%-9s%s\n","Date","Start","End","Type","Room","Device");
      for(x=0;x<74;x++){
        printf("=");
      }
      printf("\n");
    }
    printAppointmentSchedule(tenant_A_record[j],1);
  }

  for(j=0;j<tenant_B_count;j++){
    if(tenant_B_count == 0){
      break;
    }
    if(j==0){
      printf("\n");
      printf("Tenant_B has the following bookings:\n");
      printf("\n");
      printf("%-13s%-8s%-8s%-15s%-9s%s\n","Date","Start","End","Type","Room","Device");
      for(x=0;x<74;x++){
        printf("=");
      }
      printf("\n");
    }
    printAppointmentSchedule(tenant_B_record[j],1);
  }

  for(j=0;j<tenant_C_count;j++){
    if(tenant_C_count == 0){
      break;
    }
    if(j==0){
      printf("\n");
      printf("Tenant_C has the following bookings:\n");
      printf("\n");
      printf("%-13s%-8s%-8s%-15s%-9s%s\n","Date","Start","End","Type","Room","Device");
      for(x=0;x<74;x++){
        printf("=");
      }
      printf("\n");
    }
    printAppointmentSchedule(tenant_C_record[j],1);
  }
  
  for(j=0;j<tenant_D_count;j++){
    if(tenant_D_count == 0){
      break;
    }
    if(j==0){
      printf("\n");
      printf("Tenant_D has the following bookings:\n");
      printf("\n");
      printf("%-13s%-8s%-8s%-15s%-9s%s\n","Date","Start","End","Type","Room","Device");
      for(x=0;x<74;x++){
        printf("=");
      }
      printf("\n");
    }
    printAppointmentSchedule(tenant_D_record[j],1);
  }
  
  for(j=0;j<tenant_E_count;j++){
    if(tenant_E_count == 0){
      break;
    }
    if(j==0){
      printf("\n");
      printf("Tenant_E has the following bookings:\n");
      printf("\n");
      printf("%-13s%-8s%-8s%-15s%-9s%s\n","Date","Start","End","Type","Room","Device");
      for(x=0;x<74;x++){
        printf("=");
      }
      printf("\n");
    }
    printAppointmentSchedule(tenant_E_record[j],1);
  }
  printf("%-10s\n","- End -");
  for(x=0;x<74;x++){
    printf("=");
  }
  printf("\n");
  tenant_A_count = 0;
  tenant_B_count = 0;
  tenant_C_count = 0;
  tenant_D_count = 0;
  tenant_E_count = 0;
  fclose(Accepted);
}

//Print rejected report
void print_rejected(int algorithm){
  FILE *Rejected;
  int a = 0; int b = 0; int c = 0; int d = 0; int e = 0;
  tenant_A_count = 0; tenant_B_count = 0; tenant_C_count = 0; 
  tenant_D_count = 0; tenant_E_count = 0;
  if (algorithm == 1){
    Rejected = fopen("FCFSBookingRejected.log", "r");
  } else if (algorithm == 2){
    Rejected = fopen("PRIOBookingRejected.log", "r");
  }
  //Get the number of record of each tenant
  while (fscanf(Rejected, "%[^\n]\n", input_from_file) != EOF){
    countTenantRecord(input_from_file);
  }
  char tenant_A_record[tenant_A_count][255];
  char tenant_B_record[tenant_B_count][255];
  char tenant_C_record[tenant_C_count][255];
  char tenant_D_record[tenant_D_count][255];
  char tenant_E_record[tenant_E_count][255];
  fclose(Rejected);
  if (algorithm == 1){
    Rejected = fopen("FCFSBookingRejected.log", "r");
  } else if (algorithm == 2){
    Rejected = fopen("PRIOBookingRejected.log", "r");
  }
  //Add the record to corresponding array
  while (fscanf(Rejected, "%[^\n]\n", input_from_file) != EOF){
    char original_input[255];
    int n;
    strncpy(original_input, input_from_file, strlen(input_from_file));
    original_input[strlen(input_from_file)] = '\0';
    n = tenantRecordArrange(input_from_file);
    if(n == 1){
      strcpy(tenant_A_record[a], original_input);
      a += 1;
    } else if(n == 2){
      strcpy(tenant_B_record[b], original_input);
      b += 1;
    } else if(n == 3){
      strcpy(tenant_C_record[c], original_input);
      c += 1;
    } else if(n == 4){
      strcpy(tenant_D_record[d], original_input);
      d += 1;
    } else if(n == 5){
      strcpy(tenant_E_record[e], original_input);
      e += 1;
    }
  }
  if (algorithm == 1){
    printf("*** Room Booking – REJECTED / FCFS ***\n");
  } else if (algorithm == 2){
    printf("*** Room Booking – ACCEPTED / PRIO ***\n");
  }
  
  for(i=0;i<tenant_A_count;i++){
    if(tenant_A_count == 0){
      break;
    }
    if(i==0){
      printf("\n");
      printf("Tenant_A (there are %d bookings rejected)\n",tenant_A_count);
      printf("\n");
      printf("%-13s%-8s%-8s%-15s%s\n","Date","Start","End","Type","Device");
      for(j=0;j<74;j++){
        printf("=");
      }
      printf("\n");
    }
    printAppointmentSchedule(tenant_A_record[i],0);
  }
  for(i=0;i<tenant_B_count;i++){
    if(tenant_B_count == 0){
      break;
    }
    if(i==0){
      printf("\n");
      printf("Tenant_B (there are %d bookings rejected)\n",tenant_B_count);
      printf("\n");
      printf("%-13s%-8s%-8s%-15s%s\n","Date","Start","End","Type","Device");
      for(j=0;j<74;j++){
        printf("=");
      }
      printf("\n");
    }
    printAppointmentSchedule(tenant_B_record[i],0);
  }
  for(i=0;i<tenant_C_count;i++){
    if(tenant_C_count == 0){
      break;
    }
    if(i==0){
      printf("\n");
      printf("Tenant_C (there are %d bookings rejected)\n",tenant_C_count);
      printf("\n");
      printf("%-13s%-8s%-8s%-15s%s\n","Date","Start","End","Type","Device");
      for(j=0;j<74;j++){
        printf("=");
      }
      printf("\n");
    }
    printAppointmentSchedule(tenant_C_record[i],0);
  }
  for(i=0;i<tenant_D_count;i++){
    if(tenant_D_count == 0){
      break;
    }
    if(i==0){
      printf("\n");
      printf("Tenant_D (there are %d bookings rejected)\n",tenant_D_count);
      printf("\n");
      printf("%-13s%-8s%-8s%-15s%s\n","Date","Start","End","Type","Device");
      for(j=0;j<74;j++){
        printf("=");
      }
      printf("\n");
    }
    printAppointmentSchedule(tenant_D_record[i],0);
  }
  for(i=0;i<tenant_E_count;i++){
    if(tenant_E_count == 0){
      break;
    }
    if(i==0){
      printf("\n");
      printf("Tenant_E (there are %d bookings rejected)\n",tenant_E_count);
      printf("\n");
      printf("%-13s%-8s%-8s%-15s%s\n","Date","Start","End","Type","Device");
      for(j=0;j<74;j++){
        printf("=");
      }
      printf("\n");
    }
    printAppointmentSchedule(tenant_E_record[i],0);
  }
  printf("%-10s\n","- End -");
  for(x=0;x<74;x++){
    printf("=");
  }
  printf("\n");
  tenant_A_count = 0;
  tenant_B_count = 0;
  tenant_C_count = 0;
  tenant_D_count = 0;
  tenant_E_count = 0;
  fclose(Rejected);
}

//run algorithm, 1 = FCFS, 2 = PRIO, 3 = OPTI
void start_printBookings(int algorithm){
  FILE *outallBooking, *Accepted, *Rejected;
  if(algorithm == 1){
    outallBooking = fopen("allBooking.log", "r");
    Accepted = fopen("FCFSBookingAccepted.log", "a");
    Rejected = fopen("FCFSBookingRejected.log", "a");
  } else if (algorithm == 2){
    FILE *PRIOSortedBooking;
    Accepted = fopen("PRIOBookingAccepted.log", "a");
    Rejected = fopen("PRIOBookingRejected.log", "a");
    PRIOSortedBooking = fopen ("PRIOSortedBooking.log", "a");
    outallBooking = fopen("allBooking.log", "r");
    while (fscanf(outallBooking, "%[^\n]\n", input_from_file) != EOF){
      count_cmd_for_keyword(input_from_file);
    }
    char conference[conference_count][255]; char presentation[presentation_count][255]; 
    char meeting[meeting_count][255]; char device[device_count][255];
    fclose(outallBooking);
    outallBooking = fopen("allBooking.log", "r");
    while (fscanf(outallBooking, "%[^\n]\n", input_from_file) != EOF){
      char temp_input[255];
      strncpy(temp_input, input_from_file, strlen(input_from_file));
      temp_input[strlen(input_from_file)] = '\0';
      int counter = 0;
      str = strtok (input_from_file," ");
      while (str != NULL) {
        int m = 0;
        m = match(str,ptnKeyWord);
        if(m){
          if (strstr(str, "addConference") != NULL){
            strcpy(conference[conf_temp],temp_input);
            conf_temp++;
          } else if (strstr(str, "addPresentation") != NULL){
            strcpy(presentation[pres_temp],temp_input);
            pres_temp++;
          } else if (strstr(str, "addMeeting") != NULL){
            strcpy(meeting[meet_temp],temp_input);
            meet_temp++;
          } else if (strstr(str, "bookDevice") != NULL){
            strcpy(device[devi_temp],temp_input);
            devi_temp++;
          }
        }
        str = strtok (NULL, " ");
      }
    }
    fclose(outallBooking);
    for(i=0;i<conference_count;i++){fprintf(PRIOSortedBooking, "%s\n", conference[i]);}
    for(i=0;i<presentation_count;i++){fprintf(PRIOSortedBooking, "%s\n", presentation[i]);}
    for(i=0;i<meeting_count;i++){fprintf(PRIOSortedBooking, "%s\n", meeting[i]);}
    for(i=0;i<device_count;i++){fprintf(PRIOSortedBooking, "%s\n", device[i]);}
    conference_count = 0; presentation_count=0; meeting_count=0; device_count=0;
    conf_temp = 0; pres_temp = 0; meet_temp = 0; devi_temp = 0;
    fclose(PRIOSortedBooking);
    outallBooking = fopen("PRIOSortedBooking.log", "r");
  }

  while (fscanf(outallBooking, "%[^\n]\n", input_from_file) != EOF){
    char original_input[255];
    strncpy(original_input, input_from_file, strlen(input_from_file)-1);
    original_input[strlen(input_from_file)-1] = '\0';
    room = 0;
    if(extract_cmd(input_from_file)){
      if (room == 1){
        strcat(original_input, " room_A");
      } else if (room == 2){
        strcat(original_input, " room_B");
      } else if (room == 3){
        strcat(original_input, " room_C");
      }
      fprintf(Accepted, "%s\n", original_input);
    } else {
      fprintf(Rejected, "%s\n", original_input);
    }
  }
  
  //Reset Room variables
  memset(Room_A, 0, sizeof(Room_A));
  memset(Room_B, 0, sizeof(Room_B));
  memset(Room_C, 0, sizeof(Room_C));

  //Reset Device variables
  memset(webcam_FHD_1, 0, sizeof(webcam_FHD_1));
  memset(webcam_FHD_2, 0, sizeof(webcam_FHD_2));
  memset(webcam_UHD, 0, sizeof(webcam_UHD));
  memset(monitor_50_1, 0, sizeof(monitor_50_1));
  memset(monitor_50_2, 0, sizeof(monitor_50_2));
  memset(monitor_75, 0, sizeof(monitor_75));
  memset(projector_2K_1, 0, sizeof(projector_2K_1));
  memset(projector_2K_2, 0, sizeof(projector_2K_2));
  memset(projector_4K, 0, sizeof(projector_4K));
  memset(screen_100_1, 0, sizeof(screen_100_1));
  memset(screen_100_2, 0, sizeof(screen_100_2));
  memset(screen_150, 0, sizeof(screen_150));
  fclose(outallBooking);
  fclose(Accepted);
  fclose(Rejected);
  print_accepted(algorithm);
  print_rejected(algorithm);
  if(algorithm == 1){
    remove("FCFSBookingAccepted.log");
    remove("FCFSBookingRejected.log");
  } else if (algorithm == 2){
    remove("PRIOSortedBooking.log");
    remove("PRIOBookingAccepted.log");
    remove("PRIOBookingRejected.log");
  }
}
//<===============================function==============================>
int main(void) {
  char buf[80]={0}; // Child read
  char buf2[80]={0}; //Parent read

  //remove("allBooking.log");
  remove("FCFSBookingAccepted.log");
  remove("FCFSBookingRejected.log");
  remove("PRIOSortedBooking.log");
  remove("PRIOBookingAccepted.log");
  remove("PRIOBookingRejected.log");
  //Create new pipe
  for (i = 0;i < 6; i++){
    if (pipe(pipes[i]) < 0){
        printf("pipe creation error\n");
        exit(1);
    }
  }

  //Create Child
  for(k = 0;k < 3; k++){
    child[k] = fork();
    if (child[k]<0) {
    printf("Fork Failed\n");
    exit(1);
    } else if (child[k]==0) {
      //Close unused pipes in Child
      for (j = 0; j < 6; j++){
        if(j != k*2){
          close(pipes[j][0]);
        }
        if(j != (k*2+1)){
          close(pipes[j][1]);
        }
      }
      break;
    }
  }  

  if(k!=3){ //Child Process
    while(1){
      memset(buf,0,sizeof(buf));
      int n;
      n = read(pipes[2*k][0],buf,4);
      buf[n]=0;
      if(strcmp(buf,"fcfs")==0){ //Child 0 => FCFS
        start_printBookings(1);
        //printf("I am Child %d: %s\n",k,buf);
        strcpy(buf2, "Done");
        write(pipes[1][1],buf2,4);
      } else if (strcmp(buf,"prio")==0){ //Child 1 => PRIO
        start_printBookings(2);
        //printf("I am Child %d: %s\n",k,buf);
        strcpy(buf2, "Done");
        write(pipes[3][1],buf2,4 );
      } else if (strcmp(buf,"opti")==0){ //Child 2 => OPTI
        //printf("I am Child %d: %s\n",k,buf);
        strcpy(buf2, "Done");
        write(pipes[5][1],buf2,4);
      } else if (strcmp(buf,"end")==0){ //ALL Child exit
        //printf("I am Child %d: %s\n",k,buf);
        exit(0);
      }
    }
  } else { //Parent Process
    //Close unused pipes in Parent
    for (j = 0; j < 6; j++){
      if(j%2 == 0){
        close(pipes[j][0]);
      } else if (j%2 != 0){
        close(pipes[j][1]);
      }
    }
    FILE *outallBooking;
    define_keyword(ptnKeyWord);
    //printf("ptnKeyWord = %s",ptnKeyWord);
    /*program start*/
    printf("~~ WELCOME TO PolySME ~~\n");
    //Get user input
    while (1)
    {
      printf("Please enter booking:\n");
      // fgets(input, sizeof input, stdin);
      // scanf(" %[^;]",input);
      //get whole line
      fgets(input,255,stdin);
      if (strstr(input, "endProgram") != NULL) {
        for(j=0;j<5;j++){
          if(j%2==0){
            write(pipes[j][1],"end",3);
          }
        }
        //write(pipes[0][1],"end",3);
        wait(NULL);
        //remove("allBooking.log");
        remove("FCFSBookingAccepted.log");
        remove("FCFSBookingRejected.log");
        remove("PRIOSortedBooking.log");
        remove("PRIOBookingAccepted.log");
        remove("PRIOBookingRejected.log");
        //remove("OPTIBooking.log");
        printf("-> Bye!\n");
        exit(0);
        //break;
      } else if (strstr(input, "addBatch")!=NULL){
        batchFileHandler(batchFileName(input));
        printf("-> [Pending]\n");
      } else if (strstr(input, "printBookings")!=NULL){
        schedulingChecking(input);
        if(strcmp(fcfs,"fcfs")==0){
          write(pipes[0][1],"fcfs",4);
          read(pipes[1][0],buf2,4); //Return to main flow
          //printf("buf2=%s\n",buf2);
          printf("-> [Done!]\n");
          strcpy(fcfs,"");
        } else if (strcmp(prio,"prio")==0){
          write(pipes[2][1],"prio",4);
          read(pipes[3][0],buf2,4); //Return to main flow
          //printf("buf2=%s\n",buf2);
          printf("-> [Done!]\n");
          strcpy(prio,"");
        } else if (strcmp(opti,"opti")==0){
          write(pipes[4][1],"opti",4);
          read(pipes[5][0],buf2,4); //Return to main flow
          //printf("buf2=%s\n",buf2);
          printf("-> [Done!]\n");
          strcpy(opti,"");
        }
      } else if (strstr(input, "addMeeting")!=NULL || 
                strstr(input, "addPresentation")!=NULL ||
                strstr(input, "addConference")!=NULL ||
                strstr(input, "bookDevice")!=NULL){
        char user_input[100];
        strncpy(user_input, input, strlen(input)-1);
        user_input[strlen(input)-1] = '\0';
        if(devicePairChecking(input)){ //Check the device pair
          //Open allBooking.log
          outallBooking = fopen("allBooking.log", "a");
          if (outallBooking == NULL) {
            printf("Error in opening output file\n");
            exit(1);
          }
          fprintf(outallBooking, "%s\n", user_input); //Write to allBooking.log
          fclose(outallBooking);
          printf("-> [Pending]\n");
        } else {
          printf("Wrong device pair!\n");
        }
      } else {
        printf("Error, Please try again!\n");
      }
    }
    return 0;
  }
}