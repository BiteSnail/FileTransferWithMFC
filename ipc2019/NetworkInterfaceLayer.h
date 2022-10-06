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


class CNILayer : public CBaseLayer { //Thread ���� 
    pcap_if_t* alldevs; //all information of 
    pcap_if_t* selected; //adapter index
public:
    CNILayer(char* pName); //���� �ÿ� pcap_findalldevs�� adapter ���� ����
    ~CNILayer(); //�Ҹ���
    BOOL Receive(); //Packet�� �޾Ƽ� ���� layer(���⿡���� ethernet layer)�� �����Ѵ�. little endian, big endian ��ȯ�� �ʿ�?
    BOOL Send(unsigned char* ppayload, int nlength); // little endian, big endian ��ȯ�� �ʿ�?
    pcap_if_t* GetMacAddressList(); //alldevs�� return
    BOOL SetAdapter(const int index); //set inum, return value is success or fail
};

#endif // !defined(AFX_NETWORKINTERFACELAYER_H)