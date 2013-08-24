#include <FastSPI_LED2.h>

//const int ledCount = 185;
const int ledCount =100;
const int NUM_STRIPS = 2;
CRGB leds[NUM_STRIPS][ledCount];

int BOTTOM_INDEX = 0;
int TOP_INDEX = int(ledCount/2);
int EVENODD = ledCount%2;

//CRGB ledsX[ledCount]; //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY (FOR CELL-AUTOMATA, ETC)
//-PERISTENT VARS
byte idex = 0;        //-LED INDEX (0 to ledCount-1
byte ihue = 0;        //-HUE (0-360)
int ibright = 0;     //-BRIGHTNESS (0-255)
int isat = 0;        //-SATURATION (0-255)
bool bounceForward = true;  //-SWITCH FOR COLOR BOUNCE (0-1)
int bouncedirection = 0;
float tcount = 0.0;      //-INC VAR FOR SIN LOOPS
int lcount = 0;      //-ANOTHER COUNTING VAR


void setPixel(int adex, CRGB c) {
    for(int i = 0;i<NUM_STRIPS;i++)
    {
        leds[i][adex] = c;
    }
}

//-SET THE COLOR OF A SINGLE RGB LED
void setPixel(int adex, int cred, int cgrn, int cblu) {
    for(int i = 0;i<NUM_STRIPS;i++)
    {
        leds[i][adex] = CRGB(cred, cgrn, cblu);
    }
}

// fill_solid -   fill a range of LEDs with a solid color
void fillSolid(byte strand, const CRGB& color)
{
 fill_solid( leds[strand], ledCount, color);
}

void fillSolid(CRGB color)
{
    for(int i = 0;i<NUM_STRIPS;i++)
        fillSolid(i, color);
}

void fillSolid(int cred, int cgrn, int cblu) { //-SET ALL LEDS TO ONE COLOR
    fillSolid(0, CRGB(cred, cgrn, cblu));    
}

//-FIND INDEX OF HORIZONAL OPPOSITE LED
int horizontal_index(int i) {
    //-ONLY WORKS WITH INDEX < TOPINDEX
    if (i == BOTTOM_INDEX) {return BOTTOM_INDEX;}
    if (i == TOP_INDEX && EVENODD == 1) {return TOP_INDEX + 1;}
    if (i == TOP_INDEX && EVENODD == 0) {return TOP_INDEX;}
    return ledCount - i;
}


//-FIND INDEX OF ANTIPODAL OPPOSITE LED
int antipodal_index(int i) {
    //int N2 = int(ledCount/2);
    int iN = i + TOP_INDEX;
    if (i >= TOP_INDEX) {iN = ( i + TOP_INDEX ) % ledCount; }
    return iN;
}


//-FIND ADJACENT INDEX CLOCKWISE
int adjacent_cw(int i) {
    int r;
    if (i < ledCount - 1) {
        r = i + 1;
    }
    else {
        r = 0;
    }
    return r;
}


//-FIND ADJACENT INDEX COUNTER-CLOCKWISE
int adjacent_ccw(int i) {
    int r;
    if (i > 0 && i <= ledCount) {
        r = i - 1;
    }
    else {
        r = ledCount - 1;
    }
    return r;
}

void HSVtoRGB(int hue, int sat, int val, CRGB& c) {
    hsv2rgb_rainbow(CHSV(hue, sat, val), c);
}

CRGB HSVtoRGB(int hue, int sat, int val) {
    CRGB c;
    hsv2rgb_rainbow(CHSV(hue, sat, val), c);
    return c;
}

void setup()
{
  // For safety (to prevent too high of a power draw), the test case defaults to
  // setting brightness to 25% brightness
  LEDS.setBrightness(255);
  
  LEDS.addLeds<WS2811,14, RGB>(leds[0], ledCount);

 /*
    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
    pin 14: LED strip #2    All 8 are the same length.
    pin 7:  LED strip #3
    pin 8:  LED strip #4    A 100 ohm resistor should used
    pin 6:  LED strip #5    between each Teensy pin and the
    pin 20: LED strip #6    wire to the LED strip, to minimize
    pin 21: LED strip #7    high frequency ringining & noise.
    pin 5:  LED strip #8
    pin 15 & 16 - Connect together, but do not use
    pin 4 - Do not use
    pin 3 - Do not use as PWM.  Normal use is ok.
    pin 1 - Output indicating CPU usage, monitor with an oscilloscope,
            logic analyzer or even an LED (brighter = CPU busier)
*/

      
  Serial.begin(57600);
  fillSolid(0,0,0); //-BLANK STRIP
  
  LEDS.show();
}


