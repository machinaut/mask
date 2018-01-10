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
    uint8_t next_l_i;
    uint8_t next_r_i;
    uint8_t prev_l_i;
    uint8_t prev_r_i;
    uint8_t i;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel_t;

pixel_t pixels[STRIP_LEN] = {
//  nextL nextR prevL prevR  this  R  G  B
    {   1,    1,  255,  255,    0, 0, 0, 0}, // seg  1
    {   2,    2,    0,    0,    1, 0, 0, 0}, // seg  1
    {   3,    3,    1,    1,    2, 0, 0, 0}, // seg  1
    {   4,    4,    2,    2,    3, 0, 0, 0}, // seg  1
    {   5,    5,    3,    3,    4, 0, 0, 0}, // seg  1
    {   6,    6,    4,    4,    5, 0, 0, 0}, // seg  2
    {   7,    7,    5,    5,    6, 0, 0, 0}, // seg  2
    {   8,    8,    6,    6,    7, 0, 0, 0}, // seg  2
    {   9,    9,    7,    7,    8, 0, 0, 0}, // seg  2
    {  10,   10,    8,    8,    9, 0, 0, 0}, // seg  2
    {  11,   11,    9,    9,   10, 0, 0, 0}, // seg  2
    {  12,   12,   10,   10,   11, 0, 0, 0}, // seg  2
    {  13,   13,   11,   11,   12, 0, 0, 0}, // seg  2
    {  14,   14,   12,   12,   13, 0, 0, 0}, // seg  2
    {  15,   15,   13,   13,   14, 0, 0, 0}, // seg  2
    {  16,   16,   14,   14,   15, 0, 0, 0}, // seg  3
    {  17,   17,   15,   15,   16, 0, 0, 0}, // seg  3
    {  18,   18,   16,   16,   17, 0, 0, 0}, // seg  3
    {  19,   19,   17,   17,   18, 0, 0, 0}, // seg  3
    {  20,   20,   18,   18,   19, 0, 0, 0}, // seg  3
    {  21,   21,   19,   19,   20, 0, 0, 0}, // seg  3
    {  22,   22,   20,   20,   21, 0, 0, 0}, // seg  3
    {  23,   23,   21,   21,   22, 0, 0, 0}, // seg  3
    {  24,   24,   22,   22,   23, 0, 0, 0}, // seg  4
    {  25,   25,   23,   23,   24, 0, 0, 0}, // seg  4
    {  26,   26,   24,   24,   25, 0, 0, 0}, // seg  4
    {  27,   27,   25,   25,   26, 0, 0, 0}, // seg  4
    {  28,   28,   26,   26,   27, 0, 0, 0}, // seg  4
    {  29,   29,   27,   27,   28, 0, 0, 0}, // seg  4
    { 101,  101,   28,   28,   29, 0, 0, 0}, // seg  4
    {  31,   31,   90,   90,   30, 0, 0, 0}, // seg  5
    {  32,   32,   30,   30,   31, 0, 0, 0}, // seg  5
    {  33,   33,   31,   31,   32, 0, 0, 0}, // seg  5
    {  34,   34,   32,   32,   33, 0, 0, 0}, // seg  5
    {  35,   35,   33,   33,   34, 0, 0, 0}, // seg  5
    {  36,   36,   34,   34,   35, 0, 0, 0}, // seg  6
    {  37,   37,   35,   35,   36, 0, 0, 0}, // seg  6
    {  38,   38,   36,   36,   37, 0, 0, 0}, // seg  6
    {  39,   39,   37,   37,   38, 0, 0, 0}, // seg  6
    {  40,   40,   38,   38,   39, 0, 0, 0}, // seg  6
    {  41,   41,   39,   39,   40, 0, 0, 0}, // seg  6
    {  42,   42,   40,   40,   41, 0, 0, 0}, // seg  6
    {  43,   43,   41,   41,   42, 0, 0, 0}, // seg  6
    {  44,   44,   42,   42,   43, 0, 0, 0}, // seg  6
    {  45,   45,   43,   43,   44, 0, 0, 0}, // seg  7
    {  46,   46,   44,   44,   45, 0, 0, 0}, // seg  7
    {  47,   47,   45,   45,   46, 0, 0, 0}, // seg  7
    {  48,   48,   46,   46,   47, 0, 0, 0}, // seg  7
    {  49,   49,   47,   47,   48, 0, 0, 0}, // seg  7
    {  50,   50,   48,   48,   49, 0, 0, 0}, // seg  7
    {  51,   51,   49,   49,   50, 0, 0, 0}, // seg  7
    {  52,   52,   50,   50,   51, 0, 0, 0}, // seg  7
    {  53,   53,   51,   51,   52, 0, 0, 0}, // seg  7
    {  54,   54,   52,   52,   53, 0, 0, 0}, // seg  8
    {  55,   55,   53,   53,   54, 0, 0, 0}, // seg  8
    {  56,   56,   54,   54,   55, 0, 0, 0}, // seg  8
    {  57,   57,   55,   55,   56, 0, 0, 0}, // seg  8
    {  58,   58,   56,   56,   57, 0, 0, 0}, // seg  8
    {  59,   59,   57,   62,   58, 0, 0, 0}, // seg  9
    {  60,   60,   58,   58,   59, 0, 0, 0}, // seg  9
    {  61,   61,   59,   59,   60, 0, 0, 0}, // seg  9
    { 255,  255,   60,   60,   61, 0, 0, 0}, // seg  9
    {  58,   58,   63,   63,   62, 0, 0, 0}, // seg  10
    {  62,   62,   64,   64,   63, 0, 0, 0}, // seg  10
    {  63,   63,   65,   65,   64, 0, 0, 0}, // seg  10
    {  64,   64,   66,   66,   65, 0, 0, 0}, // seg  10
    {  65,   65,   67,   67,   66, 0, 0, 0}, // seg  10
    {  66,   66,   68,   68,   67, 0, 0, 0}, // seg  11
    {  67,   67,   69,   69,   68, 0, 0, 0}, // seg  11
    {  68,   68,   70,   70,   69, 0, 0, 0}, // seg  11
    {  69,   69,   71,   71,   70, 0, 0, 0}, // seg  11
    {  70,   70,   72,   72,   71, 0, 0, 0}, // seg  11
    {  71,   71,   73,   73,   72, 0, 0, 0}, // seg  11
    {  72,   72,   74,   74,   73, 0, 0, 0}, // seg  11
    {  73,   73,   75,   75,   74, 0, 0, 0}, // seg  11
    {  74,   74,   76,   76,   75, 0, 0, 0}, // seg  11
    {  75,   75,   77,   77,   76, 0, 0, 0}, // seg  12
    {  76,   76,   78,   78,   77, 0, 0, 0}, // seg  12
    {  77,   77,   79,   79,   78, 0, 0, 0}, // seg  12
    {  78,   78,   80,   80,   79, 0, 0, 0}, // seg  12
    {  79,   79,   81,   81,   80, 0, 0, 0}, // seg  12
    {  80,   80,   82,   82,   81, 0, 0, 0}, // seg  12
    {  81,   81,   83,   83,   82, 0, 0, 0}, // seg  12
    {  82,   82,   84,   84,   83, 0, 0, 0}, // seg  12
    {  83,   83,   85,   85,   84, 0, 0, 0}, // seg  12
    {  84,   84,   86,   86,   85, 0, 0, 0}, // seg  13
    {  85,   85,   87,   87,   86, 0, 0, 0}, // seg  13
    {  86,   86,   88,   88,   87, 0, 0, 0}, // seg  13
    {  87,   87,   89,   89,   88, 0, 0, 0}, // seg  13
    {  88,   88,   90,   90,   89, 0, 0, 0}, // seg  13
    {  30,   89,   91,   91,   90, 0, 0, 0}, // seg  14
    {  90,   90,   92,   92,   91, 0, 0, 0}, // seg  14
    {  91,   91,   93,   93,   92, 0, 0, 0}, // seg  14
    {  92,   92,   94,   94,   93, 0, 0, 0}, // seg  14
    {  93,   93,   95,   95,   94, 0, 0, 0}, // seg  14
    {  94,   94,   96,   96,   95, 0, 0, 0}, // seg  14
    {  95,   95,   97,   97,   96, 0, 0, 0}, // seg  14
    {  96,   96,   98,   98,   97, 0, 0, 0}, // seg  14
    {  97,   97,   99,   99,   98, 0, 0, 0}, // seg  14
    {  98,   98,  100,  100,   99, 0, 0, 0}, // seg  14
    {  99,   99,  101,  101,  100, 0, 0, 0}, // seg  14
    { 100,  100,   29,  102,  101, 0, 0, 0}, // seg  14
    { 101,  101,  103,  103,  102, 0, 0, 0}, // seg  15
    { 102,  102,  104,  104,  103, 0, 0, 0}, // seg  15
    { 103,  103,  105,  105,  104, 0, 0, 0}, // seg  15
    { 104,  104,  106,  106,  105, 0, 0, 0}, // seg  15
    { 105,  105,  107,  107,  106, 0, 0, 0}, // seg  15
    { 106,  106,  108,  108,  107, 0, 0, 0}, // seg  15
    { 107,  107,  109,  109,  108, 0, 0, 0}, // seg  15
    { 108,  108,  110,  110,  109, 0, 0, 0}, // seg  16
    { 109,  109,  111,  111,  110, 0, 0, 0}, // seg  16
    { 110,  110,  112,  112,  111, 0, 0, 0}, // seg  16
    { 111,  111,  113,  113,  112, 0, 0, 0}, // seg  16
    { 112,  112,  114,  114,  113, 0, 0, 0}, // seg  16
    { 113,  113,  115,  115,  114, 0, 0, 0}, // seg  16
    { 114,  114,  116,  116,  115, 0, 0, 0}, // seg  16
    { 115,  115,  117,  117,  116, 0, 0, 0}, // seg  16
    { 116,  116,  118,  118,  117, 0, 0, 0}, // seg  17
    { 117,  117,  119,  119,  118, 0, 0, 0}, // seg  17
    { 118,  118,  120,  120,  119, 0, 0, 0}, // seg  17
    { 119,  119,  121,  121,  120, 0, 0, 0}, // seg  17
    { 120,  120,  122,  122,  121, 0, 0, 0}, // seg  17
    { 121,  121,  123,  123,  122, 0, 0, 0}, // seg  17
    { 122,  122,  124,  124,  123, 0, 0, 0}, // seg  17
    { 123,  123,  125,  125,  124, 0, 0, 0}, // seg  17
    { 124,  124,  126,  126,  125, 0, 0, 0}, // seg  17
    { 125,  125,  127,  127,  126, 0, 0, 0}, // seg  17
    { 126,  126,  128,  128,  127, 0, 0, 0}, // seg  18
    { 127,  127,  129,  129,  128, 0, 0, 0}, // seg  18
    { 128,  128,  130,  130,  129, 0, 0, 0}, // seg  18
    { 129,  129,  131,  131,  130, 0, 0, 0}, // seg  18
    { 130,  130,  255,  255,  131, 0, 0, 0}, // seg  18
};

