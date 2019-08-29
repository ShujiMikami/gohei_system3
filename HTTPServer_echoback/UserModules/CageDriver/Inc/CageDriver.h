#ifndef _CAGEDRIVER_H_
#define _CAGEDRIVER_H_

typedef struct{
    double temperature;
    char statusMessage[26];
}CageStatus_t;

void CageDriveThread();
CageStatus_t GetCageStatus();

#endif