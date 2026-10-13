# 💧 Smart Water Level & Alternate Wetting and Drying (AWD) Monitoring System

An ESP32-based IoT prototype for monitoring water levels, identifying potential Alternate Wetting and Drying (AWD) threshold events, and transmitting sensor readings to a cloud dashboard. The project uses Wokwi for simulation and ThingSpeak for remote data visualization.

## 📌 Project Overview

Efficient water management is important in agriculture, particularly in rice cultivation, where continuous flooding can waste water. Alternate Wetting and Drying (AWD) is an irrigation technique in which fields are allowed to drain to a specified level before being irrigated again.

This project explores how an ESP32-based IoT system can support water-level monitoring and AWD decision-making by collecting readings, displaying the system status, and sending data to a cloud platform.

The current implementation is a simulation-based prototype. Physical sensor calibration and field validation are required before using it for real agricultural irrigation decisions.

## ✨ Key Features

- **Water-level monitoring:** Reads a simulated sensor value using a potentiometer in Wokwi.
- **AWD threshold analysis:** Evaluates water-depth readings against a configurable threshold.
- **Visual status indication:** Uses green and red LEDs to indicate system states.
- **OLED display:** Displays relevant readings and status information on an SSD1306 OLED.
- **IoT cloud integration:** Sends monitoring data to ThingSpeak for remote visualization.
- **Simulation-first development:** Enables testing of the firmware without initially requiring physical hardware.
- **Extensible architecture:** Provides a foundation for integrating calibrated field sensors and more robust data logging.

## 📸 Project Screenshots

### 1. ESP32 Simulation on Wokwi

The ESP32-based monitoring system can be simulated in Wokwi to test sensor readings, OLED output, and LED status indications.

![ESP32 Smart Water Level Monitoring System](images/wokwi-simulation.png)

### 2. ThingSpeak Cloud Dashboard

ThingSpeak provides charts for visualizing the monitoring data uploaded by the ESP32.

![ThingSpeak Water Level Monitoring Dashboard](images/thingspeak-dashboard.png)

## 🏗️ System Architecture

```text
     Simulated Water-Level Sensor
             (Wokwi)
                 |
                 v
              ESP32
                 |
       Read and Process Data
                 |
                 v
         AWD Threshold Check
                 |
        +--------+--------+
        |                 |
        v                 v
   OLED / LEDs      Status and Data
        |                 |
        |                 v
        |           Wi-Fi Connection
        |                 |
        |                 v
        |             ThingSpeak
        |                 |
        |                 v
        |        Cloud Charts and Trends
        |
        v
   Local Monitoring
```

## 🔧 Hardware and Components

The following components are used in, or intended for, the prototype.

| Component | Purpose |
|---|---|
| ESP32 development board | Reads inputs and runs the monitoring logic |
| Potentiometer in Wokwi | Simulates the water-level sensor output |
| SSD1306 OLED display | Displays readings and system status |
| Green LED | Indicates the configured normal status |
| Red LED | Indicates the configured alert status |
| Wi-Fi connection | Enables communication with ThingSpeak |
| Buzzer (optional) | Can provide an audible alert when configured and tested |

For physical deployment, the simulated potentiometer must be replaced with a suitable, calibrated water-level measurement device.

## 🔌 Pin Configuration

The current Wokwi prototype uses the following pin assignments.

| Component | ESP32 GPIO |
|---|---|
| Simulated sensor input (potentiometer) | GPIO 33 |
| Green LED | GPIO 25 |
| Red LED | GPIO 26 |
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |

**Note:** Verify the actual circuit in `diagram.json` and the firmware before assembling physical hardware. Pin assignments may need to change for a real sensor.

## 🧠 How It Works

1. **Initialization:** The ESP32 initializes its peripherals, display, and network configuration.
2. **Sensor reading:** The program reads the potentiometer value, which acts as a simulated water-level sensor.
3. **Data processing:** The raw reading is converted into the project's configured water-level or water-depth representation.
4. **AWD evaluation:** The calculated water depth is compared with the configured AWD threshold.
5. **Status indication:** The OLED and LEDs provide a local indication of the current reading and status.
6. **Cloud transmission:** When Wi-Fi is available, the system sends configured data fields to ThingSpeak.
7. **Visualization:** ThingSpeak charts allow users to inspect uploaded readings and changes over time.
8. **Repeated monitoring:** The process repeats according to the firmware's measurement interval.

### AWD Threshold

The prototype uses a nominal threshold of **−20 cm** for its AWD evaluation.

The interpretation of this value depends on the reference point and sign convention used for water depth. Before field deployment, the measurement reference, sensor installation, and threshold logic must be verified against actual field conditions. A threshold event should not automatically be interpreted as proof that irrigation is required.

## ☁️ ThingSpeak Cloud Integration

