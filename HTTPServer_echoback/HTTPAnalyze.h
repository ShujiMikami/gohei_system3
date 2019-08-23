#ifndef _HTTPANALYZE_H_
#define _HTTPANALYZE_H_

#define HTTP_REQUEST_BUFFER_SIZE 1024

class HTTPRequest_t{
typedef enum{
    GET,
    POST,
    NOT_DEFINED
}HTTPMethod_t;
public:
private:
    char messageBuffer[HTTP_REQUEST_BUFFER_SIZE];
    char* requestLine;
    char* uri;
    char* protocolVersion;
    char* header;
public:
    HTTPRequest_t(char* data);
    void GetRequestLine(char* buffer, unsigned short bufferSize);
    HTTPMethod_t GetMethod();
    void GetURI(char* buffer, unsigned short bufferSize);
    void GetProtocolVersion(char* buffer, unsigned short bufferSize);
    void GetHeader(char* buffer, unsigned short bufferSize);
private:
    void splitBuffer();
};


#endif
