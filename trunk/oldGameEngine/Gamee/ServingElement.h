#ifndef MYENGINE_SERVINGELEMENT_H 
#define MYENGINE_SERVINGELEMENT_H 

class ServingElement {
public:
	ServingElement() : _waiting(false) {}
	void SetWaiting(bool waiting) { _waiting = waiting; }
	bool GetWaiting() { return _waiting; }
private:
	bool _waiting;
};

#endif//MYENGINE_SERVINGELEMENT_H 