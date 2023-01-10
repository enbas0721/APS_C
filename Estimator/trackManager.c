#include <stdio.h>
#include "recordManager.h"
#include "trackManager.h"

#define SMPL 44100
#define EPS  0.8
#define TAU  1

int write_result(char * filename){
    return 0;
}

void* track_start(record_info *info)
{
    // 3つのモード
    // 1:閾値決定 2:初期送信時刻決定 3:位置推定
    int mode = 2;

    float initial_pos = 1.0;
    
    int i = 0;
    int current_index = 0;
    double current_time = 0.0;
    double start_time = 0.0;
    
    int threshold = 4000;

    double temperature = 20.0;
    double v = 331.5 + 0.6 * temperature;

    double propagation_time = 0.0;
    double distance = 0.0;

    while((info->flag) || (current_index < info->last_index))
    {
        if (info->last_index > current_index){
            current_time = current_index / SMPL;
            switch(mode){
                case 1:
                    // 閾値決定
                    current_index += 1;
                    break;
                case 2:
                    // 初期送信時刻決定
                    if (info->record_data[current_index] > threshold){
                        start_time = current_time - (initial_pos/v);
                        current_index = (int)(current_index + EPS/SMPL);
                        mode = 3;
                    }else{
                        current_index += 1;
                    }
                    break;
                case 3:
                    // 位置推定処理
                    if (info->record_data[current_index] > threshold){
                        propagation_time = current_time - start_time - TAU * i;
                        distance = propagation_time * v;
                        current_index = (int)(current_index + EPS/SMPL);
                        printf("推定距離: %lf {m}\n 振幅: %d {m}\n", distance, info->record_data[current_index]);
                    }else{
                        current_index += 1;
                    }
                    break;
                default:
                    printf("error: Non-existent mode\n");
            }
        }
    }
    int err;
    err = write_result(info->filename);
}