// Test the mask strand with a Trinket M0

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif


#define PIN 0

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
#define STRIP_LEN (128)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_LEN, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

typedef struct pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel_t;

pixel_t pixels[STRIP_LEN] = {0};

typedef struct step {
    int16_t i;
    int16_t pixel_i;
} step_t;

typedef struct particle {
    uint8_t valid;  // 0 ignore, 1 valid particle
    uint8_t path_idx;  // Index into paths array
    int16_t i;  // current index into path
    uint32_t wait;  // inverse velocity (in micros per move)
    uint32_t last;  // last time it moved (in micros)
    int8_t dir;  // direction of velocity
    uint8_t bright_r;  // brightness (red)
    uint8_t bright_g;  // brightness (green)
    uint8_t bright_b;  // brightness (blue)
    uint8_t glow_r;  // decrease in brightness nearby (red)
    uint8_t glow_g;  // decrease in brightness nearby (red)
    uint8_t glow_b;  // decrease in brightness nearby (blue)
    uint8_t updates;  // debug counter
} particle_t;

#define NUM_PARTICLES (20)
particle_t particles[NUM_PARTICLES] = {0};
#define NUM_FIXED_PARTICLES (2)
particle_t fixed_particles[NUM_FIXED_PARTICLES] = {0};

typedef struct path {
    step_t *path;
    uint16_t length;
} path_t;

