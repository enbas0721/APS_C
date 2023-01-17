#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "audioio.h"

int main(int argc, char *argv[])
{
	//変数宣言
	WAV_PRM prm_out;
	double *data_out;
	int n;
	char filename[64];

	//コマンドライン引数が違う場合
	if(argc != 2) {
		printf("引数が違います\n");
		exit( 1 );
	}

	for (int n = 0; n < sizeof(argv[1]); n++) {
		filename[n] = argv[1][n];
	}

	// パラメータコピー
	// サンプリング周波数fs*outputで欲しい秒数=L
	prm_out.fs = 44100;
	prm_out.bits = 16;
	prm_out.L =  176400;

	data_out = calloc(prm_out.L, sizeof(double)); //メモリの確保
	for (n = 0; n < prm_out.L; n++) {
		data_out[n] = sin(n);
	}

	//書き込み
	for(int i = 0; i < 5; i++){
		audio_write(data_out, &prm_out, filename);
	}

	//メモリ解放
	free(data_out);

	return 0;
}
