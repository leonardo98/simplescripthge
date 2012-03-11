#ifndef MOTIONVALUES_H
#define MOTIONVALUES_H

inline float LineMotion(float x1, float x2, float t)
{
	return x1 + (x2 - x1) * t;
}

inline float SplineMotion(float x1, float x2, float r1, float r2, float t)
{
	float res;
	float t2 = t*t;
	float t3 = t2*t;

	res = x1*(2.0f*t3-3.0f*t2+1.0f)+r1*(t3-2.0f*t2+t)+x2*(-2.0f*t3+3.0f*t2)+r2*(t3-t2);
	return res;
}

class MotionValues
{
public:

	bool line;

	struct TimedKey {
		float time;
		float value;
		float gradient;
	};
	typedef std::vector<TimedKey> Keys;
	Keys keys;
	
	MotionValues()
		: line(false)
	{
	}

	void Clear()
	{
		keys.clear();
		line = false;
	}

	float AddKey(float time, float value)
	{
		assert(keys.size() || time == 0.f);
		assert(keys.size() == 0 || time > (keys.back().time + 1e-5));
		TimedKey key;
		key.time = time;
		key.value = value;
		keys.push_back(key);
		return time;
	}

	float Value(float t)
	{
		assert(keys.size());
		if (keys.size() == 1 || t > keys.back().time) {
			return keys.back().value;
		}
		size_t start = 0;
		size_t end = keys.size() - 2;
		size_t middle = (start + end) / 2;
		while (!(keys[middle].time <= t && t <= keys[middle + 1].time)) {
			if (keys[middle].time > t) {
				end = middle;
			} else if (keys[middle + 1].time < t) {
				start = middle + 1;
			}
			middle = (start + end) / 2;
		}

		if (middle >= getSectors()) {
			return keys.back().value;
		}
		float localT = (t - keys[middle].time) / (keys[middle + 1].time - keys[middle].time);
		return getFrame(middle, localT);
	}

	void CalculateGradient()
	{		
		assert(keys.size());
		float g1, g2, g3;

		if (keys.size() > 1)
		{
			keys[0].gradient = keys[1].value - keys[0].value;
			keys[keys.size() - 1].gradient = keys[keys.size() - 1].value - keys[keys.size() - 2].value;
		}

		for (size_t i = 1; i < (keys.size()-1); i++)
		{
			g1 = keys[i].value - keys[i - 1].value;
			g2 = keys[i + 1].value - keys[i].value;
			g3 = g2 - g1;
			keys[i].gradient = g1 + 0.5f * g3;
		}
	}

private:

	size_t getSectors()
	{
		return keys.size() - 1;
	}

	float getFrame(size_t i, float t)
	{
		if (line) {
			return LineMotion(keys[i].value, keys[i+1].value, t);
		}
		return SplineMotion(keys[i].value, keys[i+1].value, keys[i].gradient, keys[i+1].gradient, t);
	}

};

#endif//MOTIONVALUES_H