step_t path_l[] = {
    {-20,  -1}, /* seg None */
    {-19,  -1}, /* seg None */
    {-18,  -1}, /* seg None */
    {-17,  -1}, /* seg None */
    {-16,  -1}, /* seg None */
    {-15,  -1}, /* seg None */
    {-14,  -1}, /* seg None */
    {-13,  -1}, /* seg None */
    {-12,  -1}, /* seg None */
    {-11,  -1}, /* seg None */
    {-10,  -1}, /* seg None */
    { -9,  -1}, /* seg None */
    { -8,  -1}, /* seg None */
    { -7,  -1}, /* seg None */
    { -6,  -1}, /* seg None */
    { -5,  -1}, /* seg None */
    { -4,  -1}, /* seg None */
    { -3,  -1}, /* seg None */
    { -2,  -1}, /* seg None */
    { -1,  -1}, /* seg None */
    {  0,   0}, /* seg 1 */
    {  1,   1}, /* seg 1 */
    {  2,   2}, /* seg 1 */
    {  3,   3}, /* seg 1 */
    {  4,   4}, /* seg 1 */
    {  5,   5}, /* seg 2 */
    {  6,   6}, /* seg 2 */
    {  7,   7}, /* seg 2 */
    {  8,   8}, /* seg 2 */
    {  9,   9}, /* seg 2 */
    { 10,  10}, /* seg 2 */
    { 11,  11}, /* seg 2 */
    { 12,  12}, /* seg 2 */
    { 13,  13}, /* seg 2 */
    { 14,  14}, /* seg 2 */
    { 15,  15}, /* seg 3 */
    { 16,  16}, /* seg 3 */
    { 17,  17}, /* seg 3 */
    { 18,  18}, /* seg 3 */
    { 19,  19}, /* seg 3 */
    { 20,  20}, /* seg 3 */
    { 21,  21}, /* seg 3 */
    { 22,  22}, /* seg 3 */
    { 23,  23}, /* seg 4 */
    { 24,  24}, /* seg 4 */
    { 25,  25}, /* seg 4 */
    { 26,  26}, /* seg 4 */
    { 27,  27}, /* seg 4 */
    { 28,  28}, /* seg 4 */
    { 29,  29}, /* seg 4 */
    { 30,  97}, /* seg -14 */
    { 31,  96}, /* seg -14 */
    { 32,  95}, /* seg -14 */
    { 33,  94}, /* seg -14 */
    { 34,  93}, /* seg -14 */
    { 35,  92}, /* seg -14 */
    { 36,  91}, /* seg -14 */
    { 37,  90}, /* seg -14 */
    { 38,  89}, /* seg -14 */
    { 39,  88}, /* seg -14 */
    { 40,  87}, /* seg -14 */
    { 41,  86}, /* seg -14 */
    { 42,  30}, /* seg 5 */
    { 43,  31}, /* seg 5 */
    { 44,  32}, /* seg 5 */
    { 45,  33}, /* seg 5 */
    { 46,  34}, /* seg 5 */
    { 47,  35}, /* seg 6 */
    { 48,  36}, /* seg 6 */
    { 49,  37}, /* seg 6 */
    { 50,  38}, /* seg 6 */
    { 51,  39}, /* seg 6 */
    { 52,  40}, /* seg 6 */
    { 53,  41}, /* seg 6 */
    { 54,  42}, /* seg 6 */
    { 55,  43}, /* seg 6 */
    { 56,  44}, /* seg 7 */
    { 57,  45}, /* seg 7 */
    { 58,  46}, /* seg 7 */
    { 59,  47}, /* seg 7 */
    { 60,  48}, /* seg 7 */
    { 61,  49}, /* seg 7 */
    { 62,  50}, /* seg 7 */
    { 63,  51}, /* seg 7 */
    { 64,  52}, /* seg 7 */
    { 65,  53}, /* seg 8 */
    { 66,  54}, /* seg 8 */
    { 67,  55}, /* seg 8 */
    { 68,  56}, /* seg 8 */
    { 69,  57}, /* seg 8 */
    { 70,  -1}, /* seg None */
    { 71,  -1}, /* seg None */
    { 72,  -1}, /* seg None */
    { 73,  -1}, /* seg None */
    { 74,  -1}, /* seg None */
    { 75,  -1}, /* seg None */
    { 76,  -1}, /* seg None */
    { 77,  -1}, /* seg None */
    { 78,  -1}, /* seg None */
    { 79,  -1}, /* seg None */
    { 80,  -1}, /* seg None */
    { 81,  -1}, /* seg None */
    { 82,  -1}, /* seg None */
    { 83,  -1}, /* seg None */
    { 84,  -1}, /* seg None */
    { 85,  -1}, /* seg None */
    { 86,  -1}, /* seg None */
    { 87,  -1}, /* seg None */
    { 88,  -1}, /* seg None */
    { 89,  -1}, /* seg None */

};
uint16_t path_l_len = sizeof(path_l) / sizeof(path_l[0]);
step_t path_r[] = {
    {-20,  -1}, /* seg None */
    {-19,  -1}, /* seg None */
    {-18,  -1}, /* seg None */
    {-17,  -1}, /* seg None */
    {-16,  -1}, /* seg None */
    {-15,  -1}, /* seg None */
    {-14,  -1}, /* seg None */
    {-13,  -1}, /* seg None */
    {-12,  -1}, /* seg None */
    {-11,  -1}, /* seg None */
    {-10,  -1}, /* seg None */
    { -9,  -1}, /* seg None */
    { -8,  -1}, /* seg None */
    { -7,  -1}, /* seg None */
    { -6,  -1}, /* seg None */
    { -5,  -1}, /* seg None */
    { -4,  -1}, /* seg None */
    { -3,  -1}, /* seg None */
    { -2,  -1}, /* seg None */
    { -1,  -1}, /* seg None */
    {  0,  127}, /* seg -18 */
    {  1,  126}, /* seg -18 */
    {  2,  125}, /* seg -18 */
    {  3,  124}, /* seg -18 */
    {  4,  123}, /* seg -18 */
    {  5,  122}, /* seg -17 */
    {  6,  121}, /* seg -17 */
    {  7,  120}, /* seg -17 */
    {  8,  119}, /* seg -17 */
    {  9,  118}, /* seg -17 */
    { 10,  117}, /* seg -17 */
    { 11,  116}, /* seg -17 */
    { 12,  115}, /* seg -17 */
    { 13,  114}, /* seg -17 */
    { 14,  113}, /* seg -17 */
    { 15,  112}, /* seg -16 */
    { 16,  111}, /* seg -16 */
    { 17,  110}, /* seg -16 */
    { 18,  109}, /* seg -16 */
    { 19,  108}, /* seg -16 */
    { 20,  107}, /* seg -16 */
    { 21,  106}, /* seg -16 */
    { 22,  105}, /* seg -16 */
    { 23,  104}, /* seg -15 */
    { 24,  103}, /* seg -15 */
    { 25,  102}, /* seg -15 */
    { 26,  101}, /* seg -15 */
    { 27,  100}, /* seg -15 */
    { 28,  99}, /* seg -15 */
    { 29,  98}, /* seg -15 */
    { 30,  97}, /* seg -14 */
    { 31,  96}, /* seg -14 */
    { 32,  95}, /* seg -14 */
    { 33,  94}, /* seg -14 */
    { 34,  93}, /* seg -14 */
    { 35,  92}, /* seg -14 */
    { 36,  91}, /* seg -14 */
    { 37,  90}, /* seg -14 */
    { 38,  89}, /* seg -14 */
    { 39,  88}, /* seg -14 */
    { 40,  87}, /* seg -14 */
    { 41,  86}, /* seg -14 */
    { 42,  85}, /* seg -13 */
    { 43,  84}, /* seg -13 */
    { 44,  83}, /* seg -13 */
    { 45,  82}, /* seg -13 */
    { 46,  81}, /* seg -13 */
    { 47,  80}, /* seg -12 */
    { 48,  79}, /* seg -12 */
    { 49,  78}, /* seg -12 */
    { 50,  77}, /* seg -12 */
    { 51,  76}, /* seg -12 */
    { 52,  75}, /* seg -12 */
    { 53,  74}, /* seg -12 */
    { 54,  73}, /* seg -12 */
    { 55,  72}, /* seg -12 */
    { 56,  71}, /* seg -11 */
    { 57,  70}, /* seg -11 */
    { 58,  69}, /* seg -11 */
    { 59,  68}, /* seg -11 */
    { 60,  67}, /* seg -11 */
    { 61,  66}, /* seg -11 */
    { 62,  65}, /* seg -11 */
    { 63,  64}, /* seg -11 */
    { 64,  63}, /* seg -11 */
    { 65,  62}, /* seg -10 */
    { 66,  61}, /* seg -10 */
    { 67,  60}, /* seg -10 */
    { 68,  59}, /* seg -10 */
    { 69,  58}, /* seg -10 */
    { 70,  -1}, /* seg None */
    { 71,  -1}, /* seg None */
    { 72,  -1}, /* seg None */
    { 73,  -1}, /* seg None */
    { 74,  -1}, /* seg None */
    { 75,  -1}, /* seg None */
    { 76,  -1}, /* seg None */
    { 77,  -1}, /* seg None */
    { 78,  -1}, /* seg None */
    { 79,  -1}, /* seg None */
    { 80,  -1}, /* seg None */
    { 81,  -1}, /* seg None */
    { 82,  -1}, /* seg None */
    { 83,  -1}, /* seg None */
    { 84,  -1}, /* seg None */
    { 85,  -1}, /* seg None */
    { 86,  -1}, /* seg None */
    { 87,  -1}, /* seg None */
    { 88,  -1}, /* seg None */
    { 89,  -1}, /* seg None */

};
uint16_t path_r_len = sizeof(path_r) / sizeof(path_r[0]);
step_t path_b[] = {
    {-20,  -1}, /* seg None */
    {-19,  -1}, /* seg None */
    {-18,  -1}, /* seg None */
    {-17,  -1}, /* seg None */
    {-16,  -1}, /* seg None */
    {-15,  -1}, /* seg None */
    {-14,  -1}, /* seg None */
    {-13,  -1}, /* seg None */
    {-12,  -1}, /* seg None */
    {-11,  -1}, /* seg None */
    {-10,  -1}, /* seg None */
    { -9,  -1}, /* seg None */
    { -8,  -1}, /* seg None */
    { -7,  -1}, /* seg None */
    { -6,  -1}, /* seg None */
    { -5,  -1}, /* seg None */
    { -4,  -1}, /* seg None */
    { -3,  -1}, /* seg None */
    { -2,  -1}, /* seg None */
    { -1,  -1}, /* seg None */
    {  0,   0}, /* seg 1 */
    {  1,   1}, /* seg 1 */
    {  2,   2}, /* seg 1 */
    {  3,   3}, /* seg 1 */
    {  4,   4}, /* seg 1 */
    {  5,   5}, /* seg 2 */
    {  6,   6}, /* seg 2 */
    {  7,   7}, /* seg 2 */
    {  8,   8}, /* seg 2 */
    {  9,   9}, /* seg 2 */
    { 10,  10}, /* seg 2 */
    { 11,  11}, /* seg 2 */
    { 12,  12}, /* seg 2 */
    { 13,  13}, /* seg 2 */
    { 14,  14}, /* seg 2 */
    { 15,  15}, /* seg 3 */
    { 16,  16}, /* seg 3 */
    { 17,  17}, /* seg 3 */
    { 18,  18}, /* seg 3 */
    { 19,  19}, /* seg 3 */
    { 20,  20}, /* seg 3 */
    { 21,  21}, /* seg 3 */
    { 22,  22}, /* seg 3 */
    { 23,  23}, /* seg 4 */
    { 24,  24}, /* seg 4 */
    { 25,  25}, /* seg 4 */
    { 26,  26}, /* seg 4 */
    { 27,  27}, /* seg 4 */
    { 28,  28}, /* seg 4 */
    { 29,  29}, /* seg 4 */
    { 30,  97}, /* seg -14 */
    { 31,  96}, /* seg -14 */
    { 32,  95}, /* seg -14 */
    { 33,  94}, /* seg -14 */
    { 34,  93}, /* seg -14 */
    { 35,  92}, /* seg -14 */
    { 36,  91}, /* seg -14 */
    { 37,  90}, /* seg -14 */
    { 38,  89}, /* seg -14 */
    { 39,  88}, /* seg -14 */
    { 40,  87}, /* seg -14 */
    { 41,  86}, /* seg -14 */
    { 42,  30}, /* seg 5 */
    { 43,  31}, /* seg 5 */
    { 44,  32}, /* seg 5 */
    { 45,  33}, /* seg 5 */
    { 46,  34}, /* seg 5 */
    { 47,  35}, /* seg 6 */
    { 48,  36}, /* seg 6 */
    { 49,  37}, /* seg 6 */
    { 50,  38}, /* seg 6 */
    { 51,  39}, /* seg 6 */
    { 52,  40}, /* seg 6 */
    { 53,  41}, /* seg 6 */
    { 54,  42}, /* seg 6 */
    { 55,  43}, /* seg 6 */
    { 56,  44}, /* seg 7 */
    { 57,  45}, /* seg 7 */
    { 58,  46}, /* seg 7 */
    { 59,  47}, /* seg 7 */
    { 60,  48}, /* seg 7 */
    { 61,  49}, /* seg 7 */
    { 62,  50}, /* seg 7 */
    { 63,  51}, /* seg 7 */
    { 64,  52}, /* seg 7 */
    { 65,  53}, /* seg 8 */
    { 66,  54}, /* seg 8 */
    { 67,  55}, /* seg 8 */
    { 68,  56}, /* seg 8 */
    { 69,  57}, /* seg 8 */
    { 70,  58}, /* seg 10 */
    { 71,  59}, /* seg 10 */
    { 72,  60}, /* seg 10 */
    { 73,  61}, /* seg 10 */
    { 74,  62}, /* seg 10 */
    { 75,  63}, /* seg 11 */
    { 76,  64}, /* seg 11 */
    { 77,  65}, /* seg 11 */
    { 78,  66}, /* seg 11 */
    { 79,  67}, /* seg 11 */
    { 80,  68}, /* seg 11 */
    { 81,  69}, /* seg 11 */
    { 82,  70}, /* seg 11 */
    { 83,  71}, /* seg 11 */
    { 84,  72}, /* seg 12 */
    { 85,  73}, /* seg 12 */
    { 86,  74}, /* seg 12 */
    { 87,  75}, /* seg 12 */
    { 88,  76}, /* seg 12 */
    { 89,  77}, /* seg 12 */
    { 90,  78}, /* seg 12 */
    { 91,  79}, /* seg 12 */
    { 92,  80}, /* seg 12 */
    { 93,  81}, /* seg 13 */
    { 94,  82}, /* seg 13 */
    { 95,  83}, /* seg 13 */
    { 96,  84}, /* seg 13 */
    { 97,  85}, /* seg 13 */
    { 98,  86}, /* seg 14 */
    { 99,  87}, /* seg 14 */
    { 100,  88}, /* seg 14 */
    { 101,  89}, /* seg 14 */
    { 102,  90}, /* seg 14 */
    { 103,  91}, /* seg 14 */
    { 104,  92}, /* seg 14 */
    { 105,  93}, /* seg 14 */
    { 106,  94}, /* seg 14 */
    { 107,  95}, /* seg 14 */
    { 108,  96}, /* seg 14 */
    { 109,  97}, /* seg 14 */
    { 110,  98}, /* seg 15 */
    { 111,  99}, /* seg 15 */
    { 112,  100}, /* seg 15 */
    { 113,  101}, /* seg 15 */
    { 114,  102}, /* seg 15 */
    { 115,  103}, /* seg 15 */
    { 116,  104}, /* seg 15 */
    { 117,  105}, /* seg 16 */
    { 118,  106}, /* seg 16 */
    { 119,  107}, /* seg 16 */
    { 120,  108}, /* seg 16 */
    { 121,  109}, /* seg 16 */
    { 122,  110}, /* seg 16 */
    { 123,  111}, /* seg 16 */
    { 124,  112}, /* seg 16 */
    { 125,  113}, /* seg 17 */
    { 126,  114}, /* seg 17 */
    { 127,  115}, /* seg 17 */
    { 128,  116}, /* seg 17 */
    { 129,  117}, /* seg 17 */
    { 130,  118}, /* seg 17 */
    { 131,  119}, /* seg 17 */
    { 132,  120}, /* seg 17 */
    { 133,  121}, /* seg 17 */
    { 134,  122}, /* seg 17 */
    { 135,  123}, /* seg 18 */
    { 136,  124}, /* seg 18 */
    { 137,  125}, /* seg 18 */
    { 138,  126}, /* seg 18 */
    { 139,  127}, /* seg 18 */
    { 140,  -1}, /* seg None */
    { 141,  -1}, /* seg None */
    { 142,  -1}, /* seg None */
    { 143,  -1}, /* seg None */
    { 144,  -1}, /* seg None */
    { 145,  -1}, /* seg None */
    { 146,  -1}, /* seg None */
    { 147,  -1}, /* seg None */
    { 148,  -1}, /* seg None */
    { 149,  -1}, /* seg None */
    { 150,  -1}, /* seg None */
    { 151,  -1}, /* seg None */
    { 152,  -1}, /* seg None */
    { 153,  -1}, /* seg None */
    { 154,  -1}, /* seg None */
    { 155,  -1}, /* seg None */
    { 156,  -1}, /* seg None */
    { 157,  -1}, /* seg None */
    { 158,  -1}, /* seg None */
    { 159,  -1}, /* seg None */

};
uint16_t path_b_len = sizeof(path_b) / sizeof(path_b[0]);

