#ifndef _MANUAL_CONTROL_HPP_INCLUDED_
#define _MANUAL_CONTROL_HPP_INCLUDED_

#include <Arduino.h>
#include <EEPROM.h>
#include "Keypad.h"
#include "LiquidCrystal_I2C.h" //Library for LCD
#include "EEPROM_deposit.hpp"

#include "utils.hpp"


// Global variables
extern Keypad customKeypad;
extern LiquidCrystal_I2C lcd;

step_param_t get_step_param();
void show_current_step_param();
void ask_for_new_step_param();

#endif