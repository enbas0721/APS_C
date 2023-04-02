#include <stdio.h>
#include <string.h>
#include <math.h>
#include "recordManager.h"
#include "trackManager.h"
#include "thermo.h"
#include <limits.h>

void write_result(char * filename, double * time, double * distances, double * ideal, int size){
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

    fprintf(fp, "Ideal Time,");
    for (n = 0; n < size-1; n++){
        fprintf(fp, "%lf,", ideal[n]);
    }
    fprintf(fp, "%lf\n", ideal[n+1]);
}
  
double sound_speed(double temperature){
    return (331.5 + (0.61 * temperature));
}

void make_chirp_wave(int th, int16_t* g){
    int n;
	double t;
    int vol = 3;
    int f0 = INIT_FREQ;
    int f1 = FINAL_FREQ;
    int size = SIGNAL_L*SMPL;
    int flag = 0;
    int16_t value;
	for (n = 0; n < size; n++)
	{
        t = (double)n/SMPL;
        value = (int)((vol*1000) * sin(2*M_PI * t * (f0 + ((f1-f0)/(2*SIGNAL_L))*t)));
        if (value >= th){
            flag = 1;
        }
        if (flag){
            g[n] = value;
        }
	}
}

void cross_correlation(double* fai, int16_t* data, int16_t* ideal_sig, int checking_index){
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
            fai[i] += ((data[first_index + j + tau] * ideal_sig[j])/10000);
        }
    }
}

int get_max_index(double* S, size_t size){
    int max_index, i;
    double max_value = 0.0;
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
    int phase = 1;
    int status = 1;
    int calibration_count = 0;
    double calibration_value = 0.1;

    double initial_pos = INIT_POS;
    
    int received_num = 0;
    int checking_index = 0;
    double current_time = 0.0;
    double start_time = 0.0;
    int start_sample = 0;
    double epsilon = 0.01;
    
    int threshold = 1000;

    double temperature = 20.0;
    double v = sound_speed(temperature);

    double propagation_time = 0.0;
    double distance = 0.0;

    int log_index = 0;
    double distances[10000];
    double received_time[10000];
    double ideal_received_time[10000];

    int16_t* ideal_signal;
    ideal_signal = (int16_t*)malloc(CRSS_WNDW_SIZ*sizeof(int16_t));
    make_chirp_wave(threshold,ideal_signal);
    double* cross_correlation_result;
    cross_correlation_result = calloc((CRSS_WNDW_SIZ), sizeof(double));

    int max_index;

    while((info->flag) || (checking_index < info->last_index))
    {
        if (info->last_index > checking_index){
            current_time = (double)checking_index / (double)SMPL;
            switch(phase){
                case 1:
                    // 信号受信判定
                    if (status)
                    {
                        printf("Waiting signal...\n");
                        status = 0;
                    }
                    if (info->record_data[checking_index] > threshold){
                        if (calibration_count > 2){
                            temperature = temp_measure(temperature);
                            v = sound_speed(temperature);
                            start_sample = checking_index - (SMPL*(double)(initial_pos/v));
                            start_time = current_time - (initial_pos/v);
                            // checking_index += (SMPL*1.2 - (checking_index - start_sample) - 10000);
                            checking_index += SMPL*1.2 - (checking_index - start_sample);
                            phase = 2;
                            status = 1;
                        }else{
                            calibration_count += 1;
                        }
                    }else{
                        calibration_count = 0;
                        checking_index += 1;
                    }
                    break;
                case 2:
                    if (status)
                    {
                        printf("Calibrating...\n");
                        status = 0;
                    }
                    cross_correlation(cross_correlation_result, info->record_data, ideal_signal, checking_index);
                    max_index = get_max_index(cross_correlation_result, CRSS_WNDW_SIZ);
                    propagation_time = (double)max_index/(double)SMPL;
                    temperature = temp_measure(temperature);
                    v = sound_speed(temperature);
                    distance = propagation_time * v;
                    double d = distance - initial_pos;
                    printf("キャリブレーション誤差: %lf {m}\n",d);
                    printf("--------------------\n");
                    if (d < (-1)*calibration_value){
                        // 変更
                        int cal_smpl = (d/v)*SMPL;
                        checking_index += (SMPL + cal_smpl);
                        calibration_count = 0;
                    }
                    else if (d > calibration_value)
                    {
                        int cal_smpl = (d/v)*SMPL;
                        checking_index += (SMPL + cal_smpl);
                        calibration_count = 0;
                    }else{
                        checking_index += SMPL;
                        calibration_count += 1;
                        printf("キャリブレーションカウント: %d\n",calibration_count);
                    }
                    if (calibration_count >= 5){
                        status = 1;
                        phase = 3;
                    }
                    distances[log_index] = distance;
                    received_time[log_index] = current_time - 1.2 + (double)max_index/SMPL;
                    ideal_received_time[log_index] = current_time - 1.2;
                    log_index += 1; 
                    break;
                case 3:
                    // 位置推定処理
                    // なんでや
                    if (status)
                    {
                        printf("Estimation started...\n");   
                        status = 0;
                    }
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
                    received_time[log_index] = current_time - 1.2 + (double)max_index/SMPL;
                    ideal_received_time[log_index] = current_time - 1.2;
                    log_index += 1; 
                    checking_index += SMPL;
                    break;
                default:
                    printf("Error: Non double * ideal, -existent phase\n");
            }
        }
    }
    
    char filename[64];
    strcpy(filename,info->filename);
    strcat(filename, ".csv");
    write_result(filename, received_time, distances, ideal_received_time, log_index-1);

    free(ideal_signal);
    free(cross_correlation_result);
}

