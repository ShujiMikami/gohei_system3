#ifndef _CAGEDRIVER_H_
#define _CAGEDRIVER_H_

typedef struct{
    double temperature;
    char statusMessage[26];
    char uvStatusMessage[5];
}CageStatus_t;

void CageDriveThread();
CageStatus_t GetCageStatus();
void UVOnFromEther();
void UVOffFromEther();
void UVToggleFromEther();

#endif