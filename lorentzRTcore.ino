#include <M5Unified.h>
#define IS_M5CORE2

#ifdef IS_M5STICKC_PLUS
#define MAXX 240
#define MAXY 135
#define SCREENROT 3
#define TEXTW 32
#define TEXTH 18
#define BRIGHT(b) (M5.Axp.ScreenBreath(map(b,0,100,7,12)));
#undef BTNBRIGHT 
#endif

#ifdef IS_M5CORE2    
#define MAXX 320
#define MAXY 240
#define SCREENROT 3
#define TEXTW 32
#define TEXTH 18
#define GRAPHW (MAXX-TEXTW)
#define GRAPHH MAXY
#define BRIGHT(b) (M5.Lcd.setBrightness(map(b,0,100,0,255)))
// With screen rotated, we used to have to define our own off screen buttons.
// This is fixed in M5Unified.
//Button BtnBright = Button(224, -40, 87, 40);
#endif

// params for circle drawing, hue cycling
int loopdelay=20;             // msec between circle draws
#define MODEMAX 150000      // msec between view modes
int pointbufsize=100;       // number of points to tail
#define RADIUS 4            // radius or points drawn
#define HUEINCREMENT 0.5    // how fast to cycle around hues

#define BIGNUM 1.0e10
int color = 0;
float sigma = 10.0;
float rho = 28;
float beta = 8.0 / 3.0;
float x = 0.0;
float y = 1.0;
float z = 1.1;
float xmin=BIGNUM ,ymin=BIGNUM ,zmin=BIGNUM;
float xmax=-BIGNUM ,ymax=-BIGNUM ,zmax=-BIGNUM;
float dt = .01;
float xdot, ydot, zdot;
int xplot, yplot, zplot ,mode;
long when = MODEMAX;
float hue = 0.0;  // 0.0..360.0
float value = 1.0;  // 0.0..1.0
int bright = 50;
char printbuf[100];
int pointbufx[10000];
int pointbufy[10000];
int epx=0, epy=0, dpx=pointbufsize-1, dpy=pointbufsize-1;

void setup() {
    Serial.begin(115200);
    mode = 0;
    M5.begin();
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setRotation(SCREENROT); // USB cable to the left
    initpointbuf();
    M5.Lcd.setTextFont(&fonts::FreeSansBold18pt7b);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_BLACK);
}

void loop() {
  	M5.update();
    if (M5.BtnB.wasClicked()) { screenbright(); }
    if (M5.BtnA.wasClicked()) {
      pointbufsize = pointbufsize * 10;
      (pointbufsize > 10000) ? pointbufsize = 10 : 0;
      sprintf(printbuf,"Points: %d",pointbufsize);
      beep(printbuf);
    }
    if (M5.BtnC.wasClicked()) {
      loopdelay = loopdelay * 10;
      (loopdelay > 200) ? loopdelay = 2 : 0;
      sprintf(printbuf,"Delay: %d",loopdelay);
      beep(printbuf);
    }

    xdot = sigma*(y - x);
    ydot = rho*x - y - x*z;
    zdot = x*y - beta*z;
    x = x + xdot * dt;
    y = y + ydot * dt;
    z = z + zdot * dt;
    (x > xmax) ? xmax = x : 0;
    (y > ymax) ? ymax = y : 0;
    (z > zmax) ? zmax = z : 0;
    (x < xmin) ? xmin = x : 0;
    (y < ymin) ? ymin = y : 0;
    (z < zmin) ? zmin = z : 0;
    // Approximate resulting values:
    // x: -20.2 to 21.3
    // y: -27.0 to 29.0
    // z: 0.8 to 53.7
    // Macrofy these scale factors for StickC:
    xplot = 7*x + (MAXX/2);
    yplot = 4*y + (MAXY/2);
    zplot = 4*z;

    delay(loopdelay);

    if (millis() > when) { 
      mode++;
      when = millis() + MODEMAX;
      (when < 0) ? when=0 : 0;
      M5.Lcd.fillScreen(TFT_BLACK);
      initpointbuf();
    }

    switch(mode) { 
      case 0:
        pointbufx[dpx] = yplot+30;
        pointbufy[dpy] = zplot+30;
        drawstuff();
        value = (x-xmin)/(xmax-xmin);
        break;
      case 1:
        pointbufx[dpx] = xplot;
        pointbufy[dpy] = zplot+5;
        drawstuff();
        value = (y-ymin)/(ymax-ymin);
        break;
      case 2:
        pointbufx[dpx] = xplot;
        pointbufy[dpy] = yplot;
        drawstuff();
        value = (z-zmin)/(zmax-zmin);
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
    hue = hue + HUEINCREMENT;
    (hue >= 360.0) ? hue=0.0 : 0;
    color = hsv2565(hue, 1.0, value);

}

void beep(char* thetext) { 
        M5.Lcd.setRotation(1);
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.fillScreen(TFT_WHITE);
        M5.Lcd.drawCenterString(thetext, MAXX/2, MAXY/2, &fonts::FreeSansBold18pt7b);
        //M5.Lcd.print(thetext);
        delay(2000);
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setRotation(SCREENROT);
}


void drawstuff() {
        // or M5.Lcd.drawPixel ...
        M5.Lcd.fillCircle(pointbufx[epx],pointbufy[epy], RADIUS, TFT_BLACK);
        M5.Lcd.fillCircle(pointbufx[dpx],pointbufy[dpy], RADIUS, color);
        dpx = (dpx+1) % pointbufsize;
        epx = (epx+1) % pointbufsize;
        dpy = (dpy+1) % pointbufsize;
        epy = (epy+1) % pointbufsize;
}
  
  
int hsv2565(float h, float s, float v) {
// r= 0..31, g = 0..63, b = 0..31
int i = 0;
float f, p, q, t;
int r,g,b;

// SPECIAL HACK FOR THIS PROGRAM: minimum value
(v < 0.15) ? v=0.15 : 0;
h /= 60.0;                          // it's OK to destroy h here
i = floor(h);                       // sector 0 to 5, int used in switch
f = h - i;                          // factorial part of h
p = v * ( 1.0 - s );                // always 0 here
q = v * ( 1.0 - s * f );            // always 1-f here
t = v * ( 1.0 - s * ( 1.0 - f ) );  // always 1-f here

//Serial.println(h);

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

void debugprint() { 
        sprintf(printbuf,"X: %f %f\n",xmin,xmax);
        Serial.print(printbuf);
        sprintf(printbuf,"Y: %f %f\n",ymin,ymax);
        Serial.print(printbuf);
        sprintf(printbuf,"Z: %f %f\n",zmin,zmax);
        Serial.print(printbuf);
        Serial.print("\n");
}

void initpointbuf() {
  for (int i=0; i<pointbufsize; i++) { 
    pointbufx[i]=0;
    pointbufy[i]=0;
  }
}
