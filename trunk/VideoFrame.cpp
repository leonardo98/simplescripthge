#include "VideoFrame.h"
#include "Ogg\Ogg.h"

void VideoFrame::TheoraInit()
{
	//������� ��� ��������� ogg/theora:
	memset(&OGG_STREAM_STATE_THEORA, 0x00, sizeof(ogg_stream_state));
	memset(&OGG_SYNC_STATE, 0x00, sizeof(ogg_sync_state));
	memset(&OGG_PAGE, 0x00, sizeof(ogg_page));
	memset(&OGG_PACKET, 0x00, sizeof(ogg_packet));
	memset(&THEORA_COMMENT, 0x00, sizeof(theora_comment));
	memset(&THEORA_INFO, 0x00, sizeof(theora_info));
	memset(&THEORA_STATE, 0x00, sizeof(theora_state));
	memset(&YUV_BUFFER, 0x00, sizeof(yuv_buffer));
	//����� ����
	currentfile=fopen(fileName.c_str(),"rb");
	if(currentfile==NULL)
		throw std::runtime_error("cannot open ogg-file");

	//�������������� �������� ��������� ���������� ogg
	ogg_stream_clear(&OGG_STREAM_STATE_THEORA);
	//�������������� "���� �������������" ogg
	ogg_sync_init(&OGG_SYNC_STATE);

	//������� ��� ��� ����������� ��� ��������� ��� �����
	theora_comment_init(&THEORA_COMMENT);
	theora_info_init(&THEORA_INFO);

	// ������ ���� ������ ����������� ������ theora
	bool START_THEORA_HEADER_STREAM=true;
	int THEORA_HEADER_PACKETS=0;  // ����� ������������ ������� ���������� theora

	do
	{
		// get_data_from_physical_stream - ������ ������ �� FILE*
		// � �������� �� � ����� ������ ogg
		if( get_data_from_physical_stream(currentfile, &OGG_SYNC_STATE) ==0)
			// �������� ����, �� ������ ����� ��� ������
			throw std::runtime_error("eof searched, terminate...");  
		// ogg_sync_pageout - ��������� ��������
		while( ogg_sync_pageout(&OGG_SYNC_STATE, &OGG_PAGE) >0)
			// 1-������ ������ �� ���������
			// 0-��������� ������ ������ ��� �������� ��������
		{
			// ��� �������� ������������ �������

			// ��� �������� ����������? ���� ���, ������� ������ ���������
			if( ogg_page_bos(&OGG_PAGE) ==false)
			{
				// ���, ��� �� �������� ����������
				// ������, �������� ���������� ���� ���������� ������� ���������
				// � �������� ������ ���� �������
				// ����� ������� ���� ���������� � ������ ������� ������

				// ���������� ��� ��������� � ���������� ����������
				push_page_into_logical_stream(&OGG_PAGE);
				// push_page_into_logical_stream - ���������� ���������
				// � ���������� ����� theora, ����
				// ��������� ������������� ����������� ������
				// � ��������� ������ �������� ������������

				// ������� �� ������
				START_THEORA_HEADER_STREAM=false;
				break;
			}
			else
			{
				// ��, ��� �������� ����������

				// �������� ���������� �����
				ogg_stream_state OGG_STREAM_STATE_TEST;
				memset(&OGG_STREAM_STATE_TEST, 0x00, sizeof(ogg_stream_state));

				// �������������� �������� ����� �� ��� �� ����� � ����� ��
				// ��������������� ������, ��� � � ������� ���������
				if(0!= ogg_stream_init(&OGG_STREAM_STATE_TEST,ogg_page_serialno(&OGG_PAGE)) )
					throw std::runtime_error("error during ogg_stream_init");

				// ��������� �������� � �������� �����
				if(0!= ogg_stream_pagein(&OGG_STREAM_STATE_TEST,&OGG_PAGE) )
					throw std::runtime_error("error during ogg_stream_pagein");

				// ���������� ������ �� ����� ��������� ������ � �����
				if( ogg_stream_packetout(&OGG_STREAM_STATE_TEST,&OGG_PACKET) ==-1)
					throw std::runtime_error("error during ogg_stream_packetout");

				// THEORA_HEADER_PACKETS - ����� �����������
				// ������������ ������� theora (�� �������)
				// �� ������������ theora ����� ������� ������ ���� ���
				if(THEORA_HEADER_PACKETS==0)
				{
					int dhr = theora_decode_header (&THEORA_INFO, &THEORA_COMMENT, &OGG_PACKET);
					// ���������� ��������� theora
					if(dhr<0)
					{
						// ��� �� ��������� theora	          
						// ������� ��������� ��������� ������
						ogg_stream_clear(&OGG_STREAM_STATE_TEST);
						//� ���������� ���� � ������� ���������� theora
					}
					else
					{
						// ��� ��������� theora!
						// ��� ����� ������� "��������������" ���������� ����� theora:
						memcpy(&OGG_STREAM_STATE_THEORA, &OGG_STREAM_STATE_TEST,
							sizeof(OGG_STREAM_STATE_TEST));
						// ������ �� ����� ������ ����� ������ �������� ������ theora
						THEORA_HEADER_PACKETS++;
						// ����� ����, ��� �� ����� ������������ �������� ����������� ������ theora,
						// ��� ���������� ��������� ��� ��������� ������������ ��������
						// ������ ������� � ��������� �� (���� �������, �������, �������)
					}
				}
			}
		}
	}
	while(START_THEORA_HEADER_STREAM);

	while(THEORA_HEADER_PACKETS<3)
	{
		int result=ogg_stream_packetout(&OGG_STREAM_STATE_THEORA,&OGG_PACKET);
		// ���� ������� ���������� ����, ������ �� ������� ������ ��� �������������
		// ������ �� ����� ��� � ������������ libogg, ��� � ����� �����

		if(result<0)
		{
			// ������ �������������, ������������ �����
			throw std::runtime_error("error during ogg_stream_packetout");
		}

		if(result>0)
		{
			// ������� ������� ������� ����� ���������� theora

			int result2=theora_decode_header(&THEORA_INFO,&THEORA_COMMENT,&OGG_PACKET);

			if(result2<0)
			{
				// ������ �������������, ������������ �����
				throw std::runtime_error("error during theora_decode_header (corrupt stream)");
			}

			++THEORA_HEADER_PACKETS;
		}

		// ��� ��������� ����������, ���� ������� �����
		// ��� ����� ��������� ����� ������, ����� ��� �������� ���-���� �������
		// �� ���������. ���� �� ��������, ����� ������ ������ ��� ������ �� �����:

		if( ogg_sync_pageout(&OGG_SYNC_STATE,&OGG_PAGE) >0)
			// ogg_sync_pageout - �������, ����� ������ �� ������ ������ ogg
			// � ���������� �� � ogg_page
		{
			//�� ����� ��������� � ������ �...
			push_page_into_logical_stream(&OGG_PAGE);
			// ...������ ��� ������ � ���������� �����
		}
		else
		{
			// ������ �� � ������ �� �����
			int ret=get_data_from_physical_stream(currentfile,&OGG_SYNC_STATE);
			// ���� ������ ������! ������ �� �� �����

			if(ret==0)
			{
				// ����� ���� ��������!
				throw std::runtime_error("eof searched. terminate...");
			}
		}
	}

	// ������� �� ��������, ���, ��� ������. �������������� ��������

	// �������������� �����-�����
	theora_decode_init(&THEORA_STATE,&THEORA_INFO);
	report_colorspace(&THEORA_INFO);
	dump_comments(&THEORA_COMMENT);
}

