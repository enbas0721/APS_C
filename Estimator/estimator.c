// 音響測位のためのメインプログラム
// 第一引数にオーディオカード名 (例. "hw:1")
// 第二引数に書き出したい録音ファイル名 (例. "output.wav")

#include <pthread.h>
#include <string.h>
#include "recordManager.h"

int main(int argc, char const *argv[])
{
    record_info info;
    strcpy(info.card,argv[1]);
    strcpy(info.filename,argv[2]);
    pthread_t record_thread;
    int ret = 0;

    ret = pthread_create(&record_thread, NULL, record_start, &info);
    return 0;
}
