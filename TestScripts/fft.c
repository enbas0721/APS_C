#include <stdio.h>
#include <math.h>

#define swap(a,b) do{int t=a; a=b; b=t;}while(0)
#define NAME 64

typedef struct complex{
    double r;
    double i;
}complex;

void cre_w(int N,complex w[]){
    for(int i=0; i<N; i++){
        w[i].r = cos(2*i*M_PI/N);
        w[i].i = sin((-2*i*M_PI)/N);
    }
}

void cal(complex a, complex b, complex *y){
    y->r = a.r*b.r - a.i*b.i;
    y->i = a.r*b.r + a.i*b.i;
}

int ff(int i){
    int r=0;
    for(; i>1;){
        i /= 2;
        r++;
    }
    return r;
}

int power2(int i){
    int r=1;
    for(int l=0; l<i; l++)
        r *= 2;
    return r;
}

void change2(int N, int k, int bit[ff(N)]){//kを2進数に変換して配列に格納
    for(int j=0; j<k; j++){
        for(int l=0; l<ff(N); l++){
            bit[l]++;
            if(bit[l]<2)//筆算の容量で位上げを実装
                break;
            else
                bit[l]=0;
        }
    }
}

int change10(int N, int bit[ff(N)]){//2進数の配列を10進数に戻す
    int r=0;
    for(int i=0; i<ff(N); i++){
        r += bit[i] * power2(i);
    }
    return r;
}

void bit_revers(int N, int bit[ff(N)]){//2進数の配列をビットリバースする 配列の長さはff(N)
    
    for(int i=0; i<(int)(ff(N)/2); i++)
        swap(bit[i], bit[ff(N)-1-i]);
}

int rev(int N, int k){//10進数の数字を渡すとその数字をビットリバースした数字を返す
    int bit[ff(N)];
    
    for(int i=0; i<ff(N); i++)
        bit[i] = 0;
    
    change2(N,k,bit);
    bit_revers(N, bit);
    
    return change10(N, bit);
}

void clear_com(int N, complex a[N]){//complexの中身をすべて0にする
    for(int i=0; i<N; i++)
        a[i].r = a[i].i = 0;
}

void FFT(int N, complex x[N], complex X[N]){//FFT xには最初は信号を渡す、Xには求める変換が返ってくる
    complex w[N];
    clear_com(N, w);
    cre_w(N, w);//N個の1のN乗根を生成
    
    complex y[N];
    clear_com(N, y);//今回のループの計算結果を格納し、次のループに渡す用の配列
    
    complex X1[N/2];
    complex X2[N/2];//次のループに渡す用の配列
    clear_com(N/2, X1);
    clear_com(N/2, X2);
    
    if(N != 2){//Nが2ではないとき、つまりまだループを繰り返す場合
        int i;
        for(i=0; i<N/2; i++){
            y[i].r = x[i].r + x[N/2+i].r;
            y[i].i = x[i].i + x[N/2+i].i;//バタフライ演算の上の部分
        }
        
        FFT(N/2, y, X1);//上半分の計算結果を次のFFTループに渡す
        
        complex k;
        k.r=0; k.i=0;
        
        for(i=0; i<N/2; i++){
            k.r = x[i].r - x[N/2+i].r;
            k.i = x[i].i - x[N/2+i].i;
            cal(w[i], k, &y[i]);//バタフライ演算の下の部分を計算しyに格納
            
        }
         FFT(N/2, y, X2);//下部分も計算結果を次のFFTループに渡す
         
        for(int j=0; j<N/2; j++){//返ってきたX1,X2をXに順に格納する
            X[j] = X1[j];
            X[N/2+j] = X2[j];
        }
        
    }else{//N=2のとき、つまりループの末端の場合
        
        X[0].r = x[0].r + x[1].r;
        X[0].i = x[0].i + x[1].i;
        X[1].r = x[0].r - x[1].r;
        X[1].i = x[0].i - x[1].i;//バタフライ演算をして１つ上のループにXとして返す
      
    }
}

void SCAN(int N, complex x[N],char fn[NAME]){ //読み込み用関数
    FILE *fp;
    int i;
    if((fp=fopen(fn,"r"))==NULL){
        printf("ファイルを読み込めません\n");
    }else{
        for(i=0; i<N; i++){
            fscanf(fp,"%lf",&x[i].r);
        }
    }
    fclose(fp);
}

void WRITE(int N, complex x[N],char fn[NAME]){ //書き込み用関数
    FILE *fp;
    int i;
    
    if((fp=fopen(fn,"w"))==NULL)
        printf("ファイルに書き込めません\n");
    else{
        for(i=0; i<N; i++){
            fprintf(fp,"%lf %lf\n",x[i].r,x[i].i);
        }
    }
    fclose(fp);
}

int main(int argc, const char * argv[]) {
  
    int N=256;//ここはフーリ変換にかける信号長の長さを自分で設定する
    char fn[NAME];
    complex x[N];//信号を格納
    complex X[N];//関数FFTで最終的に求める変換を格納する配列
    complex X_t[N];//ビットリバースした数字に対応させる配列
    
    clear_com(N, x);
    clear_com(N, X);
    clear_com(N, X_t);
    
    printf("どのファイルから読み込みますか?");
    scanf("%s",fn);
    SCAN(N, x, fn);

    for (int i = 0; i < 100; i++)
    {
        printf("%lf\n",x[i].r);
    }
    
    FFT(N, x, X);
    printf("hhhh\n");
     printf("%f %fa\n%f %f\n",X[2].r,X[2].i,X[3].r,X[3].i);
    for(int i=0; i<N; i++){//X_tにXの結果を入れる
        X_t[rev(N, i)] = X[i];
    }
    printf("hhhh\n");
    printf("%f %fa\n%f %f\n",X_t[2].r,X_t[2].i,X_t[3].r,X_t[3].i);
    printf("FFTが完了しました\n");
    
    printf("どのファイルに書き込みますか?");
    scanf("%s",fn);
    WRITE(N, X_t, fn);
    
    return 0;
}