void VideoFrame::report_colorspace(theora_info *ti)
{
	switch(ti->colorspace)
	{
	case OC_CS_UNSPECIFIED:
		// nothing to report
		break;
	case OC_CS_ITU_REC_470M:
		LOG_STRING(LOG_NOTE, "Encoder specified ITU Rec 470M (NTSC) color.");
		// ������� � ��� ���������� � �������� ������������
		break;
	case OC_CS_ITU_REC_470BG:
		LOG_STRING(LOG_NOTE, "Encoder specified ITU Rec 470BG (PAL) color.");
		break;
	default:
		LOG_STRING(LOG_NOTE, "Warning: encoder specified unknown colorspace.");
		break;
	}
}

int VideoFrame::dump_comments(theora_comment *tc)
{
	std::string vendor("Encoded by ");
	vendor+=tc->vendor;
	LOG_STRING(LOG_NOTE, vendor);

	for(int cc=0; cc<tc->comments; cc++)
	{
		std::string usercomments=tc->user_comments[cc];
		LOG_STRING(LOG_NOTE, usercomments);
	}

	return 0;
}

int VideoFrame::get_data_from_physical_stream(FILE *INFILE_, ogg_sync_state *OGG_SYNC_STATE_)
{
	const int DBS__=8192;
	// �������� ��������� �� ����� ��� ������
	char *buffer=ogg_sync_buffer(OGG_SYNC_STATE_,DBS__);

	long bytes=fread(buffer,1,DBS__,INFILE_); // ������ ��� ������

	// ���������, ������� ������ ���� ���� �������� � ����� 
	//(�.�. ��������� �� ����� - ����� ���� ����������!)
	ogg_sync_wrote(OGG_SYNC_STATE_,bytes);

	return bytes; // ����������, ������� ���������
}

