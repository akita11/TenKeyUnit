#include <stdint.h>
#include "fw_reg_stc8g.h"
#include <stdlib.h>

// STC8G1K08A-36I-SOP16
// Grove: SCL(P3.2)-SDA(P3.3)-VDD-GND

//     P34(o) P35(o) P36(o)
// P10  1      2      3         LED#0 #1  #2
// P11  4      5      6            #3 #4  #5
// P16  7      8      9            #6 #7  #8
// P17  .      0     ENT           #9 #10 #11

#define pinR0    P10
#define pinR1    P11
#define pinR2    P16
#define pinR3    P17
#define pinC0    P34
#define pinC1    P35
#define pinC2    P36
#define pinRXD   P30 // P3.0 / Grove-p2
#define pinTXD   P31 // P3.1 / Grove-p1
#define pinSCL   P32 // P3.2
#define pinSDA   P33 // P3.3
#define pinMD    P37 // P3.7, not coneccted in v1

#define FOSC 11059200UL

#define I2C_SLAVE_ADDR 0x5f // 7bit expression

#include "WS2812_P5_5.c"
void neopixel_show_long_P5_5(uint32_t dataAndLen); 
#define neopixel_show_P5_5(ADDR,LEN) neopixel_show_long_P5_5((((uint16_t)(ADDR)) & 0xFFFF) | (((uint32_t)(LEN) & 0xFFFF) << 16)); 

#define setPixel(INDEX, R, G, B)		   \
  {                                                \
    __xdata uint8_t *ptr = (ledData) + ((INDEX)*3);   \
    ptr[0] = (G);                                  \
    ptr[1] = (R);                                  \
    ptr[2] = (B);                                  \
  };

#define showPixels() neopixel_show_P5_5(ledData, NUM_BYTES)


#define NUM_LEDS 12
#define NUM_BYTES (NUM_LEDS * 3)

__xdata uint8_t ledData[NUM_BYTES]; // 3byte/LED

__xdata uint8_t led_r;
__xdata uint8_t led_g;
__xdata uint8_t led_b;

unsigned char sinTable[46] = {0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};

void setColorHSV(unsigned int hue, unsigned char sat, unsigned char val) {
  hue = (hue * 1530L + 32768) / 65536;
  if (hue < 510) { // Red to Green-1
    led_b = 0;
    if (hue < 255) { //   Red to Yellow-1
      led_r = 255;
      led_g = hue;       //     g = 0 to 254
    } else {         //   Yellow to Green-1
      led_r = 510 - hue; //     r = 255 to 1
      led_g = 255;
    }
  } else if (hue < 1020) { // Green to Blue-1
    led_r = 0;
    if (hue < 765) { //   Green to Cyan-1
      led_g = 255;
      led_b = hue - 510;  //     b = 0 to 254
    } else {          //   Cyan to Blue-1
      led_g = 1020 - hue; //     g = 255 to 1
      led_b = 255;
    }
  } else if (hue < 1530) { // Blue to Red-1
    led_g = 0;
    if (hue < 1275) { //   Blue to Magenta-1
      led_r = hue - 1020; //     r = 0 to 254
      led_b = 255;
    } else { //   Magenta to Red-1
      led_r = 255;
      led_b = 1530 - hue; //     b = 255 to 1
    }
  } else { // Last 0.5 Red (quicker than % operator)
    led_r = 255;
    led_g = led_b = 0;
  }

  // Apply saturation and value to R,G,B, pack into 32-bit result:
  unsigned long v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
  unsigned int s1 = 1 + sat;  // 1 to 256; same reason
  unsigned char s2 = 255 - sat; // 255 to 0

  led_r = (((((led_r * s1) >> 8) + s2) * v1) >> 8);
  led_g = (((((led_g * s1) >> 8) + s2) * v1) >> 8);
  led_b = (((((led_b * s1) >> 8) + s2) * v1) >> 8);
}

