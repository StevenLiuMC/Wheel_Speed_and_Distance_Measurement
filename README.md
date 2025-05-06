# 🚴 Wheel Distance and Speed Measurement System

## 📖 Project Overview

This project designs a device that measures bicycle speed and distance. The system uses photoelectric sensors to collect pulse signals from each wheel rotation, which are then processed by a microcontroller to calculate and display real-time speed and distance data. This implementation is based on the STM32 microcontroller.

## 🌟 Main Features

- Real-time wheel speed measurement and travel speed calculation  
- Cumulative distance calculation  
- Real-time travel duration display  
- Sound and light alarm for excessive speed  
- Serial communication (allowing alarm threshold modification via PC)  

## 🧱 System Architecture

The system is divided into the following modules:

- **Motor Control Module**: Simulates bicycle wheel rotation to provide pulses for the sensor  
- **Sensor Module**: Collects wheel rotation pulse signals using photoelectric sensors  
- **Sound and Light Alarm Module**: Provides warnings when speed exceeds the set threshold  
- **Display Module**: Shows speed, distance, and time data  
- **Communication Module**: Enables data interaction with a PC  

## 🔩 Hardware Components

**STM32 Implementation**:
- **Main Controller**: STM32F103C8T6  
- **Display Module**: OLED (128×64)  
- **Sensor**: Photoelectric through-beam sensor  
- **Additional Components**: Sound and light alarm circuit, motor drive circuit  

## 🧠 Software Design

### 📐 Main Algorithm

The STM32 implementation uses the **period measurement method** to calculate speed. An internal timer generates a 0.0005s unit time interval, and the count value is recorded each time the sensor is triggered.  
**Calculation Formula**:  Speed = 0.1 ÷ (0.0005s × N)
This method provides higher precision than the frequency measurement method, especially for low-frequency signals.

### 🛠️ Key Functions

- Real-time speed calculation  
- Distance accumulation  
- Time calculation  
- Sound and light alarm control  
- Serial communication (supporting hexadecimal data transmission)  

## 📋 User Instructions

1. After powering on, the system displays the initial interface including group number and alarm speed value.  
2. The alarm threshold can be modified by sending hexadecimal data through the serial port (range: 0–255).  
3. When speed exceeds the set threshold, the system automatically triggers the sound and light alarm.  
4. The OLED screen continuously displays:
   - Current speed  
   - Cumulative distance  
   - Elapsed running time  

## 🧑‍💻 Software Implementation Details

The program uses a modular design approach, relying on STM32 standard library functions for improved readability and maintainability. Interrupt-driven design ensures precise timing and efficient sensor signal processing.

### 🔑 Core Functions

- `IC_GetFreq()` – Captures waveform frequency for speed calculation  
- `Distance_Cal()` – Calculates travel distance  
- `ShowTitle()` – Displays title information on OLED  
- `Show_Info_TTI()` – Sends information to the PC via serial port  
- `Hex_Dec_Convert()` – Converts hexadecimal numbers to decimal strings