int VideoFrame::push_page_into_logical_stream(ogg_page *page)
{
	if(-1== ogg_stream_pagein(&OGG_STREAM_STATE_THEORA,page) )
		__asm nop;  // ��������, ��� �� �������� �������� � ���������� �����
	// ��������� � ������ �������� ������� 
	//(�������� � ���������� ����� theora ���� ����� vorbis)

	return 0;
}

void VideoFrame::TheoraClose()
{
	if (currentfile == NULL) {
		return;
	}
	ogg_stream_clear(&OGG_STREAM_STATE_THEORA);
	theora_clear(&THEORA_STATE);
	theora_comment_clear(&THEORA_COMMENT);
	theora_info_clear(&THEORA_INFO);

	ogg_sync_clear(&OGG_SYNC_STATE);

	//������� ��� ��������� ogg/theora:
	memset(&OGG_STREAM_STATE_THEORA, 0x00, sizeof(ogg_stream_state));
	memset(&OGG_SYNC_STATE, 0x00, sizeof(ogg_sync_state));
	memset(&OGG_PAGE, 0x00, sizeof(ogg_page));
	memset(&OGG_PACKET, 0x00, sizeof(ogg_packet));

	memset(&THEORA_COMMENT, 0x00, sizeof(theora_comment));
	memset(&THEORA_INFO, 0x00, sizeof(theora_info));
	memset(&THEORA_STATE, 0x00, sizeof(theora_state));
	memset(&YUV_BUFFER, 0x00, sizeof(yuv_buffer));

	fclose(currentfile);
	currentfile = NULL;
	current_frame = 0;

	delete sprite;
	// ������� ����� �����
	hge->Texture_Free(hTexture);
}

BYTE VideoFrame::ClampFloatToByte(const float &value)
{
	BYTE result=(BYTE)value;

	value<0?result=0:NULL;
	value>255?result=255:NULL;

	return result;
}

VideoFrame::VideoFrame(TiXmlElement *xe)
	: Messager(xe)
	, currentfile(NULL)
	, frame(NULL)
	, texture(0)
	, current_frame(0)
	, hTexture(0)
{
	hge = hgeCreate(HGE_VERSION);
	fileName = xe->Attribute("fileName");
	pos.x = atoi(xe->Attribute("x"));
	pos.y = atoi(xe->Attribute("y"));
}

