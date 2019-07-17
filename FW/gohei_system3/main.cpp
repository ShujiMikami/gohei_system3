#include "mbed.h"
#include "LCDDriver.h"
#include "ThermistorCalculator.h"

//LCD周り
BusOut lcdDataBus(p21, p22, p23, p24, p25, p26, p27, p28);
DigitalOut pin_RS(p29);
DigitalOut pin_RW(p30);
DigitalOut pin_E(p5);
SC1602Driver LCD(lcdDataBus, pin_RS, pin_RW, pin_E);

//ADC周り
AnalogIn thermistorPin(p20);

//スイッチ周り
DigitalIn settingEntrySwitch(p5);
DigitalIn settingUpSwitch(p6);
DigitalIn settingDownSwitch(p7);
DigitalIn uvControlSwitch(p8);

//制御線周り
DigitalOut heaterControl(p9);
DigitalOut uvControl(p10);
DigitalOut fanControl(p11);

const int CONTROL_STATUS_ON = 1;
const int CONTROL_STATUS_OFF = 0;

//サーミスタ周り
double calculateThermistorResistance(double adcRatio);
const double R_ROOM = 10000.0;
const double B_CONST = 3380.0;
const double R_SERIES = 10000.0;
ThermistorCalculator thermoCalculator(B_CONST, R_ROOM);
double measureTemperature();

int main() {
    while(1) {
        //スイッチ状態監視

        //状態遷移

        //状態に応じたアクション
    }
}
double calculateThermistorResistance(double adcRatio)
{
    return adcRatio / (1.0 - adcRatio) * R_SERIES;
}
double measureTemperature()
{
    double thermistorResistance = calculateThermistorResistance(thermistorPin.read());

    return thermoCalculator.CalculateTemperature(thermistorResistance);
}