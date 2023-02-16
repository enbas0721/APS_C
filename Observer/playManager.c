/* ALSA lib を使用して、WAVファイルを再生する */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <alsa/asoundlib.h>
 
/* PCMデフォルト設定 */
#define DEF_CHANNEL         1
#define DEF_FS              48000
#define DEF_BITPERSAMPLE    16
#define WAVE_FORMAT_PCM     1
#define SIGNAL_L			1.0
#define INITIAL_F			1700
#define FINAL_F				1750
#define BUF_SIZ				2048

void make_chirp_wave(int16_t* data, int f0, int f1, float size){
	int n;
	for (n = 0; n < size; n++)
	{
		data[n] = sin(2*M_PI * (f0 + ((f1-f0)*n)/2*n)*n);
	}		
}

int main(int argc, char *argv[])
{
    /* 出力デバイス */
    char *device = "default";
     /* ソフトSRC有効無効設定 */
    unsigned int soft_resample = 1;
     /* ALSAのバッファ時間[msec] */
    const static unsigned int latency = 50000;
    
    /* 符号付き16bit */
    static snd_pcm_format_t format = SND_PCM_FORMAT_S16;
 
    int16_t *buffer = NULL;
	int16_t *data = NULL;
	int f0 = INITIAL_F;
	int f1 = FINAL_F;
	// unit [s]
	float signal_length = SIGNAL_L;
	int data_size = signal_length * DEF_FS;
    int redata_size, current_index, ret, n, m;
    snd_pcm_t *hndl = NULL;
 
    /* バッファの用意 */
    buffer = (int16_t*)malloc(sizeof(int16_t)*BUF_SIZ*snd_pcm_format_width(format));
	data = (int16_t*)malloc(sizeof(int16_t)*data_size*snd_pcm_format_width(format));
    make_chirp_wave(data, f0, f1, data_size);

	for (int i = 0; i < 10; i++)
	{
		printf("data:%d\n",data[i]);
	}
	
 
    /* 再生用PCMストリームを開く */
    ret = snd_pcm_open(&hndl, device, SND_PCM_STREAM_PLAYBACK, 0);
    if(ret != 0) {
        printf( "Unable to open PCM¥n" );
        exit(1);
    }
     
    /* フォーマット、バッファサイズ等各種パラメータを設定する */
    ret = snd_pcm_set_params( hndl, format, SND_PCM_ACCESS_RW_INTERLEAVED, DEF_CHANNEL,
                              DEF_FS, soft_resample, latency);
    if(ret != 0) {
        printf( "Unable to set format¥n" );
        exit(1);
    }

	current_index, ret = 0;
    for (n = 0; n < data_size; n += ret) {
        /* PCMの読み込み */
        for (m = 0; m < BUF_SIZ; m++)
		{
			buffer[m] = data[m+current_index];
		}
 
        /* PCMの書き込み */
        redata_size = (n < BUF_SIZ) ? n : BUF_SIZ;
        ret = snd_pcm_writei(hndl, (const void*)buffer, redata_size);
        /* バッファアンダーラン等が発生してストリームが停止した時は回復を試みる */
        if (ret < 0) {
            if( snd_pcm_recover(hndl, ret, 0 ) < 0 ) {
                printf( "Unable to recover Stream." );
                exit(1);
            }
        }
		current_index += ret;
    }
 
    /* データ出力が終わったため、たまっているPCMを出力する。 */
    snd_pcm_drain(hndl);
     
    /* ストリームを閉じる */
    if (hndl != NULL) {
        snd_pcm_close(hndl);
    }
     
    /* メモリの解放 */
    if (buffer != NULL) {
        free(buffer);
    }
	if (data != NULL) {
        free(data);
    }
     
    return 0;
}