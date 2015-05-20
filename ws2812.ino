
#include <util/delay.h>

#define PIXELS 200

#define PIXEL_PORT  PORTB
#define PIXEL_DDR   DDRB
#define PIXEL_BIT   4

long string[PIXELS];

void sendBit( bool bitVal ) {
  if (  bitVal ) {
    asm volatile (
      "sbi %[port], %[bit] \n\t"
      "nop \n\t"
      "nop \n\t"
      "nop \n\t"
      "nop \n\t"
      "nop \n\t"
      "nop \n\t"
      "nop \n\t"
      "cbi %[port], %[bit] \n\t"
      "nop \n\t"
      ::
      [port]  "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT)
    );                            
  } else {
    asm volatile (
      "sbi %[port], %[bit] \n\t"
      "nop \n\t"
      "nop \n\t"
      "nop \n\t"
      "nop \n\t"
      "nop \n\t"
      "cbi %[port], %[bit] \n\t"
      "nop \n\t"
      ::
      [port]  "I" (_SFR_IO_ADDR(PIXEL_PORT)),
      [bit]   "I" (PIXEL_BIT)
    );
  }  
}  

  
void sendByte(unsigned char byte) {
  for(unsigned char bit = 0; bit < 8; bit++) {     
    sendBit( bitRead( byte , 7 ) );
    byte <<= 1;
  }           
}

void sendPixel(unsigned char r, unsigned char g, unsigned char b)  {
  sendByte(g);
  sendByte(r);
  sendByte(b); 
}

void show() {
  cli();
  for (int i = 0; i < PIXELS; i++) {
    sendPixel(string[i] >> 16, (string[i] >> 8) & 255, string[i] & 255);
  } 
  _delay_us(6);
  sei();
}

long color(int r, int g, int b) {
  return ((long)r << 16) + ((long)g << 8) + b;
}

void shiftRight() {
  long rollover = string[PIXELS-1];
  for (int i = PIXELS - 1; i > 0; i--) {
    string[i] = string[i-1];
  }
  string[0] = rollover;
}

void shiftLeft() {
  long rollover = string[0];
  for (int i = 0; i < PIXELS; i++) {
    string[i] = string[i+1];
  }
  string[PIXELS-1] = rollover;
}

void setPixel(unsigned int pixel, long color) {
  string[pixel] = color;
}

void addPixel(unsigned int pixel, long color) {
  string[pixel] += color;
}

void clearPixels() {
  for (int i = 0; i < PIXELS; i++) {
    setPixel(i, 0);
  }
}

void allOn(unsigned long color) {
  for (int i = 0; i < PIXELS; i++) {
    setPixel(i, color);
  }
}

long colorwheel(int number) {
  if (number <= 255) return color(255, number, 0);
  if (number <= 510) return color((510-number), 255, 0);
  if (number <= 765) return color(0, 255, 255-(765-number));
  if (number <= 1020) return color(0, 1020-number, 255);
  if (number <= 1275) return color(255-(1275-number), 0, 255);
  if (number <= 1530) return color(255, 0, 1530-number);
}

void applyBrightness(float brightness) {
  for (int i = 0; i < PIXELS; i++) {
    long red = string[i] >> 16;
    long blue = (string[i] >> 8) & 255;
    long green = string[i] & 255;
    red *= brightness;
    blue *= brightness;
    green *= brightness;
    string[i] = (red << 16) + (green << 8) + blue;
  }
}
  

void setup() {  
  pinMode(12, OUTPUT);
  
  clearPixels();
//  allOn(color(50, 50, 50));
//  setPixel(0, color(70, 40, 40));
//  setPixel(1, color(85, 30, 30));
//  setPixel(2, color(100, 20, 20));
//  setPixel(3, color(115, 10, 10));
//  setPixel(4, color(100, 20, 20));
//  setPixel(5, color(80, 30, 30));
//  setPixel(6, color(70, 40, 40));
  
//  setPixel(12, color(70, 40, 40));
//  setPixel(13, color(85, 30, 30));
//  setPixel(14, color(100, 20, 20));
//  setPixel(15, color(115, 10, 10));
//  setPixel(16, color(100, 20, 20));
//  setPixel(17, color(80, 30, 30));
//  setPixel(18, color(70, 40, 40));
//  setPixel(7, color(0, 5, 0));
//  Serial.begin(9600);
}



void loop() {

  /*
  for (int i = 0; i < 256; i++) {
    allOn(Wheel(i));
    show();
    delay(3);
  }
  */
  

  /*
  int red = map(analogRead(A0), 0, 1023, 10, 200);
  int green = map(analogRead(A1), 0, 1023, 10, 200);
  int blue = map(analogRead(A2), 0, 1023, 10, 200);

  clearPixels();
  for (int i = 10; i > 0; i--) {
    addPixel(red-i, color(255, 0, 0));
    addPixel(green-i, color(0, 255, 0));
    addPixel(blue-i, color(0, 0, 255));
  }
 
  show();
  */
  
  for (int i = 0; i < 1530; i=i+map(analogRead(A1), 0, 1023, 1, 100)) {
    float brightness = map(analogRead(A2), 0, 1023, 0, 100) / 100.0;
    allOn(colorwheel(i));
    applyBrightness(brightness);
    show();
    delay(map(analogRead(A0), 0, 1023, 1, 1000));
  }
}
