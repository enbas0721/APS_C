#include <stdio.h>
#include <time.h>
#include <wiringPi.h>
#include <math.h>
 
//AM2320 parameter
#define AM2320_DATAPIN 18
#define AM2320_START_SIG_LOW 1
#define AM2320_START_PullUp 20
#define AM2320_WAITRES_LOW 75
#define AM2320_WAITRES_HIGH 75
#define AM2320_DATAWAIT_LOW 48
#define AM2320_BITZERO 22
#define AM2320_BITONE 68
#define AM2320_END 45
 
int main()
{
 
    //START
    //SetUp
    int AM2320_DATA[40]= {0}; 
    wiringPiSetupGpio();
    pinMode(AM2320_DATAPIN , OUTPUT);
     
    digitalWrite(AM2320_DATAPIN , HIGH);
    delay(2000);
     
    //START OUTPUT LOW >1ms
    digitalWrite(AM2320_DATAPIN , LOW);
    delay(AM2320_START_SIG_LOW);
 
    //START PUll up
    digitalWrite(AM2320_DATAPIN , HIGH);
    delayMicroseconds(AM2320_START_PullUp);
     
    //Response LOW SIG
    pinMode(AM2320_DATAPIN , INPUT);
    for ( int i = 0 ; i < AM2320_WAITRES_LOW ; i++ ){
        if (digitalRead(AM2320_DATAPIN) == HIGH){
            break;
        }
        delayMicroseconds(1);   
    }
     
    //Response HIGH SIG
    delayMicroseconds(AM2320_WAITRES_HIGH);
    for ( int i = 0 ; i < AM2320_WAITRES_HIGH ; i++ ){
        if (digitalRead(AM2320_DATAPIN) == LOW){
            break;
        }
        delayMicroseconds(1);
    }
     
    //DATA Recieve
    for ( int i = 0 ; i < 40 ; i++){
        //start bit wait
        delayMicroseconds(AM2320_DATAWAIT_LOW);
        for ( int j = 0 ; j < AM2320_DATAWAIT_LOW ; j++){
            if(digitalRead(AM2320_DATAPIN) == HIGH){
                break;
            }
            delayMicroseconds(1);
        }
        for ( int j = 0 ; j < 255 ; j++){
            if (digitalRead(AM2320_DATAPIN) == LOW){
                if ( j  < AM2320_BITONE ){
                    AM2320_DATA[i] = 0;
                } else {
                    AM2320_DATA[i] = 1;
                }
                break;
            }
            delayMicroseconds(1);
        }
    }
     
    //End process
    delayMicroseconds(AM2320_END);
    digitalWrite(AM2320_DATAPIN , HIGH);
     
    //data check
    int dec[4] = {1,2,4,8};
    float dt[10] = {0};
    int hex[4] = {1,16,256,1536};
    for ( int i = 0 , j = 3 ,k = 0 , l = 3 ; i < 40 ; i++ , j--){
        if (AM2320_DATA[i] == 1 ){
            dt[k] += dec[j];
        }
        if ( j == 0 ){
            dt[k] = dt[k] * hex[l];
            l -= 1;
            if ( l < 0){
                l = 3 ;
            }
            j = 4;
            k += 1;
        }
    }
    float hum = (dt[0] + dt[1] + dt[2] + dt[3]) / 10 ;
    float tem = (dt[5] + dt[6] + dt[7]) / 10 ; 
     
    if (dt[4] == 1536 ){
        tem = tem * -1   ;
    }
     
    printf( "temperature %d.%d  humdity %d.%d  \n" , tem , hum );
}