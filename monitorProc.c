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

int sumOfn(char *s, int n) 
{
  int sum = 0;
  int cache = 0;
  char *line = s;

  strncpy(line, line, 5);

  for (int i = 0; i < n; i++)
  {
    sscanf(line, "%d", &cache);
    if ((line = strchr(line, ' ')) == 0) 
      break;

    sum+=cache;
    line++;
  }

  return sum;
}

void getPidStats(char *argv[], int argc)
{
  char procPath[argc-1][32];
   
  int pastNonIdleUsage;
  int nonIdleUsage;

  int userTimes[argc-1];
  int pastUserTimes[argc-1];

  int sysTimes[argc-1];
  int pastSysTimes[argc-1];

  int totalTimes[argc];
  int pastTotalTimes[argc];

  struct timespec ts;

  FILE *fa[argc-1];
  FILE *ft = fopen("resultsTOTAL.csv", "w");
  fprintf(ft, "Timestamp,Util\n");

  for(int i = 1; i < argc; i++)
  { 
    printf("%d", i);
    char saveLoc[32];

    snprintf(saveLoc, sizeof saveLoc, "results%s.csv", argv[i]);
    if (i == 1) {
	snprintf(procPath[i-1], sizeof procPath[i-1], "/proc/%s/stat", argv[i]);
    } else {
	snprintf(procPath[i-1], sizeof procPath[i-1], "/proc/%s/task/%s/stat", argv[1], argv[i]);
    }

    fa[i-1] = fopen(saveLoc, "w");
    fprintf(fa[i-1], "Timestamp,Util\n");
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

      totalTimes[i] = sumOfn(buf, 32);
      nonIdleUsage = sumOfn(buf, 3);
      
      clock_gettime(CLOCK_MONOTONIC, &ts);

      if(pastTotalTimes[i] && totalTimes[i] > pastTotalTimes[i]) 
      {
        float sumUtil = 100 * ((userTimes[i] - pastUserTimes[i]) + (sysTimes[i] - pastSysTimes[i])) / (totalTimes[i] - pastTotalTimes[i]);
        //float sysUtil = 100 * (sysTimes[i] - pastSysTimes[i]) / (totalTimes - pastTotalTimes);
        float totalUtil = 100 * (nonIdleUsage - pastNonIdleUsage) / (totalTimes[i] - pastTotalTimes[i]);

        fprintf(fa[i], "%d.%02ld,%.2f\n", ts.tv_sec, ts.tv_nsec/10000000, sumUtil); 
      }

      if(!i)
      {
        float totalUtil = 100 * (nonIdleUsage - pastNonIdleUsage) / (totalTimes[i] - pastTotalTimes[argc-1]);
        fprintf(ft, "%d.%02ld,%.2f\n", ts.tv_sec, ts.tv_nsec/10000000, totalUtil);

	pastTotalTimes[argc-1] = totalTimes[i];
      }

      pastUserTimes[i] = userTimes[i];
      pastSysTimes[i] = sysTimes[i];
      pastTotalTimes[i] = totalTimes[i];
      pastNonIdleUsage = nonIdleUsage;

      fflush(fa[i]);
      fflush(ft);
    }
    usleep(PERIOD);
  }
}

int main(int argc, char *argv[]) {
  if(argc >= 2) {
    getPidStats(argv, argc);
  } else {
    printf("Not enough arguments");
  }
}