typedef struct segment {
    uint16_t offset;
    uint16_t length;
    int8_t in0;
    int8_t in1;
    int8_t out0;
    int8_t out1;
    int8_t sign;
    int8_t twin;
} segment_t;

segment_t segments[20] = {
    // offset length in0 in1 out0 out1 sign twin
    {  0, 0, 0, 0, 0, 0, 0, 0},  // sentinel
    {  0, 5, 0, 0, 2, 0, 1,18},  // 1
    {  5,10, 1, 0, 3, 0, 1,17},  // 2
    { 15, 8, 2, 0, 4, 0, 1,16},  // 3
    { 23, 7, 3, 0,14, 0, 1,15},  // 4
    { 30, 5,14, 0, 6, 0, 1,13},  // 5
    { 35, 9, 5, 0, 7, 0, 1,12},  // 6
    { 44, 9, 6, 0, 8, 0, 1,11},  // 7
    { 53, 5, 7, 0, 9, 0, 1,10},  // 8
    { 58, 4, 8,10, 0, 0, 1, 0},  // 9  front segment
    { 62, 5,11, 0, 9, 0,-1, 8},  // 10
    { 67, 9,12, 0,10, 0,-1, 7},  // 11
    { 76, 9,13, 0,11, 0,-1, 6},  // 12
    { 85, 5,14, 0,12, 0,-1, 5},  // 13
    { 90,12, 4,15, 5,13,-1, 0},  // 14  middle segment
    {102, 7,16, 0,14, 0,-1, 4},  // 15
    {109, 8,17, 0,15, 0,-1, 3},  // 16
    {117,10,18, 0,16, 0,-1, 2},  // 17
    {127, 5, 0, 0,17, 0,-1, 1},  // 18
    {132, 0, 0, 0, 0, 0, 0, 0},  // sentinel
};

