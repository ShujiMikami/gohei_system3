#include "mbed.h"
#include "LCDDriver.h"
#include "ThermistorCalculator.h"
#include "stdlib.h"
#include "stdio.h"

//バージョン番号
const char versionNumber[] = "Ver 0.0.2";

//システムステータス
typedef enum{
    SYSTEM_SETTING = 0,
    SYSTEM_OPERATING
}SystemStatus_t;

//オペレーティングステータス
typedef enum{
    FAN_COOLING = 0,
    NATURAL_COOLING,
    HEATING
}OperatingStatus_t;

//GPIOのHL名前つけ
const int PIN_STATUS_HIGH = 1;
const int PIN_STATUS_LOW = 0;

//LCD周り
BusOut lcdDataBus(p5, p6, p7, p8, p9, p10, p11, p12);
DigitalOut pin_RS(p13);
DigitalOut pin_RW(p14);
DigitalOut pin_E(p15);
SC1602Driver LCD(lcdDataBus, pin_RS, pin_RW, pin_E);

//ADC周り
AnalogIn thermistorPin(p20);

//スイッチ周り
DigitalIn settingEntrySwitch(p17);
DigitalIn settingUpSwitch(p19);
DigitalIn settingDownSwitch(p18);
DigitalIn uvControlSwitch(p16);

//制御線周り
DigitalOut heaterControl(p22);
DigitalOut uvControl(p23);
DigitalOut fanControl(p21);

//SettingSwitchの切り替え先定義
const int SETTING_SWITCH_SETTING = PIN_STATUS_HIGH;
const int SETTING_SWITCH_OPERATING = PIN_STATUS_LOW;

//Settingのタクトボタンの負論理定義
const int SETTING_SWITCH_PUSHED = PIN_STATUS_LOW;

//UVスイッチの正論理定義
const int UV_SWITCH_ON = PIN_STATUS_HIGH;
const int UV_SWITCH_OFF = PIN_STATUS_LOW;

//制御のONOFF正論理定義
const int CONTROL_STATUS_ON = 1;
const int CONTROL_STATUS_OFF = 0;

//サーミスタ周り
double calculateThermistorResistance(double adcRatio);
const double R_ROOM = 10000.0;
const double B_CONST = 3380.0;
const double R_SERIES = 10000.0;
ThermistorCalculator thermoCalculator(B_CONST, R_ROOM);
double measureTemperature();

//operatingAction
double targetTemperature = 25.0;
double targetTemperatureLowerLimit = 20;
double targetTemperatureUpperLimit = 35;
double dangerZone = 40.0;
double deadZone = 1.0;
void operatingAction();
Timer timer;
float operatingPeriod = 1;//3秒周期制御

//settingAction
void settingAction();
Timer timer4Setting;
float buttonDisableTime = 300;//300msecはボタン無効時間
void indicateSetTemperature();

//action
void systemAction(SystemStatus_t status);

//Current Status Indicate
void indicateCurrentStatus(double currentTemperature, char* controlStatus);

//起動メッセージ系
char initialString[] = "System Start";
void indicateInitialMessage();

//状態判定系
SystemStatus_t getRequiredSystemStatus();

//ピンセッティング初期化
void initializePinSetting();

