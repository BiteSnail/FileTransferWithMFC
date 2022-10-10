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
#include <WinSock2.h>
#include <thread>
#pragma comment(lib, "packet.lib")
#pragma comment(lib, "ws2_32.lib")

class CNILayer : public CBaseLayer { //Thread 구현 
    pcap_if_t* allDevices; //all information of 
    pcap_if_t* device; //adapter index
    pcap_t* m_AdapterObject;
    UCHAR data[1500];
    char errbuf[PCAP_ERRBUF_SIZE];
    UCHAR macAddress[6];
    LPADAPTER adapter = NULL;
    PPACKET_OID_DATA OidData;
    bool canRead = false;
public:
    CNILayer(char* pName); //생성 시에 pcap_findalldevs로 adapter 정보 저장
    ~CNILayer(); //소멸자
    BOOL Receive(unsigned char * pkt); //Packet을 받아서 상위 layer(여기에서는 ethernet layer)로 전달한다. little endian, big endian 변환이 필요?
    BOOL Send(unsigned char* ppayload, int nlength); // little endian, big endian 변환이 필요?
    void GetMacAddressList(CStringArray& adapterlist); //alldevs를 return
    UCHAR* SetAdapter(const int index); //set inum, return value is MAC ADDRESS
    static UINT ThreadFunction_RECEIVE(LPVOID pParam);
    static UINT ThreadFunction_SEND(LPVOID pParam);
    void flip();
};

#endif // !defined(AFX_NETWORKINTERFACELAYER_H)