typedef struct particle {
    uint8_t valid;  // 0 ignore, 1 valid particle
    int8_t side;  // -1 left side, 0 both sides, 1 right side
    uint8_t seg;  // segment
    int8_t idx;  // index within the segment
    int8_t dir;  // direction of velocity
    uint32_t wait;  // inverse velocity (in micros per move)
    uint32_t last;  // last time it moved (in micros)
    uint8_t bright;  // brightness
    uint8_t updates;  // debug counter
} particle_t;

#define NUM_PARTICLES (30)
particle_t particles[NUM_PARTICLES] = {0};

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
    particle->side = (random(2) > 0) ? 1 : -1;
    particle->seg = seg;
    particle->idx = random(segments[seg].length);
    particle->wait = random(20000, 100000);
    particle->dir = (random(2) > 0) ? 1 : -1;
    particle->last = micros();
    particle->bright = random(8);
    particle->updates = 0;
}

void debugParticle(uint16_t i) {
    particle_t *particle = &particles[i];
    Serial.print("debugging particle: "); Serial.println(i);
    Serial.print("current micros "); Serial.println(micros());
    Serial.print("cast micros "); Serial.println((uint32_t)(micros()));
    Serial.print("current millis "); Serial.println(millis());
    Serial.print("valid "); Serial.println(particle->valid);
    Serial.print("side "); Serial.println(particle->side);
    Serial.print("seg "); Serial.println(particle->seg);
    Serial.print("idx "); Serial.println(particle->idx);
    Serial.print("wait "); Serial.println(particle->wait);
    Serial.print("dir "); Serial.println(particle->dir);
    Serial.print("last "); Serial.println(particle->last);
    Serial.print("next "); Serial.println(particle->last + particle->wait);
    Serial.print("bright "); Serial.println(particle->bright);
    Serial.print("updates "); Serial.println(particle->updates);
}