void VideoFrame::Draw()
{
	if (!currentfile) {
		return;
	}
	// theora processing...
	while( ogg_stream_packetout(&OGG_STREAM_STATE_THEORA,&OGG_PACKET) <=0)
	{
		// �� ������� ������ � ���������� ������ theora
		// ���� ��������� ������ �� ����������� ������ � ��������� �� � ���������� �����

		// ������ ������ �� �����
		int ret=get_data_from_physical_stream(currentfile,&OGG_SYNC_STATE);
		if(ret==0)
		{
			// ���� ��������, ���������� ��������� ����������� ��������
			// � ����� �� ����������
			TheoraClose();

			//LOG_NUMBER(LOG_NOTE, "frames: ", current_frame);
			//FINISHED=true;
			return;
		}

		while( ogg_sync_pageout(&OGG_SYNC_STATE,&OGG_PAGE) >0)
			// ���������� ������ �� ������ � �������� (ogg_page)
			// ���� ��� �� �������� � ������
		{
			// ������ ��� �������� � �����. ���������� ������
			push_page_into_logical_stream(&OGG_PAGE);
		}

	}

	// ������ ������������. � ������ ���������� �������������� ogg-����������
	// (�� ���� �������������� theora-����������)

	// ��������� ����� � ������� theora
	if(OC_BADPACKET== theora_decode_packetin(&THEORA_STATE,&OGG_PACKET) )
	{
		// ������ �������������
		throw std::runtime_error("error during theora_decode_packetin...");
	}

	// ��� ������ ��������, ������� ����

	// ���������� ��������� � YUV-���� � ����. ��������� yuv_buffer
	if(0!= theora_decode_YUVout(&THEORA_STATE,&YUV_BUFFER) )
	{
		// ������ �������������
		throw std::runtime_error("error during theora_decode_YUVout...");
	}

	/*
	���, ���� �������! ������, theora, ��� � ������ ������ ������ �������� � �������� ������������ YUV12, ��� Y � ���������� � ������� �����, � UV � ��������������� ��������������, ��������� �������� ���������. ����� �������, ���� ��� ���������� �����-����� ��������, ������ ��������������� �� ���� � ������ ����������� ������ �� ���� Y. :)
	������� ��� �������������� yuv->rgb �������� ���: 
	R = Y + 1.371(CR - 128) 
	G = Y - 0.698(CR - 128) - 0.336(CB - 128) 
	B = Y + 1.732(CB - 128) 
	, ��� CB � ��� U, � CR � ��� V.
	����� ����, ���� ���������, ��� ���������� Y ���� � ����� theora ������, ��� ���������� UV �����, ��� ��� ���� ���������, ��� ������� �������� �� ��� �����, ��� ���������. ��� ����������� ��������� ������ ����� ������ ����� � ��������� yuv_buffer ������� ���� y_stride � uv_stride.
	��� ��� ��������������: */
	if (current_frame == 0)
	{
		// ���� ��� ������ ����, �� ������� ����� �����
		hTexture = hge->Texture_Create(YUV_BUFFER.y_width, YUV_BUFFER.y_height);
		sprite = new hgeSprite(hTexture, 0, 0, YUV_BUFFER.y_width, YUV_BUFFER.y_height);
	}
	frame = hge->Texture_Lock(hTexture, false);
	unsigned int width = hge->Texture_GetWidth(hTexture, false);
	// ����������� yuv -> rgb
	for ( int nTempY = 0; nTempY < YUV_BUFFER.y_height; nTempY++ )
	{
		int nYShift = YUV_BUFFER.y_stride * nTempY;
		int nUVShift = YUV_BUFFER.uv_stride * ( nTempY >> 1 );

		for ( int nTempX = 0; nTempX < YUV_BUFFER.y_width; nTempX++ )
		{
			int nHX = ( nTempX >> 1 );

			BYTE nY = *(BYTE*)( YUV_BUFFER.y + nYShift + nTempX );
			BYTE nU = *(BYTE*)( YUV_BUFFER.u + nUVShift + nHX );
			BYTE nV = *(BYTE*)( YUV_BUFFER.v + nUVShift + nHX );

			int index=(nTempY * width+nTempX);

			float r=nY+1.371f*(nV-128);
			float g=nY-0.698f*(nV-128)-0.336f*(nU-128);
			float b=nY+1.732f*(nU-128);

			frame[index]= ClampFloatToByte(r) << 16
							| ClampFloatToByte(g) << 8
							| ClampFloatToByte(b)
							| 255 << 24;
		}
	}
	hge->Texture_Unlock(hTexture);
	// ��� ����� �������� ������� ��������, ����� ������ ��������������, ��������� ��� � �������� (���������) �� ������� �����.
	// �����: theora ������������ ��������� �������� ����������� (colorspaces), ������, � �� ������� ������������ �� ��� � �� ����, ��� ���� �������� ������������ � ����� �������. �������� ������� ������ ������� ����� �� ���, ��� �����, ��� ��������� ���� ����� ��� ����� ����� ����� �������� ������������ ��� ���������� � � �� �����, ��� ���� ������ ����� ��������. ��� �� �����, ���� ��� ������� �������� ������ ���������� �� ������� �������, ���������� ���������� � ����� color.html, ������� ��������� � ����� doc � ������������ theora.
	// ��� � ���, �������� ������ ���������� ����:   // ��������� ������� �����:
	sprite->Render(pos.x, pos.y);

	++current_frame;
}

void VideoFrame::OnMessage(std::string message) 
{
	if (message == "play") {
		TheoraInit();
	} else if (message == "stop") {
		TheoraClose();
	}
}

VideoFrame::~VideoFrame(void)
{
	if (currentfile) {
		TheoraClose();
	}
	hge->Release();
}
