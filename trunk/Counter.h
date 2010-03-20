// Counter.h: interface for the Counter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_COUNTER_INCLUDED_)
#define _COUNTER_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

#endif // !defined(_COUNTER_INCLUDED_)
