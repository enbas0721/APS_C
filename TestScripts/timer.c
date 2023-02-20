/*
 * Author: Hiroyuki Chishiro
 * License: 2-Clause BSD
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
 
#define MESSAGE "timer_handler() is called.\n"
#define ERROR_MESSAGE "Error: unknown signum "
#define INIT_WAIT_SEC 1
#define INTERVAL_SEC 1
#define NR_TIMER_INTERRUPTS 4
 
static int remaining = NR_TIMER_INTERRUPTS;
 
void timer_handler(int signum)
{
  char s[4];
 
  switch (signum) {
  case SIGALRM:
    write(1, MESSAGE, sizeof(MESSAGE));
    remaining--;
    break;
 
  default:
    s[0] = signum / 10 + '0';
    s[1] = signum % 10 + '0';
    s[2] = '\n';
    s[3] = '\0';
    write(2, ERROR_MESSAGE, sizeof(ERROR_MESSAGE));
    write(2, s, sizeof(s));
    break;
  }
}
 
int main(void)
{
  struct sigaction action, old_action;
  struct itimerval timer, old_timer;
 
  memset(&action, 0, sizeof(action));
  memset(&old_action, 0, sizeof(struct sigaction));
 
  action.sa_handler = timer_handler;
  action.sa_flags = SA_RESTART;
 
  if (sigaction(SIGALRM, &action, &old_action) == -1) {
    perror("sigaction");
    exit(1);
  }
 
  timer.it_value.tv_sec = INIT_WAIT_SEC;
  timer.it_value.tv_usec = 0;
  timer.it_interval.tv_sec = INTERVAL_SEC;
  timer.it_interval.tv_usec = 0;
 
  if (setitimer(ITIMER_REAL, &timer, &old_timer) == -1) {
    perror("setitimer");
    exit(2);
  }
 
  while (remaining > 0) {
    #if COUNTDOWN
        struct itimerval current_timer;
    
        if (getitimer(ITIMER_REAL, &current_timer) == -1) {
        perror("getitimer");
        exit(3);
        }
    
        printf("current_timer.it_value: tv_sec = %ld, tv_usec = %ld\n",
            current_timer.it_value.tv_sec, current_timer.it_value.tv_usec);
    #endif
  }
 
  if (setitimer(ITIMER_REAL, &old_timer, NULL) == -1) {
    perror("setitimer");
    exit(4);
  }
 
  if (sigaction(SIGALRM, &old_action, NULL) == -1) {
    perror("sigaction");
    exit(5);
  }
 
  return 0;
}