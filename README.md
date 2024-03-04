<p align="center">
  <img src="https://github.com/Den-Sec/BLOOMBOX-Smart-Greenhouse/blob/main/assets/images/logo.png" alt="BLOOMBOX Logo" width="150">
</p>

# BLOOMBOX - Smart Greenhouse

Welcome to BLOOMBOX, the next-generation smart greenhouse designed to empower plant growth through precision automation, monitoring, and innovative data management. This cutting-edge system not only fosters the perfect environment for plants but also connects you to a global community of growers through a shared data platform.

## Project Overview 🌱

BLOOMBOX is engineered to create an optimal environment for plant growth, utilizing advanced sensors and actuators to regulate climate, hydration, and lighting—regardless of external weather conditions. A standout feature is its ability to securely send data to a web server, enabling both personal and communal insights through a comprehensive dashboard. This feature bridges the gap between local care and global data sharing, enhancing the growing experience.

## Core Features 🌟

- **Automated Climate Control**: Dynamic regulation of temperature and humidity with responsive fan systems.
- **Precision Irrigation**: Smart water pumps and soil moisture sensors to deliver precise hydration.
- **Data-Driven Insights**: Real-time monitoring and data logging for trend analysis, with secure data transmission to a web server for personal and global insights and also locally on SD card.
- **Optimized Lighting**: Intelligent light management with automated growing lights for maximum yield.
- **Scheduled Operations**: RTC module for time-critical tasks ensuring consistent plant care routines.
- **Global Community Dashboard**: Access to a global dashboard that aggregates data from BLOOMBOX devices worldwide, fostering a community of informed and connected growers.

## Enhanced Hardware Suite 🛠️

- **ESP32-WROOM-32 Microcontroller**: The powerhouse of BLOOMBOX, orchestrating all tasks seamlessly on multiple cores.
- **DHT22 Sensor**: Sensor for humidity and ambient temperature readings.
- **Soil & Water Sensors**: For vigilant monitoring of moisture levels and reservoir status.
- **Automated Water Pump**: Ensures your plants are watered just right, without waste.
- **SD Card & RTC Modules**: For meticulous record-keeping and precise time management.
- **Relay Modules**: The digital switchboard for your fans, pumps, and lights.
- **Fans & Growing Light**: A breath of fresh air and a spectrum of light to simulate natural conditions.
- **Power Supply**: Robust energy source designed for uninterrupted greenhouse operations.

## Data Management and Global Dashboard 📊

BLOOMBOX goes beyond local monitoring by sending data to a secure web server, where you can access comprehensive insights about your greenhouse. The data collected from your BLOOMBOX is not only accessible in detailed formats but also contributes to a global dashboard. This innovative feature allows you to view aggregated data and general information about other devices worldwide, fostering a community of informed growers.

- **Data Transmission**: Securely sends data to a web server for storage and analysis.
- **Personal Dashboard**: View detailed reports and insights specific to your BLOOMBOX setup.
- **Global Dashboard**: Access a communal dashboard displaying generalized data from BLOOMBOX devices around the world.

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

1. **Clone the Repository**: Start by cloning this repository to your local machine. This will give you access to all necessary files for your BLOOMBOX setup.

2. **Hardware Assembly**:
   - Navigate to the `/hardware/schematics` folder within the cloned repository.
   - Follow the provided schematics to assemble your hardware components accurately. This foundational step is crucial for the functionality of your BLOOMBOX.

3. **Enclosure Assembly**:
   - Proceed to the /hardware/enclosure folder for guidance on assembling your greenhouse enclosure.
   - Follow the detailed designs to construct the enclosure that will house your BLOOMBOX setup, ensuring a stable and protective environment for your plants.

4. **Code Deployment**:
   - Find your way to the /src directory, where the source code resides.
   - Deploy this code onto your microcontroller, which is the brain behind the greenhouse's automated functions. This step brings your BLOOMBOX to life.

5. **Smart Greenhouse Activation**:
   - Double-check all connections and configurations to ensure everything is set up correctly.
   - Power on your microcontroller (e.g., ESP32) and allow it a moment to initialize. This action activates your smart greenhouse system.
     
6. **Initialize your device**:
   - Use your smartphone to connect to the Wi-Fi access point named "BLOOMBOX-AP". This network is created by your BLOOMBOX device to facilitate initial setup.
   - Once connected, you will be automatically redirected to a setup portal. If the redirection does not happen automatically, open a web browser and navigate to a default IP address provided in the documentation (e.g., 192.168.4.1).
   - Follow the on-screen instructions within the portal to connect your BLOOMBOX to your home Wi-Fi network. This step ensures your device can communicate with the internet and access the server.
   - After establishing a Wi-Fi connection, your device will be prompted to enstablish a connection to the server. This will enable your BLOOMBOX to send and receive data securely.
   - Lastly, set up the relationship between your user account and the device. This may involve entering a device ID or scanning a QR code provided by your BLOOMBOX, then linking it to your user account on the BLOOMBOX platform. This step is crucial for accessing your personalized data and insights through the dashboard.

7. **Plant Configuration**:
   - Configure your BLOOMBOX to meet the specific needs of your plant by selecting the appropriate configuration file. BLOOMBOX supports a wide variety of plants, each with unique requirements for optimal growth. Our extensive library contains thousands of pre-defined configuration files, ensuring that you can find the perfect match for your plant. If your plant has specific needs not covered by our existing configurations, BLOOMBOX also offers the flexibility to create and upload a custom configuration file..
   - To upload the chosen configuration file to your device, connect to the BLOOMBOX interface through your preferred method (Web interface, or directly uploading to the SD card). Navigate to the 'Plant Configuration' section and select the option to upload a new configuration file. Once uploaded, BLOOMBOX will automatically adjust its settings to align with the requirements specified in the configuration file, ensuring your plant receives the precise care it needs for optimal growth.

Happy gardening!

## Contribute to BLOOMBOX 🤝

Join the BLOOMBOX movement! Contributions can range from code enhancements, feature suggestions, documentation improvements, and more. Check out [how to contribute](CONTRIBUTING.md) for more details.

## License 📄

BLOOMBOX is open-source and is proudly shared under the [MIT License](LICENSE).

## Stay Connected 📬

Have questions or ideas? Let's cultivate a conversation! Reach out to us [here](https://github.com/Den-Sec/BLOOMBOX-Smart-Greenhouse/issues) or send an email to `dennis.sepede@outlook.com`.

Join us in sowing the seeds for a greener future with BLOOMBOX! 🌼


We're excited to see how BLOOMBOX grows with the community's input!
