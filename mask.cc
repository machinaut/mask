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
    step_t *path;  // What path am I on?
    int16_t i;  // current index into path
    uint32_t wait;  // inverse velocity (in micros per move)
    uint32_t last;  // last time it moved (in micros)
    int8_t dir;  // direction of velocity
    uint8_t bright;  // brightness
    uint8_t glow;  // decrease in brightness nearby
    uint8_t hue;  // color of the pixel
    uint8_t updates;  // debug counter
} particle_t;

#define NUM_PARTICLES (30)
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
    { 30,  43}, // seg -14
    { 31,  42}, // seg -14
    { 32,  41}, // seg -14
    { 33,  40}, // seg -14
    { 34,  39}, // seg -14
    { 35,  38}, // seg -14
    { 36,  37}, // seg -14
    { 37,  36}, // seg -14
    { 38,  35}, // seg -14
    { 39,  30}, // seg 5
    { 40,  31}, // seg 5
    { 41,  32}, // seg 5
    { 42,  33}, // seg 5
    { 43,  34}, // seg 5
    { 44,  35}, // seg 6
    { 45,  36}, // seg 6
    { 46,  37}, // seg 6
    { 47,  38}, // seg 6
    { 48,  39}, // seg 6
    { 49,  40}, // seg 6
    { 50,  41}, // seg 6
    { 51,  42}, // seg 6
    { 52,  43}, // seg 6
    { 53,  44}, // seg 7
    { 54,  45}, // seg 7
    { 55,  46}, // seg 7
    { 56,  47}, // seg 7
    { 57,  48}, // seg 7
    { 58,  49}, // seg 7
    { 59,  50}, // seg 7
    { 60,  51}, // seg 7
    { 61,  52}, // seg 7
    { 62,  53}, // seg 8
    { 63,  54}, // seg 8
    { 64,  55}, // seg 8
    { 65,  56}, // seg 8
    { 66,  57}, // seg 8
    { 67,  58}, // seg 9
    { 68,  59}, // seg 9
    { 69,  60}, // seg 9
    { 70,  61}, // seg 9
    { 71,  -1}, // seg None
    { 72,  -1}, // seg None
    { 73,  -1}, // seg None
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
    {  0,  14}, // seg -18
    {  1,  13}, // seg -18
    {  2,  12}, // seg -18
    {  3,  11}, // seg -18
    {  4,  10}, // seg -18
    {  5,   9}, // seg -18
    {  6,   8}, // seg -18
    {  7,   7}, // seg -18
    {  8,   6}, // seg -18
    {  9,   5}, // seg -18
    { 10,  22}, // seg -17
    { 11,  21}, // seg -17
    { 12,  20}, // seg -17
    { 13,  19}, // seg -17
    { 14,  18}, // seg -17
    { 15,  17}, // seg -17
    { 16,  16}, // seg -17
    { 17,  15}, // seg -17
    { 18,  29}, // seg -16
    { 19,  28}, // seg -16
    { 20,  27}, // seg -16
    { 21,  26}, // seg -16
    { 22,  25}, // seg -16
    { 23,  24}, // seg -16
    { 24,  23}, // seg -16
    { 25,  34}, // seg -15
    { 26,  33}, // seg -15
    { 27,  32}, // seg -15
    { 28,  31}, // seg -15
    { 29,  30}, // seg -15
    { 30,  43}, // seg -14
    { 31,  42}, // seg -14
    { 32,  41}, // seg -14
    { 33,  40}, // seg -14
    { 34,  39}, // seg -14
    { 35,  38}, // seg -14
    { 36,  37}, // seg -14
    { 37,  36}, // seg -14
    { 38,  35}, // seg -14
    { 39,  52}, // seg -13
    { 40,  51}, // seg -13
    { 41,  50}, // seg -13
    { 42,  49}, // seg -13
    { 43,  48}, // seg -13
    { 44,  47}, // seg -13
    { 45,  46}, // seg -13
    { 46,  45}, // seg -13
    { 47,  44}, // seg -13
    { 48,  57}, // seg -12
    { 49,  56}, // seg -12
    { 50,  55}, // seg -12
    { 51,  54}, // seg -12
    { 52,  53}, // seg -12
    { 53,  61}, // seg -11
    { 54,  60}, // seg -11
    { 55,  59}, // seg -11
    { 56,  58}, // seg -11
    { 57,  66}, // seg -10
    { 58,  65}, // seg -10
    { 59,  64}, // seg -10
    { 60,  63}, // seg -10
    { 61,  62}, // seg -10
    { 62,  58}, // seg 9
    { 63,  59}, // seg 9
    { 64,  60}, // seg 9
    { 65,  61}, // seg 9
    { 66,  -1}, // seg None
    { 67,  -1}, // seg None
    { 68,  -1}, // seg None
    { 69,  -1}, // seg None
    { 70,  -1}, // seg None
    { 71,  -1}, // seg None
    { 72,  -1}, // seg None
    { 73,  -1}, // seg None
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
  // testParticle();
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
    particle->path = (random(2) > 0) ? path_l : path_r;
    // Start offscreen
    if (particle->dir > 0) {
        particle->i = -20;
    } else {
        particle->i = STRIP_LEN + 19;
    }
    particle->last = micros();
    particle->bright = random(256);
    particle->glow = random(5, 100);  // TODO: tune this
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
    Serial.print("path "); Serial.println((uint32_t)(particle->path));
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
    particle->i = -100;
    particle->wait = 100000;
    particle->dir = 1;
    particle->last = 0;
    particle->bright = 255;
    particle->glow = 10;
    particle->hue = 0;
    particle->updates = 0;
}

void updateParticle(particle_t *particle) {
    uint32_t current = micros();
    if (!(particle->valid)) return;
    if (particle->last + particle->wait > current) return;
    particle->i += particle->dir;
    if (particle-> i < 0 || particle->i >= path_l_len) {  // XXX NOT VALID BOUNDS
        particle->valid = 0;
    }
    particle->updates++;
}

void updateParticles() {
    particle_t *particle;
    for (uint8_t i = 0; i < NUM_PARTICLES; i++) {
        particle = &particles[i];
        updateParticle(particle);
        if (!(particle->valid)) {
            randomParticle(particle);
            // testParticle();
        }
    }
}

void drawPixel(uint16_t i, uint8_t bright, uint8_t hue) {
    if (i >= 0 && i < STRIP_LEN) {
        // TODO: add hue
        // TODO: add gamma correction
        pixels[i].r = bright;
        pixels[i].g = bright;
        pixels[i].b = bright;
    }
}

void renderParticle(particle_t *particle) {
    uint8_t bright;
    if (!(particle->valid)) return;
    // Forward glow
    bright = particle->bright;
    for (uint8_t i = particle->i; i < STRIP_LEN; i++) {
        drawPixel(particle->path[i].pixel_i, bright, particle->hue);
        if (bright < particle->glow) break;
        bright -= particle->glow;
    }
    // Backward glow
    // bright = particle->bright;
    // for (uint8_t i = particle->i; i < STRIP_LEN; i--) {  // rely on overflow
    //     drawPixel(particle->path[i].pixel_i, bright, particle->hue);
    //     if (bright < particle->glow) break;
    //     bright -= particle->glow;
    // }
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
