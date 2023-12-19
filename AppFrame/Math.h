#pragma once
/*
 * @file  Math.h
 * @brief 数学関連
 *
 * @date 2017-07-04
 * @version 4.0
 *
 * Copyright(C) Amusement Media Academy All rights Resved.
 */
#include <cmath>
#include <limits>
#include <array>
#include <DxLib.h>

namespace Math {

    	// 円周率
	constexpr double Pi = 3.1415926535f;
	constexpr double TwoPi = Pi * 2.0f;
	constexpr double PiOver2 = Pi / 2.0f;

	// 度数から孤度(ラジアン)に変換
	inline double ToRadians(double degrees) {
		return degrees * Pi / 180.0f;
	}
	// 孤度から度数(ディグリー)に変換
	inline double ToDegrees(double radians) {
        return radians * 180.0f / Pi;
    }
    // 二つの値の大きい値を取得
    template <typename T>
    T Max(const T& a, const T& b) {
        return (a < b ? b : a);
    }
    // 二つの値の小さい値を取得
    template <typename T>
    T Min(const T& a, const T& b) {
        return (a < b ? a : b);
    }
    // 二つの値の間でクランプする
    template <typename T>
    T Clamp(const T& value, const T& lower, const T& upper) {
        return Min(upper, Max(lower, value));
    }

	inline VECTOR VMulti(VECTOR a, float b) {
		VECTOR c = a;
		c.x *= b;
		c.y *= b;
		c.z *= b;
		return c;
	};

}


// 2Dベクトルクラス
class Vector2 {
public:
	// x成分,y成分
	double x, y;
	// コンストラクタ
	Vector2() noexcept = default;
	constexpr Vector2(double ax, double ay) noexcept
		: x{ ax }
		, y{ ay } {
	}
	constexpr Vector2(int ax, int ay) noexcept
		: x{ static_cast<double>(ax) }
		, y{ static_cast<double>(ay) } {
	}
	int IntX() { return static_cast<int>(x); }
	int IntY() { return static_cast<int>(y); }


	// ベクトルの加算 a + b ※外部関数
	friend Vector2 operator+(const Vector2& a, const Vector2& b) {
		return { a.x + b.x, a.y + b.y };
	}
	// ベクトルの減算 a - b ※外部関数
	friend Vector2 operator-(const Vector2& a, const Vector2& b) {
		return { a.x - b.x, a.y - b.y };
	}
	// ベクトルのスカラー倍 v * scalar ※外部関数
	friend Vector2 operator*(const Vector2& v, double scalar) {
		return { v.x * scalar, v.y * scalar };
	}
	// ベクトルのスカラー倍 scalar * v ※外部関数
	friend Vector2 operator*(double scalar, const Vector2& v) {
		return { v.x * scalar, v.y * scalar };
	}
	// ベクトル加算代入
	Vector2& operator+=(const Vector2& right) {
		x += right.x;
		y += right.y;
		return *this;
	}
	// ベクトル減算代入
	Vector2& operator-=(const Vector2& right) {
		x -= right.x;
		y -= right.y;
		return *this;
	}
	// ベクトルのスカラ乗算代入
	Vector2& operator*=(double scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}
	// ベクトルの大きさ
	double Length() const {
		return std::sqrt(x * x + y * y);
	}
	// ベクトルの正規化
	void Normalize() {
		auto length = Length();

		// 誤差未満ならゼロとみなす。
		if (std::abs(length) < std::numeric_limits<double>::epsilon()) {
			x = 0.0;
			y = 0.0;
		}
		else {
			x /= length;
			y /= length;
		}
	}
	static Vector2 Normalize(const Vector2& vec) {
		auto temp = vec;
		temp.Normalize();
		return temp;
	}

	// ベクトルの内積(Dot product) a・b
	static double Dot(const Vector2& a, const Vector2& b) {
		return (a.x * b.x + a.y * b.y);
	}
	// ベクトルの外積(Cross product) a×b
	static double Cross(const Vector2& a, const Vector2& b) {
		return (a.x * b.y - a.y * b.x);
	}
	// ベクトルの行列変換
	static Vector2 Transform(const Vector2& vec, const class Matrix3& mat, double w = 1.0f);
};

// 3次行列クラス
class Matrix3 {
public:
	// 3列×3行
	std::array<std::array<double, 3>, 3> m;

	constexpr Matrix3() {
		*this = Matrix3::Identity;
	}
	constexpr Matrix3(
		double m00, double m01, double m02,
		double m10, double m11, double m12,
		double m20, double m21, double m22
	) noexcept
		: m{ m00, m01, m02,
			 m10, m11, m12,
			 m20, m21, m22 }
	{}
	friend static Vector2 Vector2::Transform(const Vector2& vec, const Matrix3& mat, double w);

