// FileLayer.h: interface for the CFileLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_)
#define AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "pch.h"

class CFileLayer
	: public CBaseLayer
{
public:
	void	SetFileInfo(CFile& file);
	BOOL	Receive();
	BOOL	Send(unsigned char* ppayload, int nlength);

	CFileLayer(char* pName);
	typedef struct _FILE_INFO {
		unsigned long totallength;
		unsigned char filename[512];
	}FILE_INFO, * PFILE_INFO;
	typedef struct _FILE_HEADER {
		unsigned long fapp_totlen; //4
		unsigned short fapp_type; //2
		unsigned char faa_msg_type; //1
		unsigned char unused;  //1
		unsigned long fapp_seq_num; //4
		unsigned char fapp_data[1488];
	}FILE_HEADER, * PFILE_HEADER;

	enum {
		DATA_TYPE_FILE_INFO = 0x00,
		DATA_TYPE_CONT = 0x01,
		DATA_END = 0x02
	};
	virtual ~CFileLayer();
};

#endif // !defined(AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_)
