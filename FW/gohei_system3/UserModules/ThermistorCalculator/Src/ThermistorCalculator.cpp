/*
 * ThermistorCalc.c
 *
 *  Created on: 2018/01/22
 *      Author: Shuji
 */
#include "ThermistorCalculator.h"
#include "math.h"

//! 絶対零度の摂氏温度
static const double ZERO_KELVIN = -273.15;

//! R0測定時の温度
static const double ROOM_TEMPERATURE = 25.0;

ThermistorCalculator::ThermistorCalculator(const double B, const double Rroom)
{
    b_constant = B;
    r_room = Rroom;
}

double ThermistorCalculator::CalculateTemperature(double thermistorRegistance)
{
    return 1.0 / (1.0 / (ROOM_TEMPERATURE + ZERO_KELVIN) + 1.0 / b_constant * (log(thermistorRegistance) - log(r_room)));
}