path_t paths[] = {
    {path_l, path_l_len},
    {path_r, path_r_len},
    {path_b, path_b_len},
};
uint16_t paths_len = sizeof(paths) / sizeof(paths[0]);


#define NOSE_HISTORY (20)
uint8_t x_pos;
uint32_t x_pos_last_micros;
uint8_t x_hist[NOSE_HISTORY];
uint8_t x_hist_i = 0;
uint8_t z_pos;
uint32_t z_pos_last_micros;
uint8_t z_hist[NOSE_HISTORY];
uint8_t z_hist_i = 0;
uint8_t x_pos_next;
uint8_t z_pos_next;
uint8_t zx_state = 0;


void randomParticle(particle_t *particle) {
    // TODO: types of particles? multi-modal distribution over particles
    uint8_t seg = random(1, 19);
    particle->valid = 1;
    particle->wait = random(20000, 60000);
    particle->dir = (random(2) > 0) ? 1 : -1;
    particle->path_idx = random(paths_len);
    particle->i = 0; //particle->path[0] ?  // XXX
    particle->last = micros();
    particle->bright_r = random(180);
    particle->bright_g = 0;
    particle->bright_b = random(180);
    particle->glow_r = random(5, 40);  // TODO: tune this
    particle->glow_g = 100;
    particle->glow_b = random(5, 40);  // TODO: tune this
    particle->updates = 0;
}

