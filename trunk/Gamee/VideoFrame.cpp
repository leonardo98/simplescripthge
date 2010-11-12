#include "VideoFrame.h"
#include "Ogg\Ogg.h"

#include <iostream>

using namespace std;


void VideoFrame::TheoraInit()
{
	//очищаем все структуры ogg/theora:
	memset(&OGG_STREAM_STATE_THEORA, 0x00, sizeof(ogg_stream_state));
	memset(&OGG_SYNC_STATE, 0x00, sizeof(ogg_sync_state));
	memset(&OGG_PAGE, 0x00, sizeof(ogg_page));
	memset(&OGG_PACKET, 0x00, sizeof(ogg_packet));
	memset(&THEORA_COMMENT, 0x00, sizeof(theora_comment));
	memset(&THEORA_INFO, 0x00, sizeof(theora_info));
	memset(&THEORA_STATE, 0x00, sizeof(theora_state));
	memset(&YUV_BUFFER, 0x00, sizeof(yuv_buffer));
	//видео файл
	
	currentfile=fopen(fileName.c_str(),"rb");
	if(currentfile==NULL)
		throw std::runtime_error("cannot open ogg-file");

	//инициализируем основную структуру контейнера ogg
	ogg_stream_clear(&OGG_STREAM_STATE_THEORA);
	//инициализируем "слой синхронизации" ogg
	ogg_sync_init(&OGG_SYNC_STATE);

	//очищаем все эти необходимые нам структуры для видео
	theora_comment_init(&THEORA_COMMENT);
	theora_info_init(&THEORA_INFO);

	// теперь ищем начало логического потока theora
	bool START_THEORA_HEADER_STREAM=true;
	int THEORA_HEADER_PACKETS=0;  // число обработанных пакетов заголовков theora

	do
	{
		// get_data_from_physical_stream - читает данные из FILE*
		// и передает их в буфер приема ogg
		if( get_data_from_physical_stream(currentfile, &OGG_SYNC_STATE) ==0)
		{
			// кончился файл, на данном этапе это ошибка
			throw std::runtime_error("eof searched, terminate...");
		}
		// ogg_sync_pageout - формирует страницу
		while( ogg_sync_pageout(&OGG_SYNC_STATE, &OGG_PAGE) >0)
			// 1-больше данных не требуется
			// 0-требуется больше данных для создания страницы
		{
			// что страница сформирована успешно

			// это страница заголовков? если нет, кончаем искать заголовки
			if( ogg_page_bos(&OGG_PAGE) ==false)
			{
				// нет, это не страница заголовков
				// значит, страницы заголовков всех логических потоков кончились
				// и начались данные этих потоков
				// таким образом надо переходить к чтению страниц данных

				// закидываем эту страничку в логический видеопоток
				push_page_into_logical_stream(&OGG_PAGE);
				// push_page_into_logical_stream - закидывает страничку
				// в логический поток theora, если
				// совпадает идентификатор логического потока
				// в противном случае страница игнорируется

				// выходим из циклов
				START_THEORA_HEADER_STREAM=false;
				break;
			}
			else
			{
				// да, это страница заголовков

				// тестовый логический поток
				ogg_stream_state OGG_STREAM_STATE_TEST;
				memset(&OGG_STREAM_STATE_TEST, 0x00, sizeof(ogg_stream_state));

				// инициализируем тестовый поток на тот же поток с таким же
				// идентификатором потока, как и у текущей странички
				if(0!= ogg_stream_init(&OGG_STREAM_STATE_TEST,ogg_page_serialno(&OGG_PAGE)) )
					throw std::runtime_error("error during ogg_stream_init");

				// добавляем страницу в тестовый поток
				if(0!= ogg_stream_pagein(&OGG_STREAM_STATE_TEST,&OGG_PAGE) )
					throw std::runtime_error("error during ogg_stream_pagein");

				// декодируем данные из этого тестового потока в пакет
				if( ogg_stream_packetout(&OGG_STREAM_STATE_TEST,&OGG_PACKET) ==-1)
					throw std::runtime_error("error during ogg_stream_packetout");

				// THEORA_HEADER_PACKETS - число прочитанных
				// заголовочных ПАКЕТОВ theora (не страниц)
				// по спецификации theora таких пакетов должно быть три
				if(THEORA_HEADER_PACKETS==0)
				{
					int dhr = theora_decode_header (&THEORA_INFO, &THEORA_COMMENT, &OGG_PACKET);
					// декодируем заголовок theora
					if(dhr<0)
					{
						// это не заголовок theora	          
						// очищаем структуру тестового потока
						ogg_stream_clear(&OGG_STREAM_STATE_TEST);
						//и продолжаем цикл в поисках заголовков theora
					}
					else
					{
						// это заголовок theora!
						// вот таким образом "инициализируем" логический поток theora:
						memcpy(&OGG_STREAM_STATE_THEORA, &OGG_STREAM_STATE_TEST,
							sizeof(OGG_STREAM_STATE_TEST));
						// теперь из этого потока будут всегда сыпаться пакеты theora
						THEORA_HEADER_PACKETS++;
						// после того, как мы нашли заголовочную страницу логического потока theora,
						// нам необходимо прочитать все остальные заголовочные страницы
						// других потоков и отбросить их (если таковые, конечно, имеются)
					}
				}
			}
		}
	}
	while(START_THEORA_HEADER_STREAM);

	while(THEORA_HEADER_PACKETS<3)
	{
		int result=ogg_stream_packetout(&OGG_STREAM_STATE_THEORA,&OGG_PACKET);
		// если функция возвращает нуль, значит не хватает данных для декодирования
		// почему то этого НЕТ в спецификации libogg, или я плохо искал

		if(result<0)
		{
			// ошибка декодирования, поврежденный поток
			throw std::runtime_error("error during ogg_stream_packetout");
		}

		if(result>0)
		{
			// удалось успешно извлечь пакет информации theora

			int result2=theora_decode_header(&THEORA_INFO,&THEORA_COMMENT,&OGG_PACKET);

			if(result2<0)
			{
				// ошибка декодирования, поврежденный поток
				throw std::runtime_error("error during theora_decode_header (corrupt stream)");
			}

			++THEORA_HEADER_PACKETS;
		}

		// эту страничку обработали, надо извлечь новую
		// для этого проверяем буфер чтения, вдруг там осталось что-нить похожее
		// на страничку. Если не осталось, тогда просто читаем эти данные из файла:

		if( ogg_sync_pageout(&OGG_SYNC_STATE,&OGG_PAGE) >0)
			// ogg_sync_pageout - функция, берет данные из буфера приема ogg
			// и записывает их в ogg_page
		{
			//мы нашли страничку в буфере и...
			push_page_into_logical_stream(&OGG_PAGE);
			// ...пихаем эти данные в подходящий поток
		}
		else
		{
			// ничего мы в буфере не нашли
			int ret=get_data_from_physical_stream(currentfile,&OGG_SYNC_STATE);
			// надо больше данных! читаем их из файла

			if(ret==0)
			{
				// опять файл кончился!
				throw std::runtime_error("eof searched. terminate...");
			}
		}
	}

	// наконец мы получили, все, что хотели. инициализируем декодеры

	// инициализируем видео-поток
	theora_decode_init(&THEORA_STATE,&THEORA_INFO);
	report_colorspace(&THEORA_INFO);
	dump_comments(&THEORA_COMMENT);

	deltaFps = THEORA_INFO.fps_denominator / float(THEORA_INFO.fps_numerator);

	ReadFrameFromFile();
	waitFrame = 0;
	hTexture = hge->Texture_Create(YUV_BUFFER.y_width, YUV_BUFFER.y_height);
	sprite = new hgeSprite(hTexture, 0, 0, YUV_BUFFER.y_width, YUV_BUFFER.y_height);
	PutDataToTexture();
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
		// выводим в лог информацию о цветовом пространстве
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
	// получаем указатель на буфер для данных
	char *buffer=ogg_sync_buffer(OGG_SYNC_STATE_,DBS__);

	long bytes=fread(buffer,1,DBS__,INFILE_); // читаем эти данные

	// указываем, сколько именно байт было записано в буфер 
	//(т.е. прочитано из файла - вдруг файл закончился!)
	ogg_sync_wrote(OGG_SYNC_STATE_,bytes);

	return bytes; // возвращаем, сколько прочитали
}

