//       
// BitBank ALS/Proximity Sensor Library
// Written by Larry Bank
//       
// Copyright 2023 BitBank Software, Inc. All Rights Reserved.
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
#include "bb_proximity.h"

// 
// Initialize the library
// Detects if a supported sensor is available
// returns 1 for success, 0 for failure
// 
int BBProximity::init(int iSDA, int iSCL, bool bBitBang, uint32_t u32Speed, int interruptPin)
{
uint8_t ucTemp[4];

    _intPin = interruptPin;
    _iType = BBP_TYPE_UNKNOWN;
    _bbi2c.bWire = !bBitBang; // use bit bang?
    _bbi2c.iSDA = iSDA;
    _bbi2c.iSCL = iSCL;
    I2CInit(&_bbi2c, u32Speed);
// Detect the sensor type
    if (I2CTest(&_bbi2c, BBP_APDS99xx_ADDR)) { // could be APDS99xx
         _iAddr = BBP_APDS99xx_ADDR;
        I2CReadRegister(&_bbi2c, _iAddr, 0x80 | BBP_APDS_WHO_AM_I, ucTemp, 1); // get ID
        if (ucTemp[0] == BBP_APDS9930_ID) {
            _iType = BBP_TYPE_APDS9930;
            _u32Caps = BBP_CAPS_ALS | BBP_CAPS_PROXIMITY;
            return 1;
        } else if (ucTemp[0] == BBP_APDS9960_ID) {
            _iType = BBP_TYPE_APDS9960;
            _u32Caps = BBP_CAPS_ALS | BBP_CAPS_PROXIMITY | BBP_CAPS_GESTURE | BBP_CAPS_COLORS;
            return 1;
        } else Serial.println(ucTemp[0], DEC);
    }
    if (I2CTest(&_bbi2c, BBP_LTR553_ADDR)) { // could be LTR-553ALS
        _iAddr = BBP_LTR553_ADDR;
        I2CReadRegister(&_bbi2c, _iAddr, BBP_LTR553_WHO_AM_I, ucTemp, 1); // get ID
        if (ucTemp[0] == BBP_LTR553_ID) {
            _iType = BBP_TYPE_LTR553;
            _u32Caps = BBP_CAPS_ALS | BBP_CAPS_PROXIMITY;
            return 1;
        }
    } 
    return 0; // no recognized sensor found
} /* init() */

int BBProximity::type(void)
{
    return _iType;
} /* type() */

// Sets the LED current boost value:
// APDS9930: 0=12.5mA, 1=25mA, 2=50mA, 3=100mA
// APDS9960: 0=100%, 1=150%, 2=200%, 3=300%
// LTR553: 0=5mA, 1 = 10mA, 2 = 20mA, 3 = 50mA, 4 = 100mA
void BBProximity::setLEDBoost(uint8_t boost) {
uint8_t u8, u8Temp[4];

    if (_iType == BBP_TYPE_LTR553) {
        u8 = 0x78; // default pulse period of 60kHz, 100% duty cycle
        u8 |= (boost & 0x7);
        u8Temp[0] = 0x82; // LED control register
        u8Temp[1] = u8;
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
    } else if (_iType == BBP_TYPE_APDS9930) {
        u8 = (3-boost)<<6; // PDRIVE 0=100mA, 1=50mA, 2=25mA, 3=12.5mA
        u8 |= 0x20; // reserved PDIODE value of 10 = Ch1
        u8 |= 0x00; // 1X gain for proximity, 1X gain for ALS
        u8Temp[0] = 0x8f; // control register
        u8Temp[1] = u8;
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
    } else if (_iType == BBP_TYPE_APDS9960) {
        if (boost > 3) boost = 3; // valid values 0-3
        I2CReadRegister(&_bbi2c, _iAddr, 0x90, &u8Temp[1], 1);
        u8Temp[0] = 0x90; // int enables + LED boost
        u8Temp[1] &= ~0x30; // clear boost bits
        u8Temp[1] |= (boost << 4); // merge new boost value
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
    }
} /* setLEDBoost() */

