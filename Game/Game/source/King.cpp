#include "King.h"
#include "AppFrame.h"
#include <stdlib.h>

King::King() {
	Init();
}

void King::Init() {

	//↓↓↓↓↓↓↓↓とりあえず形にしよう
	//キャラ追加キングから
	//動きの設定、キングのlowとhigh
	//次のキャラへ
	//↓↓↓↓↓↓↓↓以下は後回し、最後に先生に聞きながらやろう
	//マウス動かすとobbが逆に回転するの直したい、obbの初期化項目にMATRIXを持たせてみる、obbの座標表示して確認する
	//カメラの動作の確認
	//当たり判定の確認
	//壁床との当たり判定どうするか

	CharacterBase::Init();
	_handle = ResourceServer::GetHandles("King");
	MV1SetScale(_handle, VGet(1, 1, 1));
	_motionHandleIdle         = ResourceServer::GetHandles("KingIdle");
	_motionHandleWalk         = ResourceServer::GetHandles("KingWalk");
	_motionHandleDash         = ResourceServer::GetHandles("KingRun");
	_motionHandleRoll         = ResourceServer::GetHandles("KingRoll");
	_motionHandleRollFront    = ResourceServer::GetHandles("KingRollFront");
	_motionHandleDamage       = ResourceServer::GetHandles("KingDamage");
	_motionHandleDead         = ResourceServer::GetHandles("KingDead");
	_motionHandleAttack       = ResourceServer::GetHandles("KingNormalAtk");
	_motionHandleClassAtkLow  = ResourceServer::GetHandles("KingAtkLow");
	_motionHandleClassAtkHigh = ResourceServer::GetHandles("KingAtkHigh");
	// ステータスを「無し」に設定
	_status = STATUS::NONE;
	_motionSpd = 0.5;
	_CharacterCenterHight = 100;
	_vDir = VGet(0, 0, -1);
	_land = false;
	_r = 20;
	_nohittime = 0;
	_hit = false;
	_spd = 5;
	_dashSpd = 8;
	_enemyActionInterval = 0;
	_bulletSpd = 3;
	_action = false;
	_range = 200;
	_attackStartupFrame = 10;
	_classAtkLowStartupFrame = 40;
	_classAtkHighStartupFrame = 130;
	_recoveryFrame = 180;
	_moveInRollSpd = _spd * -3;
	_charaBody._bodyObbSize = VGet(20, 40, 20);
	_charaBody._bodyTopFrameIndex = 25;
	_charaBody._bodyUnderFrameIndex = 30;
}

void King::CharacterObbSet() {
	base::CharacterObbSet();


	BodyRegionVPosUpdate();
	_charaBody._bodyTilt = VNorm(VSub(_charaBody._bodyTop, _charaBody._bodyUnder));

	VECTOR CenterPos = VAdd(_charaBody._bodyTop, _charaBody._bodyUnder);

	OBB cObb;
	cObb.CreateObb(VGet(CenterPos.x / 2, CenterPos.y / 2, CenterPos.z / 2)
		, _charaBody._bodyObbSize);


	cObb.SetObbType(OBB::Type::CHARACTER);
	_obbs.push_back(cObb);
}

void King::Process() {
	base::Process();
}

void King::PlayerProcess(float camVDir, VECTOR target) {
	base::PlayerProcess(camVDir, target);

}

void King::EnemyProcess(VECTOR target) {
	base::EnemyProcess(target);
}

void King::Roll() {

	_movementInAction =
		[](VECTOR vPos, VECTOR targetVPos, float spd) -> VECTOR {
		VECTOR vDir = VNorm(VSub(targetVPos, vPos));
		VECTOR vSpd = VGet(vDir.x * spd, vDir.y, vDir.z * spd);
		return vPos = VAdd(vPos, vSpd);
	};
	_moveInActSpd = _moveInRollSpd;
	//if (_action) {
		//_vDir = VNorm(VSub(_target, _vPos));
	//}
	base::Roll();
}

void King::RollFront() {
	base::RollFront();
}

void King::Attack() {
	base::Attack();
}

void King::ClassAttackLow() {
	base::ClassAttackLow();

	std::function<VECTOR(VECTOR, VECTOR, float)> trajectory =
		[](VECTOR vPos, VECTOR targetVPos, float spd) -> VECTOR {
		VECTOR vDir = VNorm(VSub(targetVPos, vPos));
		VECTOR vSpd = VGet(vDir.x * spd, vDir.y, vDir.z * spd);
		return vPos = VAdd(vPos, vSpd);
	};

	OBB atkobb;
	atkobb.CreateObb(VGet(_vPos.x, _vPos.y + _CharacterCenterHight, _vPos.z)
		, VGet(20, 0, 0), VGet(0, 20, 0), VGet(0, 0, 20));
	atkobb.SetExistTime(300);
	atkobb.SetEffectiveTime(_classAtkLowStartupFrame);
	atkobb.SetObbType(OBB::Type::BULLET);
	atkobb.SetTrajectory(trajectory);
	_obbs.push_back(atkobb);
}

void King::ClassAttackHigh() {
	base::ClassAttackHigh();

	OBB atkobb;
	atkobb.CreateObb(VGet(_vPos.x, _vPos.y + _CharacterCenterHight, _vPos.z)
		, VGet(100, 0, 0), VGet(0, 100, 0), VGet(0, 0, 100));
	atkobb.SetObbType(OBB::Type::DAMAGE);
	atkobb.SetExistTime(80);
	atkobb.SetEffectiveTime(_classAtkHighStartupFrame);
	_obbs.push_back(atkobb);
}

void King::EnemyMove() {
	base::EnemyMove();
}

void King::Move() {
	base::Move();
}

void King::Damage() {
	base::Damage();
}

void King::Render() {
	base::Render();
}
