#ifndef CABSTRACTCOMMUNICATIONCONF_H
#define CABSTRACTCOMMUNICATIONCONF_H

#include "CommunicationClientDefine.h"

class CAbstractCommunicationConf
{
public:
    virtual ~CAbstractCommunicationConf(){};

    virtual nsCommunicationClient::eCommunicationMedium GetCommunicationMedium(void) = 0;
    virtual bool CheckDataAvailable(void) = 0;
};

#endif // CABSTRACTCOMMUNICATIONCONF_H