ThingSpeak is used to receive and visualize data uploaded by the ESP32.

**Cloud dashboard:** [Smart Water Level Monitoring — ThingSpeak](https://thingspeak.com/channels/3515295)

The project can use ThingSpeak charts to visualize uploaded measurements over time. The fields displayed on the dashboard should correspond to the fields configured in the firmware and the ThingSpeak channel.

To configure cloud communication:

1. Create or open your ThingSpeak channel.
2. Identify the channel fields used by the firmware.
3. Configure the Wi-Fi credentials and ThingSpeak Write API Key.
4. Upload the firmware or start the simulation.
5. Verify that new readings appear in the channel.

**Security:** Do not publish your Wi-Fi password or ThingSpeak Write API Key in source code or screenshots. Store credentials in a local configuration file excluded from Git, or use another suitable secrets-management method.

## 🧪 Run the Project

### Option 1: Run in Wokwi

1. Open the repository: [Smart-Water-Level-AWD-IoT-System](https://github.com/Kmishra987/Smart-Water-Level-AWD-IoT-System).
2. Open the project in an appropriate Wokwi environment.
3. Inspect `diagram.json` for the simulated circuit and `sketch.ino` for the firmware entry point.
4. Start the simulation.
5. Change the potentiometer value to simulate different sensor readings.
6. Observe the OLED and LED indications.
7. If cloud credentials and network configuration are valid, verify data transmission in ThingSpeak.

The potentiometer is a simulation input; it does not measure real water depth.

### Option 2: Build with PlatformIO

Install [Visual Studio Code](https://code.visualstudio.com/) and the [PlatformIO IDE extension](https://platformio.org/install/ide).

Clone the repository:

```bash
git clone https://github.com/Kmishra987/Smart-Water-Level-AWD-IoT-System.git
cd Smart-Water-Level-AWD-IoT-System
```

Build the firmware:

```bash
pio run
```

To upload to a connected ESP32, after configuring the correct serial port and board:

```bash
pio run -t upload
```

To view serial output:

```bash
pio device monitor
```

These commands assume PlatformIO Core is installed and available in your terminal. Alternatively, run the corresponding tasks from the PlatformIO toolbar in VS Code.

## 📂 Repository Structure

```text
Smart-Water-Level-AWD-IoT-System/
├── .vscode/          # Editor configuration
├── include/          # Header files
├── lib/              # Project-specific libraries
├── src/              # PlatformIO source directory
├── test/             # Test directory
├── diagram.json      # Wokwi circuit configuration
├── libraries.txt     # Library information
├── platformio.ini    # PlatformIO build configuration
├── sketch.ino        # Arduino-style firmware sketch
├── wokwi-project.txt # Wokwi project information
└── wokwi.toml        # Wokwi configuration
```

The exact location of the main firmware and the files used during compilation should be verified against `platformio.ini` and the current repository before modifying the project structure.

## 🧰 Technology Stack

- **Embedded development:** C++ and Arduino framework
- **Microcontroller:** ESP32
- **Development environment:** Visual Studio Code and PlatformIO
- **Simulation:** Wokwi
- **Display:** SSD1306 OLED over I²C
- **Cloud monitoring:** ThingSpeak
- **Communication:** Wi-Fi and HTTP-based cloud updates, where configured

## ⚠️ Limitations

- The water-level input currently uses a simulated potentiometer rather than a calibrated physical sensor.
- Simulation results do not establish accuracy under real field conditions.
- Wi-Fi outages can interrupt cloud transmission; reliable offline buffering and later synchronization must be verified before claiming data-loss protection.
- The −20 cm threshold must be validated for the selected sensor, installation geometry, and agricultural use case.
- Cloud dashboards depend on correctly configured API credentials, channel fields, and connectivity.
- Real-world deployment requires electrical protection, enclosure design, sensor calibration, and environmental testing.

## 🚀 Future Improvements

- Integrate a suitable field-ready water-level sensor.
- Calibrate raw sensor readings against manually measured water depth.
- Add robust offline data storage and automatic synchronization after connectivity returns.
- Implement sensor fault detection and communication-status indicators.
- Add alerts for threshold crossings and missing sensor readings.
- Evaluate the system using synchronized field measurements and independently verified AWD labels.
- Extend the platform to monitor multiple plots and compare water-level trends.

## 🌱 Applications

- Agricultural water-level monitoring
- Rice-field AWD monitoring research
- IoT-based irrigation experiments
- Water-resource management prototypes
- Remote sensor monitoring and visualization

## 👨‍💻 Author

**Kritish Kumar Mishra**

Computer Science and Engineering

National Institute of Technology Rourkela

## 📄 License

No license has been specified here. Add a `LICENSE` file and update this section if you intend to distribute the project under an open-source license.
