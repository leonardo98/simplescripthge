#include "BodyTemplate.h"

BodyTemplate::BodyTemplate(TiXmlElement *xe) {
	std::string type = xe->Attribute("type");
	if (type == "unbrakeable") {
		_type = TYPE_UNBREAKABLE;
	} else if (type == "blue") {
		_type = TYPE_BLUE;
	} else if (type == "explosion") {
		_type = TYPE_EXPLOSION;
	} else if (type == "ball") {
		_type = TYPE_BALL;
	}
	_shape = xe->Attribute("shape");
	if (_shape == "box") {
		_width = atof(xe->Attribute("width"));
		_height = atof(xe->Attribute("height"));
	} else if (_shape == "circle") {
		_radius = atof(xe->Attribute("radius"));
	} else {
		// unknown type
	}	
	_restitution = atof(xe->Attribute("restitution"));
	_friction = atof(xe->Attribute("friction"));
}
