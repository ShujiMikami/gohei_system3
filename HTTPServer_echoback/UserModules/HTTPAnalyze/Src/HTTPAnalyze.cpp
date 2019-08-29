#include "HTTPAnalyze.h"
#include <string.h>

#include <mbed.h>

HTTPRequest_t::HTTPRequest_t(char* binaryData)
{
    int length = strlen(binaryData);

    messageBuffer = new char[length + 1];

    strncpy(messageBuffer, binaryData, length);

    messageBuffer[length] = '\0';

    splitBuffer();
}
HTTPRequest_t::~HTTPRequest_t()
{
    delete[] messageBuffer;
}
void HTTPRequest_t::GetRequestLine(char* buffer, unsigned short bufferSize)
{
    if(strlen(requestLine) + 1 <= bufferSize){
        strcpy(buffer, requestLine);
    }
}
HTTPRequest_t::HTTPMethod_t HTTPRequest_t::GetMethod()
{
    HTTPMethod_t result = NOT_DEFINED;

    if(strncmp(requestLine, "GET", 3) == 0){
        result = GET;
    } else if(strncmp(requestLine, "POST", 4) == 0){
        result = POST;
    } else{
        result = NOT_DEFINED;
    }

    return result;
}
void HTTPRequest_t::GetURI(char* buffer, unsigned short bufferSize)
{

    char strToRead[64];

    GetRequestLine(strToRead, sizeof(strToRead));

    strtok(strToRead, " ");


}
void HTTPRequest_t::GetProtocolVersion(char* buffer, unsigned short bufferSize)
{

}
void HTTPRequest_t::GetHeader(char* buffer, unsigned short bufferSize)
{
    if(strlen(header) + 1 <= bufferSize){
        strcpy(buffer, header);
    }
}
void HTTPRequest_t::splitBuffer()
{
    //request line
    requestLine = strtok(messageBuffer, "\r\n");
    
    //header
    header = strtok(NULL, "\r\n");
}