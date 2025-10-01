#ifndef __BB_PROXIMITY__
#define __BB_PROXIMITY__
//
// ALS/Proximity Sensor Library
// written by Larry Bank
// Project started 10/21/2023
//
// Copyright 2023-2025 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================
#ifdef __LINUX__
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>
#include <time.h>

#else // !LINUX

#ifdef ARDUINO
#include <Arduino.h>
#ifndef __AVR_ATtiny85__
#include <Wire.h>
#endif // !AVR
#include <BitBang_I2C.h>
#else // ESP_IDF?
#include <stdint.h>
#endif // ARDUINO
#endif // !__LINUX__

// For Linux and esp-idf we add a file/device handle member
// to the BBI2C structure
#ifndef ARDUINO
typedef struct _tagbbi2c
{
  int file_i2c;
  uint8_t iSDA, iSCL;
  uint8_t bWire;
} BBI2C;
#endif

#define BB_PROX_SUCCESS 0
#define BB_PROX_ERROR 1

// LED boost enumeration
// The IR LED used to measure distance can be controlled
// in all of the proximity sensors. A 4 level scale is enough to support
// them all in a consistent way

enum {
   BBP_BOOST_MIN = 0,
   BBP_BOOST_LOW,
   BBP_BOOST_MED,
   BBP_BOOST_HIGH, // on the APDS99xx high=max
   BBP_BOOST_MAX
};

enum {
   BBP_TYPE_UNKNOWN = 0,
   BBP_TYPE_APDS9930,
   BBP_TYPE_APDS9960,
   BBP_TYPE_LTR553
};

#define BBP_APDS99xx_ADDR 0x39
#define BBP_LTR553_ADDR 0x23

#define BBP_APDS_WHO_AM_I 0x12
#define BBP_APDS9930_ID 0x39
#define BBP_APDS9960_ID 0xAB

#define BBP_LTR553_WHO_AM_I 0x86
#define BBP_LTR553_ID 0x92

#define BBP_CAPS_ALS       1
#define BBP_CAPS_PROXIMITY 2
#define BBP_CAPS_GESTURE   4
#define BBP_CAPS_COLORS    8

enum {
  BBP_GESTURE_NONE = 0,
  BBP_GESTURE_UP,
  BBP_GESTURE_DOWN,
  BBP_GESTURE_LEFT,
  BBP_GESTURE_RIGHT
};

class BBProximity
{
public:
    BBProximity() { _iType = BBP_TYPE_UNKNOWN; _u32Caps = 0; }
    ~BBProximity() {}
    int type(void);
    uint32_t caps(void);
    int init(int iSDA = -1, int iSCL = -1, bool bBitBang = false, uint32_t u32Speed=400000, int interruptPin = -1);
    int init(BBI2C *pBB);
    BBI2C *getBB();
    void start(uint32_t iCaps = 0xff);
    void stop(void);
    int getLight(void);
    int getGesture(void);
    int getColor(int *r, int *g, int *b, int *c);
    int getProximity(void);
    int gestureAvailable(void);
    int gestureFIFOAvailable(void);
    void setGestureSensitivity(uint8_t sensitivity);
    void setInterruptMode(int iMode, int iThreshLow, int iThreshHigh);
    void setLEDBoost(uint8_t boost);

private:
    uint16_t readWord(uint8_t reg);
    int initInternal(void);
    int _iAddr;
    int _iType;
    int _intPin;
    uint32_t _u32Caps;
    BBI2C _bbi2c;
}; // class BBProximity

#endif // __BB_PROXIMITY__
