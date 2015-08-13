/*H27 7/31~
 *Sin波生成
 *Atmega328p使用
 *動作周波数20MHz(外部セラミック発振子,分周なし)
 */
#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#define wave_f 4000 //出したい周波数(Hz) 割り込み周波数の半分程度まで
#define smp_f 20000//割り込み周波数(Hz) ICRの設定値
#define num_data 256//sin_dataの要素数(個)

/*変数宣言*/
float timer1_count = 0;/*16bitタイマ用カウント変数*/
uint16_t cos_count = 0;
uint8_t max_timer1_count = num_data-1;// 分割数-1
uint8_t  half_timer1_count = 0;
uint8_t  bufICR1 = 0;
float   speed = 0;//カウントアップの速度を調節 speed = (sin_dataの数)/分割数
float divide_wave = 0;//波形の分割数 divide_wave = (サンプリング周波数)/(出したい周波数)

float coswave_data[]={//要素数256個
    1,1,1,1,1,1,0.99,0.99,0.99,0.99,0.99,0.98,0.98,0.97,0.97,0.97,0.96,0.96,0.95,0.95,0.94,0.94,0.93,0.92,0.92,0.91,0.9,0.89,0.89,0.88,0.87,0.86,0.85,0.84,0.84,0.83,0.82,0.81,0.8,0.79,0.78,0.77,0.76,0.75,0.74,0.72,0.71,0.7,0.69,0.68,0.67,0.66,0.65,0.63,0.62,0.61,0.6,0.59,0.57,0.56,0.55,0.54,0.52,0.51,0.5,0.49,0.48,0.46,0.45,0.44,0.43,0.41,0.4,0.39,0.38,0.37,0.35,0.34,0.33,0.32,0.31,0.3,0.29,0.28,0.26,0.25,0.24,0.23,0.22,0.21,0.2,0.19,0.18,0.17,0.16,0.16,0.15,0.14,0.13,0.12,0.11,0.11,0.1,0.09,0.08,0.08,0.07,0.06,0.06,0.05,0.05,0.04,0.04,0.03,0.03,0.03,0.02,0.02,0.01,0.01,0.01,0.01,0.01,0,0,0,0,0,0,0,0,0,0,0,0.01,0.01,0.01,0.01,0.01,0.02,0.02,0.03,0.03,0.03,0.04,0.04,0.05,0.05,0.06,0.06,0.07,0.08,0.08,0.09,0.1,0.11,0.11,0.12,0.13,0.14,0.15,0.16,0.16,0.17,0.18,0.19,0.2,0.21,0.22,0.23,0.24,0.25,0.26,0.28,0.29,0.3,0.31,0.32,0.33,0.34,0.35,0.37,0.38,0.39,0.4,0.41,0.43,0.44,0.45,0.46,0.48,0.49,0.5,0.51,0.52,0.54,0.55,0.56,0.57,0.59,0.6,0.61,0.62,0.63,0.65,0.66,0.67,0.68,0.69,0.7,0.71,0.72,0.74,0.75,0.76,0.77,0.78,0.79,0.8,0.81,0.82,0.83,0.84,0.84,0.85,0.86,0.87,0.88,0.89,0.89,0.9,0.91,0.92,0.92,0.93,0.94,0.94,0.95,0.95,0.96,0.96,0.97,0.97,0.97,0.98,0.98,0.99,0.99,0.99,0.99,0.99,1,1,1,1,1,1};

/*関数宣言*/
/*高速PWMではTOP値がOCRA、比較値がOCR0Bとなる*/

void timer_ini(){//タイマー設定
    /*PWM*/
    TCCR1A |=_BV(COM1B1)|_BV(WGM11);
    /*位相基準PWM TOP値ICR1*/
    TCCR1B|=_BV(WGM13)|_BV(CS10);
    /*WGM13 WGM12 WGM11 WGM10: 1000 位相基準PWM動作 ICR1
     *CS12 CS11 CS10 : 001 分周なし*/
    
    ICR1 = 499;//割り込み周波数 20000Hz時 499
    bufICR1 = ICR1;
    TIMSK1|=_BV(ICIE1);/*タイマ/カウンタ1捕獲割り込み許可*/
    OCR1B = 0;
}

/*タイマ1 捕獲割り込み*/
ISR(TIMER1_CAPT_vect){
    if(timer1_count>max_timer1_count){
        timer1_count = 0;
    }
    timer1_count = timer1_count + 1 * speed;/*sin_dataの要素を回すための速度を出したい周波数に合わせて調整 (float)*/
    cos_count = timer1_count;/*sin_count はint*/
    OCR1B = bufICR1 * coswave_data[cos_count];
}

void pin_ini(){//ピン設定
    
    DDRB = 0b00000100;//OCR1B
    PORTB = 0b00000000;
    
}


/*メイン関数*/
int main(void){
    
    timer_ini();
    pin_ini();
    
    half_timer1_count = max_timer1_count/2;
    divide_wave = smp_f/wave_f;//分割数決定
    speed = num_data/divide_wave;//カウンタスピード決定
    
    sei();//割り込み許可
    
    while(1){
    }
    return 0;
}
