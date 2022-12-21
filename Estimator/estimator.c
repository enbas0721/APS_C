// 音響測位のためのメインプログラム
// 第一引数にオーディオカード名 (例. "hw:1")
// 第二引数に書き出したい録音ファイル名 (例. "output.wav")

#include "recordManager.h"

int main(int argc, char const *argv[])
{
    record_start(argv[1], argv[2]);
    return 0;
}
