#pragma once

/*************************************************************************************************
Basic settings

This are the settings that you should change, depending on the quality of your modules.
*************************************************************************************************/

// The time you need to press to activate the long press action. Time in milliseconds
#define LONG_PRESS_DURATION 250   // Default: 250

// The time between presses to deactivate the screen. Time in seconds
#define TIMEOUT_TIME 5L * 60L  // Default: 5L * 60L (5 minutes * 60 seconds)

// SD card configuration
#define SD_MHZ 16     // for faster sdcards
// #define SD_MHZ 5   // for slower sdcards

// Frequenty for the I2C bus to control the displays
#define OLED_SPEED 1200000 // for faster displays
// #define OLED_SPEED 500000 // for slower displays

// if your screen is flickering, choose a lower number. the worse the screen, the lower the number.
#define REFRESH_FREQUENCY 0xf2
// #define REFRESH_FREQUENCY 0xf1
// #define REFRESH_FREQUENCY 0xc1
// #define REFRESH_FREQUENCY 0x80



/*************************************************************************************************
Advanced settings

Most users probably don't need to change these settings
*************************************************************************************************/

// The amount of displays on your freedeck
#define BD_COUNT 6 // Default: 6

// This enables the custom mode order mode. Only need for own / custom made freedecks.
//#define CUSTOM_ORDER

// Change this value from 0x11 up to 0xff to reduce coil whine. different from display to display
#define PRE_CHARGE_PERIOD 0x11            // Default: 0x11

// NOT IMPLEMENTED YET
// Minimum Brightness value for displays. If your displays image quality gets
// worse at lower brighness choose a bigger value here
// #define MINIMUM_BRIGHTNESS 0x20
// #define MINIMUM_BRIGHTNESS 0x00 //almost dark, good displays only
// #define MINIMUM_BRIGHTNESS 0x30 //brightest for cheap displays



/*************************************************************************************************
Expert settings

A very few will need to change it. If you think you need to change something here, and not sure what or how
please ask for advice in the discord
*************************************************************************************************/

// Enables the debug mode. Uncommand to enable
// #define DEBUG 1

// Configurationfile settings. Should not be changed unless you know what you're doing
#define CONFIG_NAME "config.bin"          // Default: "config.bin"
#define CONFIG_NAME_TEMP "config.tmp"     // Default: "config.tmp"
#define CONFIG_NAME_OLD "config.bin.old"  // Default: "config.bin.old"

// If the freedeck should wake up on recieving serial communication 
#define WAKE_ON_SERIAL true               // Default: true



/*************************************************************************************************
Custom settings

If you have a custom FreeDeck, you should probably change some settings here
(Only works if you have enabled the CUSTOM_ORDER in the advanced section)
*************************************************************************************************/

// for ryan aukes 5x3 pcb layout or
// if your screens are not in 1..n order
#ifdef CUSTOM_ORDER
#define ADDRESS_TO_SCREEN                                                                          \
  { 13, 11, 8, 5, 2, 14, 10, 7, 4, 1, 12, 9, 6, 3, 0 }
#define ADDRESS_TO_BUTTON                                                                          \
  { 12, 14, 6, 5, 2, 11, 13, 7, 4, 1, 10, 9, 8, 3, 0 }
#endif

// Settings for the OLED's. On the default Freedeck these are the settings, so only change if you have a custom one
#define OLED_SDA 2                      // Default: 2
#define OLED_SCL 3                      // Default: 3
#define OLED_ADDRESS 0x3C               // Default: 0x3C

#define BUTTON_PIN 19                   // Default: 19
#define S0_PIN 20                       // Default: 20
#define S1_PIN 21                       // Default: 21
#define S2_PIN 22                       // Default: 22
// #define S3_PIN 5                     // Not needed for the default 2x3 freedeck.

// The pins for the SD card. These are correct for the default FreeDeck.
#define SD_SPI spi1                     // Default: spi1
#define SD_MISO 8                       // Default: 8
#define SD_MOSI 11                      // Default: 11
#define SD_SCK 10                       // Default: 10
#define SD_CS 9                         // Default: 9
#define SD_DETECT 24                    // Default: 24 (Leave it like this if your SD card doesn't have it, or you dont know what it is)

/*************************************************************************************************
Static settings

Keep off, these things are used in the program for error / ok messages.
*************************************************************************************************/

#define OK "ok"
#define ERROR "error"

