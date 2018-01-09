// Testing Mask sensors with Trinket M0
#include <Adafruit_DotStar.h>
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET

// Trinket M0 DotStar onboard
Adafruit_DotStar strip = Adafruit_DotStar(1, 7, 8, DOTSTAR_BRG);

// MICROPHONE TEST
// WIRING: (Trinket-Microphone) GND-GND 3V-VDD 1-OUT
// Measured values are clipped to [MIC_LO, MIC_HI]
#define MIC_LO 870  // Threshold for what we care about
#define MIC_HI 1020  // Maximum of what we expect
// Notes: needs some sort of low-pass filter, otherwise tap/snaps/etc set it off
// If its *right* in front of my mouth, it does what I want, I think

void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  Serial.begin(9600);
  pinMode(1, INPUT);  // Read microphone
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
}

void loop() {
  // Microphone test - shine LED blue with magnitude of volume
  int32_t volume = analogRead(1);
  int32_t clippd = constrain(volume, MIC_LO, MIC_HI);
  int32_t bright = map(clippd, MIC_LO, MIC_HI, 0, 255);
  Serial.println(volume);
  strip.setPixelColor(0, bright); strip.show();  // GRB -> brightness is all in blue
}
