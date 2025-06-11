#include <M5Unified.h>
#define IS_M5CORE2

#ifdef IS_M5STICKC_PLUS
#define MAXX 240
#define MAXY 135
#define SCREENROT 3
#define TEXTW 32
#define TEXTH 18
#define SCALE 26.0
#define XCENT 34.0
#define YCENT 66.0
#define ZCENT 100.0
#define BRIGHT(b) (M5.Axp.ScreenBreath(map(b,0,100,7,12)));
#define BTNCAL BtnA
#undef  BTNBRIGHT 
#define BTNFREEZE BtnB
#endif

#ifdef IS_M5CORE2    
#define MAXX 320
#define MAXY 240
#define SCREENROT 3
#define TEXTW 32
#define TEXTH 18
#define GRAPHW (MAXX-TEXTW)
#define GRAPHH MAXY
#define SCALE 48.0
#define XCENT 60.0
#define YCENT 120.0
#define ZCENT 180.0
#define BTNBRIGHT BtnC
#define BRIGHT(b) (M5.Lcd.setBrightness(map(b,0,100,0,255)))
// With screen rotated, we need to define our own off screen buttons for now?
// No - fixed in M5Unified
//Button BtnBright = Button(224, -40, 87, 40);
#endif

// params for circle drawing, hue cycling
// values are milliseconds
#define LPDELAY 2
#define MODEMAX 180000 

int radius = 1;
int color = 0;
float sigma = 10.0;
float rho = 28;
float beta = 8.0 / 3.0;
float x = 0.0;
float y = 1.0;
float z = 1.1;
float dt = .01;
float xdot, ydot, zdot;
int xplot, yplot, zplot ,mode;
long when = MODEMAX;
float hue = 0.0;  // 0.0 .. 360.0
int bright = 50;

void setup() {
    //Serial.begin(115200);
    mode = 0;
    M5.begin();
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setRotation(SCREENROT); // USB cable to the left
}

void loop() {
  	M5.update();
    if (M5.BTNBRIGHT.wasClicked()) { screenbright(); }

    xdot = sigma*(y - x);
    ydot = rho*x - y - x*z;
    zdot = x*y - beta*z;
    x = x + xdot * dt;
    y = y + ydot * dt;
    z = z + zdot * dt;

    xplot = 6*x + (MAXX/2);
    yplot = 4*y + (MAXY/2);
    zplot = 4*z;

    delay(LPDELAY);

    if (millis() > when) { 
      mode++;
      when = millis() + MODEMAX;
      (when < 0) ? when=0 : 0;
      M5.Lcd.fillScreen(TFT_BLACK);
    }

    switch(mode) { 
      case 0:
        //M5.Lcd.drawPixel(yplot+35 , zplot+10, color);
        M5.Lcd.fillCircle(yplot+30 , zplot+10, radius, color);
        break;
      case 1:
        //M5.Lcd.drawPixel(xplot, zplot+5, color);
        M5.Lcd.fillCircle(xplot, zplot+5, radius, color);
        break;
      case 2:
        //M5.Lcd.drawPixel(xplot, yplot, color);
        M5.Lcd.fillCircle(xplot, yplot, radius, color);
        break;
      default:
        mode = 0;
      }

    // cycle through colors
    //color = color - 1;
    //(color < 0) ? color = 65536 : 0; 
    
    // cycle through hues
    //hue = 120.0; // 0,63,0 green
    //hue = 240.0; // 0,0,31 blue
    //hue = 0.0; // 31,0,31 red
    hue = hue + 0.05; // minimum value to not get solid colors
    (hue > 360.0) ? hue=0.0 : 0;
    color = hsv2565(hue, 1.0, 1.0);

}

int hsv2565(float hue, float sat, float v) {
// r= 0..31, g = 0..63, b = 0..31
int i = 0;
float h, f, p, q, t;
int r,g,b;

h = hue;
h /= 60.0;
i = floor(h);                         // sector 0 to 5
f = h - i;                              // factorial part of h
p = v * ( 1.0 - sat );                    // always 0 here
q = v * ( 1.0 - sat * f );                // always 1-f here
t = v * ( 1.0 - sat * ( 1.0 - f ) );      // always 1-f here

// //Serial.println(h);

switch( i ) {
       case 0:
           r = v * 31;
           g = t * 63;
           b = p * 31;
           break;
       case 1:
           r = q * 31;
           g = v * 63;
           b = p * 31;
           break;
       case 2:
           r = p * 31;
           g = v * 63;
           b = t * 31;
           break;
       case 3:
           r = p * 31;
           g = q * 63;
           b = v * 31;
           break;
       case 4:
           r = t * 31;
           g = p * 63;
           b = v * 31;
           break;
       default:        // case 5:
           r = v * 31;
           g = p * 63;
           b = q * 31;
           break;
       }
   //Serial.print(r);
   //Serial.print("\t");
   //Serial.print(g);
   //Serial.print("\t");
   //Serial.print(b);
   //Serial.print("\n");
   
   return (r<<11 | g<<5 | b);
}



void screenbright() {
    bright = bright + 10;
    if (bright > 100) { bright = 0; }
    BRIGHT(bright);
}
