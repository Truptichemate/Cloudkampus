#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 20000000

#define DOOR_SENSOR_PIN   RA0  //  Door Sensor Button (RA0)
#define ROOF_SWITCH_PIN   RA1  //  Roof Switch Button (RA1)


#define DOOR_OPEN_LED     RB0
#define DOOR_CLOSED_LED   RB1
#define LIGHTING_LED      RB2


#define I2C_ADDR          0x50

// Function Prototypes
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(uint8_t data);
uint8_t I2C_Read(void);
void I2C_Acknowledge(void);
void I2C_Nack(void);

// Door status values
#define DOOR_OPEN         1
#define DOOR_CLOSED       0

// Function to initialize I2C
void I2C_Init(void) {
    TRISC = 0x80;  // Set SDA (RC4) as input and SCL (RC3) as output
    SSPCON = 0x28; // Configure I2C master mode
    SSPSTAT = 0x00; // Clear the SSPSTAT register
    TRISA = 0xFF;   // Set all PORTA pins as input (for simulated buttons)
    TRISB = 0x00;   // Set all PORTB pins as output (for LEDs)
}

// Function to start I2C communication
void I2C_Start(void) {
    SSPCON2bits.SEN = 1;   // Initiate START condition
    while (SSPCON2bits.SEN); // Wait for the start condition to complete
}

// Function to stop I2C communication
void I2C_Stop(void) {
    SSPCON2bits.PEN = 1;   // Initiate STOP condition
    while (SSPCON2bits.PEN); // Wait for the stop condition to complete
}

// Function to write data to I2C
void I2C_Write(uint8_t data) {
    SSPBUF = data;         // Load data to SSPBUF register
    while (SSPSTATbits.BF); // Wait until the transmission is complete
}

// Function to read data from I2C
uint8_t I2C_Read(void) {
    SSPCON2bits.RCEN = 1;  // Enable the reception of data
    while (!SSPSTATbits.BF); // Wait until the data is received
    return SSPBUF;         // Return the received data
}

// Acknowledge reception of data
void I2C_Acknowledge(void) {
    SSPCON2bits.ACKDT = 0; // Set ACKDT to 0 for acknowledgment
    SSPCON2bits.ACKEN = 1; // Send acknowledgment
    while (SSPCON2bits.ACKEN); // Wait for acknowledgment to complete
}

// Nack reception of data
void I2C_Nack(void) {
    SSPCON2bits.ACKDT = 1; // Set ACKDT to 1 for no acknowledgment
    SSPCON2bits.ACKEN = 1; // Send NACK
    while (SSPCON2bits.ACKEN); // Wait for NACK to complete
}

// Function to control the interior lighting
void controlLighting(uint8_t doorStatus, uint8_t roofSwitchStatus) {
    if (roofSwitchStatus == 1) {
        if (doorStatus == DOOR_OPEN) {
            LIGHTING_LED = 1;  // Turn on the light if the door is open
        } else {
            LIGHTING_LED = 0;  // Turn off the light if the door is closed
        }
    } else {
        LIGHTING_LED = 0; // Turn off light if roof switch is off
    }
}

// Function to transmit door status via I2C
void transmitDoorStatus(uint8_t doorStatus) {
    I2C_Start();
    I2C_Write(I2C_ADDR);  // Send I2C address
    I2C_Write(doorStatus); // Send door status (Open/Close)
    I2C_Stop();
}

void main(void) {
    uint8_t doorStatus = DOOR_CLOSED;
    uint8_t roofSwitchStatus = 0;

    // Initialize I2C
    I2C_Init();

    // Set initial LED states
    DOOR_OPEN_LED = 0;
    DOOR_CLOSED_LED = 1;
    LIGHTING_LED = 0;

    // Main loop
    while (1) {
        // Door Sensor Button (RA0)
        if (DOOR_SENSOR_PIN == 1) { // Button pressed = Door Open
            doorStatus = DOOR_OPEN;
            DOOR_OPEN_LED = 1;  // Indicate door is open
            DOOR_CLOSED_LED = 0; // Turn off door closed LED
        } else {
            doorStatus = DOOR_CLOSED;
            DOOR_OPEN_LED = 0;  // Indicate door is closed
            DOOR_CLOSED_LED = 1; // Turn on door closed LED
        }

        roofSwitchStatus = ROOF_SWITCH_PIN; // If button pressed, roof switch ON

        // Control lighting based on door status and roof switch mode
        controlLighting(doorStatus, roofSwitchStatus);

        // Transmit door status via I2C
        transmitDoorStatus(doorStatus);

        __delay_ms(100);
    }
}
