// 音響測位のためのメインプログラム
// 第一引数にオーディオカード名 (例. "hw:1")
// 第二引数に書き出したい録音ファイル名 (例. "output.wav")

#include <pthread.h>
#include <string.h>
#include "recordManager.h"

int main(int argc, char const *argv[])
{
    char command;
    record_info info;
    info.flag = 1;
    strcpy(info.card,argv[1]);
    strcpy(info.filename,argv[2]);
    pthread_t record_thread;
    int ret = 0;

    ret = pthread_create(&record_thread, NULL, (void*)record_start, (void*)&info);
    // scanfは止まらない？
    printf("Input close to close\n");
    scanf("%c", &command);
    printf("%c\n", command);
    if (command == 'c'){
        info.flag = 0;
    }
    ret = pthread_join(record_thread,NULL);
    if (ret != 0){
	    printf("error\n");
        exit(1);
    }
    printf("All done.\n");
    return 0;
}