	// 行列の乗算
	friend Matrix3 operator*(const Matrix3& l, const Matrix3& r) {
		return {
			// row 0
			l.m[0][0] * r.m[0][0] + l.m[0][1] * r.m[1][0] + l.m[0][2] * r.m[2][0],
			l.m[0][0] * r.m[0][1] + l.m[0][1] * r.m[1][1] + l.m[0][2] * r.m[2][1],
			l.m[0][0] * r.m[0][2] + l.m[0][1] * r.m[1][2] + l.m[0][2] * r.m[2][2],
			// row 1
			l.m[1][0] * r.m[0][0] + l.m[1][1] * r.m[1][0] + l.m[1][2] * r.m[2][0],
			l.m[1][0] * r.m[0][1] + l.m[1][1] * r.m[1][1] + l.m[1][2] * r.m[2][1],
			l.m[1][0] * r.m[0][2] + l.m[1][1] * r.m[1][2] + l.m[1][2] * r.m[2][2],
			// row 2
			l.m[2][0] * r.m[0][0] + l.m[2][1] * r.m[1][0] + l.m[2][2] * r.m[2][0],
			l.m[2][0] * r.m[0][1] + l.m[2][1] * r.m[1][1] + l.m[2][2] * r.m[2][1],
			l.m[2][0] * r.m[0][2] + l.m[2][1] * r.m[1][2] + l.m[2][2] * r.m[2][2],
		};
	}
	Matrix3& operator*=(const Matrix3& right) {
		*this = *this * right;
		return *this;
	}
	// 回転行列の作成
	static Matrix3 CreateRotation(double theta) {
		using std::sin;	using std::cos;
		return {
			cos(theta), sin(theta), 0.0,
			-sin(theta), cos(theta), 0.0,
			0.0, 0.0, 1.0,
		};
	}
	// 拡縮行列の作成
	static Matrix3 CreateScale(double xScale, double yScale) {
		return {
			xScale, 0.0, 0.0,
			0.0f, yScale, 0.0,
			0.0, 0.0, 1.0,
		};
	}
	static Matrix3 CreateScale(const Vector2& scaleVector) {
		return CreateScale(scaleVector.x, scaleVector.y);
	}
	static Matrix3 CreateScale(double scale) {
		return CreateScale(scale, scale);
	}
	// 平行移動行列の作成
	static Matrix3 CreateTranslation(const Vector2& trans) {
		return {
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			trans.x, trans.y, 1.0,
		};
	}
	// 単位行列
	static const Matrix3 Identity;
};
// 矩形クラス
class Rect {
public:
	Rect(const Vector2& lt, const Vector2& rb)
		:leftTop{ lt }
		, rightBottom{ rb }
	{
	}
	Vector2 GetLeftTop() { return leftTop; }
	Vector2 GetRightBottom() { return rightBottom; }
	double GetWidth() { return rightBottom.x - leftTop.x; }
	double GetHeight() { return rightBottom.y - leftTop.y; }
private:
	Vector2 leftTop;			// 矩形の左上
	Vector2 rightBottom;	// 矩形の右下
};
namespace MyLib {
	inline void DrawModiGraph(
		double x1, double y1,
		double x2, double y2,
		double x3, double y3,
		double x4, double y4,
		int grHandle, bool transFlag) {
		DxLib::DrawModiGraph(
			static_cast<int>(x1), static_cast<int>(y1),
			static_cast<int>(x2), static_cast<int>(y2),
			static_cast<int>(x3), static_cast<int>(y3),
			static_cast<int>(x4), static_cast<int>(y4), grHandle, transFlag);
	}
	inline void DrawModiGraph(
		Vector2& lt, Vector2& rt,
		Vector2& rb, Vector2& lb,
		int grHandle, bool transFlag) {
		DxLib::DrawModiGraph(
			static_cast<int>(lt.x), static_cast<int>(lt.y),
			static_cast<int>(rt.x), static_cast<int>(rt.y),
			static_cast<int>(rb.x), static_cast<int>(rb.y),
			static_cast<int>(lb.x), static_cast<int>(lb.y), grHandle, transFlag);
	}
	// 回転拡縮並行移動行列を使って描画する
	inline void DrawModiGraph(const Matrix3& view, const Vector2& position, const Vector2& scale, double angle, int width, int height, int rgHandle, bool isFlip) {
		// 回転×拡縮×並行移動
		Matrix3 m = Matrix3::CreateRotation(angle);
		m *= Matrix3::CreateScale(scale);
		m *= Matrix3::CreateTranslation(position);

		m *= view;

		// 描画する画像の4つの頂点座標
		std::array<Vector2, 4> pos;
		if (isFlip==true) {             //反転
		pos = {
			Vector2{ width / 2.0, -height / 2.0 },	// 右上から左回り
			Vector2{-width / 2.0, -height / 2.0 },	// 左上
			Vector2{-width / 2.0,  height / 2.0 },	// 左下
			Vector2{ width / 2.0,  height / 2.0 }		// 右下
		    };
	    }

		if (isFlip==false) {            //正常
		pos = {
			Vector2{-width / 2.0, -height / 2.0 },	// 左上から右回り
			Vector2{ width / 2.0, -height / 2.0 },	// 右上
			Vector2{ width / 2.0,  height / 2.0 },	// 右下
			Vector2{-width / 2.0,  height / 2.0 }		// 左下
			};
		}
		// 描画する画像の4つの頂点座標
		/*
		std::array<Vector2, 4> pos = {

				Vector2{ -width / 2.0, -height / 2.0 },	// 左上から右回り
					Vector2{ width / 2.0, -height / 2.0 },	// 右上
					Vector2{ width / 2.0,  height / 2.0 },	// 右下
					Vector2{ -width / 2.0,  height / 2.0 }		// 左下
					// 反転用
					//Vector2{ width / 2.0, -height / 2.0 },	// 右上から左回り
					//Vector2{-width / 2.0, -height / 2.0 },	// 左上
					//Vector2{-width / 2.0,  height / 2.0 },	// 左下
					//Vector2{ width / 2.0,  height / 2.0 }		// 右下
			};
		*/


		// 4つの頂点座標全てに行列を掛けて変換する
		for (auto&& v : pos) {
			v = Vector2::Transform(v, m);
		}
		// 変換した座標で描画する
		MyLib::DrawModiGraph(pos[0], pos[1], pos[2], pos[3], rgHandle, true);
	}
	inline void DrawModiGraph(const Matrix3& view, const Vector2& position, double zoom, double angle, int width, int height, int rgHandle,bool isFlip) {
		MyLib::DrawModiGraph(view, position, { zoom, zoom }, angle, width, height, rgHandle,isFlip);
	}
};



