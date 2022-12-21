// 音響測位のためのメインプログラム
// 第一引数にオーディオカード名 (例. "hw:1")
// 第二引数に書き出したい録音ファイル名 (例. "output.wav")
#define _GNU_SOURCE
#include "recordManager.h"

void signal_handler(int signum){
    printf("pushed Ctrl+Z\n");
    exit(1);
}

int main(int argc, char const *argv[])
{
    // sighandler_t sig = 0;
	// // Ctrl + Zを無視して，入力があればclose処理
	// signal(SIGINT, signal_handler);
    record_start(argv[1], argv[2]);
    return 0;
}
