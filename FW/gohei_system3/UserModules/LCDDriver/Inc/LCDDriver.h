#ifndef _LCDDRIVER_H_
#define _LCDDRIVER_H_

#include <mbed.h>

class SC1602Driver{
public:
private:
    typedef enum{
        INTERFACE_DATA_8BIT = (1 << 4),
        INTERFACE_DATA_4BIT = (0 << 4)
    }InterfaceDataLength_t;
    
    typedef enum{
        LINE_2 = (1 << 3),
        LINE_1 = (0 << 3)
    }NumberOfLine_t;

    typedef enum{
        FONT_SIZE_5_11 = (1 << 2),
        FONT_SIZE_5_8 = (0 << 2)
    }FontSize_t;

    typedef enum{
        DISPLAY_ON = (1 << 2),
        DISPLAY_OFF = (0 << 2)
    }DisplayONOFF_t;

    typedef enum{
        CURSOR_ON = (1 << 1),
        CURSOR_OFF = (0 << 1)
    } CursorONOFF_t;
    
    typedef enum{
        BLINK_ON = 1,
        BLINK_OFF = 0
    }BlinkONOFF_t;

    typedef enum{
        CURSOR_INCREASE = (1 << 1),
        CURSOR_DECREASE = (0 << 1)
    }CursorMoveDirection_t;

    typedef enum{
        Shift_ON = 1,
        Shift_OFF = 0
    }Shift_t;

public:
    SC1602Driver(BusOut& DataBus, DigitalOut& RSpin, DigitalOut& RWpin, DigitalOut& Epin);
    void Initialize();
    void ClearDisplay();
    void ReturnHome();
    void WriteString(char* string, int line);
private:
    void writeCommand(uint8_t data);
    void writeData(uint8_t data);
    void writeCharToPos(uint8_t pos, char data);
    void ePinControl();
    void setDDRAMaddress(uint8_t address);
    void functionSet(InterfaceDataLength_t dataTransferMode, NumberOfLine_t indicateLineNum, FontSize_t fontSize);
    void displayONOFFcontrol(DisplayONOFF_t displayON, CursorONOFF_t cursorON, BlinkONOFF_t blinkingOn);
    void entryModeSet(CursorMoveDirection_t direction, Shift_t shift);
private:
    BusOut* pDataBus;
    DigitalOut* pRsPin;
    DigitalOut* pRwPin;
    DigitalOut* pEpin;
};



#endif