int VideoFrame::push_page_into_logical_stream(ogg_page *page)
{
	if(-1== ogg_stream_pagein(&OGG_STREAM_STATE_THEORA,page) )
		__asm nop;  // означает, что мы пытаемся засунуть в логический поток
	// страничку с другим серийным номером 
	//(например в логический поток theora суем пакет vorbis)

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

	//очищаем все структуры ogg/theora:
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


	if(hTexture) {
		delete sprite;
		// удаляем буфер кадра
		hge->Texture_Free(hTexture);
		hTexture = 0;
	}
}

__forceinline BYTE VideoFrame::ClampFloatToByte(const float &value)
{
	BYTE result=(BYTE)value;

	value<0?result=0:NULL;
	value>255?result=255:NULL;

	return result;
}

__forceinline BYTE VideoFrame::ClampShortToByte(signed short value)
{
	value>255?value=255:( value<0?value=0:NULL );
	return (BYTE)value;
}

__forceinline BYTE VideoFrame::ClampIntToByte(int value)
{
  value>255?value=255:( value<0?value=0:NULL );

  return (BYTE)value;
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
	if (!currentfile || !hTexture) {
		return;
	}
	sprite->Render(pos.x, pos.y);
}

void VideoFrame::OnMessage(const std::string &message) 
{
	if (message == "play") {
		try {
			TheoraInit();
		}
		catch ( exception &e ) {
			MessageBox(hge->System_GetState(HGE_HWND),e.what(),"Runtime Error in Theora!", MB_ICONERROR);
			TheoraClose();
		};

	} else if (message == "stop") {
		TheoraClose();
	} else if (message == "pause") {
		_pause = !_pause;
	} else if (message.find("set ") == 0) {
		TheoraClose();
		fileName = message.substr(std::string("set ").length());
	}
}

