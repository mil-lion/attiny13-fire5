// !!! 1,2GHz, No Millis, No Tone !!!!

//#define F_CPU 9.6E6L /* CPU Freq. Must come before delay.h include. 9.6MHz */
#define F_CPU 1200000L /* CPU Freq. Must come before delay.h include. 1.2MHz */

//#include <avr/io.h>
//#include <avr/interrupt.h>
#include <util/delay.h>

#define CH1_PIN PB0 /* Bind output channels to specific PortB pins. This depends on schematic. */
#define CH2_PIN PB1
#define CH3_PIN PB2
#define CH4_PIN PB3
#define CH5_PIN PB4

#define LIGHT     200
#define LIGHT_MIN  56
#define DELAY_MS   10

uint8_t ch1, ch2, ch3, ch4, ch5; /* The one-byte PWM level of each channel */
/*
This one-byte flag holds five channel direction bit flags
0x [nothing] [nothing] [nothing] [ch5] [ch4] [ch3] [ch2] [ch1]
*/

volatile uint8_t ISRcounter = 0; /* Count the number of times the ISR has run */
//volatile uint32_t delayCount = 0;

//#define TIMER_COUNT ((F_CPU / 256) / 30)            /* 94 = PWM 50Hz, 156 = PWM 30Hz */
#define TIMER_COUNT 100                             /* 100 = PWM 46,87Hz */
#define ticksPerMs  ((F_CPU / TIMER_COUNT) / 1000)  /* 12 = PWM 50Hz */

void setup() {
//  cli(); // disable interrupts
  
  DDRB  = 0xFF; // Every PORTB pin is output
  PORTB = 0x00; // Start with every pin low

  TCCR0A |= (1 << WGM01);  // Режим таймера по достижению значения OCR0A (CTC mode)
  TCCR0B =  (1 << CS00);  // disable timer prescale (=clock rate)
//  TIMSK0 |= (1 << TOIE0); // enable timer overflow interrupt specifically 
  TIMSK0 |= (1 << OCIE0B); // enable iterrupt 
  OCR0A = TIMER_COUNT; // 1200000 / 94 ticks / 256 = 50Hz (94)
  OCR0B = 0;

  sei();  // enable interrupts in general

  /* test
  ch1 = 0;
  ch2 = 63;
  ch3 = 127;
  ch4 = 191;
  ch5 = 255;
  */
}

void loop() {
//  if (delayCount == 0) {
    ch1 = random(LIGHT) + LIGHT_MIN;
    ch2 = random(LIGHT) + LIGHT_MIN;
    ch3 = random(LIGHT) + LIGHT_MIN;
    ch4 = random(LIGHT) + LIGHT_MIN;
    ch5 = random(LIGHT) + LIGHT_MIN;
    delay(random(DELAY_MS));
    //delayCount = random(DELAY_MS * ticksPerMs);
//  }
  /* test
  ch1++;
  ch2++;
  ch3++;
  ch4++;
  ch5++;
  _delay_ms(100);
  */
}

void writeOutput() {
  if(ISRcounter < ch1) PORTB |= (1 << CH1_PIN); // Write the ch1 pin high
  else PORTB &= ~(1 << CH1_PIN);                // Write the ch1 pin low

  if(ISRcounter < ch2) PORTB |= (1 << CH2_PIN);
  else PORTB &= ~(1 << CH2_PIN);

  if(ISRcounter < ch3) PORTB |= (1 << CH3_PIN);
  else PORTB &= ~(1 << CH3_PIN);

  if(ISRcounter < ch4) PORTB |= (1 << CH4_PIN);
  else PORTB &= ~(1 << CH4_PIN);

  if(ISRcounter < ch5) PORTB |= (1 << CH5_PIN);
  else PORTB &= ~(1 << CH5_PIN);

  ISRcounter++;
//  if (delayCount > 0) delayCount--;
}

/*
* The ISR is responsible for toggling the states of the five output channels
* based on the current global variables for the desired brightness levels.
* Because the brightnesses (ch1 ... ch5) are single-byte values, the range
* of values is 0 to 255\. Thus the ISR needs to restart its counting cycle every
* 256th time it is called.
*
* To avoid undesired wiggle on the PWM, the ISR updates evey channel's pin
* every time it runs. It should take the same number of cycles every time.
* The duty period of the PWM is the first section. Thus the LED goes ON then OFF.
*/

//ISR(TIM0_OVF_vect)
//{
//  writeOutput();
//}

//ISR(TIM0_COMPA_vect)
//{
//  writeOutput();
//}

ISR(TIM0_COMPB_vect)
{
  writeOutput();
}
