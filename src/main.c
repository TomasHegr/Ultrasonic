#include "main.h"
#include "delay.h"
#include "milis.h"
#include "uart1.h"
#include <stdbool.h>
#include <stdio.h>
#include <stm8s.h>

// Discovery Board
#ifdef STM8S003
#define LED_PORT GPIOD
#define LED_PIN GPIO_PIN_0
#define BTN_PORT GPIOB
#define BTN_PIN GPIO_PIN_7
#endif
// Blue-Board, Dero-Board
#ifdef STM8S103
//   Dero-Board
/*#define LED_PORT GPIOD*/
/*#define LED_PIN  GPIO_PIN_4*/
//   Blue-Board
#define LED_PORT GPIOB
#define LED_PIN GPIO_PIN_5
#endif
// Discovery Board
#ifdef STM8S105
#define LED_PORT GPIOD
#define LED_PIN GPIO_PIN_0
#endif
// Nucleo Kit
#ifdef STM8S208
#define LED_PORT GPIOC
#define LED_PIN GPIO_PIN_5
#define BTN_PORT GPIOE
#define BTN_PIN GPIO_PIN_4
#endif


#define TRIGG_PORT GPIOB
#define TRIGG_PIN GPIO_PIN_6

#define ECHO_PORT GPIOD
#define ECHO_PIN GPIO_PIN_4

void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz
    init_milis();
    init_uart1();

    GPIO_Init(TRIGG_PORT, TRIGG_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    GPIO_Init(ECHO_PORT, ECHO_PIN, GPIO_MODE_IN_FL_NO_IT);

    // TIMER NECHÁME BĚŽET RYCHLOSTÍ 1 TIK ZA 1 us
    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 0xFFFF);
    TIM2_Cmd(ENABLE);
}
uint16_t getEchotime(void)
{
    uint16_t time1_rise;
    uint16_t time2_fall;
    uint32_t time;

    time = milis();

    // trigger pulse
    HIGH(TRIGG);
    delay_us(12);
    LOW(TRIGG);

    // čekám na náběžnou hranu
    while (READ(ECHO) == 0) {
        if (milis() - time > 64) {
            return 0;
        }
    };
    time1_rise = TIM2_GetCounter();


    // čekám na sestupnou hranu

    while (READ(ECHO) == 1) {
        if (milis() - time > 64) {
            return 1;
        }
    };
    time2_fall = TIM2_GetCounter();

    return time2_fall - time1_rise;
}

int main(void)
{

    uint32_t time = 0;
    uint16_t echotime;
    uint16_t range;

    init();

    while (1) {
        if (milis() - time > 1000) {
            time = milis();
            echotime = getEchotime();
            printf("echotime: %u us   ", echotime);
            range = (uint16_t)((uint32_t)343*echotime/2000);
            printf("range = %u mm\n",range);
            
        }
        // delay_ms(333);
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