void debugParticle(uint16_t i) {
    particle_t *particle = &particles[i];
    Serial.print("debugging particle: "); Serial.println(i);
    Serial.print("particle: "); Serial.println((uint32_t)(particle));
    Serial.print("current micros "); Serial.println(micros());
    Serial.print("cast micros "); Serial.println((uint32_t)(micros()));
    Serial.print("current millis "); Serial.println(millis());
    Serial.print("valid "); Serial.println(particle->valid);
    Serial.print("path_idx "); Serial.println(particle->path_idx);
    Serial.print("i "); Serial.println(particle->i);
    Serial.print("wait "); Serial.println(particle->wait);
    Serial.print("dir "); Serial.println(particle->dir);
    Serial.print("last "); Serial.println(particle->last);
    Serial.print("next "); Serial.println(particle->last + particle->wait);
    // Serial.print("bright "); Serial.println(particle->bright);
    // Serial.print("glow "); Serial.println(particle->glow);
    // Serial.print("hue "); Serial.println(particle->hue);
    Serial.print("updates "); Serial.println(particle->updates);
}

void debugZX() {
    Serial.print("x: "); Serial.print(x_pos);
    Serial.print("\tz: "); Serial.println(z_pos);
}

void testParticle() {
    particle_t *particle = &particles[0];
    // Serial.println("testParticle() writing out");
    particle->valid = 1;
    particle->i = 0;
    particle->path_idx = 0;
    particle->wait = 10000;
    particle->dir = 1;
    particle->last = 0;
    // particle->bright = 80;
    // particle->glow = 10;
    // particle->hue = 0;
    particle->updates = 0;
}

