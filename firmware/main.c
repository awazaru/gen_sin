/*H27 7/31~
 *Sin波生成
 *Atmega328p使用
 */
#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include <avr/interrupt.h>
//#include <util/delay.h>

//#define PI 3.14

/*関数宣言*/
void sin_gen(void);

/*変数宣言*/
uint8_t timer1_count = 0;/*16bitタイマ用カウント変数*/
uint8_t max_timer1_count = 255;//1KHz
uint8_t  half_timer1_count = 0;
//uint8_t OCR0B_up  = 1;/*OCR0Aを増加させたいときは1を減少させたいときは0を入れる*/
//double   rad = 0;//deg * PI / 180.0;
//float sinwave = 0;
//float abs_sin =0;

const uint8_t sinwave_data[]={
    50,51,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,
    68,68,69,70,71,72,73,73,74,75,76,76,77,78,78,79,80,80,81,
    82,82,83,83,84,84,85,85,86,86,86,87,87,87,88,88,88,89,89,
    89,89,89,89,89,89,90,90,90,89,89,89,89,89,89,89,89,88,88,
    88,88,87,87,86,86,86,85,85,84,84,83,83,82,82,81,80,80,79,
    79,78,77,76,76,75,74,73,73,72,71,70,69,68,68,67,66,65,64,
    63,62,61,60,59,58,57,56,55,54,53,52,52,51,50,49,48,47,46,
    45,44,43,42,41,40,39,38,37,36,35,34,33,32,32,31,30,29,28,
    27,27,26,25,24,23,23,22,21,21,20,19,19,18,17,17,16,16,15,
    15,14,14,13,13,13,12,12,12,11,11,11,10,10,10,10,10,10,10,
    10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,12,12,12,13,
    13,13,14,14,15,15,16,16,17,17,18,19,19,20,21,21,22,23,23,
    24,25,26,26,27,28,29,30,31,31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,48,49
};


/*関数宣言*/
/*void usart_tx(unsigned char data){//送信用関数
 while( !(UCSR0A & (1<<UDRE0)) );        //送信ﾊﾞｯﾌｧ空き待機
 UDR0 = data;
 }
 unsigned char usart_rx(void){//受信用関数
 while( !(UCSR0A & (1<<RXC0)) );                //受信完了待機
 return UDR0;                                //受信ﾃﾞｰﾀ返す
 }
 void puts_tx(char *str){//文字列送信用関数
 while( *str != 0x00 ){ //nullになるまで実行
 usart_tx(*str);
 str++;                                    //ｱﾄﾞﾚｽ+1
 }
 }
 void serial_ini(){// シリアル通信設定
 UBRR0 = MYUBRR;
 UCSR0A=0b00000000;//受信すると10000000 送信有効になると00100000
 UCSR0B|=0b00011000;//送受信有効
 UCSR0C=0b00000110;//データ8bit、非同期、バリティなし、stop1bit
 }
 */

/*高速PWMではTOP値がOCR0A、比較値がOCR0Bとなるので注意*/

void timer_ini(){//タイマー設定
    /*単位タイマ　 100us*/
    TCCR1A = 0;
    TCCR1B|=_BV(WGM13)|_BV(WGM12)|_BV(CS00);
    /*WGM13 WGM12 WGM11 WGM10: 1100 比較一致タイマ/CTC動作 ICR1
     *CS12 CS11 CS10 : 001 分周なし*/
    ICR1 = 99;/*0.0001s*/
    TIMSK1|=_BV(ICIE1);/*タイマ/カウンタ1比較A割り込み許可*/
    
    /*8bitタイマ1 PWM用*/
    TCCR0A|=_BV(COM0B1)|_BV(WGM01)|_BV(WGM00);
    /*COM0B1 COM0B0 : 10 比較一致でLOW,OC0Bピンに出力
     *(WGM02) WGM01 WGM00 : 111 高速PWM動作,TOPはOCR0A
     */
    TCCR0B|=_BV(WGM02)|_BV(CS00);
    /*CS02 CS01 CS00:001 クロック源=CLKi/o 前置分周なし
     */
    //TIMSK0|=_BV(TOIE0);
    /* TOIE0 :1     タイマ/カウンタ0溢れ割り込み許可
     */
    OCR0A = 99;//TOP
    /*10000HzのPWM周波数, 0.0001s
     */
    OCR0B = 50 ;//比較
}

/*単位タイマ
 *10usで1回カウント
 */

ISR (TIMER1_CAPT_vect){
    if(timer1_count>max_timer1_count)
        timer1_count = 0;
    timer1_count++;
    //sin_gen();
    //OCR0B=50 + 40*(sinwave_data[timer1_count]*0.001);
    
}

void pin_ini(){//ピン設定
    DDRD = 0b01100000;//PD6(OCR0A)を出力設定
    PORTD =0b00000000;
}

/*void sin_gen(void){
 //half_timer1_count = max_timer1_count*0.5;
 if(timer1_count<=half_timer1_count){
 rad =  (timer1_count*180.0/half_timer1_count) * PI / 180.0;
 sinwave = sin(rad);
 //abs_sin = abs(sinwave);
 OCR0B = 50 + 40 * sinwave;
 }else if (timer1_count>half_timer1_count){
 rad =  ((timer1_count-half_timer1_count)*180.0/half_timer1_count) * PI / 180.0;
 sinwave = sin(rad);
 //abs_sin = abs(sinwave);
 OCR0B = 50 - 40 * sinwave;
 }
 }*/


/*メイン関数*/
int main(void){
    
    timer_ini();
    pin_ini();
    
    //half_timer1_count = max_timer1_count/2.0;
    sei();//割り込み許可
    
    for(;;){
        OCR0B = sinwave_data[timer1_count];
        //  sin_gen();
    }
    return 0;
}
