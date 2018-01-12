// Test the mask strand with a Trinket M0

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <Wire.h>
#include <ZX_Sensor.h>


#define PIN 4  // Trinket M0

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
    uint8_t bright;  // brightness
    uint8_t glow;  // decrease in brightness nearby
    uint8_t hue;  // color of the pixel
    uint8_t updates;  // debug counter
} particle_t;

#define NUM_PARTICLES (10)
particle_t particles[NUM_PARTICLES] = {0};
#define NUM_FIXED_PARTICLES (2)
particle_t fixed_particles[NUM_FIXED_PARTICLES] = {0};

typedef struct path {
    step_t *path;
    uint16_t length;
} path_t;

step_t path_l[] = {
{  0,  -1}, /* seg None */{  1,  -1}, /* seg None */{  2,  -1}, /* seg None */{  3,  -1}, /* seg None */{  4,  -1}, /* seg None */{  5,  -1}, /* seg None */{  6,  -1}, /* seg None */{  7,  -1}, /* seg None */{  8,  -1}, /* seg None */{  9,  -1}, /* seg None */{ 10,  -1}, /* seg None */{ 11,  -1}, /* seg None */{ 12,  -1}, /* seg None */{ 13,  -1}, /* seg None */{ 14,  -1}, /* seg None */{ 15,  -1}, /* seg None */{ 16,  -1}, /* seg None */{ 17,  -1}, /* seg None */{ 18,  -1}, /* seg None */{ 19,  -1}, /* seg None */{ 20,  -1}, /* seg None */{ 21,  -1}, /* seg None */{ 22,  -1}, /* seg None */{ 23,  -1}, /* seg None */{ 24,  -1}, /* seg None */{ -1,  -1}, /* seg None */{ -2,  -1}, /* seg None */{ -3,  -1}, /* seg None */{ -4,  -1}, /* seg None */{ -5,  -1}, /* seg None */{ -6,  -1}, /* seg None */{ -7,  -1}, /* seg None */{ -8,  -1}, /* seg None */{ -9,  -1}, /* seg None */{-10,  -1}, /* seg None */{-11,  -1}, /* seg None */{-12,  -1}, /* seg None */{-13,  -1}, /* seg None */{-14,  -1}, /* seg None */{-15,  -1}, /* seg None */{-16,  -1}, /* seg None */{-17,  -1}, /* seg None */{-18,  -1}, /* seg None */{-19,  -1}, /* seg None */{-20,  -1}, /* seg None */{ 25,  -1}, /* seg None */{ 26,  -1}, /* seg None */{ 27,  -1}, /* seg None */{ 28,  -1}, /* seg None */{ 29,  -1}, /* seg None */{ 30,  -1}, /* seg None */{ 31,  -1}, /* seg None */
};
uint16_t path_l_len = sizeof(path_l) / sizeof(path_l[0]);
step_t path_r[] = {
{  0,  -1}, /* seg None */{  1,  -1}, /* seg None */{  2,  -1}, /* seg None */{  3,  -1}, /* seg None */{  4,  -1}, /* seg None */{  5,  -1}, /* seg None */{  6,  -1}, /* seg None */{  7,  -1}, /* seg None */{  8,  -1}, /* seg None */{  9,  -1}, /* seg None */{ 10,  -1}, /* seg None */{ 11,  -1}, /* seg None */{ 12,  -1}, /* seg None */{ 13,  -1}, /* seg None */{ 14,  -1}, /* seg None */{ 15,  -1}, /* seg None */{ 16,  -1}, /* seg None */{ 17,  -1}, /* seg None */{ 18,  -1}, /* seg None */{ 19,  -1}, /* seg None */{ 20,  -1}, /* seg None */{ 21,  -1}, /* seg None */{ 22,  -1}, /* seg None */{ 23,  -1}, /* seg None */{ 24,  -1}, /* seg None */{ -1,  -1}, /* seg None */{ -2,  -1}, /* seg None */{ -3,  -1}, /* seg None */{ -4,  -1}, /* seg None */{ -5,  -1}, /* seg None */{ -6,  -1}, /* seg None */{ -7,  -1}, /* seg None */{ -8,  -1}, /* seg None */{ -9,  -1}, /* seg None */{-10,  -1}, /* seg None */{-11,  -1}, /* seg None */{-12,  -1}, /* seg None */{-13,  -1}, /* seg None */{-14,  -1}, /* seg None */{-15,  -1}, /* seg None */{-16,  -1}, /* seg None */{-17,  -1}, /* seg None */{-18,  -1}, /* seg None */{-19,  -1}, /* seg None */{-20,  -1}, /* seg None */{ 25,  -1}, /* seg None */{ 26,  -1}, /* seg None */{ 27,  -1}, /* seg None */{ 28,  -1}, /* seg None */{ 29,  -1}, /* seg None */{ 30,  -1}, /* seg None */{ 31,  -1}, /* seg None */
};
uint16_t path_r_len = sizeof(path_r) / sizeof(path_r[0]);
step_t path_b[] = {
{  0,  -1}, /* seg None */{  1,  -1}, /* seg None */{  2,  125}, /* seg 18 */{  3,  126}, /* seg 18 */{  4,  127}, /* seg 18 */{  5,  -1}, /* seg None */{  6,  -1}, /* seg None */{  7,  -1}, /* seg None */{  8,  -1}, /* seg None */{  9,  -1}, /* seg None */{ 10,  -1}, /* seg None */{ 11,  -1}, /* seg None */{ 12,  -1}, /* seg None */{ 13,  -1}, /* seg None */{ 14,  -1}, /* seg None */{ 15,  -1}, /* seg None */{ 16,  -1}, /* seg None */{ 17,  -1}, /* seg None */{ 18,  -1}, /* seg None */{ 19,  -1}, /* seg None */{ 20,  -1}, /* seg None */{ 21,  -1}, /* seg None */{ 22,  -1}, /* seg None */{ 23,  -1}, /* seg None */{ 24,  -1}, /* seg None */{ -1,  -1}, /* seg None */{ -2,  -1}, /* seg None */{ -3,  -1}, /* seg None */{ -4,  -1}, /* seg None */{ -5,  -1}, /* seg None */{ -6,  -1}, /* seg None */{ -7,  -1}, /* seg None */{ -8,  -1}, /* seg None */{ -9,  -1}, /* seg None */{-10,  -1}, /* seg None */{-11,  -1}, /* seg None */{-12,  -1}, /* seg None */{-13,  -1}, /* seg None */{-14,  -1}, /* seg None */{-15,  -1}, /* seg None */{-16,  -1}, /* seg None */{-17,  -1}, /* seg None */{-18,  -1}, /* seg None */{-19,  -1}, /* seg None */{-20,  -1}, /* seg None */{ 25,  -1}, /* seg None */{ 26,  -1}, /* seg None */{ 27,  -1}, /* seg None */{ 28,  -1}, /* seg None */{ 29,  -1}, /* seg None */{ 30,  -1}, /* seg None */{ 31,  -1}, /* seg None */
};
uint16_t path_b_len = sizeof(path_b) / sizeof(path_b[0]);

