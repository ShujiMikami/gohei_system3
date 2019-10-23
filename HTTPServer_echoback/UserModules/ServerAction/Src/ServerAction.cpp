#include "ServerAction.h"
#include "mbed.h"
#include "EthernetInterface.h"
#include <stdio.h>
#include <string.h>
#include "HTTPAnalyze.h"
#include "HTTPGenerator.h"
#include "CageDriver.h"

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) while(0)
#endif

#define PORT   80

EthernetInterface eth;

TCPSocketServer svr;
bool serverIsListened = false;

TCPSocketConnection client;
bool clientIsConnected = false;

DigitalOut led2(LED2); //socket connecting status

//なぜかTickを宣言しないとEthernetがこける
Ticker dummyTick;

void requestAction(char* requestMessage);

//DHCPServerConnection
static void connectToDHCPServer();

void ServerThreadFunc()
{
    //setup ethernet interface
    eth.init(); //Use DHCP
    
    //Print MAC Address
    DEBUG_PRINT("[Server Thread]MAC Address = %s\r\n", eth.getMACAddress());

    //Connect to DHCP server
    connectToDHCPServer();

    //Print IP Address
    DEBUG_PRINT("[Server Thread]IP Address is %s\n\r", eth.getIPAddress());

    //setup tcp socket
    if(svr.bind(PORT)< 0) {
        DEBUG_PRINT("[Server Thread]tcp server bind failed.\n\r");
        return;
    } else {
        DEBUG_PRINT("[Server Thread]tcp server bind successed.\n\r");
        serverIsListened = true;
    }

    if(svr.listen(1) < 0) {
        DEBUG_PRINT("[Server Thread]tcp server listen failed.\n\r");
        return;
    } else {
        DEBUG_PRINT("[Server Thread]tcp server is listening...\n\r");
    }

    //listening for http GET request
    while (serverIsListened) {
        //blocking mode(never timeout)
        if(svr.accept(client)<0) {
            DEBUG_PRINT("[Server Thread]failed to accept connection.\n\r");
        } else {
            DEBUG_PRINT("[Server Thread]connection success!\n\rIP: %s\n\r",client.get_address());
            clientIsConnected = true;
            led2 = true;
            
            while(clientIsConnected) {
                char buffer[1024] = {};
                int receiveStatus = client.receive(buffer, 1023);

                switch(receiveStatus) {
                    case 0:
                        DEBUG_PRINT("[Server Thread]recieved buffer is empty.\n\r");
                        clientIsConnected = false;
                        break;
                    case -1:
                        DEBUG_PRINT("[Server Thread]failed to read data from client.\n\r");
                        clientIsConnected = false;
                        break;
                    default:
                        requestAction(buffer);
                        DEBUG_PRINT("[Server Thread]Recieved Data: %d\n\r\n\r%.*s\n\r",strlen(buffer), strlen(buffer), buffer);
                        break;
                }
            }
            DEBUG_PRINT("[Server Thread]close connection.\n\r[Server Thread]tcp server is listening...\n\r");
            client.close();
            led2 = false;
        }
    }
}
void requestAction(char* requestMessage)
{
    //request line
    HTTPRequest_t request(requestMessage);

    char requestLine[250];

    request.GetRequestLine(requestLine, sizeof(requestLine));
    DEBUG_PRINT("request line = %s\r\n", requestLine);

    request.GetURI(requestLine, sizeof(requestLine));
    DEBUG_PRINT("uri is = %s\r\n", requestLine);

    if(strcmp(requestLine, "/") == 0){
        char htmlToSend[1024] = {};
        
        CageStatus_t cageStatus = GetCageStatus();
        CreateTopPage(htmlToSend, sizeof(htmlToSend), cageStatus.temperature, cageStatus.statusMessage, cageStatus.uvStatusMessage);
        DEBUG_PRINT("[Server Thread]Top page access\r\n");
        DEBUG_PRINT("[Server Thread]%s", htmlToSend);

        client.send(htmlToSend, strlen(htmlToSend));
    }else if(strcmp(requestLine, "/UVToggle") == 0){
        char htmlToSend[1024] = {};

        UVToggleFromEther();

        CageStatus_t cageStatus = GetCageStatus();

        CreateTopPage(htmlToSend, sizeof(htmlToSend), cageStatus.temperature, cageStatus.statusMessage, cageStatus.uvStatusMessage);
        DEBUG_PRINT("[Server Thread]toggle page access\r\n");
        DEBUG_PRINT("[Server Thread]%s", htmlToSend);

        client.send(htmlToSend, strlen(htmlToSend));
    }else if(strcmp(requestLine, "/UVToggle?") == 0){
        char htmlToSend[1024] = {};

        UVToggleFromEther();

        CageStatus_t cageStatus = GetCageStatus();

        CreateTopPage(htmlToSend, sizeof(htmlToSend), cageStatus.temperature, cageStatus.statusMessage, cageStatus.uvStatusMessage);
        DEBUG_PRINT("[Server Thread]toggle page access\r\n");
        DEBUG_PRINT("[Server Thread]%s", htmlToSend);

        client.send(htmlToSend, strlen(htmlToSend));
    }


    clientIsConnected = false;
    request.GetProtocolVersion(requestLine, sizeof(requestLine));
    DEBUG_PRINT("protocol is %s \r\n", requestLine);
}

void connectToDHCPServer()
{
    while(true){
        DEBUG_PRINT("[Server Thread]Trying to connect DHCPServer...\r\n");

        if(eth.connect() == 0){
            DEBUG_PRINT("[Server Thread]connection success\r\n");
            break;
        } else {
            DEBUG_PRINT("[Server Thread]connection fail\r\n");
        }
    }
}