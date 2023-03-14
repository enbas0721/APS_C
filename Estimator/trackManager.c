#include <stdio.h>
#include <string.h>
#include <math.h>
#include "recordManager.h"
#include "trackManager.h"
#include "thermo.h"
#include <limits.h>

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

void make_chirp_wave(int16_t* g){
    int n;
	double t;
    int vol = 3;
    int f0 = INIT_FREQ;
    int f1 = FINAL_FREQ;
    int size = SIGNAL_L*SMPL;
	for (n = 0; n < size; n++)
	{
        t = (double)n/SMPL;
        g[n] = (int)((vol*1000) * sin(2*M_PI * t * (f0 + ((f1-f0)/(2*SIGNAL_L))*t)));
	}
}

void cross_correlation(long int* fai, int16_t* data, int16_t* ideal_sig, int checking_index){
    int i, j, tau;
    int first_index = checking_index - CRSS_WNDW_SIZ*2;
    for (i = 0; i < CRSS_WNDW_SIZ; i++)
    {
        fai[i] = 0;
    }
    for (i = 0; i < CRSS_WNDW_SIZ; i++)
    {
        tau = i;
        for (j = 0; j < CRSS_WNDW_SIZ; j++)
        {   
            fai[i] += (data[first_index + j + tau] * ideal_sig[j]);
        }
    }
}

int get_max_index(long int* S, size_t size){
    int max_index, i;
    long int max_value = 0;
    for (i = 0; i < size; i++)
    {
        if(S[i] > max_value){
            max_value = S[i];
            max_index = i;
        }
    }
    return max_index;
}

void* track_start(record_info *info)
{
    int phase = 2;

    double initial_pos = 0.10;
    
    int received_num = 0;
    int checking_index = 0;
    double current_time = 0.0;
    double start_time = 0.0;
    int start_sample = 0;
    
    int threshold = 1000;

    double temperature = 20.0;
    double v = sound_speed(temperature);

    double propagation_time = 0.0;
    double distance = 0.0;

    int log_index = 0;
    double distances[10000];
    double received_time[10000];

    int16_t* ideal_signal;
    ideal_signal = (int16_t*)malloc(CRSS_WNDW_SIZ*sizeof(int16_t));
    make_chirp_wave(ideal_signal);
    long int* cross_correlation_result;
    cross_correlation_result = calloc((CRSS_WNDW_SIZ), sizeof(long int));

    int max_index;

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
                        start_sample = checking_index - (SMPL*(double)(initial_pos/v));
                        start_time = current_time - (initial_pos/v);
                        printf("初期受信サンプル : %d\n", checking_index);
                        printf("初期送信サンプル : %d\n", start_sample);
                        printf("1.2秒先のindex: %d\n", (checking_index - start_sample));
                        checking_index += (SMPL*1.2 - (checking_index - start_sample) - 100);
                        phase = 3;
                    }else{
                        checking_index += 1;
                    }
                    break;
                case 3:
                    // 位置推定処理
                    cross_correlation(cross_correlation_result, info->record_data, ideal_signal, checking_index);
                    max_index = get_max_index(cross_correlation_result, CRSS_WNDW_SIZ);
                    propagation_time = (double)max_index/(double)SMPL;
                    temperature = temp_measure(temperature);
                    v = sound_speed(temperature);
                    distance = propagation_time * v;
                    printf("伝播時間 %lf {s}\n",propagation_time);
                    printf("推定距離: %lf {m}\n", distance);
                    printf("--------------------\n");
                    
                    distances[log_index] = distance;
                    received_time[log_index] = current_time + (double)max_index/SMPL;
                    log_index += 1;
                    checking_index += SMPL;
                    break;
                default:
                    printf("Error: Non-existent phase\n");
            }
        }
    }
    
    char filename[64];
    strcpy(filename,info->filename);
    strcat(filename, ".csv");
    write_result(filename, received_time, distances, log_index-1);

    free(ideal_signal);
    free(cross_correlation_result);
}
