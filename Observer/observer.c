#include <stdio.h>
#include <pthread.h>
#include "sendManager.h"

int main(int argc, char const *argv[])
{
    char command;
    send_info info;
    info.vol = atoi(argv[1]);
    info.flag = 1;
    pthread_t send_thread;
    int ret = 0;
    
    ret = pthread_create(&send_thread, NULL, (void*)send_start, (void*)&info);
    printf("Input c to close\n");
    scanf("%c", &command);
    if (command == 'c'){
        info.flag = 0;
    }
    ret = pthread_join(send_thread,NULL);
    if (ret != 0){
	    printf("error\n");
        exit(1);
    }
    printf("All done.\n");
    return 0;
}
