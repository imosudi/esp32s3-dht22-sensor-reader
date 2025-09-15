# # ESP32 DHT-22 Interrupt Reader
ESP32-S3 DHT-22 sensor library with interrupt-based edge detection and hardware timer scheduling. Provides reliable temperature and humidity readings every 5 seconds with minimal CPU overhead and automatic error handling.



Efficient DHT-22 temperature and humidity sensor reader for ESP32-S3 using interrupt-driven data capture and timer-based sampling.

## Features

- **Non-blocking operation** - Uses interrupts and timers
- **Automatic sampling** - Reads every 5 seconds
- **Precise timing** - Microsecond-level edge detection
- **Data validation** - Built-in checksum verification
- **Minimal CPU overhead** - Interrupt-driven capture

## Hardware Requirements

- ESP32-S3 development board
- DHT-22 temperature/humidity sensor


## Wiring

```
DHT-22    ESP32-S3	Logic Analyzer		Purpose
------    --------	-------------		-------
VCC   →   3.3V					Vc+
DATA  →   GPIO 4 	CH1			DHT22 data line
GND   →   GND	→	GND			Common ground
```

## Usage

1. Connect the DHT-22 sensor according to the wiring diagram
2. Upload the code to your ESP32-S3
3. Open Serial Monitor (115200 baud)
4. Temperature and humidity readings will appear every 5 seconds

## Output Format

```
 1: 65.9%  25.5C
 2: 66.1%  25.4C
 3: 66.1%  25.4C
```

## Configuration

- **Sampling interval**: Modify `timerAlarmWrite(bit_timer, 5000000, true)` (microseconds)
- **Data pin**: Change `DHT_PIN` constant (default: GPIO 4)

## How It Works

1. Hardware timer triggers readings every 5 seconds
2. Start signal sent to DHT-22 (low pulse + high pulse)
3. GPIO interrupt captures all edge transitions
4. Pulse widths decoded to extract 40-bit data
5. Checksum validation ensures data integrity
6. Temperature and humidity calculated from raw values