int main() {
    //起動メッセージ表示
    indicateInitialMessage();

    //入力ピンinitialize
    initializePinSetting();

    SystemStatus_t status = SYSTEM_OPERATING;

    while(1) {
        //スイッチ状態監視と状態遷移
        status = getRequiredSystemStatus();

        //状態に応じたアクション
        systemAction(status);
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
void operatingAction()
{
    //温度を測定
    double currentTemperature = measureTemperature();

    //動作モード確定
    static OperatingStatus_t operatingStatus = NATURAL_COOLING;
    if(currentTemperature > dangerZone){
        operatingStatus = FAN_COOLING;
    }else if(targetTemperature + deadZone < currentTemperature && currentTemperature <= dangerZone){
        operatingStatus = NATURAL_COOLING;
    }else if(targetTemperature - deadZone < currentTemperature && currentTemperature <= targetTemperature + deadZone){
        //deadZone内では, 前のセッティングを保持
    }else if(currentTemperature <= targetTemperature - deadZone){
        operatingStatus = HEATING;
    }

    //表示する状態文字を指定
    char line2Buf[26];
    if(operatingStatus == FAN_COOLING){
        sprintf(line2Buf, "%s", "Fan Cooling");
    }else if(operatingStatus == NATURAL_COOLING){
        sprintf(line2Buf, "%s", "Natural Cooling");
    }else if(operatingStatus == HEATING){
        sprintf(line2Buf, "%s", "HEATING");
    }else{
        sprintf(line2Buf, "%s", "NOT DEFINED");
    }

    //ファン, ヒータ制御
    if(operatingStatus == FAN_COOLING){
        heaterControl = CONTROL_STATUS_OFF;
        fanControl = CONTROL_STATUS_ON;
    }else if(operatingStatus == NATURAL_COOLING){
        heaterControl = CONTROL_STATUS_OFF;
        fanControl = CONTROL_STATUS_OFF;
    }else{
        heaterControl = CONTROL_STATUS_ON;
        fanControl = CONTROL_STATUS_OFF;
    }

    //LCDに反映
    indicateCurrentStatus(currentTemperature, line2Buf);
}
void settingAction()
{

    static bool buttonEnabled = true;

    if(buttonEnabled){
        if(settingUpSwitch == SETTING_SWITCH_PUSHED){
            buttonEnabled = false;
            timer4Setting.start();
            if(targetTemperature < targetTemperatureUpperLimit){
                targetTemperature++;
            }
            indicateSetTemperature();
        }else if(settingDownSwitch == SETTING_SWITCH_PUSHED){
            buttonEnabled = false;
            timer4Setting.start();
            if(targetTemperature > targetTemperatureLowerLimit){
                targetTemperature--;
            }
            indicateSetTemperature();
        }
    }else{
        if(timer4Setting.read_ms() > buttonDisableTime){
            buttonEnabled = true;
            timer4Setting.stop();
            timer4Setting.reset();
        }
    }
}
void systemAction(SystemStatus_t status)
{
    //UVスイッチ監視, 制御
    if(uvControlSwitch == UV_SWITCH_ON){
        uvControl = CONTROL_STATUS_ON;
    }else{
        uvControl = CONTROL_STATUS_OFF;
    }

    static bool isTimerStatrted = false;

    static bool outOfSettingModeFlag = true;

    if(status == SYSTEM_OPERATING){
        outOfSettingModeFlag = true;

        if(!isTimerStatrted){
            timer.start();
            isTimerStatrted = true;
        }

        if(timer.read() > operatingPeriod){
            operatingAction();
            timer.reset();
        }
    }else{
        timer.stop();
        timer.reset();
        isTimerStatrted = false;

        //セッティングモードに入った初回の表示
        if(outOfSettingModeFlag){
            indicateSetTemperature();
            outOfSettingModeFlag = false;
        }

        settingAction();
    }
}
void indicateSetTemperature()
{
    char buffer[16];

    sprintf(buffer, "%2.1f%c%c", targetTemperature, (char)0xDF, 'C');

    LCD.ClearDisplay();

    LCD.WriteString(buffer, 1);
}
void indicateCurrentStatus(double currentTemperature, char* controlStatus)
{
    LCD.ClearDisplay();
    char line1Buf[16];
    sprintf(line1Buf, "%2.1f%c%c", currentTemperature, (char)0xDF, 'C');

    LCD.WriteString(line1Buf, 1);
    LCD.WriteString(controlStatus, 2);
}
void indicateInitialMessage()
{
    LCD.Initialize();
    LCD.WriteString(initialString, 1);
    LCD.WriteString((char*)versionNumber, 2);
    wait(3);
}
void initializePinSetting()
{
    settingEntrySwitch.mode(PullUp);
    settingUpSwitch.mode(PullUp);
    settingDownSwitch.mode(PullUp);
    uvControlSwitch.mode(PullUp);
}
SystemStatus_t getRequiredSystemStatus()
{
    SystemStatus_t result = SYSTEM_OPERATING;
    if(settingEntrySwitch == SETTING_SWITCH_SETTING){
        result = SYSTEM_SETTING;
    }else{
        result = SYSTEM_OPERATING;
    }

    return result;
}