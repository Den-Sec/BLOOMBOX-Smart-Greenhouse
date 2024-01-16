# BLOOMBOX - Smart Greenhouse

Welcome to BLOOMBOX, an innovative smart greenhouse project designed to optimize plant growth, automate essential processes, and monitor your greenhouse environment with precision.

## Project Overview

BLOOMBOX aims to create a controlled environment to cultivate plants effectively regardless of the external weather conditions. Using a combination of sensors and actuators, the system can regulate temperature, humidity, soil moisture, and water levels to ensure optimal plant health and productivity.

## Features

- **Automated Climate Control**: Regulate temperature and humidity using fans and a growing light to mimic natural conditions.
- **Precision Irrigation**: Manage watering cycles effectively using soil moisture sensors and automated water pumps.
- **Real-Time Monitoring and Data Logging**: Monitor environmental conditions in real-time and log data to an SD card for trend analysis.
- **Lighting Management**: Automated growing light control to ensure plants receive the optimal light for growth.
- **Time-Sensitive Operations**: RTC module for managing time-critical functions like light cycles and irrigation schedules.


## Hardware Components

- ESP32 Microcontroller: The central processing unit of BLOOMBOX, coordinating all automated tasks and processing sensor data.
- DHT11 Temperature and Humidity Sensor: Monitors the ambient temperature and humidity to maintain the ideal climate.
- MLX90614 Infrared Thermometer: Provides precise temperature readings for advanced environmental control.
- Soil Moisture Sensor: Checks the moisture level of the soil to inform irrigation decisions.
- Water Pump: Automates the irrigation process, ensuring plants receive the right amount of water.
- Water Level Sensor: Monitors the water reservoir level to prevent overflow or dry running conditions.
- SD Card Module: Facilitates data logging for environmental conditions and irrigation patterns.
- RTC Module: Real-Time Clock for maintaining accurate timekeeping even after power cycles, crucial for timing-based tasks.
- Relay Modules: Acts as digital switches for controlling fans, pumps, and lights.
- Fans: Used for air circulation to regulate temperature and humidity for plant health.
- Power Supply: A reliable power source to ensure continuous operation of the greenhouse system.
- Growing Light: Provides the necessary light spectrum to support plant growth, controllable to simulate natural light cycles.


## Repository Structure

- `/code`: All the source code files for the BLOOMBOX project.
- `/schematics`: Electronic schematics and wiring diagrams.
- `/docs`: Documentation and additional project resources.

## Getting Started

To get started with BLOOMBOX, clone this repository and review the hardware setup in the `/schematics` folder. Set up your hardware according to the diagrams, then upload the code from the `/code` folder to your ESP32.

## Contribution

Interested in contributing to BLOOMBOX? We welcome contributions of all forms, from code improvements and bug fixes to hardware suggestions and documentation enhancements.

## License

This project is licensed under the [MIT License](LICENSE) - see the LICENSE file for details.

## Contact

For questions, suggestions, or collaborations, feel free to reach out. You can [open an issue](https://github.com/Den-Sec/BLOOMBOX-Smart-Greenhouse/issues) here on GitHub or contact us directly at dennis.sepede@outlook.com`.

We're excited to see how BLOOMBOX grows with the community's input!
