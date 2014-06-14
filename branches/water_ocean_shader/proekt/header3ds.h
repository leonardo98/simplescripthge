#ifndef _HEADER3DS_H
#define _HEADER3DS_H
 
//>------ Главный Chunk, в начале каждого 3ds-файла
#define PRIMARY       0x4D4D
 
//>------ Главнык Chunk-и
#define OBJECTINFO    0x3D3D			// Это предоставляет версию меша перед информацией об обьекте
#define VERSION       0x0002			// Предоставляет версию .3ds файла
#define EDITKEYFRAME  0xB000			// Хидер для всей информации о кадрах
 
//>------ под-дефайны OBJECTINFO
#define MATERIAL	  0xAFFF		// Информация о текстурах
#define OBJECT		  0x4000		// Полигоны, вершины, и т.д...
 
//>------ под-дефайны для MATERIAL
#define MATNAME       0xA000			// Название материала
#define MATDIFFUSE    0xA020			// Хранит цвет обьекта/материала
#define MATMAP        0xA200			// Хидер для нового материала
#define MATMAPFILE    0xA300			// Хранит имя файла текстуры
 
#define OBJECT_MESH   0x4100			// Даёт нам знать, что начинаем считывать новый обьект
 
//>------ под-дефайны для OBJECT_MESH
#define OBJECT_VERTICES		0x4110		// Вершины обьекта
#define OBJECT_FACES		0x4120		// Полигоны обьекта
#define OBJECT_MATERIAL		0x4130		// Дефайн находится, если обьект имеет материал, иначе цвет/текстура
#define OBJECT_UV		0x4140		// UV текстурные координаты
 
 
// Структура для индексов 3DS (так как .3ds хранит 4 unsigned short)
struct tIndices {
	unsigned short a, b, c, bVisible;	// Это хранит индексы для точки 1,2,3 массива
						// вершин, плюс флаг видимости
};
 
// Хранит информацию о chunk-е
struct tChunk
{
	unsigned short int ID;			// ID chunk-а
	unsigned int length;			// Длинна chunk-а
	unsigned int bytesRead;			// Число читаемых байт для этого chunk-а
};
 
// Класс содержит весь код загрузки
class CLoad3DS
{
public:
	CLoad3DS();		// Тут будут инициализироватся все данные
 
	// Эта ф-я и будет вызыватся для загрузки 3DS
	bool Import3DS(t3DModel *pModel, char *strFileName);
 
private:
	// Читает строку и сохраняет её в переданный массив char-ов
	int GetString(char *);
 
	// Читает следующий chunk
	void ReadChunk(tChunk *);
 
	// Читает следующий длинный chunk
	void ProcessNextChunk(t3DModel *pModel, tChunk *);
 
	// Читает chunk-и обьекта
	void ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *);
 
	// Читает chunk-и материала
	void ProcessNextMaterialChunk(t3DModel *pModel, tChunk *);
 
	// Читает RGB-значение цвета обьекта
	void ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk);
 
	// Читает вершины обьекта
	void ReadVertices(t3DObject *pObject, tChunk *);
 
	// Читает информацию полигонов обьекта
	void ReadVertexIndices(t3DObject *pObject, tChunk *);
 
	// Читает текстурные координаты обьекта
	void ReadUVCoordinates(t3DObject *pObject, tChunk *);
 
	// Читает имя материала, присвоенного обьекту, и устанавливает materialID
	void ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk);
 
	// Рассчитывает нормали вершин обьекта
	void ComputeNormals(t3DModel *pModel);
 
	// This frees memory and closes the file
	// Освобождает память и закрывает файл
	void CleanUp();
 
	// Указатель на файл
	FILE *m_FilePointer;
};
 
 
#endif
