#ifndef SPLINE_H
#define SPLINE_H

inline float SplineInterpolation(float x1, float x2, float r1, float r2, float t)
{
	float res;
	float t2 = t*t;
	float t3 = t2*t;

	res = x1*(2.0f*t3-3.0f*t2+1.0f)+r1*(t3-2.0f*t2+t)+x2*(-2.0f*t3+3.0f*t2)+r2*(t3-t2);
	return res;
}

inline float GetGradient(float x1, float x2, float r1, float r2, float t)
{
	float res;
	float t2 = t*t;
	res = 6.0f*x1*(t2-t)+r1*(3.0f*t2-4.0f*t+1.0f)+6.0f*x2*(t-t2)+r2*(3.0f*t2-2.0f*t);
	return res;
}

/*
	Класс SplinePath

	Класс для гладкой интерпляции Catmull-Rom сплайном

	Для использования необходимо сначала заполнить экземпляр класса ключевыми значениями
	вызовами addKey (последовательно в порядке интерполяции). После заполнения необходимо
	вызвать CalculateGradient для подсчета коэффицентов сплайна.

	После можно получить интерполированное значение между ключами вызовом getGlobalFrame(t),
	в качестве параметра методу передается нормированное время для интерполяции (таким образом
	t=0 соответствует первому ключу, а t=1 последнему).
*/

class SplinePath
{
public:
	typedef std::pair<float, float> KeyFrame;

	std::vector<KeyFrame> keys;

	void Clear()
	{
		keys.clear();
	}

	void addKey(const float& key)
	{
		keys.push_back(KeyFrame(key, key));
	}

	float getFrame(int sector, float t)
	{
		size_t i = static_cast<size_t>(sector);
		return SplineInterpolation(keys[i].first, keys[i+1].first, keys[i].second, keys[i+1].second, t);
	}

	float getGradient(int sector, float t)
	{
		size_t i = static_cast<size_t>(sector);
		return GetGradient(keys[i].first, keys[i+1].first, keys[i].second, keys[i+1].second, t);
	}

	float getGlobalFrame(float t)
	{
		float timeStep = (1.0f/getSectors());
		int tessSector = static_cast<int>(t/timeStep);
		if (tessSector >= getSectors())
			return keys.back().first;
		float tessLocalTime = (t-tessSector*timeStep)/timeStep;
		return getFrame(tessSector, tessLocalTime);
	}

	float getGlobalGradient(float t)
	{
		float timeStep = (1.0f/getSectors());
		int tessSector = static_cast<int>(t/timeStep);
		if (tessSector >= getSectors())
			return keys.back().first;
		float tessLocalTime = (t-tessSector*timeStep)/timeStep;
		return getGradient(tessSector, tessLocalTime);
	}

	int getSectors()
	{
		return static_cast<int>(keys.size()-1);
	}

	void CalculateGradient(bool cycled = false)
	{
		if (cycled)
		{
			keys.push_back(keys[0]);
		}
		
		float g1, g2, g3;

		if (cycled)
		{
			g1 = keys[0].first-keys[keys.size()-2].first;
			g2 = keys[1].first-keys[0].first;
			g3 = g2-g1;
			keys[0].second = g1+0.5f*g3;
			keys[keys.size()-1].second = keys[0].second;
		}
		else
		{
			if (keys.size() > 1)
			{
				keys[0].second = keys[1].first-keys[0].first;
				keys[keys.size()-1].second = keys[keys.size()-1].first-keys[keys.size()-2].first;
			}
		}

		for (size_t i = 1; i < (keys.size()-1); i++)
		{
			g1 = keys[i].first-keys[i-1].first;
			g2 = keys[i+1].first-keys[i].first;
			g3 = g2-g1;
			keys[i].second = g1+0.5f*g3;
		}
	}
};

#endif//SPLINE_H