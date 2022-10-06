// NetworkInterfaceLayer.cpp: implementation of the NetworkInterfaceLayer class.

#include "stdafx.h"
#include "pch.h"
#include "NetworkInterfaceLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNILayer::CNILayer(char *pName) 
	: CBaseLayer(pName), selected(NULL) {
	char errbuf[PCAP_ERRBUF_SIZE];
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		alldevs = NULL;
	}

	OidData = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
	OidData->Oid = 0x01010101;
	OidData->Length = 6;
}

CNILayer::~CNILayer() {
	pcap_if_t* temp = alldevs;

	while (temp) {
		temp = alldevs;
		alldevs = alldevs->next;
		delete(temp);
	}
}

BOOL CNILayer::Receive() {
	return 1;
}

BOOL CNILayer::Send(unsigned char* payload, int nlength) {
	return 1;
}

UCHAR* CNILayer::SetAdapter(const int index) { //MAC 주소를 전달!
	CString macaddress;
	selected = alldevs;
	for (int i = 0; i < index && selected; i++) {
		selected = selected->next;
	}
	adapter = PacketOpenAdapter(selected->name);
	PacketRequest(adapter, FALSE, OidData);
	net_addr.s_addr = net;
	mask_addr.s_addr = mask; 

	return OidData->Data;
}

void CNILayer::GetMacAddressList(CStringArray &adapterlist) {
	for (pcap_if_t* d = alldevs; d; d = d->next) {
		adapterlist.Add(d->description);
	}
}