void updateParticle(particle_t *particle) {
    uint32_t current = micros();
    if (!(particle->valid)) return;
    // Serial.print("updating particle: "); Serial.println((uint32_t)(particle));
    // Serial.print("updating last: "); Serial.println(particle->last);
    // Serial.print("updating wait: "); Serial.println(particle->wait);
    // Serial.print("updating next: "); Serial.println(particle->last + particle->wait);
    // Serial.print("updating current: "); Serial.println(current);
    // Serial.print("updating ready: "); Serial.println((uint8_t)(particle->last + particle->wait <= current));
    if (particle->last + particle->wait > current) return;
    particle->i += particle->dir;
    // Serial.print("updated i: "); Serial.println(particle->i);
    if (particle->i < 0 || particle->i >= paths[particle->path_idx].length) {
        particle->valid = 0;
    }
    // Serial.print("updated valid: "); Serial.println(particle->valid);
    particle->last = micros();
    // Serial.print("updated last: "); Serial.println(particle->last);
    // Serial.print("finished updating: "); Serial.println((uint32_t)(particle));
    particle->updates++;
}

void updateParticles() {
    particle_t *particle;
    for (uint8_t i = 0; i < NUM_PARTICLES; i++) {
        particle = &particles[i];
        updateParticle(particle);
        if (!(particle->valid)) {
            randomParticle(particle);
            // if (particle == &particles[0]) {
            //     testParticle();
            // }
        }
    }
}

