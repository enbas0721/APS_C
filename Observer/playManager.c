/* ALSA lib を使用して、WAVファイルを再生する */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <alsa/asoundlib.h>
 
/* PCMデフォルト設定 */
#define DEF_CHANNEL         2
#define DEF_FS              48000
#define DEF_BITPERSAMPLE    16
#define WAVE_FORMAT_PCM     1
#define BUF_SAMPLES         1024
 
/* ChunkID 定義 */
const char ID_RIFF[4] = "RIFF";
const char ID_WAVE[4] = "WAVE";
const char ID_FMT[4]  = "fmt ";
const char ID_DATA[4] = "data";
 
/* PCM情報格納用構造体 */
typedef struct {
    uint16_t      wFormatTag;         // format type  
    uint16_t      nChannels;          // number of channels (1:mono, 2:stereo)
    uint32_t      nSamplesPerSec;     // sample rate
    uint32_t      nAvgBytesPerSec;    // for buffer estimation
    uint16_t      nBlockAlign;        // block size of data
    uint16_t      wBitsPerSample;     // number of bits per sample of mono data
    uint16_t      cbSize;             // extra information
} WAVEFORMATEX;
 
/* CHUNK */
typedef struct {
    char        ID[4];  // Chunk ID
    uint32_t    Size;   // Chunk size;
} CHUNK;
 
/* WAVE ファイルのヘッダ部分を解析、必要な情報を構造体に入れる
 *  ・fp は DATA の先頭位置にポイントされる
 *  ・戻り値は、成功時：データChunk のサイズ、失敗時：-1
 */
static int readWavHeader(FILE *fp, WAVEFORMATEX *wf)
{
    char  FormatTag[4];
    CHUNK Chunk;
    int ret = -1;
    int reSize;
     
    /* Read RIFF Chunk*/
    if((fread(&Chunk, sizeof(Chunk), 1, fp) != 1) ||
       (strncmp(Chunk.ID, ID_RIFF, 4) != 0)) {
        printf("file is not RIFF Format ¥n");
        goto RET;
    }
 
    /* Read Wave */
    if((fread(FormatTag, 1, 4, fp) != 4) ||
       (strncmp(FormatTag, ID_WAVE, 4) != 0)){
        printf("file is not Wave file¥n");
        goto RET;
    }
             
    /* Read Sub Chunk (Expect FMT, DATA) */
    while(fread(&Chunk, sizeof(Chunk), 1, fp) == 1) {
        if(strncmp(Chunk.ID, ID_FMT, 4) == 0) {
            /* 小さい方に合せる(cbSize をケアするため) */
            reSize = (sizeof(WAVEFORMATEX) < Chunk.Size) ? sizeof(WAVEFORMATEX) : Chunk.Size;
            fread(wf, reSize, 1, fp);
            if(wf->wFormatTag != WAVE_FORMAT_PCM) {
                printf("Input file is not PCM¥n");
                goto RET;
            }
        }
        else if(strncmp(Chunk.ID, ID_DATA, 4) == 0) {
            /* DATA Chunk を見つけたらそのサイズを返す */
            ret = Chunk.Size;
            break;
        }
        else {
            /* 知らない Chunk は読み飛ばす */
            fseek(fp, Chunk.Size, SEEK_CUR);
            continue;
        }
    };
     
RET:
    return ret;
}
 
int main(int argc, char *argv[])
{
    /* 出力デバイス */
    char *device = "default";
     /* ソフトSRC有効無効設定 */
    unsigned int soft_resample = 1;
     /* ALSAのバッファ時間[msec] */
    const static unsigned int latency = 50000;
     /* PCM 情報 */
    WAVEFORMATEX wf = { WAVE_FORMAT_PCM,   // PCM
                        DEF_CHANNEL,
                        DEF_FS,
                        DEF_FS * DEF_CHANNEL * (DEF_BITPERSAMPLE/8),
                        (DEF_BITPERSAMPLE/8) * DEF_CHANNEL,
                        DEF_BITPERSAMPLE,
                        0};
    /* 符号付き16bit */
    static snd_pcm_format_t format = SND_PCM_FORMAT_S16;
 
    int16_t *buffer = NULL;
    int dSize, reSize, ret, n;
    FILE *fp = NULL;
    snd_pcm_t *hndl = NULL;
     
 
    if(argc != 2) {
        printf("no input file¥n");
    }
 
    /* WAVファイルを開く*/
    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        printf("Open error:%s¥n", argv[1]);
        goto End;
    }
 
    /* WAVのヘッダーを解析する */
    dSize = readWavHeader(fp, &wf);  
    if (dSize <= 0) {
        goto End;
    }
     
    /* PCMフォーマットの確認と情報出力を行う */
    printf("format : PCM, nChannels = %d, SamplePerSec = %d, BitsPerSample = %d¥n",
            wf.nChannels, wf.nSamplesPerSec, wf.wBitsPerSample);
 
    /* バッファの用意 */
    buffer = malloc(BUF_SAMPLES * wf.nBlockAlign);
    if(buffer == NULL) {
        printf("malloc error¥n");
        goto End;
    }
 
    /* 再生用PCMストリームを開く */
    ret = snd_pcm_open(&hndl, device, SND_PCM_STREAM_PLAYBACK, 0);
    if(ret != 0) {
        printf( "Unable to open PCM¥n" );
        goto End;
    }
     
    /* フォーマット、バッファサイズ等各種パラメータを設定する */
    ret = snd_pcm_set_params( hndl, format, SND_PCM_ACCESS_RW_INTERLEAVED, wf.nChannels,
                              wf.nSamplesPerSec, soft_resample, latency);
    if(ret != 0) {
        printf( "Unable to set format¥n" );
        goto End;
    }
 
    for (n = 0; n < dSize; n += BUF_SAMPLES * wf.nBlockAlign) {
        /* PCMの読み込み */
        fread(buffer, wf.nBlockAlign, BUF_SAMPLES, fp);
 
        /* PCMの書き込み */
        reSize = (n < BUF_SAMPLES) ? n : BUF_SAMPLES;
        ret = snd_pcm_writei(hndl, (const void*)buffer, reSize);
        /* バッファアンダーラン等が発生してストリームが停止した時は回復を試みる */
        if (ret < 0) {
            if( snd_pcm_recover(hndl, ret, 0 ) < 0 ) {
                printf( "Unable to recover Stream." );
                goto End;
            }
        }
    }
 
    /* データ出力が終わったため、たまっているPCMを出力する。 */
    snd_pcm_drain(hndl);
 
End:
     
    /* ストリームを閉じる */
    if (hndl != NULL) {
        snd_pcm_close(hndl);
    }
     
    /* ファイルを閉じる */
    if (fp != NULL) {
        fclose(fp);
    }
     
    /* メモリの解放 */
    if (buffer != NULL) {
        free(buffer);
    }
     
    return 0;
}