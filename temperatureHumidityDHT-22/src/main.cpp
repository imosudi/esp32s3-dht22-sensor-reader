// Humidity and temperature reader using DHT-22 temperature sensor (ESP32-S3)
#include <Arduino.h>

const uint8_t DHT_PIN = 4;

volatile bool start_read = false;
volatile bool capture_done = false;

constexpr int edge_limit = 90;

volatile uint32_t edges[edge_limit];  // store up to the maximu number of edge_limit edges (enough for 40 bits + start)
volatile uint8_t edge_count = 0;
int captureCount = 1;

hw_timer_t *bit_timer = nullptr;

float humidity = 0.0;
float temperature = 0.0;


void IRAM_ATTR dht_isr() {
  if (edge_count < edge_limit) {
    edges[edge_count++] = micros();
  }
}


void IRAM_ATTR onTimer() {
  start_read = true;  // only set flag
}

void start_dht_read() {
  capture_done = false;
  edge_count = 0;

  // Send start signal
  pinMode(DHT_PIN, OUTPUT);
  digitalWrite(DHT_PIN, LOW);
  delay(2); // >1ms
  digitalWrite(DHT_PIN, HIGH);
  delayMicroseconds(30);

  // Switch to input and wait for response
  pinMode(DHT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(DHT_PIN), dht_isr, CHANGE);
}

bool decode_dht() {
  if (edge_count < 82) return false; // not enough data

  uint8_t data[5] = {0};

  // Parse pulses: high pulse length determines bit
  for (int i = 0; i < 40; i++) {
    uint32_t low = edges[2 + i*2];      // falling edge
    uint32_t high = edges[3 + i*2];     // next rising edge
    uint32_t pulse = high - low;

    if (pulse > 50) {
      data[i/8] |= (0x80 >> (i % 8));
    }
  }

  detachInterrupt(digitalPinToInterrupt(DHT_PIN));

  uint8_t checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
  if (checksum != data[4]) {
    Serial.println("Checksum error");
    return false;
  }

  uint16_t raw_hum = (data[0] << 8) | data[1];
  uint16_t raw_temp = (data[2] << 8) | data[3];

  humidity = raw_hum / 10.0f;
  if (raw_temp & 0x8000) {
    raw_temp &= 0x7FFF;
    temperature = -(raw_temp / 10.0f);
  } else {
    temperature = raw_temp / 10.0f;
  }

  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-S3 DHT22 Reader (Interrupt + Timer safe)");

  pinMode(DHT_PIN, INPUT_PULLUP);

  // Timer: 5s
  bit_timer = timerBegin(0, 80, true); // 1µs tick
  timerAttachInterrupt(bit_timer, &onTimer, true);
  timerAlarmWrite(bit_timer, 5000000, true);
  timerAlarmEnable(bit_timer);
}

void loop() {
  if (start_read) {
    start_read = false;
    start_dht_read();
  }

  // when enough edges captured
  if (!capture_done && edge_count >= 82) {
    capture_done = true;
    if (decode_dht()) {
      //Serial.printf("Humidity: %.1f %% | Temp: %.1f °C\n", humidity, temperature);
      //Serial.printf("%d. Humidity: %.1f %% | Temp: %.1f °C\n", captureCount, humidity, temperature);
      Serial.printf("%2d: %.1f%%  %.1fC\n", captureCount, humidity, temperature);
      captureCount++;
    }
  }
}




