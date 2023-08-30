#pragma once
#include <math.h>
#include <vector>

class dim3vectors
{

};

class double3
{
public:
	double x;
	double y;
	double z;

	double3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {};
	double3() : x(0.0), y(0.0), z(0.0) {};
	double3(std::vector<double> values)
	{
		x = values[0];
		y = values[1];
		z = values[2];
	}


	inline double abs()
	{
		return sqrt(x * x + y * y + z * z);
	}
};

class bool3 {
public:
	bool x;
	bool y;
	bool z;

	bool3() : x(false), y(false), z(false) {};
	bool3(bool _x, bool _y, bool _z) : x(_x), y(_y), z(_z) {};

	inline int abs()
	{
		return x + y + z;
	}
};

struct int3 {
	int x;
	int y;
	int z;

	int3() : x(0), y(0), z(0) { };
	int3(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {};
};

inline double3 operator+(const double3& lh, const double3& rh)
{
	return double3(
		lh.x + rh.x,
		lh.y + rh.y,
		lh.z + rh.z
	);
}

template <typename T>
inline double3 operator+(const double3& lh, const T& rh)
{
	return double3(
		lh.x + rh,
		lh.y + rh,
		lh.z + rh
	);
}

inline double3 operator-(const double3& lh, const double3& rh)
{
	return double3(
		lh.x - rh.x,
		lh.y - rh.y,
		lh.z - rh.z
	);
}

template <typename T>
inline double3 operator-(const double3& lh, const T& rh)
{
	return double3(
		lh.x - rh,
		lh.y - rh,
		lh.z - rh
	);
}

inline double operator*(const double3& lh, const double3& rh)
{
	return lh.x * rh.x + lh.y * rh.y + lh.z * rh.z;
}

template <typename T>
inline double3 operator*(const T& lh, const double3& rh)
{
	return double3(
		lh * rh.x,
		lh * rh.y,
		lh * rh.z
	);
}

template <typename T>
inline double3 operator*(const double3& lh, const T& rh)
{
	return double3(
		lh.x * rh,
		lh.y * rh,
		lh.z * rh
	);
}


inline double operator*(const double3& lh, const bool3& rh)
{
	return lh.x * rh.x + lh.y * rh.y + lh.z * rh.z;
}


template <typename T>
inline double3 operator/(const double3& lh, const T& rh)
{
	return double3(
		lh.x / rh,
		lh.y / rh,
		lh.z / rh
	);
}

inline bool3 operator<(const double3& lh, const double3& rh)
{
	return bool3(
		lh.x < rh.x,
		lh.y < rh.y,
		lh.z < rh.z
	);
}

template <typename T>
inline bool3 operator<(const double3& lh, const T& rh)
{
	return bool3(
		lh.x < rh,
		lh.y < rh,
		lh.z < rh
	);
}

inline bool3 operator>(const double3& lh, const double3& rh)
{
	return bool3(
		lh.x > rh.x,
		lh.y > rh.y,
		lh.z > rh.z
	);
}

template <typename T>
inline bool3 operator>(const double3& lh, const T& rh)
{
	return bool3(
		lh.x > rh,
		lh.y > rh,
		lh.z > rh
	);
}