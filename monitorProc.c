#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#define PERIOD 100000

// Substrings string s from nth occurence of char c
const char *nth_strchr(const char *s, int c, int n) 
{
  const char *nth = s;
  assert(c != '\0');

  for (int i = 0; i < n; i++)
  {
    if ((nth = strchr(nth, c)) == 0)
      break;
    nth++;
  }

  return nth;
}

// Returns sum of all numbers in a given string (space separated)
int sumOfAll(char *s) 
{
  int sum = 0;
  int cache = 0;
  char *line = s;

  strncpy(line, line, 5);

  for (;;)
  {
    sscanf(line, "%d", &cache);
    if ((line = strchr(line, ' ')) == 0) 
      break;

    sum+=cache;
    line++;
  }

  return sum;
}

char* getPidStats(char *argv[], int argc)
{
  char procPath[argc-1][32];

  int userTimes[argc-1];
  int pastUserTimes[argc-1];

  int sysTimes[argc-1];
  int pastSysTimes[argc-1];

  int totalTimes[argc-1];
  int pastTotalTimes[argc-1];

  struct timespec ts;

  FILE *fa[argc-1];
  for(int i = 1; i < argc; i++)
  { 
    printf("%d", i);
    char saveLoc[32];

    snprintf(saveLoc, sizeof saveLoc, "results%s.csv", argv[i]);
    snprintf(procPath[i-1], sizeof procPath[i], "/proc/%s/stat", argv[i]);

    fa[i-1] = fopen(saveLoc, "w");
    fprintf(fa[i-1], "Timestamp,Sys Util,Usr Util\n");
  }
  
  for (;;)
  {
    for(int i = 0; i < argc-1; i++)
    {
      static char buf[128];
      FILE *fd=fopen(procPath[i],"r");
      fgets(buf, sizeof buf, fd);
      fclose(fd);

      strcpy(buf, nth_strchr(buf, ' ', 13));
      sscanf(buf, "%d %d", &userTimes[i], &sysTimes[i]);

      FILE *fg=fopen("/proc/stat", "r");
      fgets(buf, sizeof buf, fg);
      fclose(fg);

      totalTimes[i] = sumOfAll(buf);

      if(pastTotalTimes[i] && totalTimes[i] != pastTotalTimes[i]) 
      {
        float userUtil = 100 * (userTimes[i] - pastUserTimes[i]) / (totalTimes[i] - pastTotalTimes[i]);
        float sysUtil = 100 * (sysTimes[i] - pastSysTimes[i]) / (totalTimes[i] - pastTotalTimes[i]);

        clock_gettime(CLOCK_MONOTONIC, &ts);

        fprintf(fa[i], "%d.%02ld,%.2f,%.2f\n", ts.tv_sec, ts.tv_nsec/10000000, userUtil, sysUtil);
      }

      pastUserTimes[i] = userTimes[i];
      pastSysTimes[i] = sysTimes[i];
      pastTotalTimes[i] = totalTimes[i];

      fflush(fa[i]);
    }

    usleep(PERIOD);
  }

  return "Placeholder for loop breaks";
}

int main(int argc, char *argv[]) {
  if(argc >= 2) {
    char* result = getPidStats(argv, argc);
    printf("|%s", result);
  } else {
    printf("Not enough arguments");
  }
}