#include "BodyTemplate.h"

BodyTemplate::BodyTemplate(TiXmlElement *xe) {
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
	} else if (_shape == "circle") {
		_radius = atof(xe->Attribute("radius"));
	} else {
		// unknown type
	}	
	_restitution = atof(xe->Attribute("restitution"));
	_friction = atof(xe->Attribute("friction"));
	_collection[_type] = this;
}

BodyTemplate::Collection BodyTemplate::_collection;