#include "LCDDriver.h"

//ピンの状態名
const int RSPINSTATE_CONTROL = 0;
const int RSPINSTATE_DATA = 1;
const int RWPINSTATE_W = 0;
const int RWPINSTATE_R = 1;
const int PINSTATE_HIGH = 1;
const int PINSTATE_LOW = 0;

//SC1602固有の時間[ms]
const int T_PW = 1; //仕様は140nsec
const int DESCRIPTION_TIME_RETURN_HOME = 2; //仕様は1.52ms
const int DESCRIPTION_TIME_CLEAR_DISPLAY = 2; //仕様は1.52ms
const int BOOT_TIME = 40; //起動後40msec待つらしい
const int DESCRIPTION_TIME_FUNCTION_SET = 1; //仕様は37us
const int DESCRIPTION_TIME_DISPLAY_ONOFF = 1; //仕様は37us
const int DESCRIPTION_TIME_ENTRYMODE_SET = 1; //仕様は37us

//コマンドフラグ
const int SET_DDRAM_COMMAND = 0x80;
const int SET_CGRAM_COMMAND = 0x40;
const int FUNCTION_SET_COMMAND = 0x20;
const int CURSOR_OR_DISPLAY_SHIFT_COMMAND = 0x10;
const int DISPLAY_ON_OFF_COMMAND = 0x08;
const int ENTRY_MODE_SET_COMMAND = 0x04;
const int RETURN_HOME_COMMAND = 0x02;
const int CLEAR_DISPLAY_COMMAND = 0x01;

//パラメータ定義域
const int DDRAM_ADDRESS_LINE1_MIN = 0x00;
const int DDRAM_ADDRESS_LINE1_MAX = 0x0F;
const int DDRAM_ADDRESS_LINE2_MIN = 0x40;
const int DDRAM_ADDRESS_LINE2_MAX = 0x4F;
const int DDRAM_LENGTH_PER_LINE = DDRAM_ADDRESS_LINE1_MAX - DDRAM_ADDRESS_LINE1_MIN + 1;
inline bool isAddressInRange(uint8_t address);

SC1602Driver::SC1602Driver(BusOut& DataBus, DigitalOut& RSpin, DigitalOut& RWpin, DigitalOut& Epin)
{
    pDataBus = &DataBus;
    pRsPin = &RSpin;
    pRwPin = &RWpin;
    pEpin = &Epin;
}
void SC1602Driver::Initialize()
{
    //起動待ち
    wait_ms(BOOT_TIME);

    //ファンクションセット
    functionSet(INTERFACE_DATA_8BIT, LINE_2, FONT_SIZE_5_8);

    //DisplayONOFFセット
    displayONOFFcontrol(DISPLAY_ON, CURSOR_OFF, BLINK_OFF);

    //Displayクリア
    ClearDisplay();

    //Entry modeセット
    entryModeSet(CURSOR_INCREASE, Shift_OFF);    
}
void SC1602Driver::functionSet(InterfaceDataLength_t dataTransferMode, NumberOfLine_t indicateLineNum, FontSize_t fontSize)
{
    writeCommand(FUNCTION_SET_COMMAND | (int)dataTransferMode | (int) indicateLineNum | (int)fontSize);

    wait_ms(DESCRIPTION_TIME_FUNCTION_SET);
}
void SC1602Driver::displayONOFFcontrol(DisplayONOFF_t displayON, CursorONOFF_t cursorON, BlinkONOFF_t blinkingOn)
{
    writeCommand(DISPLAY_ON_OFF_COMMAND | (int)displayON | (int)cursorON | (int) blinkingOn);

    wait_ms(DESCRIPTION_TIME_DISPLAY_ONOFF);
}
void SC1602Driver::entryModeSet(CursorMoveDirection_t direction, Shift_t shift)
{
    writeCommand(ENTRY_MODE_SET_COMMAND | (int)direction | (int)shift);

    wait_ms(DESCRIPTION_TIME_ENTRYMODE_SET);
}
void SC1602Driver::ClearDisplay()
{
    writeCommand(CLEAR_DISPLAY_COMMAND);

    wait_ms(DESCRIPTION_TIME_CLEAR_DISPLAY);
}
void SC1602Driver::ReturnHome()
{
    writeCommand(RETURN_HOME_COMMAND);
    
    wait_ms(DESCRIPTION_TIME_RETURN_HOME);
}
void SC1602Driver::WriteString(char* string, int line)
{
    int dataLength = strlen(string);

    if(dataLength > DDRAM_LENGTH_PER_LINE){
        dataLength = DDRAM_LENGTH_PER_LINE;
    }

    int startPos = DDRAM_ADDRESS_LINE1_MIN;
    if(line == 1){
        startPos = DDRAM_ADDRESS_LINE1_MIN;
    }
    else{
        startPos = DDRAM_ADDRESS_LINE2_MIN;
    }

    for(int i = 0; i < dataLength; i++){
        writeCharToPos(i + startPos, string[i]);
    }
}
void SC1602Driver::writeCommand(uint8_t data)
{
    //RSPinをコントロール
    pRsPin->write(RSPINSTATE_CONTROL);

    //RWPinを書き込み
    pRwPin->write(RWPINSTATE_W);

    //データバスセット
    pDataBus->write(data);

    //データ送信
    ePinControl();
}
void SC1602Driver::ePinControl()
{
    //T_PWの正パルスを与える
    pEpin->write(PINSTATE_HIGH);
    wait_ms(T_PW);
    pEpin->write(PINSTATE_LOW);
}
void SC1602Driver::writeData(uint8_t data)
{
    //RSPinをコントロール
    pRsPin->write(RSPINSTATE_DATA);

    //RWPinを書き込み
    pRwPin->write(RWPINSTATE_W);

    //データバスセット
    pDataBus->write(data);

    //データ送信
    ePinControl();
}
void SC1602Driver::writeCharToPos(uint8_t pos, char data)
{
    setDDRAMaddress(pos);

    writeData((uint8_t)data);
}
void SC1602Driver::setDDRAMaddress(uint8_t address)
{
    if(isAddressInRange(address)){
        writeCommand(SET_DDRAM_COMMAND | address);
    }
}
inline bool isAddressInRange(uint8_t address)
{
    return ((address <= DDRAM_ADDRESS_LINE1_MAX)
            || (DDRAM_ADDRESS_LINE2_MIN <= address && address <= DDRAM_ADDRESS_LINE2_MAX));
}