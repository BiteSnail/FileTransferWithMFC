#include "pch.h"
#include "FileTransLayer.h"

FileTransLayer::FileTransLayer(char* name):CBaseLayer(name) {
	FilePath = "";
	mHeader.fapp_totlen = 0;
	mHeader.fapp_type = 0;
	mHeader.fapp_seq_num = 0;
	mHeader.faa_msg_type = 0;
	mHeader.unused = 0;
	memset(mHeader.fapp_data, 0, MAX_APP_DATA);
	return;
}

FileTransLayer::~FileTransLayer() {
	return;
}

BOOL FileTransLayer::Send(unsigned char * ppayload, int nlength) {
	return ((CEthernetLayer*)(this->GetUnderLayer()))->Send((unsigned char*)&mHeader, nlength + CHAT_HEADER_SIZE, 0x2090);
}

BOOL FileTransLayer::Receive(unsigned char* frame) {
	return true;
}

void FileTransLayer::SetHeader(
	unsigned long len,
	unsigned short type,
	unsigned long seq_num,
	unsigned char* data
) {
	mHeader.fapp_totlen = len;
	mHeader.fapp_type = type;
	mHeader.fapp_seq_num = seq_num;
	memcpy(mHeader.fapp_data, data, len > MAX_APP_DATA ? MAX_APP_DATA : len);
}

UINT FileTransLayer::FILE_SEND(LPVOID pParam) {
	FileTransLayer* pFL = (FileTransLayer*)pParam;
	CFile SendFile;
	unsigned long totallength;
	unsigned char buffer[MAX_APP_DATA];
	unsigned int seq = 0;
	SendFile.Open(pFL->GetFilePath(), CFile::modeRead); //open file for sending
	totallength = SendFile.GetLength();

	CArchive read_file(&SendFile, CArchive::load);

	PFILE_APP header = &(pFL->mHeader);

	pFL->SetHeader(totallength, FILE_INFO, seq,(unsigned char*)(SendFile.GetFileName().GetBuffer(0)));
	pFL->Send((unsigned char*)&(pFL->mHeader), SIZE_FILE_HEADER + MAX_APP_DATA);
	
	while (read_file.Read(buffer, MAX_APP_DATA)) {
		pFL->SetHeader(totallength > MAX_APP_DATA ? MAX_APP_DATA : totallength, FILE_MORE, ++seq, buffer);
		pFL->Send((unsigned char*)&(pFL->mHeader), SIZE_FILE_HEADER + totallength > MAX_APP_DATA ? MAX_APP_DATA : totallength);
		totallength -= MAX_APP_DATA;
	}
	
	pFL->SetHeader(0, FILE_LAST, ++seq, (unsigned char*)"aaaaaaaa");
	pFL->Send((unsigned char*)&(pFL->mHeader), SIZE_FILE_HEADER + 8);


	return 0;
}