uint16_t BBProximity::readWord(uint8_t reg)
{
uint16_t u16;
uint8_t u8;
	I2CReadRegister(&_bbi2c, _iAddr, reg | 0x80, &u8, 1);
        u16 = u8;
        I2CReadRegister(&_bbi2c, _iAddr, (reg+1) | 0x80, &u8, 1);
        u16 |= (u8 << 8);
        return u16; 
} /* readWord() */

void BBProximity::start(uint32_t iCaps)
{
uint8_t u8, u8Temp[4];

    if (_iType == BBP_TYPE_LTR553) {
        if (iCaps & BBP_CAPS_ALS) { // activate the ALS
             u8Temp[0] = 0x80; // ALS control
             u8Temp[1] = 0x01; // gain = 1X, not SW reset, active mode
             I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
        }
        if (iCaps & BBP_CAPS_PROXIMITY) { // activate the PS
             u8Temp[0] = 0x81; // PS control
             u8Temp[1] = 0x22; // saturation indicator on, active mode
             I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
             u8Temp[0] = 0x84; // pulse count (default is 1)
             u8Temp[1] = 0xf; // try 15
             I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
             u8Temp[0] = 0x84; // PS measurement rate
             u8Temp[1] = 0x4; // try 500ms
             I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
        }
    } else if (_iType == BBP_TYPE_APDS9930) {
        u8 = 0x1; // | 0x8; // enable power + wait
        if (iCaps & BBP_CAPS_ALS)
             u8 |= 2; // AEN
        if (iCaps & BBP_CAPS_PROXIMITY)
             u8 |= 4; // PEN
        u8Temp[0] = 0x80; // control reg
        u8Temp[1] = u8;
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
        u8Temp[0] = 0xA1; // ATIME, PTIME, WTIME
        u8Temp[1] = 0xff; // min ALS integration time
        u8Temp[2] = 0xff; // min prox integration time
        u8Temp[3] = 0xff; // min wait time
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 4);
        u8Temp[0] = 0xae; 
        u8Temp[1] = 8; // min prox pulse count = 8
        u8Temp[2] = 0x2c; // 100mA PDRIVE, Ch1 diode, PGAIN = 8x, AGAIN = 1x
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 3);
    } else if (_iType == BBP_TYPE_APDS9960) {
        u8Temp[0] = 0x80; // control reg
        u8Temp[1] = 0; // set everything off
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
        delay(10);
        
        u8Temp[0] = 0x80; // enable
        u8Temp[1] = 1 ; // enable power
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
        delay(10);
        
        // set WTIME
        u8Temp[0] = 0x83; // WTIME
        u8Temp[1] = 0xff;
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
        
        // set PPULSE (proximity pulse to 16us, 16 pulses)
        u8Temp[0] = 0x8E; // PPULSE
        u8Temp[1] = 0x8F;
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);

        // enable gesture mode
        I2CReadRegister(&_bbi2c, _iAddr, 0xAB, &u8Temp[1], 1);
        u8Temp[0] = 0xAB; // GCONF4
        u8Temp[1] |= 1; // enable gesture mode
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);

        // set gesture proximity threshold to 50
        u8Temp[0] = 0xA0; // GPENTH
        u8Temp[1] = 50;
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);

        // set GPULSE (gesture pulse to 32us, 10 pulses)
        u8Temp[0] = 0xA6; // GPULSE
        u8Temp[1] = 0xC9;
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
        
        u8Temp[0] = 0x80; // enable
        u8Temp[1] = 1 | 8; // enable wait
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);

        // set ADC integration time (ATIME) to 10ms
        u8Temp[0] = 0x81; // ATIME
        u8Temp[1] = (256 - (10.0/2.78));
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);

        // set ADC gain 4x
        u8Temp[0] = 0x8F; // CONTROL
        u8Temp[1] = 2;
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
        delay(10);
        
        u8 = 0x1 | 0x8; // enable power + wait
        if (iCaps & BBP_CAPS_ALS)
             u8 |= 2; // AEN
        if (iCaps & BBP_CAPS_PROXIMITY)
             u8 |= 4; // PEN
        if (iCaps & BBP_CAPS_GESTURE)
            u8 |= 0x40; // GEN
        u8Temp[0] = 0x80; // control reg
        u8Temp[1] = u8;
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
    } // APDS9960
} /* start() */

