#include "mbed.h"

#include "ServerAction.h"
#include "CageDriver.h"
#include "rtos.h"

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) while(0)
#endif



DigitalOut led1(LED1); //server listning status

int main (void)
{
    wait(5);
    Thread threadEtherLamp;
    threadEtherLamp.start(EtherStatusLampThreadFunc);
    
    Thread threadServer(osPriorityNormal, DEFAULT_STACK_SIZE * 3);
    threadServer.start(ServerThreadFunc);

    Thread threadCageDrive;
    threadCageDrive.start(CageDriveThread);

    while(1){
        DEBUG_PRINT("[Main Thread]Toggle LED\r\n");
        led1 = !led1;

        CageStatus_t cageStatus = GetCageStatus();
        DEBUG_PRINT("[Main Thread]Cage Status check. Temperature = %2.1f, Operation = %s, UV = %s\r\n", cageStatus.temperature, cageStatus.statusMessage, cageStatus.uvStatusMessage);

        wait(10);
    }

    return 0;
}
