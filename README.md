<p align="center">
  <img src="https://github.com/Den-Sec/BLOOMBOX-Smart-Greenhouse/assets/156823722/289e457c-592f-4e86-87cc-6aa00146be92" alt="BLOOMBOX Logo" width="150">
</p>

# BLOOMBOX - Smart Greenhouse

Welcome to BLOOMBOX, the next-generation smart greenhouse designed to empower plant growth through precision automation and monitoring

## Project Overview 🌱

BLOOMBOX is engineered to foster a perfect environment for plants, leveraging advanced sensors and actuators to control climate, hydration, and lighting—irrespective of external weather patterns.

## Core Features 🌟

- **Automated Climate Control**: Dynamic regulation of temperature and humidity with responsive fan systems.
- **Precision Irrigation**: Smart water pumps and soil moisture sensors to deliver precise hydration.
- **Data-Driven Insights**: Real-time monitoring with data logging for trend analysis via an SD card.
- **Optimized Lighting**: Intelligent light management with automated growing lights for maximum yield.
- **Scheduled Operations**: RTC module for time-critical tasks ensuring consistent plant care routines.

## Enhanced Hardware Suite 🛠️

- **ESP32 Microcontroller**: The powerhouse of BLOOMBOX, orchestrating all tasks seamlessly.
- **DHT11 & MLX90614 Sensors**: Dual sensor setup for humidity and ambient temperature readings.
- **Soil & Water Sensors**: For vigilant monitoring of moisture levels and reservoir status.
- **Automated Water Pump**: Ensures your plants are watered just right, without waste.
- **SD Card & RTC Modules**: For meticulous record-keeping and precise time management.
- **Relay Modules**: The digital switchboard for your fans, pumps, and lights.
- **Fans & Growing Light**: A breath of fresh air and a spectrum of light to simulate natural conditions.
- **Power Supply**: Robust energy source designed for uninterrupted greenhouse operations.

## Design and Schematics

### Overview of BLOOMBOX Schematics

Get an insight into the BLOOMBOX setup with our detailed schematic. This illustrates how various components of BLOOMBOX are interconnected. For detailed pinout information for each module, refer to our pinout guide in the repository at  `/hardware/schematics/Pinouts.md`.

<p align="center">
  <img src="hardware/schematics/Schematics BLOOMBOX.png" alt="BLOOMBOX Schematic">
</p>

### BLOOMBOX in Action:

Below are images showcasing the final assembly of BLOOMBOX. These renders give you a glimpse of what the setup looks like from different angles, providing a better understanding of its design and functionality.

<p align="center">
  <table>
    <tr>
      <td>
        <img src="hardware/enclosure/3D-Renders/BLOOM-BOX Inside.png" alt="BLOOM-BOX Inside">
      </td>
      <td>
        <img src="hardware/enclosure/3D-Renders/BLOOM-BOX 45' Render.png" alt="BLOOMBOX 45' Render">
      </td>
      <td>
        <img src="hardware/enclosure/3D-Renders/BLOOM-BOX Front Render.png" alt="BLOOM-BOX Front Render">
      </td>
    </tr>
  </table>
</p>

## Quickstart Guide 🚀

Begin your BLOOMBOX journey with these simple steps:

1. **Clone the Repository**: Start by cloning this repository to your local machine.

2. **Hardware Assembly**:
   - Navigate to the `/hardware/schematics` folder.
   - Assemble your hardware components following the schematics provided.

3. **Enclosure Assembly**:
   - Navigate to the `/hardware/enclosure` folder.
   - Assemble your greenhouse enclosure as shown in the provided designs.

4. **Code Deployment**:
   - Navigate to the `/src` directory.
   - Deploy the code onto your ESP32 to control the greenhouse's functions.

5. **Plant Configuration**:
   - Locate the specific configuration file for your plant type. Each plant has its own configuration file.
   - Upload the configuration file onto the SD card connected to the ESP32. This file contains the specifications and settings for your specific plant.

6. **Smart Greenhouse Activation**:
   - Ensure all components are properly connected and configured.
   - Power on the ESP32 and watch your plants thrive in their new smart environment!

Happy gardening!

## Contribute to BLOOMBOX 🤝

Join the BLOOMBOX movement! Contributions can range from code enhancements, feature suggestions, documentation improvements, and more. Check out [how to contribute](CONTRIBUTING.md) for more details.

## License 📄

BLOOMBOX is open-source and is proudly shared under the [MIT License](LICENSE).

## Stay Connected 📬

Have questions or ideas? Let's cultivate a conversation! Reach out to us [here](https://github.com/Den-Sec/BLOOMBOX-Smart-Greenhouse/issues) or send an email to `dennis.sepede@outlook.com`.

Join us in sowing the seeds for a greener future with BLOOMBOX! 🌼


We're excited to see how BLOOMBOX grows with the community's input!
