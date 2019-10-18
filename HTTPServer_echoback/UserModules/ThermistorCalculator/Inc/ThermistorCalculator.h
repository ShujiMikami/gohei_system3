/*
 * ThermistorCalc.h
 *
 *  Created on: 2018/01/22
 *      Author: Shuji
 */

#ifndef THERMISTORCALC_INC_THERMISTORCALC_H_
#define THERMISTORCALC_INC_THERMISTORCALC_H_

class ThermistorCalculator{
public:
    ThermistorCalculator(const double B, const double Rroom);
public:
    double CalculateTemperature(double thermistorRegistance);
private:
    double b_constant;
    double r_room;
};

#endif /* THERMISTORCALC_INC_THERMISTORCALC_H_ */