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
	: CBaseLayer(pName), device(NULL) {
	if (pcap_findalldevs(&allDevices, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		allDevices = NULL;
	}

	OidData = (PPACKET_OID_DATA)malloc(sizeof(PACKET_OID_DATA));
	OidData->Oid = 0x01010101;
	OidData->Length = 6;
}

CNILayer::~CNILayer() {
	pcap_if_t* temp = allDevices;

	while (temp) {
		temp = allDevices;
		allDevices = allDevices->next;
		delete(temp);
		delete(OidData);
	}
}

BOOL CNILayer::Receive() {
	struct pcap_pkthdr* header;
	const u_char* pkt_data;

	int result = pcap_next_ex(m_AdapterObject, &header, &pkt_data);

	if (result == 0) {
		AfxMessageBox("패킷 없음 ㅋㅋ");
		return FALSE;
	}
	else if (result == 1) {
		AfxMessageBox("패킷 받았음 ㅋㅋ");
		return TRUE;
	}
}

BOOL CNILayer::Send(unsigned char* ppayload, int nlength) {
	if (pcap_sendpacket(m_AdapterObject, ppayload, nlength))
	{
		AfxMessageBox(_T("패킷 전송 실패"));
		return FALSE;
	}
	return TRUE;
}

UCHAR* CNILayer::SetAdapter(const int index) { //MAC 주소를 전달!
	device = allDevices;
	for (int i = 0; i < index && device; i++) {
		device = device->next;
	}

	m_AdapterObject = pcap_open_live(device->name, 65536, 0, 1000, errbuf);
	if (m_AdapterObject == nullptr)
	{
		AfxMessageBox(_T("어뎁터 연결 실패!"));
	}

	adapter = PacketOpenAdapter(device->name);
	PacketRequest(adapter, FALSE, OidData);
	net_addr.s_addr = net;
	mask_addr.s_addr = mask; 

	return OidData->Data;
}

void CNILayer::GetMacAddressList(CStringArray &adapterlist) {
	for (pcap_if_t* d = allDevices; d; d = d->next) {
		adapterlist.Add(d->description);
	}
}