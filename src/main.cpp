#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN 6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 120
#define S_PIXEL_COUNT 16  // pixels 0 - 15, 22.5
#define M_PIXEL_COUNT 24  // pixels 16 - 39, 15 degs
#define L_PIXEL_COUNT 35  // pixels 40 - 74, 10.3 degs
#define XL_PIXEL_COUNT 45 // pixels 75 - 119, 8 degs
const int S_PIXEL_START = 0;
const int S_PIXEL_END = 15;
const int M_PIXEL_START = S_PIXEL_COUNT;                  // 16
const int M_PIXEL_END = M_PIXEL_START + M_PIXEL_COUNT;    // 40
const int L_PIXEL_START = M_PIXEL_START + M_PIXEL_COUNT;  // 40
const int L_PIXEL_END = L_PIXEL_START + L_PIXEL_COUNT;    // 75
const int XL_PIXEL_START = L_PIXEL_START + L_PIXEL_COUNT; // 75
const int XL_PIXEL_END = XL_PIXEL_START + XL_PIXEL_COUNT; // 120

short pixel_colors[LED_COUNT][3];                     // array to store colors in their original, primary position
short shifted_pixel_colors[LED_COUNT][3];             // array to start colors after being shifted
short gradient_colors[2][3] = {{0, 0, 0}, {0, 0, 0}}; // array of 2 rbg colors for the gradient

bool adjust_colors = true; // set whether you want to adjust colors or not (so you don't pointless adjust to same color every loop)

const float micro_step_amt = 0.02;
bool free_shift = true;  // mode for shifting colors around each ring individually
bool sync_shift = false; // mode for shifting colors around all rings in sync
float xl_shift_counter = 0.00;
float l_shift_counter = 0.00;
float m_shift_counter = 0.00;
float s_shift_counter = 0.00;

// put function declarations here:
int myFunction(int, int);

void setup()
{
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop()
{
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y)
{
  return x + y;
}