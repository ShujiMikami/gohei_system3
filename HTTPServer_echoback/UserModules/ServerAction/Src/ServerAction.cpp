#include "ServerAction.h"
#include "mbed.h"
#include "EthernetInterface.h"
#include <stdio.h>
#include <string.h>
#include "HTTPAnalyze.h"
#include "HTTPGenerator.h"
#include "CageDriver.h"

#define PORT   80

EthernetInterface eth;

TCPSocketServer svr;
bool serverIsListened = false;

TCPSocketConnection client;
bool clientIsConnected = false;

DigitalOut led2(LED2); //socket connecting status

DigitalIn linkSignal(P1_25);
DigitalIn speedSignal(P1_26);
DigitalOut linkLamp(p30);
DigitalOut speedLamp(p29);

//なぜかTickを宣言しないとEthernetがこける
Ticker dummyTick;

void requestAction(char* requestMessage);

//DHCPServerConnection
static void connectToDHCPServer();

void EtherStatusLampThreadFunc()
{
    while(true){
        linkLamp = !linkSignal;
        speedLamp = !speedSignal;
        wait(0.05);
    }
}

void ServerThreadFunc()
{
    //setup ethernet interface
    eth.init(); //Use DHCP
    
    //Print MAC Address
    printf("[Server Thread]MAC Address = %s\r\n", eth.getMACAddress());

    //Connect to DHCP server
    connectToDHCPServer();

    //Print IP Address
    printf("[Server Thread]IP Address is %s\n\r", eth.getIPAddress());

    //setup tcp socket
    if(svr.bind(PORT)< 0) {
        printf("[Server Thread]tcp server bind failed.\n\r");
        return;
    } else {
        printf("[Server Thread]tcp server bind successed.\n\r");
        serverIsListened = true;
    }

    if(svr.listen(1) < 0) {
        printf("[Server Thread]tcp server listen failed.\n\r");
        return;
    } else {
        printf("[Server Thread]tcp server is listening...\n\r");
    }

    //listening for http GET request
    while (serverIsListened) {
        //blocking mode(never timeout)
        if(svr.accept(client)<0) {
            printf("[Server Thread]failed to accept connection.\n\r");
        } else {
            printf("[Server Thread]connection success!\n\rIP: %s\n\r",client.get_address());
            clientIsConnected = true;
            led2 = true;
            
            while(clientIsConnected) {
                char buffer[400] = {};

                int receiveStatus = client.receive(buffer, 399);

                switch(receiveStatus) {
                    case 0:
                        printf("[Server Thread]recieved buffer is empty.\n\r");
                        clientIsConnected = false;
                        break;
                    case -1:
                        printf("[Server Thread]failed to read data from client.\n\r");
                        clientIsConnected = false;
                        break;
                    default:
                        printf("[Server Thread]Recieved Data: %d\n\r\n\r%.*s\n\r",strlen(buffer),strlen(buffer),buffer);
                        requestAction(buffer);
                        break;
                }
            }
            printf("[Server Thread]close connection.\n\r[Server Thread]tcp server is listening...\n\r");
            client.close();
            led2 = false;
        }
    }
}
void requestAction(char* requestMessage)
{
    //request line
    HTTPRequest_t request(requestMessage);

    char requestLine[50];

    request.GetRequestLine(requestLine, sizeof(requestLine));
    printf("request line = %s\r\n", requestLine);

    request.GetURI(requestLine, sizeof(requestLine));
    printf("uri is = %s\r\n", requestLine);

    if(strcmp(requestLine, "/") == 0){
        char htmlToSend[256] = {};
        char message[] = "Cooling";
        CreateTopPage(htmlToSend, sizeof(htmlToSend), 25.5, message);
        printf("[Server Thread]%s", htmlToSend);

        client.send(htmlToSend, strlen(htmlToSend));
       
        clientIsConnected = false;
    }else if(strcmp(requestLine, "UVToggle")){
        char htmlToSend[256] = {};
        char message[] = "Cooling";
        CreateTopPage(htmlToSend, sizeof(htmlToSend), 30.5, message);
        printf("[Server Thread]%s", htmlToSend);

        client.send(htmlToSend, strlen(htmlToSend));

        clientIsConnected = false;
    }

    request.GetProtocolVersion(requestLine, sizeof(requestLine));
    printf("protocol is %s \r\n", requestLine);
}

void connectToDHCPServer()
{
    while(true){
        printf("[Server Thread]Trying to connect DHCPServer...\r\n");

        if(eth.connect() == 0){
            printf("[Server Thread]connection success\r\n");
            break;
        } else {
            printf("[Server Thread]connection fail\r\n");
        }
    }
}