void flame() {
    CRGB acolor;
    int idelay = random(0,35);
    
    float hmin = 0.1; float hmax = 45.0;
    float hdif = hmax-hmin;
    int randtemp = random(0,3);
    float hinc = (hdif/float(TOP_INDEX))+randtemp;
    
    int ahue = hmin;
    for(int i = 0; i < TOP_INDEX; i++ ) {
        
        ahue = ahue + hinc;
        
        HSVtoRGB(ahue, 255, 255, acolor);
        setPixel(i,acolor);
        int ih = horizontal_index(i);
        setPixel(ih,acolor);
        setPixel(TOP_INDEX,CRGB::White);
    }
}

void spinner() { //-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
    static long lastBounceTime;
    const int bounceInterval = 250;

    static int leftInsideIndex = 33;
    
    static byte phase = 0;
    phase++;
    static byte trailLength = 5;
    if (phase % 8 == 0)
        trailLength = random(4,12);
    byte trailDecay = (255-64)/trailLength;
    static byte hue = random(0,10);

    fillSolid(HSVtoRGB(200, 255, 128));
    
    for(int i = 0;i<trailLength;i++)
    {
        setPixel(adjacent_cw(idex-i), HSVtoRGB(hue, 255, 255 - trailDecay*i));
        setPixel(adjacent_cw(leftInsideIndex-i), HSVtoRGB(hue, 255, 255 - trailDecay*i));
    }
    idex++;leftInsideIndex++;
    if (idex == 32) {
        idex = 2;
    }    
    if (leftInsideIndex == 47)
    {
    	leftInsideIndex = 33;
    }
}


void tailSpin() { //-BOUNCE COLOR (SIMPLE MULTI-LED FADE)
    static long lastBounceTime;
    const int bounceInterval = 250;
    static int rindex = 50;
    
    static byte phase = 0;
    phase++;
    static byte trailLength = 5;
    if (phase % 8 == 0)
        trailLength = random(4,12);
    byte trailDecay = (255-64)/trailLength;
    static byte hue = random(0,10);

    static bool dirForward = true;
    const byte firstHue = 5;
    const byte lastHue = 18;
    static byte solidHue = firstHue;
    if (dirForward == true)
	    solidHue++;
	else
		solidHue--;

	if (solidHue == lastHue)
	{
		Serial.println(solidHue);
		dirForward = false;
	}

	if (solidHue == firstHue)
	{
		Serial.println(solidHue);
		dirForward = true;
	}

	fillSolid(HSVtoRGB(solidHue,255,255));

    for(int i = 0;i<trailLength;i++)
    {
        setPixel(adjacent_cw(idex-i), HSVtoRGB(hue, 255, 255 - trailDecay*i));
    }
    idex++;
    if (idex == 50) {
        idex = 0;
    }

    //Serial.print("bouncing backwards idex:");Serial.print(idex);
    //todo: the trail is running off the array
    for(int i = 0;i<trailLength;i++)
    {
        setPixel(adjacent_cw(rindex-i), HSVtoRGB(hue, 255, 255 - trailDecay*i));
    }
    rindex++;
    if (rindex == 100) {
    	rindex = 50;
    }
}

void rotatingRainbow()
{
    static byte hue = 0;
    for(int i = 0;i < NUM_STRIPS;i++)
    {
//        fill_rainbow(leds[i], ledCount, hue++, 1);
        fillSolid(HSVtoRGB(hue++,255,255));
        Serial.println(hue);
    }
}


int xAccel;
void loop()
{
    Serial.println("Hello World");
    Serial.println(sizeof(leds));

//  rotatingRainbow();
  tailSpin();
//  spinner();
  //fillSolid(CRGB::HotPink);
  LEDS.show();
  delay(20);
  //fillSolid(CRGB::Pink);
	// delay(delayTime);
}

