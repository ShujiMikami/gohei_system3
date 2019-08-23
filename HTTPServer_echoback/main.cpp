#include "mbed.h"
#include "EthernetInterface.h"
#include <stdio.h>
#include <string.h>

#define PORT   80

EthernetInterface eth;

TCPSocketServer svr;
bool serverIsListened = false;

TCPSocketConnection client;
bool clientIsConnected = false;

DigitalOut led1(LED1); //server listning status
DigitalOut led2(LED2); //socket connecting status

DigitalIn linkSignal(P1_25);
DigitalIn speedSignal(P1_26);
DigitalOut linkLamp(p30);
DigitalOut speedLamp(p29);

//なぜかTickを宣言しないとEthernetがこける
Ticker dummyTick;

void etherStatusLampThreadFunc();
void serverThreadFunc();

int main (void)
{
    Thread threadEtherLamp;
    threadEtherLamp.start(etherStatusLampThreadFunc);
    
    Thread threadServer;
    threadServer.start(serverThreadFunc);

    while(1){
        led1 = !led1;

        wait(0.5);

    }

    return 0;
}
void etherStatusLampThreadFunc()
{
    while(true){
        linkLamp = !linkSignal;
        speedLamp = !speedSignal;
        wait(0.05);
    }
}

void serverThreadFunc()
{
    //setup ethernet interface
    eth.init(); //Use DHCP
    
    //Print MAC Address
    printf("MAC Address = %s\r\n", eth.getMACAddress());

    //Connect to DHCP server
    bool connectedToDHCPServer = false;
    while(!connectedToDHCPServer){
        printf("Trying to connect DHCPServer...\r\n");

        int connectionResult = eth.connect();

        if(connectionResult == 0){
            printf("connection success\r\n");
            connectedToDHCPServer = true;
        } else {
            printf("connection fail\r\n");
        }
    }


    printf("IP Address is %s\n\r", eth.getIPAddress());

    //setup tcp socket
    if(svr.bind(PORT)< 0) {
        printf("tcp server bind failed.\n\r");
        return;
    } else {
        printf("tcp server bind successed.\n\r");
        serverIsListened = true;
    }

    if(svr.listen(1) < 0) {
        printf("tcp server listen failed.\n\r");
        return;
    } else {
        printf("tcp server is listening...\n\r");
    }

    //listening for http GET request
    while (serverIsListened) {
        //blocking mode(never timeout)
        if(svr.accept(client)<0) {
            printf("failed to accept connection.\n\r");
        } else {
            printf("connection success!\n\rIP: %s\n\r",client.get_address());
            clientIsConnected = true;
            led2 = true;
            
            while(clientIsConnected) {
                char buffer[1024] = {};
                switch(client.receive(buffer, 1023)) {
                    case 0:
                        printf("recieved buffer is empty.\n\r");
                        clientIsConnected = false;
                        break;
                    case -1:
                        printf("failed to read data from client.\n\r");
                        clientIsConnected = false;
                        break;
                    default:
                        printf("Recieved Data: %d\n\r\n\r%.*s\n\r",strlen(buffer),strlen(buffer),buffer);
                        if(buffer[0] == 'G' && buffer[1] == 'E' && buffer[2] == 'T' ) {
                            printf("GET request incomming.\n\r");
                            //setup http response header & data
                            char echoHeader[256] = {};
                            sprintf(echoHeader,"HTTP/1.1 200 OK\n\rContent-Length: %d\n\rContent-Type: text\n\rConnection: Close\n\r\n\r",strlen(buffer));
                            client.send(echoHeader,strlen(echoHeader));
                            client.send(buffer,strlen(buffer));
                            clientIsConnected = false;
                            printf("echo back done.\n\r");
                        }
                        break;
                }
            }
            printf("close connection.\n\rtcp server is listening...\n\r");
            client.close();
            led2 = false;
        }
    }
}