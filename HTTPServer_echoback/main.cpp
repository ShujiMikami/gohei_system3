#include "mbed.h"

#include "ServerAction.h"

#include "rtos.h"
DigitalOut led1(LED1); //server listning status

int main (void)
{
    Thread threadEtherLamp;
    threadEtherLamp.start(EtherStatusLampThreadFunc);
    
    Thread threadServer;
    threadServer.start(ServerThreadFunc);

    while(1){
        printf("[Main Thread]Toggle LED\r\n");
        led1 = !led1;

        wait(0.5);

    }

    return 0;
}
