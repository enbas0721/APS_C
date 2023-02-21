// 音信号を生成し、周期的に送信する。
// 音量を設定。設定範囲：　1~30

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <alsa/asoundlib.h>
#include "sendManager.h"

void make_chirp_wave(int16_t* data, int vol, int f0, int f1, int size){
	int n;
	double t;
	for (n = 0; n < size; n++)
	{
        if (n <= DEF_FS * SIGNAL_L)
        {
            t = (double)n/DEF_FS;
		    data[n] = (int)((vol*1000) * sin(2*M_PI * t * (f0 + ((f1-f0)/(2*SIGNAL_L))*t)));		    
        }else{
            data[n] = 0;
        }
	}		
}

void make_sin_wave(int16_t* data, int vol, int f, int size){
	int n;
	double t;
	for (n = 0; n < size; n++){
        if (n <= DEF_FS * SIGNAL_L){
            t = (double)n/DEF_FS;
		    data[n] = (int)(vol*sin(2*M_PI*SIGNAL_L*DEF_FS*f*(1/DEF_FS)));
        }else{
            data[n] = 0;
        }
	}
}

void* send_start(send_info *info)
{
    // 出力デバイス
    char *device = "default";
    //  ソフトSRC有効無効設定
    unsigned int soft_resample = 1;
    // ALSAのバッファ時間[msec]
    const static unsigned int latency = 50000;
    
    // 符号付き16bit
    static snd_pcm_format_t format = SND_PCM_FORMAT_S16;
 
    int16_t *buffer = NULL;
	int16_t *data = NULL;

	// 信号生成用パラメータ
	int f0 = INITIAL_F;
	int f1 = FINAL_F;
	float signal_length = SIGNAL_L; 
	int data_size = DEF_FS;
    int redata_size, current_index, ret, n, m;

    snd_pcm_t *hndl = NULL;
 
    /* バッファの用意 */
    buffer = (int16_t*)malloc(BUF_SIZ*snd_pcm_format_width(format));
	data = (int16_t*)malloc(data_size*snd_pcm_format_width(format));
    make_chirp_wave(data, info->vol, f0, f1, data_size);
 
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

    time_t start_time;
    ret = 0;
    while(info->flag)
    {
        current_index = 0;
        start_time = time(NULL);
        for (n = 0; n < data_size; n += BUF_SIZ) {
            /* データをバッファに読み込み */
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
        printf("period:%ld",(time(NULL)-start_time));
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
