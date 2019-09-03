#include "HTTPGenerator.h"
#include <string.h>
#include <stdio.h>

static void createHTMLBody(char* buffer, uint16_t bufferLength, double temperature, char* controlStatus);

void CreateTopPage(char* buffer, uint16_t bufferLength, double temperature, char* controlStatus)
{
    char html[256];
    createHTMLBody(html, sizeof(html), temperature, controlStatus);
    
    if(bufferLength > strlen(html)){

        sprintf(buffer, "HTTP/1.1 200 OK\n\rContent-Length: %d\n\rContent-Type: text\n\rConnection: Close\n\r\n\r%s\r\n", strlen(html), html);
    }
}
void createHTMLBody(char* buffer, uint16_t bufferLength, double temperature, char* controlStatus)
{
    //title
    char htmlBody[256] = {};

    sprintf(htmlBody, 
            "<h1>Gohei System ver3</h1>"
            "<h1>Current Temperature : %2.1f</h1>"
            "<h1>Control Status = %s</h1>"
            "<form method=\"GET\" action=\"./UVToggle\">"
            "<button type=\"submit\">UVToggle</button>"
            "</form>"
             , temperature, controlStatus);
    
    if(bufferLength > strlen(htmlBody)){
        strcpy(buffer, htmlBody);
    }
}