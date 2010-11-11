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
	} else {
		assert(false);
	}
	_shape = xe->Attribute("shape");
	if (_shape == "box") {
		_width = atof(xe->Attribute("width"));
		_height = atof(xe->Attribute("height"));
		if (_type == BODY_TYPE_EXPLOSION) {
			_maxForce = atof(xe->Attribute("maxForce"));
			_radius = atof(xe->Attribute("radius"));
		}
	} else if (_shape == "circle") {
		_radius = atof(xe->Attribute("radius"));
		_width = 2 * _radius;
		_height = 2 * _radius;
	} else {
		// unknown type
	}	
	_restitution = atof(xe->Attribute("restitution"));
	_friction = atof(xe->Attribute("friction"));
	
	TiXmlElement *uvdesc = xe->FirstChildElement("uv");
	float u1 = atof(uvdesc->Attribute("u1"));
	float u2 = atof(uvdesc->Attribute("u2"));
	float v1 = atof(uvdesc->Attribute("v1"));
	float v2 = atof(uvdesc->Attribute("v2"));

	_uv[0].u = u1; _uv[0].v = v1;
	_uv[1].u = u2; _uv[1].v = v1;
	_uv[2].u = u2; _uv[2].v = v2;
	_uv[3].u = u1; _uv[3].v = v2;

	for (unsigned int i = 0; i < MAX; i++) {
		FPoint2D p0(-_width/2,  _height/2);
		FPoint2D p1( _width/2,  _height/2);
		FPoint2D p2( _width/2, -_height/2);
		FPoint2D p3(-_width/2, -_height/2);

		float angle = (-static_cast<float>(i) / MAX) * (2 * M_PI);
		_positions[i][0] = *p0.Rotate(angle);
		_positions[i][1] = *p1.Rotate(angle);
		_positions[i][2] = *p2.Rotate(angle);
		_positions[i][3] = *p3.Rotate(angle);
	}
}
