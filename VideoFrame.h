#pragma once

#include "Theora\Theora.h"
#include "Messager.h"
#include "types.h"

class VideoFrame 
	: public Object
	, public Messager
{
public:
	VideoFrame(TiXmlElement *xe);
	virtual void OnMessage(std::string message);
	virtual void Draw();
	virtual ~VideoFrame(void);
private:
	std::string fileName;
	hgeVector pos;
	HGE *hge;

	FILE *currentfile;
	DWORD *frame;
	UINT texture;
	ogg_stream_state OGG_STREAM_STATE_THEORA;
	ogg_sync_state OGG_SYNC_STATE;
	ogg_page OGG_PAGE;
	ogg_packet OGG_PACKET;
	theora_comment THEORA_COMMENT;
	theora_info THEORA_INFO;
	theora_state THEORA_STATE;
	yuv_buffer YUV_BUFFER;
	unsigned int current_frame;
	void TheoraInit();
	// печатает информацию об используемом цветовом пространстве
	static void report_colorspace(theora_info *ti);
	// печатает информацию о хедере theora
	int dump_comments(theora_comment *tc);
	// читает данные из FILE*
	int get_data_from_physical_stream(FILE *INFILE_, ogg_sync_state *OGG_SYNC_STATE_);
	// добавляет завершенные страницы в поток
	int push_page_into_logical_stream(ogg_page *page);
	void TheoraClose();
	BYTE ClampFloatToByte(const float &value);

	HTEXTURE hTexture;
	hgeSprite *sprite;
};
