// Test the mask strand with a Trinket M0

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 4  // Trinket M0

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
#define STRIP_LEN (132)
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
    uint8_t bright;  // brightness
    uint8_t glow;  // decrease in brightness nearby
    uint8_t hue;  // color of the pixel
    uint8_t updates;  // debug counter
} particle_t;

#define NUM_PARTICLES (1)
particle_t particles[NUM_PARTICLES] = {0};

step_t path_l[] = {
    {-20,  -1}, // seg None
    {-19,  -1}, // seg None
    {-18,  -1}, // seg None
    {-17,  -1}, // seg None
    {-16,  -1}, // seg None
    {-15,  -1}, // seg None
    {-14,  -1}, // seg None
    {-13,  -1}, // seg None
    {-12,  -1}, // seg None
    {-11,  -1}, // seg None
    {-10,  -1}, // seg None
    { -9,  -1}, // seg None
    { -8,  -1}, // seg None
    { -7,  -1}, // seg None
    { -6,  -1}, // seg None
    { -5,  -1}, // seg None
    { -4,  -1}, // seg None
    { -3,  -1}, // seg None
    { -2,  -1}, // seg None
    { -1,  -1}, // seg None
    {  0,   0}, // seg 1
    {  1,   1}, // seg 1
    {  2,   2}, // seg 1
    {  3,   3}, // seg 1
    {  4,   4}, // seg 1
    {  5,   5}, // seg 2
    {  6,   6}, // seg 2
    {  7,   7}, // seg 2
    {  8,   8}, // seg 2
    {  9,   9}, // seg 2
    { 10,  10}, // seg 2
    { 11,  11}, // seg 2
    { 12,  12}, // seg 2
    { 13,  13}, // seg 2
    { 14,  14}, // seg 2
    { 15,  15}, // seg 3
    { 16,  16}, // seg 3
    { 17,  17}, // seg 3
    { 18,  18}, // seg 3
    { 19,  19}, // seg 3
    { 20,  20}, // seg 3
    { 21,  21}, // seg 3
    { 22,  22}, // seg 3
    { 23,  23}, // seg 4
    { 24,  24}, // seg 4
    { 25,  25}, // seg 4
    { 26,  26}, // seg 4
    { 27,  27}, // seg 4
    { 28,  28}, // seg 4
    { 29,  29}, // seg 4
    { 30,  101}, // seg -14
    { 31,  100}, // seg -14
    { 32,  99}, // seg -14
    { 33,  98}, // seg -14
    { 34,  97}, // seg -14
    { 35,  96}, // seg -14
    { 36,  95}, // seg -14
    { 37,  94}, // seg -14
    { 38,  93}, // seg -14
    { 39,  92}, // seg -14
    { 40,  91}, // seg -14
    { 41,  90}, // seg -14
    { 42,  30}, // seg 5
    { 43,  31}, // seg 5
    { 44,  32}, // seg 5
    { 45,  33}, // seg 5
    { 46,  34}, // seg 5
    { 47,  35}, // seg 6
    { 48,  36}, // seg 6
    { 49,  37}, // seg 6
    { 50,  38}, // seg 6
    { 51,  39}, // seg 6
    { 52,  40}, // seg 6
    { 53,  41}, // seg 6
    { 54,  42}, // seg 6
    { 55,  43}, // seg 6
    { 56,  44}, // seg 7
    { 57,  45}, // seg 7
    { 58,  46}, // seg 7
    { 59,  47}, // seg 7
    { 60,  48}, // seg 7
    { 61,  49}, // seg 7
    { 62,  50}, // seg 7
    { 63,  51}, // seg 7
    { 64,  52}, // seg 7
    { 65,  53}, // seg 8
    { 66,  54}, // seg 8
    { 67,  55}, // seg 8
    { 68,  56}, // seg 8
    { 69,  57}, // seg 8
    { 70,  58}, // seg 9
    { 71,  59}, // seg 9
    { 72,  60}, // seg 9
    { 73,  61}, // seg 9
    { 74,  -1}, // seg None
    { 75,  -1}, // seg None
    { 76,  -1}, // seg None
    { 77,  -1}, // seg None
    { 78,  -1}, // seg None
    { 79,  -1}, // seg None
    { 80,  -1}, // seg None
    { 81,  -1}, // seg None
    { 82,  -1}, // seg None
    { 83,  -1}, // seg None
    { 84,  -1}, // seg None
    { 85,  -1}, // seg None
    { 86,  -1}, // seg None
    { 87,  -1}, // seg None
    { 88,  -1}, // seg None
    { 89,  -1}, // seg None
    { 90,  -1}, // seg None
    { 91,  -1}, // seg None
    { 92,  -1}, // seg None
    { 93,  -1}, // seg None
};
uint16_t path_l_len = sizeof(path_l) / sizeof(path_l[0]);

