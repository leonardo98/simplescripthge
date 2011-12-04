#include "AnimationEditor.h"
#include "../Core/Core.h"
#include "../Core/Matrix.h"

AnimationEditor::AnimationEditor(TiXmlElement *xe)
: Messager("animator")
, _saved(true)
, _play(false)
, _mirror(false)
, _animation(NULL)
, _timeCounter(0.f)
, STEP_DOWN(18.f)
{
}

AnimationEditor::~AnimationEditor() {
}

bool AnimationEditor::IsMouseOver(const FPoint2D &mousePos) {
	return true;
}

void AnimationEditor::Draw() {
	//_background.Render();
	if (_animation) {
		Render::PushMatrix();
		Render::MatrixMove(300.f, 400.f);
		_animation->Draw(_timeCounter);
		Render::PopMatrix();
	}
	DrawTree();
}

void AnimationEditor::OnMouseDown(const FPoint2D &mousePos) {
}

void AnimationEditor::OnMouseUp() {
}

void AnimationEditor::OnMouseMove(const FPoint2D &mousePos) {
	_lastMousePos = mousePos;
}

void AnimationEditor::Update(float dt) {
	if (_animation && _play) {
		_timeCounter += ( dt / _animation->Time() );
		while (_timeCounter >= 1.f) {
			_timeCounter -= 1.f;
		}
	}
}

void AnimationEditor::OnMessage(const std::string &message) {
	if (message == "new") {
	} else if (message == "open") {
		if (_saved) {
			std::string fileName = ShowLoadDialog();	
			if (fileName != "") {
				bool res = LoadFile(fileName);
			}
		}
	} else if (message == "save") {
	} else if (message == "selector") {
		Messager::SendMessage("ModalList", "clear");
		for (Core::AnimationMap::iterator i = Core::_animations.begin(), e = Core::_animations.end(); i != e; ++i) {
			Messager::SendMessage("ModalList", "add " + i->first);
		}
		Messager::SendMessage("ModalList", "special add [ NEW ]");
		Messager::SendMessage("ModalList", "special add [ CANCEL ]");
	} else if (message == "button pressed [ CANCEL ]") {
		Messager::SendMessage("ModalList", "clear");
	} else if (message == "playstop") {
		_play = !_play;
		if (_play) {
			Messager::SetValue("caption", "stop");
		} else {
			Messager::SetValue("caption", "play");
		}
	}
}

bool AnimationEditor::LoadFile(const std::string &fileName) {
	try {
		if (_animation) {
			delete _animation;
			_animation = NULL;
		}
		Core::LoadAnimations(fileName.c_str());
		if (Core::_animations.size()) {
			_animation = new Animation(*Core::_animations.begin()->second);
			Messager::SetValueS("Selector", "caption", Core::_animations.begin()->first);
		}
	} catch (...) {
		Core::Release();
		Core::OnMessage("load xml start.xml");
		// ошибка при чтении файла fileName
	}
	return false;
}

std::string AnimationEditor::ShowLoadDialog() {
    /*
	char path[512];
	DWORD r = GetCurrentDirectory(512, path);
	assert(r <= 512);
	char *mask = "*.xml";
	std::string file;
	OPENFILENAME fn;
	fn.lStructSize = sizeof(fn);
	fn.hInstance = 0;
	fn.hwndOwner = Render::GetDC()->System_GetState(HGE_HWND);
	fn.lpstrInitialDir = path;
	fn.lpstrFilter = mask;
	fn.nFilterIndex = 0;
	fn.lpstrFile = new char[1024];
	fn.lpstrFile[0] = 0;
	fn.nMaxFile = 1024 - 1;
	fn.lpstrFileTitle = NULL;
	fn.nMaxFileTitle = NULL;
	fn.lpstrTitle = NULL;
	fn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	fn.lpstrCustomFilter = NULL;
	fn.nMaxCustFilter = 0;
	fn.nFileOffset = 0;
	fn.nFileExtension = 0;
	fn.lpstrDefExt = NULL;
	fn.lCustData = NULL;
	fn.lpfnHook = NULL;
	fn.FlagsEx = 0;
	fn.lpTemplateName = "";
	fn.dwReserved = 0;
	fn.pvReserved = 0;
	bool res = GetOpenFileName(&fn);
	if (res) {
		return fn.lpstrFile;
	}
     */
	return "";
}

void AnimationEditor::DrawTree() {
	float x = 10.f;
	float y = 50.f;
	if (_animation) {
		for (BoneList::iterator i = _animation->_bones.begin(), e = _animation->_bones.end(); i != e; ++i) {
			std::string type;
			if ((*i)->boneType == BT_MovingPart) {
				type = "MovingPart";
			} else if ((*i)->boneType == BT_IKTwoBone) {
				type = "IKTwoBone";
			} else {
				assert(false);
			}
			Render::PrintString(x, y += STEP_DOWN, (*i)->boneName == "" ? type : (*i)->boneName, HGETEXT_LEFT);
			y += DrawChild(*i, x + STEP_DOWN, y);
		}
	}
}

float AnimationEditor::DrawChild(Bone *bone, float x, float y) {
	float oldY = y;
	for (BoneList::iterator i = bone->_bones.begin(), e = bone->_bones.end(); i != e; ++i) {
		std::string type;
		if ((*i)->boneType == BT_MovingPart) {
			type = "MovingPart";
		} else if ((*i)->boneType == BT_IKTwoBone) {
			type = "IKTwoBone";
		} else {
			assert(false);
		}
		Render::PrintString(x, y += STEP_DOWN, (*i)->boneName == "" ? type : (*i)->boneName, HGETEXT_LEFT);
		y += DrawChild(*i, x + STEP_DOWN, y);
	}
	return y - oldY;
}
