#include "HTTPGenerator.h"
#include <string.h>
#include <stdio.h>

static void createHTMLBody(char* buffer, uint16_t bufferLength, double temperature, char* controlStatus, char* uvCOntrolStatus);

void CreateTopPage(char* buffer, uint16_t bufferLength, double temperature, char* controlStatus, char* uvControlStatus)
{
    char html[512];
    createHTMLBody(html, sizeof(html), temperature, controlStatus, uvControlStatus);
    
    if(bufferLength > strlen(html)){
        //sprintf(buffer, "HTTP/1.1 200 OK\n\rContent-Length: %d\n\rContent-Type: text\n\rConnection: Close\n\r\n\r%s\r\n", strlen(html), html);
        sprintf(buffer, "HTTP/1.1 200 OK\n\rContent-Length: %d\n\rContent-Type: text/html\n\rConnection: Close\n\r\n\r%s\n\r", strlen(html), html);
    }
}
void createHTMLBody(char* buffer, uint16_t bufferLength, double temperature, char* controlStatus, char* uvControlStatus)
{
    //title
    char htmlBody[512] = {};

    sprintf(htmlBody, 
            "<h1>Gohei System ver3</h1>"
            "<h1>Current Temperature : %2.1f</h1>"
            "<h1>Control Status = %s</h1>"
            "<h1>UV Control Status = %s</h1>"
            "<form method=\"GET\" action=\"./UVToggle\">"
            "<button type=\"submit\">UVToggle</button>"
            "</form>"
             , temperature, controlStatus, uvControlStatus);
    
    if(bufferLength > strlen(htmlBody)){
        strcpy(buffer, htmlBody);
    }
}