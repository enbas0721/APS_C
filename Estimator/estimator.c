// 音響測位のためのメインプログラム
// 第一引数にオーディオカード名 (例. "hw:1")
// 第二引数に書き出したい録音ファイル名 (例. "output.wav")
#define _GNU_SOURCE
#include "recordManager.h"

int main(int argc, char const *argv[])
{
    sighandler_t sig = 0;
	// Ctrl + Zを無視して，入力があればclose処理
	sig = signal(SIGTSTP, SIG_IGN);
	if(SIG_ERR == sig){
		printf("Pushed Ctrl+Z\n");
	}
    record_start(argv[1], argv[2]);
    return 0;
}
