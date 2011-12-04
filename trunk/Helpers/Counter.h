// Counter.h: interface for the Counter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef COUNTER_H
#define COUNTER_H

class Counter  
{
public:
	Counter();
	virtual ~Counter();
	void Update(float deltaTime);
	bool Action();
	bool Done();
	void Init(float actionTime, float startTime = 0.f);
	float Progress();
private:
	float _actionTime;
	float _actionTimeCounter;
};

#endif//COUNTER_H