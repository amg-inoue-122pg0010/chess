#include "Vector3D.h"
#include	<math.h>


// オペレータオーバーロード（クラス外）
const Vector3D operator+(const Vector3D& left, const Vector3D& right) {
	return Vector3D(left) += right;
}
const Vector3D operator-(const Vector3D& left, const Vector3D& right) {
	return Vector3D(left) -= right;
}
const Vector3D operator*(const Vector3D& left, const Vector3D& right) {
	return Vector3D(left) *= right;
}
const Vector3D operator*(const Vector3D& left, const double right) {
	return Vector3D(left) *= right;
}
const Vector3D operator*(const double left, const Vector3D& right) {
	return Vector3D(right) *= left;
}
const Vector3D operator/(const Vector3D& left, const Vector3D& right) {
	return Vector3D(left) /= right;
}
const Vector3D operator/(const Vector3D& left, const double right) {
	return Vector3D(left) /= right;
}
const Vector3D operator/(const double left, const Vector3D& right) {
	return Vector3D(right) /= left;
}





Vector3D	Vector3D::operator+=(const Vector3D& right) {
	Vector3D tmp;
	tmp._x = _x + right._x;
	tmp._y = _y + right._y;
	tmp._z = _z + right._z;
	return tmp;
}
Vector3D	Vector3D::operator-=(const Vector3D& right) {
	Vector3D tmp;
	tmp._x = _x - right._x;
	tmp._y = _y - right._y;
	tmp._z = _z - right._z;
	return tmp;
}
Vector3D	Vector3D::operator*=(const Vector3D& right) {
	Vector3D tmp;
	tmp._x = _x * right._x;
	tmp._y = _y * right._y;
	tmp._z = _z * right._z;
	return tmp;
}
Vector3D	Vector3D::operator*=(const double right) {
	Vector3D tmp;
	tmp._x = _x * right;
	tmp._y = _y * right;
	tmp._z = _z * right;
	return tmp;
}
Vector3D	Vector3D::operator/=(const Vector3D& right) {
	Vector3D tmp;
	tmp._x = _x / right._x;
	tmp._y = _y / right._y;
	tmp._z = _z / right._z;
	return tmp;
}
Vector3D	Vector3D::operator/=(const double right) {
	Vector3D tmp;
	tmp._x = _x / right;
	tmp._y = _y / right;
	tmp._z = _z / right;
	return tmp;
}
bool		Vector3D::operator==(const Vector3D& right) {
	if (_x == right._x && _y == right._y && _z == right._z) {
		return true;
	}
	return false;
}
bool		Vector3D::operator!=(const Vector3D& right) {
	return !(*this == right);
}

Vector3D& Vector3D::operator=(const VECTOR& right) {
	_x = right.x;
	_y = right.y;
	_z = right.z;
	return (*this);
}
VECTOR		Vector3D::dxl() {
	VECTOR v;
	v.x = (float)_x;
	v.y = (float)_y;
	v.z = (float)_z;
	return v;
}

double	Vector3D::Dot(Vector3D& in1, Vector3D& in2) {
	return ((in1._x * in2._x) + (in1._y * in2._y) + (in1._z * in2._z));
}
Vector3D	Vector3D::Cross(Vector3D& in1, Vector3D& in2) {
	Vector3D tmp;
	tmp._x = in1._y * in2._z - in1._z * in2._y;
	tmp._y = in1._z * in2._x - in1._x * in2._z;
	tmp._z = in1._x * in2._y - in1._y * in2._x;
	return tmp;
}

double	Vector3D::Length(Vector3D& in) {
	return sqrt(Dot(in, in));
}
double	Vector3D::Length(Vector3D& in1, Vector3D& in2) {
	Vector3D in = in1 - in2;
	return Length(in);
}

Vector3D Vector3D::Normalize(Vector3D& in) {
	const double division = Length(in);
	if (division == 0.0) {
		// ゼロ除算発生
	}
	return in * (1.0 / division);
}