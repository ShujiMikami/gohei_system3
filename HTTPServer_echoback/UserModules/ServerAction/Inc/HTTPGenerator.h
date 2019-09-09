#ifndef _HTTPGENERATOR_H_
#define _HTTPGENERATOR_H_

#include <stdint.h>

void CreateTopPage(char* buffer, uint16_t bufferLength, double temperature, char* controlStatus, char* uvControlStatus);

#endif