void updateFixedParticle(uint8_t i, particle_t *particle) {
    Serial.print("Updating fixed particle: "); Serial.print((uint32_t)(particle));
    Serial.print("\ti: "); Serial.println(i);
    // if (!(particle->valid)) return;
    if (i == 0 || i == 1) {
        Serial.print("fixed particle: "); Serial.println(i);
        Serial.print("particle->i: "); Serial.println(particle->i);
        particle->valid = 1;
        particle->path_idx = i;
        if (i == 0) {
            particle->i = map(x_pos, 0, 255, 0, paths[particle->path_idx].length);
        } else {
            particle->i = map(z_pos, 0, 255, 0, paths[particle->path_idx].length);
        }
        particle->bright_r = 80;
        particle->bright_g = 250;
        particle->bright_b = 80;
        particle->glow_r = 10;
        particle->glow_g = 5;
        particle->glow_b = 10;
        particle->updates++;
    }
}

void updateFixedParticles() {
    particle_t *particle;
    for (uint8_t i = 0; i < NUM_FIXED_PARTICLES; i++) {
        particle = &fixed_particles[i];
        updateFixedParticle(i, particle);
    }
}

void drawPixel(int16_t i, uint8_t red, uint8_t green, uint8_t blue) {
    // TODO: add hue, properly
    pixels[i].r += pgm_read_byte(&gamma8[red]);
    pixels[i].g += pgm_read_byte(&gamma8[green]);
    pixels[i].b += pgm_read_byte(&gamma8[blue]);
}

