#include <stdio.h>
#include <stdlib.h>
#include "audioio.h"

int main(int argc, char *argv[])
{
	//変数宣言
	WAV_PRM prm_in, prm_out;
	double *data_in, *data_out;
	int n;
	char filename[64];

	//コマンドライン引数が違う場合
	if(argc != 2) {
		printf("引数が違います\n");
		exit( 1 );
	}

	//出力ファイル名入力
	printf("output file name : ");
	scanf("%s", filename);

	//wavファイルの読み込み
	data_in = audio_read(&prm_in, argv[1]);

	//パラメータコピー
	prm_out.fs =  prm_in.fs;
	prm_out.bits = prm_in.bits;
	prm_out.L =  prm_in.L;

	//データコピー(実際にはこの代わりにエフェクト処理をかける)
	data_out = calloc(prm_out.L, sizeof(double)); //メモリの確保
	for (n = 0; n < prm_out.L; n++) {
		data_out[n] = data_in[n];
	}

	//書き込み
	audio_write(data_out, &prm_out, filename);

	//メモリ解放
	free(data_in);
	free(data_out);

	return 0;
}
