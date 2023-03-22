// 音響測位のためのメインプログラム
// 第一引数にオーディオカード名 (例. "hw:1")
// 第二引数に書き出したい録音ファイル名 (例. "output.wav")

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "recordManager.h"
#include "trackManager.h"

int main(int argc, char const *argv[])
{
    char command;
    record_info info;
    info.flag = 1;
    strcpy(info.filename,argv[1]);
    info.last_index = 0;
    pthread_t record_thread;
    pthread_t track_thread;
    int ret = 0;
    
    ret = pthread_create(&record_thread, NULL, (void*)record_start, (void*)&info);
    ret = pthread_create(&track_thread, NULL, (void*)track_start, (void*)&info);
    printf("Input close to close\n");
    while (1)
    {
        scanf("%c", &command);
        if (command == 'c'){
            info.flag = 0;
            break;
    }
    ret = pthread_join(track_thread,NULL);
    if (ret != 0){
	    printf("error\n");
        exit(1);
    }
    ret = pthread_join(record_thread,NULL);
    if (ret != 0){
	    printf("error\n");
        exit(1);
    }
    free(info.record_data);
    printf("All done.\n");
    return 0;
}