void testParticle() {
    particle_t *particle = &particles[0];
    particle->valid = 1;
    particle->side = -1;
    particle->seg = 1;
    particle->idx = 0;
    particle->wait = 100000;
    particle->dir = 1;
    particle->last = 0;
    particle->bright = 7;
    particle->updates = 0;
}

void updateParticle(particle_t *particle) {
    uint32_t current = micros();
    int8_t next_idx;
    segment_t *segment = &segments[particle->seg];
    if (!(particle->valid)) return;
    if (particle->last + particle->wait > current) return;
    particle->updates += 1;
    particle->last = current;
    next_idx = particle->idx + particle->dir * segment->sign;
    if (next_idx < 0) {
        if (segment->sign > 0) {
            if (particle->side >= 0 && segment->in1 != 0) {
                particle->seg = segment->in1;
            } else {  // TODO handle side == 0
                particle->seg = segment->in0;
            }
        } else {
            if (particle->side < 0 && segment->out1 != 0) {
                particle->seg = segment->out1;
            } else {  // TODO handle side == 0
                particle->seg = segment->out0;
            }
        }
        if (segment->sign * segments[particle->seg].sign > 0) {
            next_idx = segments[particle->seg].length - 1;
        } else {
            next_idx = 0;
        }
    } else if (next_idx >= segment->length) {
        if (segment->sign > 0) {
            if (particle->side < 0 && segment->out1 != 0) {
                particle->seg = segment->out1;
            } else {  // TODO handle side == 0
                particle->seg = segment->out0;
            }
        } else {
            if (particle->side < 0 && segment->in1 != 0) {
                particle->seg = segment->in1;
            } else {  // TODO handle side == 0
                particle->seg = segment->in0;
            }
        }
        if (segment->sign * segments[particle->seg].sign > 0) {
            next_idx = 0;
        } else {
            next_idx = segments[particle->seg].length - 1;
        }
    }
    particle->idx = next_idx;
    if (particle->seg < 1 || particle->seg > 18) {
        particle->valid = 0;
    }
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

void drawPixel(uint16_t index, uint8_t bright) {
    uint8_t value = 1 << bright;
    if (index >= 0 && index < STRIP_LEN) {
        pixels[index].r = value;
        pixels[index].g = value;
        pixels[index].b = value;
    }
}

void renderParticle(particle_t *particle) {
    segment_t *segment = &segments[particle->seg];
    uint16_t index = segment->offset + particle->idx;
    if (!(particle->valid)) return;
    drawPixel(index, particle->bright);
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

uint8_t window[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
uint16_t window_len = sizeof(window) / sizeof(window[0]);

void drawWindow(uint16_t i) {
  for (uint16_t j = 0; j < window_len; j++) {
    if (j + i < STRIP_LEN) {
      strip.setPixelColor(j + i, window[j], window[j], window[j]);
    }
  }
}

void chase() {
  uint16_t len = strip.numPixels();

  for (uint16_t i = 0; i < STRIP_LEN + window_len; i++) {
    blank();
    drawWindow(i);
    strip.show();
    delay(50);
  }
}

void blank() {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
}

void colorMask() {
  uint16_t offset = 0;
  uint16_t count = 1;  // Start at 1 to see light on first strand
  colorSegment(5, offset, count);  offset += 5; count += 1;
  colorSegment(10, offset, count);  offset += 10; count += 1;
  colorSegment(8, offset, count);  offset += 8; count += 1;
  colorSegment(7, offset, count);  offset += 7; count += 1;
  colorSegment(5, offset, count);  offset += 5; count += 1;
  colorSegment(9, offset, count);  offset += 9; count += 1;
  colorSegment(9, offset, count);  offset += 9; count += 1;
  colorSegment(5, offset, count);  offset += 5; count += 1;
  colorSegment(4, offset, count);  offset += 4; count += 1;
  colorSegment(5, offset, count);  offset += 5; count += 1;
  colorSegment(9, offset, count);  offset += 9; count += 1;
  colorSegment(9, offset, count);  offset += 9; count += 1;
  colorSegment(5, offset, count);  offset += 5; count += 1;
  colorSegment(12, offset, count);  offset += 12; count += 1;
  colorSegment(7, offset, count);  offset += 7; count += 1;
  colorSegment(8, offset, count);  offset += 8; count += 1;
  colorSegment(10, offset, count);  offset += 10; count += 1;
  colorSegment(5, offset, count);  offset += 5; count += 1;
  // Fake 3 at the end
  colorSegment(3, offset, count);  offset += 3; count += 1;
  strip.show();
}

void colorSegment(uint16_t len, uint16_t offset, uint16_t count) {
  uint16_t i;
  Serial.print(" len ");  Serial.print(len);
  Serial.print(" offset ");  Serial.print(offset);
  Serial.print(" count ");  Serial.print(count);
  Serial.println("");
  for (i = offset; i < offset + len; i++) {
    strip.setPixelColor(i, 132 - offset, 0, offset - 132);
  }
}
