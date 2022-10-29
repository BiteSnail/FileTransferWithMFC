#pragma once
#include "BaseLayer.h"
#include "pch.h"

#define MAX_APP_DATA 1488
#define SIZE_FILE_HEADER 12

class FileTransLayer :
    public CBaseLayer
{

public:
    FileTransLayer();
    ~FileTransLayer();
    BOOL Receive(unsigned char* frame); 
    BOOL Send(unsigned char* ppayload, int nlength); 

    struct _FILE_APP {
        unsigned long   fapp_totlen;
        unsigned short  fapp_type;
        unsigned char   faa_msg_type;
        unsigned char   unused;
        unsigned long   fapp_seq_num;
        unsigned char   fapp_data[MAX_APP_DATA];
    }FILE_APP, *PFILE_APP;
};

