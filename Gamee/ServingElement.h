#ifndef SERVINGELEMENT_H 
#define SERVINGELEMENT_H 

class ServingElement {
public:
	ServingElement() : _waiting(false) {}
	void SetWaiting(bool waiting) { _waiting = waiting; }
	bool GetWaiting() { return _waiting; }
private:
	bool _waiting;
};

#endif//SERVINGELEMENT_H 