step_t path_r[] = {
    {-20,  -1}, // seg None
    {-19,  -1}, // seg None
    {-18,  -1}, // seg None
    {-17,  -1}, // seg None
    {-16,  -1}, // seg None
    {-15,  -1}, // seg None
    {-14,  -1}, // seg None
    {-13,  -1}, // seg None
    {-12,  -1}, // seg None
    {-11,  -1}, // seg None
    {-10,  -1}, // seg None
    { -9,  -1}, // seg None
    { -8,  -1}, // seg None
    { -7,  -1}, // seg None
    { -6,  -1}, // seg None
    { -5,  -1}, // seg None
    { -4,  -1}, // seg None
    { -3,  -1}, // seg None
    { -2,  -1}, // seg None
    { -1,  -1}, // seg None
    {  0,  131}, // seg -18
    {  1,  130}, // seg -18
    {  2,  129}, // seg -18
    {  3,  128}, // seg -18
    {  4,  127}, // seg -18
    {  5,  126}, // seg -17
    {  6,  125}, // seg -17
    {  7,  124}, // seg -17
    {  8,  123}, // seg -17
    {  9,  122}, // seg -17
    { 10,  121}, // seg -17
    { 11,  120}, // seg -17
    { 12,  119}, // seg -17
    { 13,  118}, // seg -17
    { 14,  117}, // seg -17
    { 15,  116}, // seg -16
    { 16,  115}, // seg -16
    { 17,  114}, // seg -16
    { 18,  113}, // seg -16
    { 19,  112}, // seg -16
    { 20,  111}, // seg -16
    { 21,  110}, // seg -16
    { 22,  109}, // seg -16
    { 23,  108}, // seg -15
    { 24,  107}, // seg -15
    { 25,  106}, // seg -15
    { 26,  105}, // seg -15
    { 27,  104}, // seg -15
    { 28,  103}, // seg -15
    { 29,  102}, // seg -15
    { 30,  101}, // seg -14
    { 31,  100}, // seg -14
    { 32,  99}, // seg -14
    { 33,  98}, // seg -14
    { 34,  97}, // seg -14
    { 35,  96}, // seg -14
    { 36,  95}, // seg -14
    { 37,  94}, // seg -14
    { 38,  93}, // seg -14
    { 39,  92}, // seg -14
    { 40,  91}, // seg -14
    { 41,  90}, // seg -14
    { 42,  89}, // seg -13
    { 43,  88}, // seg -13
    { 44,  87}, // seg -13
    { 45,  86}, // seg -13
    { 46,  85}, // seg -13
    { 47,  84}, // seg -12
    { 48,  83}, // seg -12
    { 49,  82}, // seg -12
    { 50,  81}, // seg -12
    { 51,  80}, // seg -12
    { 52,  79}, // seg -12
    { 53,  78}, // seg -12
    { 54,  77}, // seg -12
    { 55,  76}, // seg -12
    { 56,  75}, // seg -11
    { 57,  74}, // seg -11
    { 58,  73}, // seg -11
    { 59,  72}, // seg -11
    { 60,  71}, // seg -11
    { 61,  70}, // seg -11
    { 62,  69}, // seg -11
    { 63,  68}, // seg -11
    { 64,  67}, // seg -11
    { 65,  66}, // seg -10
    { 66,  65}, // seg -10
    { 67,  64}, // seg -10
    { 68,  63}, // seg -10
    { 69,  62}, // seg -10
    { 70,  58}, // seg 9
    { 71,  59}, // seg 9
    { 72,  60}, // seg 9
    { 73,  61}, // seg 9
    { 74,  -1}, // seg None
    { 75,  -1}, // seg None
    { 76,  -1}, // seg None
    { 77,  -1}, // seg None
    { 78,  -1}, // seg None
    { 79,  -1}, // seg None
    { 80,  -1}, // seg None
    { 81,  -1}, // seg None
    { 82,  -1}, // seg None
    { 83,  -1}, // seg None
    { 84,  -1}, // seg None
    { 85,  -1}, // seg None
    { 86,  -1}, // seg None
    { 87,  -1}, // seg None
    { 88,  -1}, // seg None
    { 89,  -1}, // seg None
    { 90,  -1}, // seg None
    { 91,  -1}, // seg None
    { 92,  -1}, // seg None
    { 93,  -1}, // seg None
};
uint16_t path_r_len = sizeof(path_r) / sizeof(path_r[0]);

