#include "stdafx.h"
#include "pch.h"
#include "FileTransLayer.h"

CFileTransLayer::CFileTransLayer(char* pName)
	: CBaseLayer(pName), Head(nullptr), totalLength(0)
{}

CFileTransLayer::~CFileTransLayer()
{}

void CFileTransLayer::make_frame(unsigned char* ppayload, unsigned long nlength, unsigned short type, int seq, unsigned char fname) {
	m_sFileApp.fapp_totlen = nlength;
	m_sFileApp.fapp_type = type;
	m_sFileApp.fapp_seq_num = (unsigned long)seq;
	m_sFileApp.fapp_msg_type = fname;
	memcpy(m_sFileApp.fapp_data, ppayload + (seq * MAX_APP_DATA), nlength);
	((CEthernetLayer*)(this->GetUnderLayer()))->Send((unsigned char*)&m_sFileApp, nlength + SIZE_FILE_HEADER, 0x2090);
}

BOOL CFileTransLayer::Send(unsigned char* ppayload, int nlength, CString strPath)
{	
	CString strFileName;
	strFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\'));
	//message length가 1496bytes 보다 작으면, 바로 하위 레이어로 전달
	if (nlength < MAX_APP_DATA) {
		make_frame(ppayload, nlength, 0x00, 0, (unsigned char)strFileName.GetBuffer(0));
		return TRUE;
	}
	//message length가 1496bytes 보다 큰 경우 단편화 작업...
	else {
		int length = nlength - MAX_APP_DATA;	//최초 단편화 frame을 고려해 1496bytes 만큼 뺀다.
		int i = 0;	//몇 번 단편화 하는지 count 변수

		//최초 단편화 된 frame을 만들어 하위 레이어로 전달
		make_frame(ppayload, MAX_APP_DATA, 0x01, i, (unsigned char)strFileName.GetBuffer(0));
		i++;

		for (; nlength - (i * MAX_APP_DATA) > MAX_APP_DATA; i++, length -= (i * MAX_APP_DATA)) {
			//처음과 끝 부분을 제외한 중간 부분 frame을 만들어 하위 레이어로 전달
			make_frame(ppayload, MAX_APP_DATA, 0x02, i, (unsigned char)strFileName.GetBuffer(0));
		}
		//마지막 단편화 된 frame을 만들어 하위 레이어로 전달
		make_frame(ppayload, length, 0x03, i, (unsigned char)strFileName.GetBuffer(0));
		return TRUE;
	}
	return FALSE;
}

void CFileTransLayer::add_after(FrameSeq* prev, unsigned char* data, unsigned char seq) {
	FrameSeq* tmp = new FrameSeq;
	(tmp->data).Format(_T("%s"), data);
	tmp->seq = seq;
	tmp->next = prev->next;
	prev->next = tmp;
}

bool CFileTransLayer::seq_compare(FrameSeq* p, unsigned char seq) {
	if (p->seq < seq)return true;
	else return false;
}

void CFileTransLayer::add(unsigned char* data, unsigned char seq) {
	FrameSeq* p = Head;
	FrameSeq* q = nullptr;
	while (seq_compare(p, seq)) {
		q = p;
		p = p->next;
	}
	add_after(q, data, seq);
}


BOOL CFileTransLayer::Receive(unsigned char* ppayload)
{
	LPFILE_APP file_data = (LPFILE_APP)ppayload;

	if (file_data->fapp_type == 0x00)
	{
		unsigned char size = file_data->fapp_totlen;
		unsigned char GetBuff[MAX_APP_DATA];
		memset(GetBuff, '\0', MAX_APP_DATA);  // GetBuff를 초기화해준다.

		memcpy(GetBuff, file_data->fapp_data, size);
		CString a = _T("");
		a.Format(_T("%s"), file_data->fapp_msg_type);
		CFile file;
		file.Open(a, CFile::modeCreate | CFile::modeWrite, NULL);
		file.Write(GetBuff, size);
		file.Close();
		/*CString Msg;

		Msg.Format(_T("%s"), GetBuff);

		this->mp_aUpperLayer[0]->Receive((unsigned char*)Msg.GetBuffer(0));*/
		return true;
	}
	else {
		if (file_data->fapp_type == 0x01) {
			FrameSeq* FirstFrame = new FrameSeq;
			FirstFrame->seq = 0;
			(FirstFrame->data).Format(_T("%s"), file_data->fapp_data);
			FirstFrame->next = nullptr;
			totalLength = file_data->fapp_totlen;
			Head = FirstFrame;
			return true;
		}
		else if (file_data->fapp_type == 0x02) {
			add(file_data->fapp_data, file_data->fapp_seq_num);
			return true;
		}
		else {
			CString mergeMsg = "";
			int i = 0;
			for (FrameSeq* _head = Head; _head != nullptr; i++, _head = _head->next) {
				mergeMsg += _head->data;
			}
			mp_aUpperLayer[0]->Receive((unsigned char*)mergeMsg.GetBuffer(0));
			deleteList();
			return true;
		}
	}
	return false;
}

void CFileTransLayer::deleteList() {
	FrameSeq* tmp = Head;
	while (tmp != nullptr) {
		Head = Head->next;
		delete tmp;
		tmp = Head;
	}
	Head = nullptr;
}
