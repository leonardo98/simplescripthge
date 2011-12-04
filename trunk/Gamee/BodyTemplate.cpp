#include "BodyTemplate.h"

BodyTemplate::BodyTemplate(TiXmlElement *xe) {
	_id = xe->Attribute("id");

	assert(std::string(xe->Attribute("destroyOnTap")) == "false" || std::string(xe->Attribute("destroyOnTap")) == "true");
	_destroyOnTap = (std::string(xe->Attribute("destroyOnTap")) == "true");

	_explosion = false;
	if (xe->Attribute("explosion")) {
		assert(std::string(xe->Attribute("explosion")) == "false" || std::string(xe->Attribute("explosion")) == "true");
		if (std::string(xe->Attribute("explosion")) == "true") {
			_maxForce = static_cast<float>(atof(xe->Attribute("explosionForce")));
			_explosionRadius = static_cast<float>(atof(xe->Attribute("explosionRadius")));
			_explosion = true;
		}
	}

	_breakable = false;
	if (xe->Attribute("breakable")) {
		assert(std::string(xe->Attribute("breakable")) == "false" || std::string(xe->Attribute("breakable")) == "true");
		_breakable = (std::string(xe->Attribute("breakable")) == "true");
	}

	_fixed = false;
	if (xe->Attribute("fixed")) {
		assert(std::string(xe->Attribute("fixed")) == "false" || std::string(xe->Attribute("fixed")) == "true");
		_fixed = (std::string(xe->Attribute("fixed")) == "true");
	}

	_hero = false;
	if (xe->Attribute("hero")) {
		assert(std::string(xe->Attribute("hero")) == "false" || std::string(xe->Attribute("hero")) == "true");
		_hero = (std::string(xe->Attribute("hero")) == "true");
	}

	_shape = xe->Attribute("shape");
	if (_shape == "box" || _shape == "triangle") {
		_width = static_cast<float>(atof(xe->Attribute("baseWidth")));
		_height = static_cast<float>(atof(xe->Attribute("baseHeight")));
	} else if (_shape == "circle") {
		_radius = static_cast<float>(atof(xe->Attribute("baseRadius")));
		_width = 2 * _radius;
		_height = 2 * _radius;
	} else {
		assert(false && "unknown type");
	}	

	_restitution = static_cast<float>(atof(xe->Attribute("restitution")));
	_friction = static_cast<float>(atof(xe->Attribute("friction")));
	
	TiXmlElement *uvdesc = xe->FirstChildElement("uv");
	float u1 = static_cast<float>(atof(uvdesc->Attribute("u1")));
	float u2 = static_cast<float>(atof(uvdesc->Attribute("u2")));
	float v1 = static_cast<float>(atof(uvdesc->Attribute("v1")));
	float v2 = static_cast<float>(atof(uvdesc->Attribute("v2")));

	_uv[0].u = u1; _uv[0].v = v1;
	_uv[1].u = u2; _uv[1].v = v1;
	_uv[2].u = u2; _uv[2].v = v2;
	_uv[3].u = u1; _uv[3].v = v2;
	
	/*
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
	*/
}