VideoFrame::~VideoFrame(void)
{
	if (currentfile) {
		TheoraClose();
	}
	hge->Release();
}

void VideoFrame::Update(float dt)
{
	if (_pause) {
		return;
	}
	if (!currentfile || !hTexture) {
		return;
	}
	waitFrame += dt;
	unsigned int mustBeFrame = (unsigned int)(current_frame + waitFrame / deltaFps);
	if (current_frame == mustBeFrame) {
		return;
	}
	while (current_frame < mustBeFrame) {
		if (!ReadFrameFromFile()){			
			return;
		}
		++current_frame;
	}
	PutDataToTextureIntPre();
	while (waitFrame > deltaFps) {
		waitFrame -= deltaFps;
	}
}

void VideoFrame::PutDataToTexture()
{
	frame = hge->Texture_Lock(hTexture, false);
	unsigned int width = hge->Texture_GetWidth(hTexture, false);
	/*
	theora, как и многие другие кодеки работает в цветовом пространстве YUV12, где Y – информация о яркости точки, а UV – цветоразностные характеристики, придающие картинке цветность. Таким образом, если вас устраивает черно-белая картинка, ничего преобразовывать не надо – просто используйте данные из слоя Y. :)
	Формула для преобразования yuv->rgb выглядит так: 
	R = Y + 1.371(CR - 128) 
	G = Y - 0.698(CR - 128) - 0.336(CB - 128) 
	B = Y + 1.732(CB - 128) 
	, где CB – это U, а CR – это V.
	Кроме того, надо учитывать, что разрешение Y слоя в кадре theora меньше, чем разрешение UV слоев, так как было подмечено, что яркость меняется не так резко, как цветность. Для определения положения нужной точки внутри слоев в структуре yuv_buffer имеются поля y_stride и uv_stride.
	Вот код преобразования: */
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
			
			frame[index] = ClampFloatToByte(r) << 16
							| ClampFloatToByte(g) << 8
							| ClampFloatToByte(b)
							| 255 << 24;			
		}
	}
	hge->Texture_Unlock(hTexture);
}

// далее два варианта оптимизации, но они мне ничем не помогли потому без них пока
void VideoFrame::PutDataToTextureInt()
{
	frame = hge->Texture_Lock(hTexture, false);
	unsigned int width = hge->Texture_GetWidth(hTexture, false);
	unsigned int index = 0;
	for ( int nTempY = 0; nTempY < YUV_BUFFER.y_height; nTempY++ )
	{
		int nYShift = YUV_BUFFER.y_stride * nTempY;
		int nUVShift = YUV_BUFFER.uv_stride * ( nTempY >> 1 );

		for ( int nTempX = 0; nTempX < YUV_BUFFER.y_width; nTempX++ )
		{
			register unsigned char nY = *reinterpret_cast<unsigned char*>( YUV_BUFFER.y + nYShift + nTempX);
			register signed char nU = 
			 (unsigned char)128 ^ *reinterpret_cast<signed char*>(YUV_BUFFER.u + nUVShift + (nTempX >> 1));
			register signed char nV = 
			  (unsigned char)128 ^ *reinterpret_cast<signed char*>(YUV_BUFFER.v + nUVShift + (nTempX >> 1));
			// (float)  0 - 2
			// (BYTE)  0 - 256
			// (float)  1.371
			// (BYTE)  175
			// (float)  0.698
			// (BYTE)   89
			// (float)  0.336
			// (BYTE)   43
			// (float)  1.732
			// (BYTE)  222
			register signed short r =
			  (signed short)nY + (signed short)(((unsigned char)175 * nV) >> 7);
			register signed short g =
			  (signed short)nY - (signed short)(((unsigned char)89 * nV + (unsigned char)43 * nU) >> 7);
			register signed short b =
			  (signed short)nY + (signed short)(((unsigned char)222 * nU) >> 7);

			frame[index] = ClampShortToByte(r) << 16
							| ClampShortToByte(g) << 8
							| ClampShortToByte(b)
							| 255 << 24;
			++index;
		}
		index += width - YUV_BUFFER.y_width;
	}
	hge->Texture_Unlock(hTexture);
}

