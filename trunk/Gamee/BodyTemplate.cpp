#include "BodyTemplate.h"

BodyTemplate::BodyTemplate(TiXmlElement *xe) {
	_id = xe->Value();
	std::string type = xe->Attribute("type");
	if (type == "unbrakeable") {
		_type = BODY_TYPE_UNBREAKABLE;
	} else if (type == "blue") {
		_type = BODY_TYPE_BLUE;
	} else if (type == "explosion") {
		_type = BODY_TYPE_EXPLOSION;
	} else if (type == "ball") {
		_type = BODY_TYPE_BALL;
	} else if (type == "ground") {
		_type = BODY_TYPE_GROUND;
	} else {
		assert(false);
	}
	_shape = xe->Attribute("shape");
	if (_shape == "box") {
		_width = static_cast<float>(atof(xe->Attribute("width")));
		_height = static_cast<float>(atof(xe->Attribute("height")));
		if (_type == BODY_TYPE_EXPLOSION) {
			_maxForce = static_cast<float>(atof(xe->Attribute("maxForce")));
			_radius = static_cast<float>(atof(xe->Attribute("radius")));
		}
	} else if (_shape == "circle") {
		_radius = static_cast<float>(atof(xe->Attribute("radius")));
		_width = 2 * _radius;
		_height = 2 * _radius;
	} else {
		// unknown type
	}	
	_restitution = static_cast<float>(atof(xe->Attribute("restitution")));
	_friction = static_cast<float>(atof(xe->Attribute("friction")));
	
	TiXmlElement *uvdesc = xe->FirstChildElement("uv");
	float u1 = static_cast<float>(atof(uvdesc->Attribute("u1")));
	float u2 = static_cast<float>(atof(uvdesc->Attribute("u2")));
	float v1 = static_cast<float>(atof(uvdesc->Attribute("v1")));
	float v2 = static_cast<float>(atof(uvdesc->Attribute("v2")));

#ifdef IOS_COMPILE_KEY
#define UV16(a) static_cast<int16>(a * (1 << 12));
	_uv[0].u = UV16(u1); _uv[0].v = UV16(v1);
	_uv[1].u = UV16(u2); _uv[1].v = UV16(v1);
	_uv[2].u = UV16(u2); _uv[2].v = UV16(v2);
	_uv[3].u = UV16(u1); _uv[3].v = UV16(v2);
#else
	_uv[0].u = u1; _uv[0].v = v1;
	_uv[1].u = u2; _uv[1].v = v1;
	_uv[2].u = u2; _uv[2].v = v2;
	_uv[3].u = u1; _uv[3].v = v2;
#endif

	for (int i = 0; i < MAX; i++) {//1.04f - дополнительный множитель масшатаба для коробок
		FPoint2D p0(-_width/2,  _height/2);
		FPoint2D p1( _width/2,  _height/2);
		FPoint2D p2( _width/2, -_height/2);
		FPoint2D p3(-_width/2, -_height/2);

		float angle = static_cast<float>((-static_cast<float>(i) / MAX) * (2 * M_PI));
		_positions[i][0] = *p0.Rotate(angle);
		_positions[i][1] = *p1.Rotate(angle);
		_positions[i][2] = *p2.Rotate(angle);
		_positions[i][3] = *p3.Rotate(angle);
	}
}