typedef struct path {
    step_t *path;
    uint16_t length;
} path_t;

path_t paths[] = {
    {path_l, path_l_len},
    {path_r, path_r_len},
};
uint16_t paths_len = sizeof(paths) / sizeof(paths[0]);

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  Serial.begin(9600);
  randomSeed(analogRead(1));
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  testParticle();
}

void loop() {
    debugParticle(0);
    updateParticles();
    renderParticles();
    showPixels();
}

void randomParticle(particle_t *particle) {
    uint8_t seg = random(1, 19);
    particle->valid = 1;
    particle->wait = random(20000, 100000);
    particle->dir = (random(2) > 0) ? 1 : -1;
    particle->path_idx = random(paths_len);
    particle->i = 0; //particle->path[0] ?  // XXX
    particle->last = micros();
    particle->bright = random(256);
    particle->glow = random(20, 100);  // TODO: tune this
    particle->hue = random(256);
    particle->updates = 0;
}

void debugParticle(uint16_t i) {
    particle_t *particle = &particles[i];
    Serial.print("debugging particle: "); Serial.println(i);
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
    Serial.print("bright "); Serial.println(particle->bright);
    Serial.print("glow "); Serial.println(particle->glow);
    Serial.print("hue "); Serial.println(particle->hue);
    Serial.print("updates "); Serial.println(particle->updates);
}

void testParticle() {
    particle_t *particle = &particles[0];
    particle->valid = 1;
    particle->i = 0;
    particle->path_idx = 0;
    particle->wait = 10000;
    particle->dir = 1;
    particle->last = 0;
    particle->bright = 120;
    particle->glow = 10;
    particle->hue = 0;
    particle->updates = 0;
}

void updateParticle(particle_t *particle) {
    uint32_t current = micros();
    if (!(particle->valid)) return;
    if (particle->last + particle->wait > current) return;
    particle->i += particle->dir;
    if (particle->i < 0 || particle->i >= paths[particle->path_idx].length) {
        particle->valid = 0;
    }
    particle->last = micros();
    particle->updates++;
}

void updateParticles() {
    particle_t *particle;
    for (uint8_t i = 0; i < NUM_PARTICLES; i++) {
        particle = &particles[i];
        updateParticle(particle);
        if (!(particle->valid)) {
            // randomParticle(particle);
            testParticle();
        }
    }
}

void drawPixel(int16_t i, uint8_t bright, uint8_t hue) {
    if (i >= 0 && i < STRIP_LEN) {
        // TODO: add hue
        // TODO: add gamma correction
        pixels[i].r += pgm_read_byte(&gamma8[bright]);
        pixels[i].g += pgm_read_byte(&gamma8[bright]);
        pixels[i].b += pgm_read_byte(&gamma8[bright]);
    }
}

void renderParticle(particle_t *particle) {
    uint8_t bright;
    if (!(particle->valid)) return;
    // Forward glow
    bright = particle->bright;
    for (int16_t i = particle->i; i < paths[particle->path_idx].length; i++) {
        step_t *path = paths[particle->path_idx].path;
        drawPixel(path[i].pixel_i, bright, particle->hue);
        if (bright < particle->glow) break;
        bright -= particle->glow;
    }
    // Backward glow
    bright = particle->bright;
    for (int16_t i = particle->i; i > 0; i--) {
        step_t *path = paths[particle->path_idx].path;
        drawPixel(path[i].pixel_i, bright, particle->hue);
        if (bright < particle->glow) break;
        bright -= particle->glow;
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
    blankPixels();
    for (uint8_t i = 0; i < NUM_PARTICLES; i++) {
        renderParticle(&particles[i]);
    }
}

void showPixels() {
    for (uint16_t i = 0; i < STRIP_LEN; i++) {
        strip.setPixelColor(i, pixels[i].r, pixels[i].g, pixels[i].b);
    }
    strip.show();
}