void VideoFrame::PutDataToTextureIntPre()
{
	static int b0_[256];
	static int b1_[256];
	static int b2_[256];
	static int b3_[256];
	// расчет таблиц, заодно их кэширование
	for(signed int cc=0; cc<256; cc++)
	{
	  b0_[cc] = ( 113443 * (cc-128) + 32768 ) >> 16;
	}

	for(signed int cc=0; cc<256; cc++)
	{
	  b1_[cc] = (  45744 * (cc-128) + 32768 ) >> 16;
	}

	for(signed int cc=0; cc<256; cc++)
	{
	  b2_[cc] = (  22020 * (cc-128) + 32768 ) >> 16;
	}

	for(signed int cc=0; cc<256; cc++)
	{
	  b3_[cc] = ( 113508 * (cc-128) + 32768 ) >> 16;
	}
	frame = hge->Texture_Lock(hTexture, false);
	unsigned int width = hge->Texture_GetWidth(hTexture, false);
	for ( int nTempY = 0; nTempY < YUV_BUFFER.y_height; nTempY++ )
	{
		int nYShift = YUV_BUFFER.y_stride * nTempY;
		int nUVShift = YUV_BUFFER.uv_stride * ( nTempY >> 1 );

		for ( int nTempX = 0; nTempX < YUV_BUFFER.y_width; nTempX++ )
		{
			int nTempX_ = nTempX >> 1;
			register
			int nY = (int) ( *reinterpret_cast<unsigned char*>( YUV_BUFFER.y + nYShift + nTempX) );
			register unsigned char nU = *(YUV_BUFFER.u + nUVShift + nTempX_);
			register unsigned char nV = *(YUV_BUFFER.v + nUVShift + nTempX_);
			register int r = nY + b0_[nV];
			register int g = nY - b1_[nV] - b2_[nU];
			register int b = nY + b3_[nU];
			*frame = ClampIntToByte(r) << 16
							| ClampIntToByte(g) << 8
							| ClampIntToByte(b)
							| 255 << 24;
			++frame;
		} 
		frame += (width - YUV_BUFFER.y_width);
	}
	hge->Texture_Unlock(hTexture);
}

bool VideoFrame::ReadFrameFromFile()
{
	// theora processing...
	while( ogg_stream_packetout(&OGG_STREAM_STATE_THEORA,&OGG_PACKET) <=0)
	{
		// не хватает данных в логическом потоке theora
		// надо надергать данных из физического потока и затолкать их в логический поток

		// читаем данные из файла
		int ret=get_data_from_physical_stream(currentfile,&OGG_SYNC_STATE);
		if(ret==0)
		{
			// файл кончился, необходимо выполнить закрывающие действия
			// и выйти из приложения
			TheoraClose();

			//LOG_NUMBER(LOG_NOTE, "frames: ", current_frame);
			//FINISHED=true;
			return false;
		}

		while( ogg_sync_pageout(&OGG_SYNC_STATE,&OGG_PAGE) >0)
			// декодируем данные из буфера в страницы (ogg_page)
			// пока они не кончатся в буфере
		{
			// пихаем эти страницы в соотв. логические потоки
			push_page_into_logical_stream(&OGG_PAGE);
		}

	}

	// удачно декодировали. в пакете содержится декодированная ogg-информация
	// (то бишь закодированная theora-информация)

	// загружаем пакет в декодер theora
	if(OC_BADPACKET== theora_decode_packetin(&THEORA_STATE,&OGG_PACKET) )
	{
		// ошибка декодирования
		throw std::runtime_error("error during theora_decode_packetin...");
	}

	// все данные получены, готовим кадр

	// декодируем страничку в YUV-виде в спец. структуру yuv_buffer
	if(0!= theora_decode_YUVout(&THEORA_STATE,&YUV_BUFFER) )
	{
		// ошибка декодирования
		throw std::runtime_error("error during theora_decode_YUVout...");
	}
	return true;
}