void BBProximity::stop(void)
{
uint8_t u8Temp[4];

    if (_iType == BBP_TYPE_LTR553) {
         u8Temp[0] = 0x80; // ALS control
         u8Temp[1] = 0x00; // gain = 1X, not SW reset, inactive mode
         I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
         u8Temp[0] = 0x81; // PS control
         u8Temp[1] = 0x00; // saturation indicator off, inactive mode
         I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
    } else if (_iType == BBP_TYPE_APDS9930 || _iType == BBP_TYPE_APDS9960) {
        u8Temp[0] = 0x80; // control reg
        u8Temp[1] = 0x00; // disable power
        I2CWrite(&_bbi2c, _iAddr, u8Temp, 2);
    }
} /* stop() */


int BBProximity::gestureFIFOAvailable(void)
{
uint8_t r = 0;
    
    I2CReadRegister(&_bbi2c, _iAddr, 0xAF, &r, 1); // read gesture status reg
    if ((r & 0x01) == 0x00) return -1; // no gestures available
    I2CReadRegister(&_bbi2c, _iAddr, 0xAE, &r, 1); // read gesture FIFO level
    return r; // return FIFO level
} /* gestureFIFOAvailable() */

int BBProximity::gestureAvailable(void)
{
  if (_intPin >= 0) {
    if (digitalRead(_intPin) != 0) { // nothing available
      return 0;
    }
  } else if (gestureFIFOAvailable() <= 0) { // nothing in the FIFO
    return 0;
  }
 // gesture is available
    return 1;
} /* gestureAvailable() */

int BBProximity::getGesture(void)
{
    int iGesture = BBP_GESTURE_NONE;
    long t = 0;
    int DCount = 0, UCount = 0, LCount = 0, RCount = 0;
    
    if (gestureAvailable()) {
        while (1) {
            int u_d_diff = 0, l_r_diff = 0;
            delay(30);
            int available = gestureFIFOAvailable();
            //  Serial.print("Avail = ");
            //  Serial.println(available, DEC);
            if (available <= 0) return BBP_GESTURE_NONE;
            uint8_t fifo_data[128];
            I2CReadRegister(&_bbi2c, _iAddr, 0xFC, fifo_data, available); // internal FIFO RAM will reset register address from FC to FF (U, D, L, R) repeatedly
            if (abs((int)fifo_data[0] - (int)fifo_data[1]) > 13) {
                u_d_diff += (int)fifo_data[0] - (int)fifo_data[1];
            }
            if (abs((int)fifo_data[2] - (int)fifo_data[3]) > 13) {
                l_r_diff += (int)fifo_data[2] - (int)fifo_data[3];
            }
            if (u_d_diff) {
                if (u_d_diff < 0) {
                    if (DCount > 0) {
                        iGesture = BBP_GESTURE_UP;
                    } else UCount++;
                } else if (u_d_diff > 0) {
                    if (UCount > 0) {
                        iGesture = BBP_GESTURE_DOWN;
                    } else DCount++;
                }
            }
            if (l_r_diff) {
                if (l_r_diff < 0) {
                    if (RCount > 0) {
                        iGesture = BBP_GESTURE_LEFT;
                    } else LCount++;
                } else if (l_r_diff > 0) {
                    if (LCount > 0) {
                        iGesture = BBP_GESTURE_RIGHT;
                    } else RCount++;
                }
            }
            if (u_d_diff || l_r_diff)
                t = millis();
            
            if (iGesture != BBP_GESTURE_NONE || millis() - t > 300) {
                //  DCount = UCount = LCount = RCount = 0;
                return iGesture;
            }
        } // while (1)
    } // if gestureAvailable()
	return iGesture; // DEBUG
} /* getGesture() */

