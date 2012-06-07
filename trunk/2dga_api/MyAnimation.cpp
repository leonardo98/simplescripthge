#include "Animation.h"
#include "../Core/Render.h"

namespace My {
//
// Деструктор - удаляем все подвижные спрайты
// (текстуры выгружает только менеджер анимаций, 
// т.к. на одной текстуре может быть несколько разных анимаций)
//
Animation::~Animation() {
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		delete _bones[i];
	}
}

//
// Конструктор - параметры анимации читаются из XML 
// можно использовать любой парсер,
// который умеет перебирать ноды и читать атрибуты,
// здесь успользован TinyXml
//
Animation::Animation(TiXmlElement *xe, HTEXTURE hTexture)
{
	_time = fatof(xe->Attribute("time"));
	_pivotPos.x = fatof(xe->Attribute("pivotX"));
	_pivotPos.y = fatof(xe->Attribute("pivotY"));

	TiXmlElement *element = xe->FirstChildElement();
	while (element) {
		_bones.push_back(new MovingPart(element, hTexture));
		element = element->NextSiblingElement();
	}
	_subPosition.Unit();
	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);
}

//
// Настройить положение анимации на экране, 
// "pos" - положение анимации на экране, 
// чтобы нарисовать зеркально - ставим "mirror" в "true".
// Метод лучше всего вызывать перед каждым вызывом Draw()
// исключение может быть, если такая анимация на экране всего одна
//
void Animation::SetPos(const hgeVector &pos, bool mirror) {
	_subPosition.Unit();
	_subPosition.Move(-_pivotPos.x, -_pivotPos.y);
	if (mirror) {
		_subPosition.Scale(-1.f, 1.f);
	}
	_subPosition.Move(pos.x, pos.y);
}

//
// Нарисовать анимацию на экране 
// "position" - значение времени для анимации из диапазона [0, 1]
//
void Animation::Draw(float position) {
	_matrixsStack.clear(); 
	// для задания положения все спрайтов использую свой внутренний "стек" матриц
	_matrixsStack.push_back(_subPosition);
	for(int i = 0; i < (int)_bones.size();i++) {
		_bones[i]->Draw(position, _matrixsStack);
	}
}

//
// Время полного цикла анмиации 
// эту функцию нужно использовать в Update для расчета 
// значения "position" метода Draw
// например, для зацикленой анимации можно считать так:
// progress += deltaTime / anim->Time();
// if(progress >= 1.f) progress = 0.f;
//
float Animation::Time() const {
	return _time;
}


/* РЕАЛИЗАЦИЯ МЕТОДОВ МЕНЕДЖЕРА АНИАМАЦИЙ */

// Загрузить файл анимации
// в случае успеха - вернет "true"
// можно загружать несколько файлов - менеджер "коллекционирует" анимации
//
// здесь и далее используется HGE для загрузки и выгрузки текстур
// эти части кода нужно заменить при портировании на другие движки 
//
bool AnimationManager::Load(const std::string &fileName) {
	
	// загружаем текстуру - записываем указатель на нее в "tex", который используется далее
	//
	// начало блока - этот код при портировании надо переписать
	HGE *hge = hgeCreate(HGE_VERSION);
	HTEXTURE tex = hge->Texture_Load((fileName.substr(0, fileName.length() - 3) + "png").c_str());
	if (tex == 0) { // если текстура не найдена - прерываем загрузку, возвращаем "false"
		hge->Release();
		return false;
		LOG("assigned png file not found " + fileName);
	}
	// конец блока

	TiXmlDocument doc;
	if (doc.LoadFile(hge->Resource_MakePath(fileName.c_str()))) { // если файл существует - читаем XML с описанием анимаций
		TiXmlElement *root = doc.RootElement();
		TiXmlElement *animation = root->FirstChildElement("Animation");
		while (animation) {
			const char *id = animation->Attribute("id");
			// ВНИМАНИЕ - из двух и более анимаций с одинаковыми id - будет загружена только первая
			if (_animations.find(id) == _animations.end()) { 
				_animations[id] = new Animation(animation, tex);
			}
			animation = animation->NextSiblingElement("Animation");
		}
		_textures.push_back(tex);

		// начало блока - этот код при портировании надо переписать
		hge->Release();
		// конец блока

		return true;
	} else { 
		// файл анимаций не найден или его не удалось прочитать
		// выгружаем текстуру, возвращаем "false"
		//
		// начало блока - этот код при портировании надо переписать
		hge->Texture_Free(tex);
		hge->Release();
		// конец блока

		char buffer[1024];
		GetCurrentDirectory(1024, buffer);
		LOG("file not found: %s" + fileName);
		return false;
	}
}

//
// Выгрузить все анимации и текстуры из памяти
//
void AnimationManager::UnloadAll() {
	HGE *hge = hgeCreate(HGE_VERSION);
	for (AnimationMap::iterator it = _animations.begin(); it != _animations.end(); it++) {
		delete (*it).second;
	}
	for (unsigned int i = 0; i < _textures.size();i++) {
		hge->Texture_Free(_textures[i]);
	}
	_animations.clear();
	_textures.clear();
	hge->Release();
}

//
// Получить указатель на анимацию по animationId - это имя анимации в редакторе 
// (ВАЖНО нельзя вызывать delete для нее!)
//
Animation *AnimationManager::getAnimation(const std::string &animationId) {
	AnimationMap::iterator it_find = _animations.find(animationId);
	if (it_find != _animations.end()) {
		return (*it_find).second;
	}
	LOG("animation " + animationId + " not found.");
	return NULL;
}

AnimationManager::AnimationMap AnimationManager::_animations;
std::vector<HTEXTURE> AnimationManager::_textures;

};