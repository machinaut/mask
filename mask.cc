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

pixel_t buffer[STRIP_LEN];

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
    showBuffer();
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
        buffer[index].r = value; buffer[index].g = value; buffer[index].b = value;
    }
}

void renderParticle(particle_t *particle) {
    segment_t *segment = &segments[particle->seg];
    uint16_t index = segment->offset + particle->idx;
    if (!(particle->valid)) return;
    drawPixel(index, particle->bright);
}

void blankBuffer() {
    for (uint16_t i = 0; i < STRIP_LEN; i++) {
        buffer[i].r = 0; buffer[i].g = 0; buffer[i].b = 0;
    }
}

void renderParticles() {
    blankBuffer();
    for (uint8_t i = 0; i < NUM_PARTICLES; i++) {
        renderParticle(&particles[i]);
    }
}

void showBuffer() {
    for (uint16_t i = 0; i < STRIP_LEN; i++) {
        strip.setPixelColor(i, buffer[i].r, buffer[i].g, buffer[i].b);
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
