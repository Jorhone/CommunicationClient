#ifndef CABSTRACTCOMMUNICATIONCONFIG_H
#define CABSTRACTCOMMUNICATIONCONFIG_H

#include "CommunicationClientDefine.h"

class CAbstractCommunicationConfig
{
public:
    CAbstractCommunicationConfig(){};
    virtual ~CAbstractCommunicationConfig(){};

    virtual nsCommunicationClient::eCommunicationMedium GetCommunicationMedium(void) = 0;
    virtual bool CheckDataAvailable(void) = 0;
};

#endif // CABSTRACTCOMMUNICATIONCONFIG_H
