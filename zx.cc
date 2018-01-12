
#include <Wire.h>
#include <ZX_Sensor.h>

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

void setup() {

  uint8_t ver;

  // Initialize Serial port
  Serial.begin(9600);
  Serial.println("Hello, world.");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Initialize ZX Sensor (configure I2C and read model ID)
  if ( zx_sensor.init() ) {
    Serial.println("ZX Sensor initialization complete");
  } else {
    Serial.println("Something went wrong during ZX Sensor init!");
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
    Serial.println("NOT! LOL! ROFLMao!");
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

void loop() {
  // If there is position data available, read and print it
  if ( zx_sensor.positionAvailable() ) {
    uint8_t x_pos, z_pos;
    x_pos = zx_sensor.readX();
    if ( x_pos != ZX_ERROR ) {
      Serial.print("X: ");
      Serial.print(x_pos);
      x_hist[x_hist_i] = x_pos;
      x_hist_i = (x_hist_i + 1) % NOSE_HISTORY;
    }
    z_pos = zx_sensor.readZ();
    if ( z_pos != ZX_ERROR ) {
      Serial.print(" Z: ");
      Serial.println(z_pos);
      z_hist[z_hist_i] = z_pos;
      z_hist_i = (z_hist_i + 1) % NOSE_HISTORY;
      analogWrite(LED_BUILTIN, z_pos);
    }
  }
}
