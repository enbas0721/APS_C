#include <stdio.h>
#include <string.h>
#include "recordManager.h"
#include "trackManager.h"
#include "thermo.h"

void write_result(char * filename, double * time, double * distances, int size){
    FILE *fp;
    fp = fopen(filename, "w");
    int n;
    fprintf(fp, "Time,");
    for (n = 0; n < size-1; n++){
        fprintf(fp, "%lf,", time[n]);
    }
    fprintf(fp, "%lf\n", time[n+1]);

    fprintf(fp, "Distance,");
    for (int n = 0; n < size-1; n++){
        fprintf(fp, "%lf,", distances[n]);
    }
    fprintf(fp, "%lf\n", distances[n+1]);
}

double sound_speed(double temperature){
    return (331.5 + (0.61 * temperature));
}

void* track_start(record_info *info)
{
    int phase = 2;

    double initial_pos = 1.0;
    
    int received_num = 0;
    int checking_index = 0;
    double current_time = 0.0;
    double start_time = 0.0;
    
    int threshold = 1000;

    double temperature = 20.0;
    double v = sound_speed(temperature);

    double propagation_time = 0.0;
    double distance = 0.0;

    int log_index = 0;
    double distances[10000];
    double received_time[10000];

    while((info->flag) || (checking_index < info->last_index))
    {
        if (info->last_index > checking_index){
            current_time = (double)checking_index / (double)SMPL;
            switch(phase){
                case 1:
                    // 閾値決定
                    checking_index += 1;
                    break;
                case 2:
                    // 初期送信時刻決定
                    if (info->record_data[checking_index] > threshold){
                        temperature = temp_measure(temperature);
                        v = sound_speed(temperature);
                        start_time = current_time - (initial_pos/v);
                        printf("初期送信時刻 : %lf\n", start_time);
                        checking_index = (int)(checking_index + (EPS * SMPL));
                        phase = 3;
                    }else{
                        checking_index += 1;
                    }
                    break;
                case 3:
                    // 位置推定処理
                    if (info->record_data[checking_index] > threshold){
                        
                        received_num = (int)((current_time - start_time)/TAU);
                        propagation_time = current_time - start_time - TAU * received_num;

                        temperature = temp_measure(temperature);

                        v = sound_speed(temperature);
                        printf("音速: %lf {m}\n", v);
                        distance = propagation_time * v;
                        printf("受信時刻: %lf {m}\n", current_time);
                        printf("推定距離: %lf {m}\n振幅: %d\n", distance, info->record_data[checking_index]);
                        
                        distances[log_index] = distance;
                        received_time[log_index] = current_time;
                        log_index += 1;

                        checking_index = (int)(checking_index + (EPS * SMPL));
                    }else{
                        checking_index += 1;
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
