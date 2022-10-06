#pragma once
//NetworkInterfaceLayer.h: interface for the CNetworkInterfaceLayer class.

#if !defined(AFX_NETWORKINTERFACELAYER_H)
#define AFX_NETWORKINTERFACELAYER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"
#include <pcap.h>
#include <tchar.h>
#include <Packet32.h>


class CNILayer : public CBaseLayer { //Thread 구현 
    pcap_if_t* alldevs; //all information of 
    int inum; //adapter index
public:
    CNILayer(char* pName); //생성 시에 pcap_findalldevs로 adapter 정보 저장
    ~CNILayer(); //소멸자
    BOOL Receive(); //Packet을 받아서 상위 layer(여기에서는 ethernet layer)로 전달한다. little endian, big endian 변환이 필요?
    BOOL Send(unsigned char* ppayload, int nlength); // little endian, big endian 변환이 필요?
    pcap_if_t* GetMacAddressList(); //alldevs를 return
    BOOL SetAdapter(const int index); //set inum, return value is success or fail
};

#endif // !defined(AFX_NETWORKINTERFACELAYER_H)