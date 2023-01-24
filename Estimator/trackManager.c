#include <stdio.h>
#include <string.h>
#include "recordManager.h"
#include "trackManager.h"
#include "thermo.h"

void write_result(char * filename, double * time, double * distances, int size){
    FILE *fp;
    fp = fopen(filename, "w");
    fprintf(fp, "Time,");
    for (int n = 0; n < size; n++){
        fprintf(fp, "%lf,", time[n]);
    }
    fprintf(fp, "\n");

    fprintf(fp, "Distance,");
    for (int n = 0; n < size; n++){
        fprintf(fp, "%lf,", distances[n]);
    }
    fprintf(fp, "\n");
}

double sound_speed(double temperature){
    return (331.5 + (0.61 * temperature));
}

void* track_start(record_info *info)
{
    int phase = 2;

    float initial_pos = 1;
    
    int received_num = 0;
    int current_index = 0;
    double current_time = 0.0;
    double start_time = 0.0;
    
    int threshold = 1500;

    double temperature = 20.0;
    double v = sound_speed(temperature);

    double propagation_time = 0.0;
    double distance = 0.0;

    int log_index = 0;
    double distances[10000];
    double received_time[10000];

    while((info->flag) || (current_index < info->last_index))
    {
        if (info->last_index > current_index){
            current_time = (double)current_index / (double)SMPL;
            switch(phase){
                case 1:
                    // 閾値決定
                    current_index += 1;
                    break;
                case 2:
                    // 初期送信時刻決定
                    if (info->record_data[current_index] > threshold){
                        printf("初期送信時刻決定\n");
                        temperature = temp_measure(temperature);
                        v = sound_speed(temperature);
                        start_time = current_time - (initial_pos/v);
                        current_index = (int)(current_index + (EPS * SMPL));
                        phase = 3;
                    }else{
                        current_index += 1;
                    }
                    break;
                case 3:
                    // 位置推定処理
                    if (info->record_data[current_index] > threshold){
                        
                        received_num = (int)((current_time - start_time)/TAU);
                        propagation_time = current_time - start_time - TAU * received_num;
                        printf("propagation_time : %f\n",propagation_time);
                        printf("current_time : %lf\n",current_time);

                        temperature = temp_measure(temperature);
                        printf("温度：%lf\n",temperature);

                        v = sound_speed(temperature);
                        distance = propagation_time * v;
                        printf("推定距離: %lf {m}\n振幅: %d\n", distance, info->record_data[current_index]);
                        
                        distances[log_index] = distance;
                        received_time[log_index] = current_time;
                        log_index += 1;

                        current_index = (int)(current_index + (EPS * SMPL));
                    }else{
                        current_index += 1;
                    }
                    break;
                default:
                    printf("Error: Non-existent phase\n");
            }
        }
    }

    char filename[64];
    strcpy(filename,info->filename);
    strcat(filename, ".csv");
    write_result(filename, received_time, distances, log_index);

}
