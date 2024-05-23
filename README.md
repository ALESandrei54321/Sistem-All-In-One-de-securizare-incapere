# Smart Security System

## Overview
This Arduino project is a comprehensive smart security system that combines multiple security features such as fingerprint recognition, RFID card access, keypad password entry, and environmental monitoring (temperature, humidity, and smoke detection). The system is designed to ensure robust access control and safety in a secure and user-friendly manner.

## Components
    - Arduino Board
    - LiquidCrystal_I2C (LCD Display)
    - Adafruit_Fingerprint Sensor
    - MFRC522 RFID Reader
    - I2CKeyPad
    - DHT11 Temperature and Humidity Sensor
    - MQ2 Gas Sensor
    - Buzzer
    - Relay Module (for lock control)
    - LED
    - Wires and Breadboard

## Features
    - Fingerprint Recognition: Uses the Adafruit Fingerprint sensor to authenticate users based on their fingerprints.
    - RFID Card Access: Utilizes the MFRC522 module to read RFID cards and grant access based on card UID.
    - Keypad Password Entry: Users can enter a password via an I2C keypad for authentication.
    - Environmental Monitoring: Monitors temperature using a DHT11 sensor and smoke levels using an MQ2 sensor.
    - Buzzer Alerts: Provides auditory feedback and alerts using a buzzer.
    - LCD Display: Displays system messages and status updates on an I2C LCD.

## Setup
Circuit Connections:

    - Connect the LCD to the I2C pins of the Arduino.
    - Connect the fingerprint sensor to the designated software serial pins.
    - Connect the MFRC522 RFID reader to the SPI pins.
    - Connect the keypad to the I2C bus.
    - Connect the DHT11 sensor to a digital pin.
    - Connect the MQ2 sensor to an analog pin.
    - Connect the relay module to the lock control pin.
    - Connect the buzzer to a digital pin.

## Libraries

Install the following libraries in the Arduino IDE:

    - LiquidCrystal_I2C
    - Adafruit_Fingerprint
    - Wire
    - MFRC522
    - I2CKeyPad
    - TimerOne
    - DHT
    - SoftwareSerial

## Configuration

1. Define the constants for pins, UID for RFID cards, and the password in the code.

2. Use the Adafruit example sketch to enroll fingerprints into the sensor before using this system.

## Usage

### Startup:
- On powering up, the system initializes and displays "Setting up..." on the LCD.
- The startup function ensurest that all components are functional and connected before proceeding.

### Authentication:
- **Fingerprint**: Place a registered finger on the fingerprint sensor. The system grants access if the fingerprint matches.
- **RFID Card**: Present a registered RFID card to the reader. The system grants access if the card UID matches predefined values.
- **Password**: Enter the password on the keypad and press 'A' to submit. The system grants access if the password is correct.

### Environmental Monitoring:
- The system continuously monitors temperature and smoke levels.
- If a fire is detected (temperature > 40°C and smoke > 400), an alert is displayed, the buzzer sounds, and the lock is disengaged to allow people to leave.

## Code Explanation
- **Setup Function**: Initializes all components, sets pin modes, and checks the fingerprint sensor.
- **Loop Function**: Continuously checks for fingerprint, RFID, and keypad input. Updates the LCD with temperature and smoke readings.
- **openDoor Function**: Activates the lock relay and buzzer sequence for door unlocking.
- **checkFingerprint Function**: Authenticates the user via fingerprint.
- **checkRFID Function**: Authenticates the user via RFID card.
- **checkPassword Function**: Authenticates the user via keypad password entry.
- **fireCheck Function**: Periodically checks environmental conditions and triggers a fire alert if necessary.
- **getFingerprintIDez Function**: Simplifies fingerprint reading and ID retrieval.

## Notes
- Ensure the fingerprint sensor is properly enrolled with user fingerprints before deploying the system.
- Adjust the sensor threshold values based on the specific environment and requirements.
- Regularly maintain and test the system to ensure all components are functioning correctly.