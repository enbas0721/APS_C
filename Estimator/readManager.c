#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "WavManager/audioio.h"
#include "recordManager.h"
#include "readManager.h"

void* read_start(record_info *info)
{
    // 変数宣言
    WAV_PRM prm_in;
    int16_t *data_in,*buffer;
    int n;
    int current_index = 0;
    char filename[64];

    printf("Input file name: ");
    scanf("%s",filename);

    data_in = audio_read(&prm_in, filename);
    info->record_data = calloc(prm_in.L, sizeof(int16_t));
    while(current_index < prm_in.L)
    {
        if ((current_index + BUF_SIZ) < prm_in.L){
            for (n = 0; n < BUF_SIZ; n++)
            {
                info->record_data[current_index] = data_in[current_index];
                current_index += 1;
            }
            info->last_index += BUF_SIZ;
        }else{
            for (n = 0; n < (prm_in.L - current_index); n++){
                info->record_data[current_index] = data_in[current_index];
                current_index += 1;
            }           
            info->last_index += (prm_in.L - current_index);
        }
    }
    return 0;
}