void renderParticle(particle_t *particle) {
    int16_t bright_r = 0;
    int16_t bright_g = 0;
    int16_t bright_b = 0;
    if (!(particle->valid)) return;
    // Serial.print("rendering particle"); Serial.println((uint32_t)(particle));
    // Serial.print("starting at: "); Serial.println(particle->i);
    // Forward glow
    bright_r = particle->bright_r;
    bright_g = particle->bright_g;
    bright_b = particle->bright_b;
    for (int16_t i = particle->i; i < paths[particle->path_idx].length; i++) {
        step_t *path = paths[particle->path_idx].path;
        // Serial.print("drawing pixel: "); Serial.println(i);
        // Serial.print("step i"); Serial.print(path[i].i);
        // Serial.print(" pixel_i "); Serial.println(path[i].pixel_i);
        drawPixel(path[i].pixel_i, bright_r, bright_g, bright_b);
        if ((bright_r < particle->glow_r) &&
            (bright_g < particle->glow_g) &&
            (bright_b < particle->glow_b)) break;
        bright_r = MAX(bright_r - particle->glow_r, 0);
        bright_g = MAX(bright_g - particle->glow_g, 0);
        bright_b = MAX(bright_b - particle->glow_b, 0);
    }
    // Backward glow
    bright_r = particle->bright_r;
    bright_b = particle->bright_b;
    for (int16_t i = particle->i; i > 0; i--) {
        step_t *path = paths[particle->path_idx].path;
        drawPixel(path[i].pixel_i, bright_r, bright_g, bright_b);
        if ((bright_r < particle->glow_r) &&
            (bright_g < particle->glow_g) &&
            (bright_b < particle->glow_b)) break;
        bright_r = MAX(bright_r - particle->glow_r, 0);
        bright_g = MAX(bright_g - particle->glow_g, 0);
        bright_b = MAX(bright_b - particle->glow_b, 0);
    }
}

void blankPixels() {
    for (uint16_t i = 0; i < STRIP_LEN; i++) {
        pixels[i].r = 0;
        pixels[i].g = 0;
        pixels[i].b = 0;
    }
}

void renderParticles() {
    // Serial.println("render particles start");
    blankPixels();
    for (uint8_t i = 0; i < NUM_PARTICLES; i++) {
        renderParticle(&particles[i]);
    }
    for (uint8_t i = 0; i < NUM_FIXED_PARTICLES; i++) {
        renderParticle(&fixed_particles[i]);
    }
    // Serial.println("render particles end");
}

void debugPixels() {
    Serial.println("pixels");
    for (uint16_t i = 0; i < STRIP_LEN; i++) {
        if (pixels[i].r || pixels[i].g || pixels[i].b) {
            Serial.print("i"); Serial.print(i);
            Serial.print("r"); Serial.print(pixels[i].r);
            Serial.print("g"); Serial.print(pixels[i].g);
            Serial.print("b"); Serial.print(pixels[i].b);
        }
    }
    Serial.println("");
    Serial.println("pixels");
}

void showPixels() {
    // Serial.println("showing pixels");
    for (uint16_t i = 0; i < STRIP_LEN; i++) {
        strip.setPixelColor(i, pixels[i].r, pixels[i].g, pixels[i].b);
    }
    strip.show();
}

void parseZXSerial() {
  int c = Serial1.read();
  while (c >= 0) {
      // Serial.print("c: "); Serial.println(c);
      if (c < 0) return;
      switch (zx_state) {
        case 0: parseZXSerial_expect(c, 'Z'); break;
        case 1: parseZXSerial_setZnext(c); break;
        case 2: parseZXSerial_expect(c, 'X'); break;
        case 3: parseZXSerial_setXnext(c); break;
        default: parseZXSerial_reset();
      }
      c = Serial1.read();
  }
  if (zx_state == 4) {
    x_pos = x_pos_next;
    z_pos = z_pos_next;
    zx_state = 0;
  }
  // Serial.print("x: "); Serial.print(x_pos);
  // Serial.print("\tz: "); Serial.print(z_pos);
  // Serial.print("\txn: "); Serial.print(x_pos_next);
  // Serial.print("\tzn: "); Serial.println(z_pos_next);
}

void parseZXSerial_expect(int c, uint8_t e) {if (c == e) { zx_state++; } else { zx_state = 0; }}
void parseZXSerial_setXnext(int c) {x_pos_next = c; zx_state++;}
void parseZXSerial_setZnext(int c) {z_pos_next = c; zx_state++;}
void parseZXSerial_reset() {zx_state = 0;}

void setup() {
    // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
    #if defined (__AVR_ATtiny85__)
        if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
    #endif
    // End of trinket special code

    Serial.begin(9600);
    Serial1.begin(9600);
    // Serial.println("Hello, world.");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    randomSeed(analogRead(1));
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    // testParticle();
}

void loop() {
    // Serial.println("loop:");
    // debugZX();
    parseZXSerial();
    updateParticles();
    updateFixedParticles();
    // debugParticle(0);
    renderParticles();
    // debugPixels();
    showPixels();
}
