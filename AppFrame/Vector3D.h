#include "DxLib.h"

class Vector3D {
public:
	Vector3D() : _x(0), _y(0), _z(0) {}
	Vector3D(double x, double y, double z) : _x(x), _y(y), _z(z) {}
	Vector3D(const VECTOR v) :_x(v.x), _y(v.y), _z(v.z) {}		// DXライブラリ向け
	virtual ~Vector3D() {}

	// オペレータオーバーロード（クラス内）
	Vector3D	operator+=(const Vector3D& right);
	Vector3D	operator-=(const Vector3D& right);
	Vector3D	operator*=(const Vector3D& right);
	Vector3D	operator*=(const double right);
	Vector3D	operator/=(const Vector3D& right);
	Vector3D	operator/=(const double right);
	bool		operator==(const Vector3D& right);
	bool		operator!=(const Vector3D& right);

	Vector3D& operator=(const VECTOR& right);		// DXライブラリ向け
	VECTOR		dxl();

	static double	Dot(Vector3D& in1, Vector3D& in2);		// 内積
	static Vector3D	Cross(Vector3D& in1, Vector3D& in2);	// 外積
	static double	Length(Vector3D& in);					// 距離
	static double	Length(Vector3D& in1, Vector3D& in2);	// 距離(2点間)
	static Vector3D Normalize(Vector3D& in);				// 正規化

public:
	double	_x, _y, _z;
};

// オペレータオーバーロード（クラス外）
const Vector3D operator+(const Vector3D& left, const Vector3D& right);
const Vector3D operator-(const Vector3D& left, const Vector3D& right);
const Vector3D operator*(const Vector3D& left, const Vector3D& right);
const Vector3D operator*(const Vector3D& left, const double right);
const Vector3D operator*(const double left, const Vector3D& right);
const Vector3D operator/(const Vector3D& left, const Vector3D& right);
const Vector3D operator/(const Vector3D& left, const double right);
const Vector3D operator/(const double left, const Vector3D& right);

