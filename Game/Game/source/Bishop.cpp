#include "Bishop.h"
#include "AppFrame.h"
#include <stdlib.h>

Bishop::Bishop() {	
	Init();
}

void Bishop::Init() {
	CharacterBase::Init();
    _handle = ResourceServer::GetHandles("Bishop");
    MV1SetScale(_handle, VGet(1,1,1));
	_motionHandleIdle          = ResourceServer::GetHandles("Idle");
	_motionHandleWalk		   = ResourceServer::GetHandles("Walk");
	_motionHandleDash		   = ResourceServer::GetHandles("Run");
	_motionHandleRoll		   = ResourceServer::GetHandles("Roll");
	_motionHandleRollFront	   = ResourceServer::GetHandles("RollFront");
	_motionHandleDamage		   = ResourceServer::GetHandles("Damage");
	_motionHandleDead		   = ResourceServer::GetHandles("Dead");
	_motionHandleAttack		   = ResourceServer::GetHandles("Punch");
	_motionHandleClassAtkLow   = ResourceServer::GetHandles("BishopAtkLow");
	_motionHandleClassAtkHigh  = ResourceServer::GetHandles("BishopAtkHigh");
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
	_charaBody._bodyTopFrameIndex = 3;
	_charaBody._bodyUnderFrameIndex = 0;
}

void Bishop::CharacterObbSet() {
	base::CharacterObbSet();

	BodyRegionVPosUpdate();
	_charaBody._bodyTilt = VNorm(VSub(_charaBody._bodyTop, _charaBody._bodyUnder));

	VECTOR CenterPos = VAdd(_charaBody._bodyTop, _charaBody._bodyUnder);

	OBB cObb;
	cObb.CreateObb(VGet(CenterPos.x / 2, CenterPos.y / 2, CenterPos.z / 2)
		, _charaBody._bodyObbSize);

	MATRIX MV1GetFrameLocalWorldMatrix(int MHandle, int FrameIndex);

	cObb.SetObbType(OBB::Type::CHARACTER);
	_obbs.push_back(cObb);
}

void Bishop::Process() {
	base::Process();
}

void Bishop::PlayerProcess(float camVDir, VECTOR target) {
	base::PlayerProcess(camVDir, target);
	
}

void Bishop::EnemyProcess(VECTOR target) {
	base::EnemyProcess(target);
}

void Bishop::Roll() {

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

void Bishop::RollFront() {
	base::RollFront();
}

void Bishop::Attack() {
	base::Attack();
}

void Bishop::ClassAttackLow() {
	base::ClassAttackLow();

	std::function<VECTOR(VECTOR, VECTOR,float)> trajectory =
		[](VECTOR vPos, VECTOR targetVPos,float spd) -> VECTOR {
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

void Bishop::ClassAttackHigh() {
	base::ClassAttackHigh();

	OBB atkobb;
	atkobb.CreateObb(VGet(_vPos.x, _vPos.y + _CharacterCenterHight, _vPos.z)
		, VGet(100, 0, 0), VGet(0, 100, 0), VGet(0, 0, 100));
	atkobb.SetObbType(OBB::Type::DAMAGE);
	atkobb.SetExistTime(80);
	atkobb.SetEffectiveTime(_classAtkHighStartupFrame);
	_obbs.push_back(atkobb);
}

void Bishop::EnemyMove() {
	base::EnemyMove();
}

void Bishop::Move() {
	base::Move();
}

void Bishop::Damage() {
	base::Damage();
}

void Bishop::Render() {
	base::Render();
}