path_t paths[] = {
    {path_l, path_l_len},
    {path_r, path_r_len},
    {path_b, path_b_len},
};
uint16_t paths_len = sizeof(paths) / sizeof(paths[0]);


ZX_Sensor zx_sensor = ZX_Sensor(0x10);  // ZX Sensor I2C address
#define NOSE_HISTORY (20)
uint8_t x_pos;
uint32_t x_pos_last_micros;
uint8_t x_hist[NOSE_HISTORY];
uint8_t x_hist_i = 0;
uint8_t z_pos;
uint32_t z_pos_last_micros;
uint8_t z_hist[NOSE_HISTORY];
uint8_t z_hist_i = 0;


void randomParticle(particle_t *particle) {
    uint8_t seg = random(1, 19);
    particle->valid = 1;
    particle->wait = random(20000, 100000);
    particle->dir = (random(2) > 0) ? 1 : -1;
    particle->path_idx = random(paths_len);
    particle->i = 0; //particle->path[0] ?  // XXX
    particle->last = micros();
    particle->bright = random(100);
    particle->glow = random(100 - particle->bright, 100);  // TODO: tune this
    particle->hue = random(256);
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
    Serial.print("bright "); Serial.println(particle->bright);
    Serial.print("glow "); Serial.println(particle->glow);
    Serial.print("hue "); Serial.println(particle->hue);
    Serial.print("updates "); Serial.println(particle->updates);
}

void debugZX() {
    Serial.print("ZX Sensor: ");
    Serial.print("X: ");
    Serial.print(x_pos);
    Serial.print(" Z: ");
    Serial.println(z_pos);
}

void testParticle() {
    particle_t *particle = &particles[0];
    Serial.println("testParticle() writing out");
    particle->valid = 1;
    particle->i = 0;
    particle->path_idx = 1;
    particle->wait = 10000;
    particle->dir = 1;
    particle->last = 0;
    particle->bright = 80;
    particle->glow = 10;
    particle->hue = 0;
    particle->updates = 0;
}

void updateParticle(particle_t *particle) {
    uint32_t current = micros();
    if (!(particle->valid)) return;
    Serial.print("updating particle: "); Serial.println((uint32_t)(particle));
    Serial.print("updating last: "); Serial.println(particle->last);
    Serial.print("updating wait: "); Serial.println(particle->wait);
    Serial.print("updating next: "); Serial.println(particle->last + particle->wait);
    Serial.print("updating current: "); Serial.println(current);
    Serial.print("updating ready: "); Serial.println((uint8_t)(particle->last + particle->wait <= current));
    if (particle->last + particle->wait > current) return;
    particle->i += particle->dir;
    Serial.print("updated i: "); Serial.println(particle->i);
    if (particle->i < 0 || particle->i >= paths[particle->path_idx].length) {
        particle->valid = 0;
    }
    Serial.print("updated valid: "); Serial.println(particle->valid);
    particle->last = micros();
    Serial.print("updated last: "); Serial.println(particle->last);
    Serial.print("finished updating: "); Serial.println((uint32_t)(particle));
    particle->updates++;
}

