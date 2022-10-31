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

void CFileTransLayer::SetFilePath(CString strPath)
{
	//���� ��θ� �޾ƿ�
	//�޾ƿ� ���� ��ο��� ���� �̸� ����
	CString strFileName;
	strFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind('\\'));
	unsigned char fName = new unsigned char[strFileName.GetLength + 1];
	fName = strFileName;

	CString a = _T("");
	a.Format(_T("%s"), strFileName);

	CFile file;
	file.Open(a, CFile::modeRead, NULL);
	int size = file.GetLength();
	unsigned char* content = new unsigned char[size+1];
	ZeroMemory(content,size);
	file.Read(content, size);	//���� ���� �б�
	Send(content, size, fName);
	file.Close();
}

BOOL CFileTransLayer::Send(unsigned char* ppayload, int nlength, unsigned char strFileName)
{
	//message length�� 1488bytes ���� ������, �ٷ� ���� ���̾�� ����
	if (nlength < MAX_APP_DATA) {
		make_frame(ppayload, nlength, 0x00, 0, strFileName);
		return TRUE;
	}
	//message length�� 1488bytes ���� ū ��� ����ȭ �۾�...
	else {
		int length = nlength - MAX_APP_DATA;	//���� ����ȭ frame�� ����� 1488bytes ��ŭ ����.
		int i = 0;	//�� �� ����ȭ �ϴ��� count ����

		//���� ����ȭ �� frame�� ����� ���� ���̾�� ����
		make_frame(ppayload, MAX_APP_DATA, 0x01, i,strFileName);
		i++;

		for (; nlength - (i * MAX_APP_DATA) > MAX_APP_DATA; i++, length -= (i * MAX_APP_DATA)) {
			//ó���� �� �κ��� ������ �߰� �κ� frame�� ����� ���� ���̾�� ����
			make_frame(ppayload, MAX_APP_DATA, 0x02, i, strFileName);
		}
		//������ ����ȭ �� frame�� ����� ���� ���̾�� ����
		make_frame(ppayload, length, 0x03, i, strFileName);
		return TRUE;
	}
	return FALSE;
}

void CFileTransLayer::add_after(FrameSeq* prev, unsigned char* data, unsigned long seq) {
	FrameSeq* tmp = new FrameSeq;
	(tmp->data).Format(_T("%s"), data);
	tmp->seq = seq;
	tmp->next = prev->next;
	prev->next = tmp;
}

bool CFileTransLayer::seq_compare(FrameSeq* p, unsigned long seq) {
	if (p->seq < seq)return true;
	else return false;
}

void CFileTransLayer::add(unsigned char* data, unsigned long seq) {
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

	CString a = _T("");
	a.Format(_T("%s"), file_data->fapp_msg_type);
	CFile file;

	if (file_data->fapp_type == 0x00)
	{
		unsigned char size = file_data->fapp_totlen;
		unsigned char GetBuff[MAX_APP_DATA];
		memset(GetBuff, '\0', MAX_APP_DATA);  // GetBuff�� �ʱ�ȭ���ش�.
		memcpy(GetBuff, file_data->fapp_data, size);
		//���� ���� ����
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

			//���� ����
			file.Open(a, CFile::modeCreate, NULL);
			file.Close();
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
			file.Open(a, CFile::modeWrite, NULL);
			file.Write(mergeMsg, mergeMsg.GetLength());
			file.Close();
			/*mp_aUpperLayer[0]->Receive((unsigned char*)mergeMsg.GetBuffer(0));*/
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
