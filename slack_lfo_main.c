/* 
 * File:   slack_lfo_main.c
 * Author: Ian Maltby
 *
 * Created on October 29, 2014, 5:39 PM
 *                 __________
 *          VDD ---|         |--- VSS
 *              ---| 12F1822 |--- Depth pot
 *    Speed pot ---|         |--- Shape pot
 *              ---|_________|--- PWM Output
 */

#include <xc.h>

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

#define _XTAL_FREQ 32000000  //processor speed for delay macro
#define depth_pot pots[0]
#define shape_pot pots[1]
#define speed_pot pots[2]
#define test RA5

volatile const signed int sine[256] = {0, 6, 13, 19, 25, 31, 38, 44, 50, 56, 63, 69, 75, 81, 87, 94, 100, 106, 112, 118, 124, 130, 136, 142, 148, 154, 160, 166, 172, 178, 184, 190, 196, 201, 207, 213, 219, 224, 230, 235, 241, 246, 252, 257, 263, 268, 273, 279, 284, 289, 294, 299, 304, 309, 314, 319, 324, 329, 334, 339, 343, 348, 352, 357, 361, 366, 370, 374, 379, 383, 387, 391, 395, 399, 403, 407, 411, 414, 418, 421, 425, 428, 432, 435, 438, 442, 445, 448, 451, 454, 457, 459, 462, 465, 467, 470, 472, 475, 477, 479, 481, 483, 485, 487, 489, 491, 493, 494, 496, 497, 499, 500, 501, 502, 504, 505, 505, 506, 507, 508, 509, 509, 510, 510, 510, 511, 511, 511, 511, 511, 511, 511, 510, 510, 510, 509, 509, 508, 507, 506, 505, 504, 503, 502, 501, 500, 499, 497, 496, 494, 492, 491, 489, 487, 485, 483, 481, 479, 477, 474, 472, 470, 467, 464, 462, 459, 456, 453, 451, 448, 444, 441, 438, 435, 432, 428, 425, 421, 418, 414, 410, 406, 403, 399, 395, 391, 387, 383, 378, 374, 370, 366, 361, 357, 352, 348, 343, 338, 334, 329, 324, 319, 314, 309, 304, 299, 294, 289, 284, 278, 273, 268, 262, 257, 252, 246, 241, 235, 229, 224, 218, 212, 207, 201, 195, 189, 184, 178, 172, 166, 160, 154, 148, 142, 136, 130, 124, 118, 112, 105, 99, 93, 87, 81, 75, 68, 62, 56, 50, 43, 37, 31, 25, 18, 12, 6};
volatile const unsigned int speed[256] = {32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 47, 48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 65, 66, 67, 69, 70, 72, 73, 75, 76, 78, 80, 82, 83, 85, 87, 89, 91, 93, 95, 98, 100, 102, 105, 107, 110, 113, 115, 118, 121, 124, 127, 131, 134, 137, 141, 145, 148, 152, 156, 160, 164, 169, 173, 178, 183, 187, 192, 198, 203, 209, 214, 220, 226, 232, 239, 245, 252, 259, 266, 274, 282, 290, 298, 306, 315, 324, 333, 342, 352, 362, 373, 383, 394, 406, 417, 429, 442, 454, 468, 481, 495, 510, 524, 540, 556, 572, 589, 606, 624, 642, 661, 680, 700, 721, 742, 764, 787, 810, 834, 859, 884, 911, 938, 966, 994, 1024, 1055, 1086, 1118, 1152, 1186, 1222, 1258, 1296, 1335, 1375, 1416, 1458, 1502, 1547, 1594, 1641, 1691, 1742, 1794, 1848, 1904, 1961, 2020, 2081, 2144, 2208, 2275, 2343, 2414, 2487, 2562, 2639, 2719, 2801, 2886, 2973, 3063, 3156, 3251, 3349, 3451, 3555, 3663, 3774, 3888, 4006, 4127, 4252, 4381, 4514, 4650, 4791, 4937, 5086, 5240, 5399, 5563, 5732, 5906, 6085, 6270, 6460, 6656, 6858, 7066, 7281, 7502, 7730, 7964, 8206, 8456, 8712, 8977, 9250, 9531, 9820, 10119, 10426, 10743, 11070, 11406, 11753, 12110, 12478, 12858, 13249, 13651, 14066, 14494, 14935, 15389, 15857, 16339, 16836, 17348, 17876, 18420, 18980, 19558, 20152, 20766, 21397, 22048};
volatile unsigned char pots[3] = {0, 0, 0};
volatile unsigned char shape = 1;

void main() {

    OSCCONbits.IRCF = 14; //8Mhz clock * 4x PLL = 32Mhz
    CM1CON0bits.C1ON = 0; //disable comparator
    CCP1CONbits.CCP1M = 12; //PWM mode active high
    T2CONbits.T2CKPS = 0; //Timer 2 prescaler 1
    PR2 = 255; //Timer period ~31Khz PWM freq
    T2CONbits.TMR2ON = 1; //Timer 2 On
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.T0SE = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0;
    INTCONbits.T0IF = 0;
    INTCONbits.T0IE = 1;
    TMR0 = 0;

    TRISAbits.TRISA2 = 0; //RA2 (pin 5) output PWM out
    TRISAbits.TRISA5 = 0; //RA5 (pin 2) output test
    TRISAbits.TRISA0 = 1; //RA0 (pin 7) input
    TRISAbits.TRISA1 = 1; //RA1 (pin 6) input
    TRISAbits.TRISA4 = 1; //RA4 (pin 3) input
    ANSELAbits.ANSA0 = 1; //AN0 (pin 7) analogue input
    ANSELAbits.ANSA1 = 1; //AN1 (pin 6) analogue input
    ANSELAbits.ANSA4 = 1; //AN3 (pin 3) analogue input
    WPUAbits.WPUA3 = 1; //enable weak pullup on RA3
    ADCON1bits.ADCS = 3; //ADC clock FRC
    ADCON1bits.ADFM = 0; //ADC Left justified
    ADCON0bits.ADON = 1; //ADC on

    test = 0;
    const unsigned char adc_channel[3] = {0, 1, 3};
    unsigned char count = 0;

    for (;;) {
        // test = 0;
        count++;
        if (count == 3)
            count = 0;

        ADCON0bits.CHS = adc_channel[count];
        __delay_ms(20);

        GO_nDONE = 1;
        while (GO_nDONE); //Do ADC
        pots[count] = ADRESH;

        if (shape_pot >> 5 == (shape_pot + 4) >> 5)
            shape = (shape_pot >> 5) + 1;
        //  test = 1;
    }


}
//ISR runs every 64us

void interrupt isr(void) {

    INTCONbits.T0IF = 0;
   // test = 1;
    static unsigned int phase_accum = 0;
    static unsigned int pos = 0;
    signed int output = 0;

    phase_accum = phase_accum + speed[speed_pot];
    if (phase_accum > 32767) {
        phase_accum = phase_accum & 32767;
        pos = (pos + 1) & 511;

        output = sine[pos & 255] * shape;

        if (output > 511)
            output = 511;

        if (pos > 255)
            output = ~output;

        output = (output * (depth_pot >> 2) >> 6) + (depth_pot << 1) + 1;
        
        CCPR1L = output >> 2;
        CCP1CONbits.DC1B = output;

    }
   // test = 0;
}
