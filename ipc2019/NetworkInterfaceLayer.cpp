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

}

BOOL CNILayer::SetAdapter(const int index) {
	selected = alldevs;
	for (int i = 0; i < index && selected; i++) {
		selected = selected->next;
	}
	return selected ? TRUE : FALSE;
}