// hue = 0-179(360/2)
void setRGB(unsigned int hue, unsigned char sat, unsigned char val) {
  for (unsigned char i = 0; i < NUM_LEDS; i++) {
    setColorHSV(hue, sat, val);
    setPixel(i, led_r, led_g, led_b); //Choose the color order depending on the LED you use
    hue += 655;
  }
  showPixels();
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void delay100ms(unsigned char n) //@11.0592MHz, from STC-ISP
{
  unsigned char i, j, k;
  do{
    __asm__("nop");
    i = 6; j = 157; k = 59;
    do { do{ while (--k); } while (--j);} while (--i);
  } while (--n);
}

void delay10ms(unsigned char n)	//@11.0592MHz, from STC-ISP
{
  unsigned char i, j;
  do{
    __asm__("nop"); __asm__("nop");
    i = 144; j = 157; do{ while (--j); } while (--i);
  } while (--n);
}

void delay1ms(unsigned char n) //@11.0592MHz, from STC-ISP
{
  unsigned char i, j;
  do{
    i = 15; j = 90;
    do {while (--j);} while (--i);
  } while (--n);
}

uint8_t key = 0;
uint8_t md = 0;
uint8_t st = 0;

char keyMap[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', 0x0d};

uint8_t scanColumn = 0;

unsigned char d = 0;

void sendByte(unsigned char c)
{
  //  while(SCON & 0x02); // wait fot TX complete
  SBUF = c;
}

void main(void)
{
  // M1=0/M0=0 : QB/quasi bidirectional
  // M1=0/M0=1 : PP/push-pull output
  // M1=1/M0=0 : IN/input
  // M1=1/M0=1 : OD/open-drain
  P3M1 = 0x00; P3M0 = 0x70; // set P3.0-3=QB, P3.4-6=PP
  P1M1 = 0x00; P1M0 = 0x00; // set P1.0-P1.7=QB
  P5M1 = 0x00; P5M0 = 0x00; // set P5.4=QB
  P_SW2 = 0xb0; // I2C=P3.2(SCL)/P3.3(SDA), XFR access enabled
  P3PU |= 0x0c; // enable pull-up of P3.2/P3.3 (does it works?)
  
  // 9600bpsU(UART1)@11.0592MHz, from STC-ISP
  PCON &= 0x7F;	// Baudrate no doubled
  SCON = 0x50;  // 8 bits and variable baudrate (Mode1), RX enabled
  AUXR |= 0x40; // imer clock is 1T mode
  AUXR &= 0xFE;	// UART 1 use Timer1 as baudrate generator
  TMOD &= 0x0F;	// Set timer work mode
  TMOD |= 0x20;	// Set timer work mode
  TL1 = 0xDC;	// Initial timer value
  TH1 = 0xDC;	// Set reload value
  ET1 = 0;	// Disable Timer1 interrupt
  TR1 = 1;	// Timer1 start run

  I2CCFG = 0x81; // enable I2C, slave mode, speed clock=1
  I2CSLADR = I2C_SLAVE_ADDR << 1;

  I2CSLST = 0x00;
  I2CSLCR = 0x00;
  I2CTXD = 0x00;

  // mode check, for v1.1
  P1 |= 0x80; __asm__("nop"); __asm__("nop");
  if ((P1 & 0x80) == 0) md = 1; else md = 0;

  //     P34(o) P35(o) P36(o)
  // P10  1      2      3         LED#0 #1  #2
  // P11  4      5      6            #3 #4  #5
  // P16  7      8      9            #6 #7  #8
  // P17  .      0     ENT           #9 #10 #11

  #define LED_BRIGHTNESS 50

  unsigned int brightness = LED_BRIGHTNESS;

  for (unsigned char i = 0; i < NUM_LEDS; i++) setPixel(i, 0, 0, 0);
  showPixels();

  // Fade In
  for (brightness = 0; brightness < LED_BRIGHTNESS; brightness++){
    for (unsigned char i = 0; i < NUM_LEDS; i++)
      setPixel(i, brightness, brightness, brightness);
    showPixels();
    delay1ms(1);
  }
  /*
  for (unsigned char i = 0; i < NUM_LEDS; i++)
  {
    setPixel(i, LED_BRIGHTNESS, LED_BRIGHTNESS, LED_BRIGHTNESS);
    showPixels();
    //    delay10ms(3);
    delay10ms(1);
  }
    */
  // Rotate
  //  for (unsigned int baseHue = 0xFF00; baseHue > 0; baseHue -= 0x100)
  for (unsigned int baseHue = 0; baseHue < 0xfdff; baseHue += 0x100)
  {
    for (unsigned int i = 0; i < NUM_LEDS; i++)
    {
      //      setLED(i, ledIndex[index], map(index, 0, ROWS * COLS, 0, 65535) + (uint16_t)baseHue, LED_BRIGHTNESS);
      setColorHSV(map(i, 0, NUM_LEDS, 0, 65535) + (unsigned int)baseHue, 250, LED_BRIGHTNESS);
      setPixel(i, led_r, led_g, led_b);
    }
    showPixels();
    delay1ms(1);
    //delay1ms(8);
  }
  
  // Fade Out
  //  for (unsigned int baseHue = 0xFF00; baseHue > 0; baseHue -= 0x100)
  for (unsigned int baseHue = 0; baseHue < 0xfeff; baseHue += 0x100)
  {
    brightness--;
    for (unsigned char i = 0; i < NUM_LEDS; i++)
    {
      setColorHSV(map(i, 0, NUM_LEDS, 0, 65535) + (unsigned int)baseHue, 250, brightness);
      setPixel(i, led_r, led_g, led_b);
    }
    showPixels();
    if (brightness == 0) break;
    delay1ms(1);
    //delay1ms(8);
  }

  for (unsigned char i = 0; i < NUM_LEDS; i++) setPixel(i, 0, 0, 0);
  showPixels();

  while(1){
    key = 0;
    for (scanColumn = 0; scanColumn < 3; scanColumn++){
      P3 |= 0x70;
      switch(scanColumn){
      case 0 : P3 &= ~0x10; break; // P3.4=0
      case 1 : P3 &= ~0x20; break; // P3.5=0
      case 2 : P3 &= ~0x40; break; // P3.6=0
      }
      delay1ms(1);
      P1 |= 0xc3; __asm__("nop"); __asm__("nop");
      if ((P1 & 0x01) == 0) key =  1 + scanColumn;
      if ((P1 & 0x02) == 0) key =  4 + scanColumn;
      if ((P1 & 0x40) == 0) key =  7 + scanColumn;
      if ((P1 & 0x80) == 0) key = 10 + scanColumn;
    }

    if (md == 1){ // swap 1-2-3 / 7-8-9, for v1.1
      if (key >= 1 && key <= 3) key = key - 6;
      else if (key >= 7 && key <= 9) key = key - 6;
    }
    
    if (st == 0 && key != 0){
      st = 1;
      unsigned char r, g, b;
      // r = rand() & 0x1f; g = rand() & 0x1f; b = rand() & 0x1f; // random color
      //      r = 20; g = 20; b = 20; // white
      //      r = 100; g = 100; b = 100; // white
      r = 200; g = 200; b = 200; // white
      setPixel(key - 1, r, g, b); showPixels();
      delay100ms(1);
      setPixel(key - 1, 0, 0, 0); showPixels();
    }
    else{
      if (st == 2 && key == 0) st = 0;
    }


    if (I2CSLST & 0x10){ // I2C SENT event
      I2CSLST &= ~0x10;
      if (I2CSLST & 0x02){
	//Stop receiving data when receiving NAK
	if (st == 1){
	  I2CTXD = keyMap[key - 1]; st = 2;
	}
	else I2CTXD = 0x00;
      }
      else{
	//Continue reading data when receiving ACK
	if (st == 1){
	  I2CTXD = keyMap[key - 1]; st = 2;
	}
	else I2CTXD = 0x00;
      }
    }
  }
}

/*
void I2C_ISR(void) __interrupt (24)
{
  //  _push_(P_SW2);
  //  P_SW2 |= 0x80;
  if (I2CSLST & 0x40){ // START received
    I2CTXD = d++;
    I2CSLST &= ~0x40;
  }
  //  _pop_(P_SW2);
}
*/


