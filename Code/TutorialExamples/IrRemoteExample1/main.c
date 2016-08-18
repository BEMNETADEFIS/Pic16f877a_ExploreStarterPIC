#include <pic16f877a.h>
#include "uart.h"
#include "stdutils.h"

 uint32_t bitPattern = 0, newChar = 0;
 uint16_t timeValue;
 uint8_t OverFlowFlag = 0;
 int bitCnt = 0;
 int flag =0;

void interrupt interrupt_isr() {
    if (TMR0IF == 1) {
        if(OverFlowFlag<50)
        OverFlowFlag++;
        TMR0IF = 0; // Clear timer interrupt flag
        TMR0 = 236;
    }
    else if (INTF == 1) {
        INTF = 0;
        timeValue = OverFlowFlag;
        OverFlowFlag = 0;
        TMR0 = 236;

        bitCnt++;

        if ((timeValue >= 50)) 
        {
            bitCnt = -2;
            bitPattern = 0;
        } 
        else if ((bitCnt >= 0) && (bitCnt < 32)) 
        {

            if (timeValue >= 2) 
            {
                bitPattern |= (uint32_t) 1 << (31 - bitCnt);
            } 
            else 
            {

            }
        } 
        else if (bitCnt >= 32) 
        {
            newChar = bitPattern;
            bitPattern=0;
            bitCnt = 0;
            flag = 1;
        }
    }
}



int main() {
    int oldCnt=0;
    TRISB = 0x01;
    PORTB = 0x01;

    UART_Init(9600);

    OPTION_REG = 0x07;  // Timer0 with external freq and 32 as prescalar
    TMR0 = 236;;       // Load the time value for 1ms delay
    TMR0IE=1;       //Enable timer interrupt bit in PIE1 register
    INTE=1;       //Enable External interrupt bit in INTCON register
    GIE = 1; //Enable Global Interrupt
    PEIE = 1; //Enable the Peripheral Interrupt
    TMR1ON = 1; //Start Timer1    

    UART_TxString("Init Done");
    while (1) {                
        if (newChar != 0) {
            UART_Printf("\n\rval:%X", newChar);
            newChar = 0;
        }
    }
}


/* NEC IR Remote Codes
0xFFA25D: CH-
0xFF629D: CH
0xFFE21D: CH+
0xFF22DD: PREV
0xFF02FD: NEXT
0xFFC23D: PLAY/PAUSE
0xFFE01F: VOL-
0xFFA857: VOL+
0xFF906F: EQ
0xFF6897: 0
0xFF9867: 100+
0xFFB04F: 200+
0xFF30CF: 1
0xFF18E7: 2
0xFF7A85: 3
0xFF10EF: 4
0xFF38C7: 5
0xFF5AA5: 6
0xFF42BD: 7
0xFF4AB5: 8
0xFF52AD: 9
 */