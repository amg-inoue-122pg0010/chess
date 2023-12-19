#pragma once
#include "DxLib.h"
#include <functional>
#include "Math.h"
struct CharacterBody{

	VECTOR _headTop;
	VECTOR _headUnder;

	VECTOR _bodyTop;//obbの上面の中心(上端)の座標
	VECTOR _bodyUnder;//obbの底面の中心(下端)の座標
	int _bodyTopFrameIndex;//obbを添わせるモデルのフレーム番号
	int _bodyUnderFrameIndex;
	VECTOR _bodyObbSize;//obbの大きさ
	VECTOR _bodyTilt;//体の傾き

	VECTOR _leftArmTop;
	VECTOR _leftArmUnder;
	VECTOR _rightArmTop;
	VECTOR _rightArmUnder;

	VECTOR _leftLegTop;
	VECTOR _leftLegUnder;
	VECTOR _rightLegTop;
	VECTOR _rightLegUnder;

};

class OBB
{
	
protected:
	VECTOR m_Pos;              // 位置
	VECTOR m_Direct[3];        //　辺の向きと長さ、x,y,z
	VECTOR m_NormaDirect[3];   // 方向ベクトル,X,Y,Z
	float m_fLength[3];        // 各軸方向の長さ
	bool _hit;                 //OBB同士がぶつかっているか
	int _existTime;            //obbが消えるまでの時間
	bool _del;                 //このobbを削除するかどうか
	int  _effectiveTime;       //このobbが有効かどうか、有効にするまでの時間
	std::function<VECTOR(VECTOR, VECTOR, float)> _trajectory;   //遠距離攻撃の軌道

	
public:
	
	void CreateObb(VECTOR vpos, VECTOR x, VECTOR y, VECTOR z);//受け取ったベクトルでOBBを作成
	void CreateObb(VECTOR vpos, VECTOR size);//それぞれのベクトルがｘ、ｙ、ｚ軸に平行なOBBを作成する場合

	void RotateObb(VECTOR rotate);//ラジアンで角度を受け取り、OBBをx→y→zの順に回転させる
	void RotateObb(MATRIX matrix);//受け取った回転行列通りに回す

	void Update(VECTOR vpos);
	bool Update();
	void Update(VECTOR targetVPos, float spd);
	void Update(CharacterBody&);

	void Render();

	bool GetHit() { return _hit; }
	void SetHit(bool h) { _hit = h; }
	void SetExistTime(int n) { _existTime = n; }
	bool GetDel() { return _del; }
	void SetEffectiveTime(int e) { _effectiveTime = e; }
	void SetTrajectory(std::function<VECTOR(VECTOR, VECTOR, float)> trajectory) { _trajectory = trajectory; }
	std::function<VECTOR(VECTOR, VECTOR, float)> GatTrajectory() { return _trajectory; }

	enum class Type {
		CHARACTER,
		DAMAGE,
		BULLET,
		NONE
	};
	Type _type;
	void SetObbType(Type t) { _type = t; }
	Type GetObbType() { return _type; }

	VECTOR GetDirect(int elem) { return m_NormaDirect[elem]; };    // 指定軸番号の方向ベクトルを取得
	float GetLen_W(int elem) { return m_fLength[elem]; };          // 指定軸方向の長さを取得
	VECTOR GetPos_W() { return m_Pos; };             // 位置を取得
	void SetPos_W(VECTOR pos) { m_Pos = pos; };      //位置を指定
	float LenSegOnSeparateAxis(VECTOR Sep, VECTOR e1, VECTOR e2, VECTOR* e3 =0);
	bool ColOBBs(OBB& obb1, OBB& obb2);

};