int BBProximity::getColor(int *r, int *g, int *b, int *c)
{
    return 0; // DEBUG
} /* getColor() */

int BBProximity::getProximity(void)
{
uint8_t u8Temp[4];
int iDist = 0;

    if (_iType == BBP_TYPE_APDS9930 || _iType == BBP_TYPE_APDS9960) {
        I2CReadRegister(&_bbi2c, _iAddr, 0x93, u8Temp, 1); // see if PVALID is true
        if (u8Temp[0] & 2) { // PVALID
            if (_iType == BBP_TYPE_APDS9930) {
                //iDist = readWord(0x18);
                I2CReadRegister(&_bbi2c, _iAddr, 0x98, u8Temp, 1);
                I2CReadRegister(&_bbi2c, _iAddr, 0x99, &u8Temp[1], 1);
                iDist = u8Temp[0] + (u8Temp[1]<<8);
            } else { // APDS-9960
                I2CReadRegister(&_bbi2c, _iAddr, 0x9c, u8Temp, 1);
                iDist = u8Temp[0];
            }
        }
    } else if (_iType == BBP_TYPE_LTR553) {
        I2CReadRegister(&_bbi2c, _iAddr, 0x8d, u8Temp, 2);
        iDist = u8Temp[0] | (u8Temp[1] << 8);
    }
    return iDist; // DEBUG - returns the photon counts (inverse of distance)
} /* getProximity() */

void BBProximity::setGestureSensitivity(uint8_t sensitivity)
{
   (void)sensitivity; // DEBUG
} /* setGestureSensitivity() */

void BBProximity::setInterruptMode(int iMode, int iThreshLow, int iThreshHigh)
{
    (void)iMode; (void) iThreshLow; (void)iThreshHigh;
} /* setInterruptMode() */

uint32_t BBProximity::caps(void)
{
    return _u32Caps;
}

int BBProximity::getLight(void)
{
uint8_t u8Temp[4];
int LUX=0, iCH0, iCH1, IAC1, IAC2, IAC;

    if (_iType == BBP_TYPE_LTR553) {
        I2CReadRegister(&_bbi2c, _iAddr, 0x88, u8Temp, 4);
        LUX = u8Temp[0] | (u8Temp[1]<<8);
        return LUX;
    } else {
        LUX = readWord(0x14); // clear data
        return LUX;
    }
        I2CReadRegister(&_bbi2c, _iAddr, 0x93, u8Temp, 1);
        if (u8Temp[0] & 1) { // ALS data is valid
	// read the Channel 0 (visible+ir) and 1 (ir) counts
        iCH0 = readWord(0x14);
        iCH1 = readWord(0x16);
// LUX equation
// IAC1 = CH0 - B * CH1
// IAC2 = C * CH0 - D * CH1
// IAC = Max(IAC1, IAC2, 0);
// LPC = GA * DF / (ALSIT * AGAIN)
// LUX = IAC * LPC
// Coefficients in open air:
// LPC (assumed) = 0.06, GA = 0.49, B = 1.862, C = 0.746, D = 1.291
        IAC1 = (iCH0*256) - (477 * iCH1); // keep as integers
        IAC2 = (iCH0*191) - (330 * iCH1);
        IAC = (IAC1 > IAC2) ? IAC1 : IAC2;
        if (IAC < 0) IAC = 0;
        LUX = (IAC * 61) >> 8;
        }
    return LUX;
}  /* getLight() */
