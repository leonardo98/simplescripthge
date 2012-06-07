#include "MovingPart.h"
#include "Math.h"

namespace My {

//
// ��������� ������� �� ���� XML 
// ���� ������� ���������� - �� ������������� � float,
// ���� ��� - ������������ defaultValue
//
float Read(TiXmlElement *xe, const char *name, const float defaultValue) {
	const char *tmp = xe->Attribute(name);
	return (tmp ? fatof(tmp) : defaultValue);
}

// 
// ���������� hgeQuad
// ��� ����������� ���������� ���������� ��� �������,
// ������ �� ��� �������� ������� �������� �� ������ "texture"
//
//
void MovingPart::CreateQuad(HTEXTURE hTexture, const std::string &texture) {

	// ������ ����� - ���� ��� ������� �� ������������� ������
	float width = (float)_hge->Texture_GetWidth(hTexture);
	float height = (float)_hge->Texture_GetHeight(hTexture);
	// ����� �����

	std::string::size_type start = 0;
	std::string::size_type end = texture.find(':', start);
	float x = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float y = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float w = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float h = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	end = texture.find(':', start);
	float offX = fatof(texture.substr(start, end - start).c_str());
	start = end + 1;
	float offY = fatof(texture.substr(start).c_str());

	// ������ ����� - ���� ��� ������� �� ������������� ������
	_quad.blend = 2;
	_quad.tex = hTexture;
	_quad.v[0].col = 0xFFFFFFFF;
	_quad.v[0].tx = x / width; _quad.v[0].ty = y / height;
	_quad.v[0].z = 0.f;
	_quad.v[1].col = 0xFFFFFFFF;
	_quad.v[1].tx = (x + w) / width; _quad.v[1].ty = y / height;
	_quad.v[1].z = 0.f;
	_quad.v[2].col = 0xFFFFFFFF;
	_quad.v[2].tx = (x + w) / width; _quad.v[2].ty = (y + h) / height;
	_quad.v[2].z = 0.f;
	_quad.v[3].col = 0xFFFFFFFF;
	_quad.v[3].tx = x / width; _quad.v[3].ty = (y + h) / height;
	_quad.v[3].z = 0.f;
	// ����� �����

	x = offX;
	y = offY;
	_origin[0].x = x;     _origin[0].y = y;
	_origin[1].x = x + w;     _origin[1].y = y;
	_origin[2].x = x + w; _origin[2].y = y + h;
	_origin[3].x = x; _origin[3].y = y + h;

}

// 
// ���������� �������� �� �������� "������� ���������"
// � _bottomBone ���������� ��� �������� ������� ������� �������� �� �������� �������
// � _topBone ���������� ��� �������� ������� ������� �������� ����� �������� �������
//
void MovingPart::ResortBones() {
	_topBone.clear();
	_bottomBone.clear();
	for (unsigned int i = 0; i < _bones.size(); ++i) {
		if (_bones[i]->_offparent == OffParentOrder_top) 
			_topBone.push_back(_bones[i]);
		 else if (_bones[i]->_offparent == OffParentOrder_bottom)
			_bottomBone.push_back(_bones[i]);
		 else 
			assert(false);
		
	}
}

//
// ���������� - ����������� ���������� ������
//
MovingPart::~MovingPart() {
	for (unsigned int i = 0; i < _bones.size(); ++i) 
		delete _bones[i];
	_hge->Release();
}


//
// ����������� - ������ ������ ������� �� XML
// ���������� ��������, ������� �� ������� ������
//
MovingPart::MovingPart(TiXmlElement *xe, HTEXTURE hTexture)
{
	// ������ ����� - ���� ��� ������� �� ������������� ������
	_hge = hgeCreate(HGE_VERSION);
	// ����� �����

	boneName =  xe->Attribute("name");

	const char *tmp = xe->Attribute("moving_type");
	if (tmp == NULL || strcmp(tmp, "spline") == 0) {
		_movingType = MotionValues::m_spline;
	} else if (strcmp(tmp, "line") == 0) {
		_movingType = MotionValues::m_line;
	} else {
		_movingType = MotionValues::m_discontinuous;
	}
	tmp = xe->Attribute("offparent");
	_offparent = ((tmp && strcmp(tmp, "bottom") == 0) ? OffParentOrder_bottom : OffParentOrder_top);
	_center.x = Read(xe, "centerX", 0.f);
	_center.y = Read(xe, "centerY", 0.f);
	tmp = xe->Attribute("texture");
	if (_hasTexture = (tmp != NULL)) {
		CreateQuad(hTexture, tmp);
	}

	//
	// ��� ��� ����� ������ ���� - ��� �������� ������ 
	// � ��������� ���������� ������� �� ������ � ��� �������� 
	// �� �������
	//
	TiXmlElement *pos = xe->FirstChildElement("pos");
	if (pos == NULL) {
		_x.AddKey(0.f, 0.f);
		_y.AddKey(0.f, 0.f);
		_scaleX.AddKey(0.f, 1.f);
		_scaleY.AddKey(0.f, 1.f);
		_angle.AddKey(0.f, 0.f);
	}
	while (pos) {
		float time = Read(pos, "time", 0.f);
		_x.AddKey(time, Read(pos, "x", 0.f));
		_y.AddKey(time, Read(pos, "y", 0.f));	
		_scaleX.AddKey(time, Read(pos, "scaleX", 1.f));
		_scaleY.AddKey(time, Read(pos, "scaleY", 1.f));
		_angle.AddKey(time, Read(pos, "angle", 0.f));

		pos = pos->NextSiblingElement("pos");
	}
	_x.SetType(_movingType);
	_y.SetType(_movingType);	
	_scaleX.SetType(_movingType);
	_scaleY.SetType(_movingType);
	_angle.SetType(_movingType);

	// 
	// ���� ���� - ������� �������� ��������� �������
	//
	TiXmlElement *element = xe->FirstChildElement();
	while (element) {
		std::string name = element->Value();
		if (name == "movingPart") {
			_bones.push_back( new MovingPart(element, hTexture) );
		}
		element = element->NextSiblingElement();
	}
	ResortBones();
}

//
// ������� ������ �� �����
// ��������� �� ������� �������� - p, [ 0<= p <= 1 ]
// ��������� �� ������ - stack
//
void MovingPart::Draw(float p, MatrixStack &stack) {
	
	float localT;
	int index = _x.Value(p, localT);

	// ���� ������� ���������� ��� �������� "p" - ������
	// ��� �������� ������ ��� "_x" - �.�. � ��������� ������ �������� ������� ����������� ����� ��
	if (index >= 0) {
		stack.push_back(stack.back());
		stack.back().Move(_x.GetFrame(index, localT), _y.GetFrame(index, localT));
		stack.back().Rotate(_angle.GetFrame(index, localT));
		stack.back().Scale(_scaleX.GetFrame(index, localT), _scaleY.GetFrame(index, localT));

		stack.back().Move(-_center.x, -_center.y);

		for (unsigned int i = 0; i < _bottomBone.size(); ++i) {
			_bottomBone[i]->Draw(p, stack);
		}
		if (_hasTexture) {
			stack.back().Mul(_origin[0].x, _origin[0].y, _quad.v[0].x, _quad.v[0].y);
			stack.back().Mul(_origin[1].x, _origin[1].y, _quad.v[1].x, _quad.v[1].y);
			stack.back().Mul(_origin[2].x, _origin[2].y, _quad.v[2].x, _quad.v[2].y);
			stack.back().Mul(_origin[3].x, _origin[3].y, _quad.v[3].x, _quad.v[3].y);
			// ������ ����� - ���� ��� ������� �� ������������� ������
			_hge->Gfx_RenderQuad(&_quad);
			// ����� �����
		}

		for (unsigned int i = 0; i < _topBone.size(); ++i) 
			_topBone[i]->Draw(p, stack);
		
		stack.pop_back();
	}
}

};