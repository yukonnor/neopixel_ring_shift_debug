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

bool adjust_colors = true; // set whether you want to adjust colors or not (to avoid adjusting to same color every loop)

bool free_shift = true;  // mode for shifting colors around each ring individually
bool sync_shift = false; // mode for shifting colors around all rings in sync
// const float micro_step_amt = 0.02;
// float xl_shift_counter = 0.00;
// float l_shift_counter = 0.00;
// float m_shift_counter = 0.00;
// float s_shift_counter = 0.00;
int xl_shift_counter = 0;
int l_shift_counter = 0;
int m_shift_counter = 0;
int s_shift_counter = 0;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // (Total number of pixels, Arduino pin number, Pixel type flags)

void light_leds()
{
  // This function sets the pixel colors based on the final color setting (currently the color after they have been shifted)
  for (int pixel = 0; pixel < LED_COUNT; pixel++)
  {
    strip.setPixelColor(pixel, shifted_pixel_colors[pixel][0], shifted_pixel_colors[pixel][1], shifted_pixel_colors[pixel][2]);
  }

  strip.show(); // update the colors of the pixels
}

void set_gradient_color(short color_num, short r, short g, short b)
{
  // set the two colors for the gradient in the gradient color array
  gradient_colors[color_num][0] = r;
  gradient_colors[color_num][1] = g;
  gradient_colors[color_num][2] = b;
}

short get_ring_pixel_count(char circle)
{
  // define pixel count and offset for each circle type
  if (circle == 's')
  {
    S_PIXEL_COUNT;
  }
  else if (circle == 'm')
  {
    M_PIXEL_COUNT;
  }
  else if (circle == 'l')
  {
    L_PIXEL_COUNT;
  }
  else if (circle == 'x')
  {
    XL_PIXEL_COUNT;
  }
}

short get_ring_first_pixel(char circle)
{
  if (circle == 's')
  {
    return S_PIXEL_START;
  }
  else if (circle == 'm')
  {
    return M_PIXEL_START;
  }
  else if (circle == 'l')
  {
    return L_PIXEL_START;
  }
  else if (circle == 'x')
  {
    return XL_PIXEL_START;
  }
}

void apply_gradient(char circle, short color_1[], short color_2[])
{ // Applies the color gradient between two colors on a given pixel ring to the main pixel_colors array.
  // TODO: define array size?

  short pixels_per_color = get_ring_pixel_count(circle) / 2; // find how many pixels each color should have based on the ring
  short ring_first_pixel = get_ring_first_pixel(circle);     // was circle_offset    // determines where the first color should start based on which circle we're working on

  // add one to pixels_per_color for the rings with odd # leds
  if (pixels_per_color % 2 == 1)
    pixels_per_color++;

  // for each color of the 2 color array
  for (int color = 0; color < 2; color++)
  {
    // figure out which range of pixels we're working on based on the circle: [start pixel, end pixel]
    short current_range[2];

    // to fix bug of xl 2nd color going out of array range
    if (circle == 'x' && color == 1)
    {
      current_range[0] = (color * pixels_per_color) + ring_first_pixel;
      current_range[1] = ((color + 1) * pixels_per_color) + ring_first_pixel - 1;
    }
    else
    {
      current_range[0] = (color * pixels_per_color) + ring_first_pixel;
      current_range[1] = ((color + 1) * pixels_per_color) + ring_first_pixel;
    }

    short final_color[3];

    // set the starting color for the range (if color = 0 it's red. if color = 1 it's blue)
    if (color == 0)
    {
      final_color[0] = color_1[0]; // r
      final_color[1] = color_1[1]; // g
      final_color[2] = color_1[2]; // b
    }
    if (color == 1)
    {
      final_color[0] = color_2[0];
      final_color[1] = color_2[1];
      final_color[2] = color_2[2];
    }

    for (int pixel = current_range[0]; pixel < current_range[1]; pixel++)
    {
      // apply the gradient color to the main pixel color array:
      pixel_colors[pixel][0] = final_color[0];
      pixel_colors[pixel][1] = final_color[1];
      pixel_colors[pixel][2] = final_color[2];

      // define next color in the gradient:
      if (color == 0)
      {
        final_color[0] += int((color_2[0] - color_1[0]) / pixels_per_color);
        final_color[1] += int((color_2[1] - color_1[1]) / pixels_per_color);
        final_color[2] += int((color_2[2] - color_1[2]) / pixels_per_color);
      }
      if (color == 1)
      {
        final_color[0] += int((color_1[0] - color_2[0]) / pixels_per_color);
        final_color[1] += int((color_1[1] - color_2[1]) / pixels_per_color);
        final_color[2] += int((color_1[2] - color_2[2]) / pixels_per_color);
      }
    }
  }
}

// shift the pixels around the individual circles
int shift_pixels(char circle, short shift_amt, int shift_counter)
{
  short pixel_count = get_ring_pixel_count(circle);
  short first_pixel = get_ring_first_pixel(circle);

  shift_counter += shift_amt;

  // reset the counter once it does a full loop.
  if (shift_counter == pixel_count)
    shift_counter = 0;
  if (shift_counter == -1 * pixel_count)
    shift_counter = 0;

  for (int i = 0; i < pixel_count; i++)
  {
    short index_to_get = first_pixel + ((i + shift_counter) % pixel_count);

    // fix c's handling of negative modulos
    if (index_to_get < 0)
    {
      index_to_get = pixel_count + index_to_get;
    }

    shifted_pixel_colors[i + first_pixel][0] = pixel_colors[index_to_get][0];
    shifted_pixel_colors[i + first_pixel][1] = pixel_colors[index_to_get][1];
    shifted_pixel_colors[i + first_pixel][2] = pixel_colors[index_to_get][2];
  }

  // for (int i = 0; i < S_PIXEL_COUNT; i++) {
  //   Serial.print("Shifted_pixel_color for pixel "); Serial.print(i); Serial.print(" ");
  //   Serial.print(shifted_pixel_colors[i][0]); Serial.print(" ");  Serial.print(shifted_pixel_colors[i][1]); Serial.print(" ");  Serial.println(shifted_pixel_colors[i][2]);
  // }

  return shift_counter;
}

void setup()
{
  Serial.begin(9600); // open the serial port at 9600 bps:
  strip.begin();      // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.clear();
  strip.show();                            // Turn OFF all pixels (turns off as no pixel colors have been defined yet)
  strip.setBrightness(5);                  // Set BRIGHTNESS (max = 255)
  Serial.println("Hello. Setup running."); // trying to debug why setup is looping...

  // setup initial colors
  // set the gradient colors
  set_gradient_color(0, 255, 0, 0);
  set_gradient_color(1, 0, 0, 255);

  // apply the gradient using the gradient colors
  apply_gradient('s', gradient_colors[0], gradient_colors[1]);
  apply_gradient('m', gradient_colors[0], gradient_colors[1]);
  apply_gradient('l', gradient_colors[0], gradient_colors[1]);
  apply_gradient('x', gradient_colors[0], gradient_colors[1]);

  // fill the shifted color array:
  // for (int pixel = 0; pixel < LED_COUNT; pixel++)
  // {
  //   shifted_pixel_colors[pixel][0] = pixel_colors[pixel][0];
  //   shifted_pixel_colors[pixel][1] = pixel_colors[pixel][1];
  //   shifted_pixel_colors[pixel][2] = pixel_colors[pixel][2];
  // }

  delay(500);
}

void loop()
{
  // shift the small circle's led colors
  s_shift_counter = ('s', 1, s_shift_counter);

  // update the led colors and show them
  light_leds();
}
