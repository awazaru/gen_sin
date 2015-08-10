/*H27 7/31~
 *Sin波生成
 *Atmega328p使用
 *動作周波数8MHz
 */
#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include <avr/interrupt.h>
//#include <util/delay.h>

//#define PI 3.14
#define wave_f 1000 //出したい周波数 サンプリング周波数の半分程度まで
#define smp_f 8000//サンプリング周波数
#define num_data 20//sin_dataの要素数
/*関数宣言*/
void sin_gen(void);

/*変数宣言*/
float timer1_count = 0;/*16bitタイマ用カウント変数*/
uint16_t sin_count = 0;
uint8_t max_timer1_count = 19;// 分割数-1
uint8_t  half_timer1_count = 0;
uint8_t  half_ICR1 = 0;
float   speed = 0;//カウントアップの速度を調節 speed = (sin_dataの数)/分割数
float divide_wave = 0;//波形の分割数 divide_wave = (サンプリング周波数)/(出したい周波数)


float sinwave_data[]={//要素数20個
    0,0.342,0.643,0.866,
    0.985,0.985,0.866,0.643,0.342,
    0,-0.342,-0.643,-0.866,-0.985,
    -0.985,-0.866,-0.643,-0.342,0
};


/*関数宣言*/


/*高速PWMではTOP値がOCR0A、比較値がOCR0Bとなるので注意*/

void timer_ini(){//タイマー設定
    /*PWM*/
    TCCR1A |=_BV(COM1B1)|_BV(WGM11);
    /*位相基準PWM TOP値ICR1*/
    TCCR1B|=_BV(WGM13)|_BV(CS10);
    /*WGM13 WGM12 WGM11 WGM10: 1000 位相基準PWM動作 ICR1
     *CS12 CS11 CS10 : 001 分周なし*/

    ICR1 = 499;
    half_ICR1 = ICR1/2.0;
    TIMSK1|=_BV(ICIE1);/*タイマ/カウンタ1捕獲割り込み許可*/
    OCR1B = 0;
}

/*タイマ1 捕獲割り込み*/
ISR(TIMER1_CAPT_vect){
    if(timer1_count>max_timer1_count){
        timer1_count = 0;
    }
    timer1_count = timer1_count + 1 * speed;
    sin_count = timer1_count;
    OCR1B = half_ICR1+(half_ICR1*sinwave_data[sin_count]);
}

void pin_ini(){//ピン設定

    DDRB = 0b00000100;
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
