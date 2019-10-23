#include "mbed.h"

DigitalIn linkSignal(P1_25);
DigitalIn speedSignal(P1_26);
DigitalOut linkLamp(p30);
DigitalOut speedLamp(p29);

void EtherStatusLampThreadFunc()
{
    while(true){
        linkLamp = !linkSignal;
        speedLamp = !speedSignal;
        wait(0.05);
    }
}