void updateParticles() {
    particle_t *particle;
    for (uint8_t i = 0; i < NUM_PARTICLES; i++) {
        particle = &particles[i];
        updateParticle(particle);
        if (!(particle->valid)) {
            // randomParticle(particle);
        }
    }
}

void updateFixedParticle(uint8_t i, particle_t *particle) {
    Serial.print("Updating fixed particle: "); Serial.println((uint32_t)(particle));
    if (!(particle->valid)) return;
    if (i == 0 || i == 1) {
        particle->valid = 1;
        particle->path_idx = i;
        if (i == 0) {
            particle->i = map(x_pos, 0, 255, 0, paths[particle->path_idx].length);
        } else {
            particle->i = map(z_pos, 0, 255, 0, paths[particle->path_idx].length);
        }
        particle->bright = 80;
        particle->glow = 20;
        particle->hue = 0;
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

void drawPixel(int16_t i, uint8_t bright, uint8_t hue) {
    if (i >= 0 && i < STRIP_LEN) {
        // TODO: add hue
        // TODO: add gamma correction
        pixels[i].r += pgm_read_byte(&gamma8[bright]);
        // pixels[i].g += pgm_read_byte(&gamma8[bright]);
        pixels[i].b += pgm_read_byte(&gamma8[bright]);
    }
}

void renderParticle(particle_t *particle) {
    uint8_t bright;
    if (!(particle->valid)) return;
    Serial.print("rendering particle"); Serial.println((uint32_t)(particle));
    Serial.print("starting at: "); Serial.println(particle->i);
    // Forward glow
    bright = particle->bright;
    for (int16_t i = particle->i; i < paths[particle->path_idx].length; i++) {
        step_t *path = paths[particle->path_idx].path;
        Serial.print("drawing pixel: "); Serial.println(i);
        drawPixel(path[i].pixel_i, bright, particle->hue);
        if (bright < particle->glow) break;
        bright -= particle->glow;
    }
    // Backward glow
    bright = particle->bright;
    for (int16_t i = particle->i; i > 0; i--) {
        step_t *path = paths[particle->path_idx].path;
        drawPixel(path[i].pixel_i, bright, particle->hue);
        Serial.print("drawing pixel: "); Serial.println(i);
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
    for (uint8_t i = 0; i < NUM_FIXED_PARTICLES; i++) {
        renderParticle(&fixed_particles[i]);
    }
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
    for (uint16_t i = 0; i < STRIP_LEN; i++) {
        strip.setPixelColor(i, pixels[i].r, pixels[i].g, pixels[i].b);
    }
    strip.show();
}

void initZX(){
      uint8_t ver;
      // Initialize ZX Sensor (configure I2C and read model ID)
      if ( zx_sensor.init() ) {
        Serial.println("ZX Sensor initialization complete");
      } else {
        Serial.println("Something went wrong during ZX Sensor init!");
        // XXX
      }

      // Read the model version number and ensure the library will work
      ver = zx_sensor.getModelVersion();
      if ( ver == ZX_ERROR ) {
        Serial.println("Error reading model version number");
      } else {
        Serial.print("Model version: ");
        Serial.println(ver);
      }
      if ( ver != ZX_MODEL_VER ) {
        Serial.print("Model version needs to be ");
        Serial.print(ZX_MODEL_VER);
        Serial.println(" to work with this library. Ignoring sensor.");
        Serial.println("NOT! LOL! ROFLMao!"); // XXX
      }

      // Read the register map version and ensure the library will work
      ver = zx_sensor.getRegMapVersion();
      if ( ver == ZX_ERROR ) {
        Serial.println("Error reading register map version number");
      } else {
        Serial.print("Register Map Version: ");
        Serial.println(ver);
      }
      if ( ver != ZX_REG_MAP_VER ) {
        Serial.print("Register map version needs to be ");
        Serial.print(ZX_REG_MAP_VER);
        Serial.print(" to work with this library. Stopping.");
      }
}

void readZX() {
    if ( zx_sensor.positionAvailable() ) {
        x_pos = zx_sensor.readX();
        if ( x_pos != ZX_ERROR ) {
            x_pos_last_micros = micros();
            x_hist[x_hist_i] = x_pos;
            x_hist_i = (x_hist_i + 1) % NOSE_HISTORY;
        }
        z_pos = zx_sensor.readZ();
        if ( z_pos != ZX_ERROR ) {
            z_pos_last_micros = micros();
            z_hist[z_hist_i] = z_pos;
            z_hist_i = (z_hist_i + 1) % NOSE_HISTORY;
            analogWrite(LED_BUILTIN, z_pos);
        }
    }
}

void setup() {
    // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
    #if defined (__AVR_ATtiny85__)
        if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
    #endif
    // End of trinket special code

    Serial.begin(9600);
    Serial.println("Hello, world.");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    randomSeed(analogRead(1));
    initZX();
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    testParticle();
}

void loop() {
    Serial.println("loop:");
    readZX();
    debugZX();
    updateParticles();
    updateFixedParticles();
    debugParticle(0);
    renderParticles();
    debugPixels();
    showPixels();
}
