#include "gta_math.h"

Vector3 Vector3::normalized() const
{
	float m = mag();
	if (!m)
		return *this;
	float k = 1 / m;
	return Vector3{ x * k, y * k, z * k };
}

Vector3& Vector3::operator +=(const Vector3& right)
{
	x += right.x;
	y += right.y;
	z += right.z;
	return *this;
}

Vector3& Vector3::operator -=(const Vector3& right)
{
	x -= right.x;
	y -= right.y;
	z -= right.z;
	return *this;
}

Vector3& Vector3::operator *=(float k)
{
	x *= k;
	y *= k;
	z *= k;
	return *this;
}

Vector3& Vector3::operator /=(float k)
{
	if (k)
	{
		float ik = 1 / k;
		x *= ik;
		y *= ik;
		z *= ik;
	}
	return *this;
}

bool operator ==(const Vector3& left, const Vector3& right)
{
	return left.x == right.x && left.y == right.y && left.z == right.z;
}

bool operator !=(const Vector3& left, const Vector3& right) { return !(left == right); }
Vector3 operator -(const Vector3& vec) { return Vector3{ -vec.x, -vec.y, -vec.z }; }
Vector3 operator +(Vector3 left, Vector3 right) { return left += right; }
Vector3 operator -(Vector3 left, Vector3 right) { return left -= right; }
Vector3 operator *(Vector3 left, float k) { return left *= k; }
Vector3 operator /(Vector3 left, float k) { return left /= k; }

Vector3 crossProduct(const Vector3& left, const Vector3& right)
{
	Vector3 result;
	result.x = left.y * right.z - left.z * right.y;
	result.y = left.z * right.x - left.x * right.z;
	result.z = left.x * right.y - left.y * right.x;
	return result;
}

void normalize(Vector3& v)
{
	float m = v.mag();
	if (!m)
		return;
	float k = 1 / m;
	v *= k;
}