#include "CharacterBase.h"

CharacterBase::CharacterBase() {
}

void CharacterBase::Init() {
	_maxVPos = VGet(5266, 144, 5760);
	_minVPos = VGet(-4270, 0, -3760);
	_range = 0;
	_movementInAction = 0;
	_vPos = VGet(0, 0, 0);
	_playerNohittime = 120;
	_EnemyNohittime = _playerNohittime + 100;
}

void CharacterBase::CharacterObbSet() {

}

void CharacterBase::BodyRegionVPosUpdate() {
	_charaBody._bodyTop = MV1GetFramePosition(_handle, _charaBody._bodyTopFrameIndex);
	_charaBody._bodyUnder = MV1GetFramePosition(_handle, _charaBody._bodyUnderFrameIndex);
}

void CharacterBase::Process() {

	//攻撃受けた後の
	 int Nohittime = 0;
	 switch (_characterType)
	 {
	 case CharacterBase::CharacterType::PLAYER:
		 Nohittime = _playerNohittime;
		 break;
	 case CharacterBase::CharacterType::ENEMY:
		 Nohittime = _EnemyNohittime;
		 break;
	 default:
		 break;
	 }
	
	if (_hit) {
		_action = false;

		if (_nohittime > 0) {
			--_nohittime;
			if (_nohittime <= 0) {
				_hit = false;
				_nohittime += Nohittime;
				return;
			}
		}

		//アニメーションを設定
	// ステータスが変わっていないか？
		if (_oldStatus != _status) {
			AnimationSet(_action);
			_oldStatus = _status;
		}
		else {
			SetAnimeNoInterruption(_action);
		}

		AnimationPlay();
		return;
	}
	else if (_nohittime) {
		--_nohittime;
	}

	_oldStatus = _status;

	//使い終わったobbのイテレータの消し方見直し↓
	//Mathのファイルを一回取り除いて、Mathの整理、Mathに好きに関数を追加できるように

	//体にobb添って回転するように
	//生数字消し
	//キャラ追加King、モデルの読み込みから
	//必要ならEnemyProcessはキャラ毎に書く

	std::vector<int> dellist;
	int n = 0;
	for (auto iteObb = _obbs.begin(); iteObb != _obbs.end(); ++iteObb) {
		switch (iteObb->GetObbType())
		{
		case OBB::Type::CHARACTER:
			BodyRegionVPosUpdate();
			iteObb->Update(_charaBody);
			break;
		case OBB::Type::DAMAGE:
			iteObb->Update();
			if (iteObb->GetDel()) {
				dellist.push_back(n);
			}
			break;
		case OBB::Type::BULLET:
			iteObb->Update(_target, _bulletSpd);
			if (iteObb->GetDel()) {
				dellist.push_back(n);
			}
			break;
		default:
			break;
		}
		++n;
	}

	n = 0;
	if (dellist.size() > 0) {
		for (auto del : dellist) {
			_obbs.erase(_obbs.begin() + del - n);
			++n;
		}
	}
}

void CharacterBase::PlayerProcess(float camVDir, VECTOR target) {
	Process();
	if (_hit) {
		return;
	}
	_target = target;
	_cameraVDir = camVDir;
	int key = ApplicationMain::GetInstance()->GetKey();
	int trg = ApplicationMain::GetInstance()->GetTrg();
	int rel = ApplicationMain::GetInstance()->GetRel();

	if (!_action) {
		//アクション中でなければ
		if (GetMouseInput() & MOUSE_INPUT_RIGHT) {
			if (key & PAD_INPUT_5 ||
				key & PAD_INPUT_8 ||
				key & PAD_INPUT_4 ||
				key & PAD_INPUT_6) {
				RollFront(); _action = true;
			}
			else {
				Roll(); _action = true;
			}
		}
		else if (GetMouseInput() & MOUSE_INPUT_LEFT) { Attack(); _action = true; }
		else if (trg & PAD_INPUT_3) {ClassAttackLow(); _action = true;}
		else if (trg & PAD_INPUT_7) {ClassAttackHigh(); _action = true;}
		else { Move(); }
	}
	else if (_status == STATUS::ROLLFRONT) {
		_vPos = VAdd(_vPos, _normalVDir);
	}
	else if (_status == STATUS::NONE) {
		_status = STATUS::IDLE;
	}

	//アニメーションを設定
	// ステータスが変わっていないか？
	if (_oldStatus != _status) {
		AnimationSet(_action);
	}
	else {
		SetAnimeNoInterruption(_action);
	}

	AnimationPlay();
	_action = AnimationNoInterruption();
}

void CharacterBase::EnemyProcess(VECTOR target) {
	Process();
	if (_hit) {
		return;
	}
	_target = target;
	_oldStatus = _status;
	const float length = VSize(VSub(_target, _vPos));

	//アクション中でないなら
	if (!_action) {
		_movementInAction = 0;
		//インターバルが無ければアクション
		if (_enemyActionInterval <= 0) {
			_vDir = VNorm(VSub(_target, _vPos));
			//ターゲットとの距離で行動を変える
			if (length >= _range) {
				ClassAttackLow();
			}
			else {
				int s = rand() % 3;
				switch (s)
				{
				case 0:
					Roll();
					break;
				case 1:
					Attack();
					break;
				case 2:
					ClassAttackHigh();
					break;
				default:
					break;
				}
			}
			_action = true;
			_enemyActionInterval = _recoveryFrame;
		}
		else{
			_status = STATUS::IDLE;
		}
		--_enemyActionInterval;
	}
	else if(_movementInAction){
		_vPos = _movementInAction(_vPos, _target, _moveInActSpd);
	}

	//アニメーションを設定
	// ステータスが変わっていないか？
	if (_oldStatus != _status) {
		AnimationSet(_action);
	}
	else {
		SetAnimeNoInterruption(_action);
	}

	//次のアクションまでの間隔に、再生中のモーションの時間を足す
	if (_action && _enemyActionInterval == 0) {
		_enemyActionInterval = (*_vAnim.begin())->_total_time + _recoveryFrame;
	}

	AnimationPlay();
	_action = AnimationNoInterruption();
}

void CharacterBase::Move() {
	int key = ApplicationMain::GetInstance()->GetKey();
	int trg = ApplicationMain::GetInstance()->GetTrg();
	int rel = ApplicationMain::GetInstance()->GetRel();

	_normalVDir = { 0,0,0 };

	/*
	int InputX, InputY;
	GetJoypadAnalogInput(&InputX, &InputY, DX_INPUT_KEY_PAD1);
	if (key & PAD_INPUT_5) { _normalVDir.z = 1 * InputY; };
	if (key & PAD_INPUT_8) { _normalVDir.z = 1 * InputY; };
	if (key & PAD_INPUT_4) { _normalVDir.x = -1 * InputX; };
	if (key & PAD_INPUT_6) { _normalVDir.x = -1 * InputX; };
	*/
	if (key & PAD_INPUT_5) { _normalVDir.z = 1; };
	if (key & PAD_INPUT_8) { _normalVDir.z = -1; };
	if (key & PAD_INPUT_4) { _normalVDir.x = 1; };
	if (key & PAD_INPUT_6) { _normalVDir.x = -1; };

	float spd = _spd;
	_status = STATUS::WALK;
	if (GetKeyState(VK_SHIFT) & 0x8000) {
		_status = STATUS::DASH;
		spd = _dashSpd;
	}

	if (VSize(_normalVDir) > 0.f) {
		_rad = atan2(_normalVDir.z, _normalVDir.x);

		// vをrad分回転させる
		_normalVDir.x = cos(_rad + _cameraVDir) * spd;
		_normalVDir.z = sin(_rad + _cameraVDir) * spd;
	}
	else {
		_status = STATUS::IDLE;
	}

	_vPos = VAdd(_vPos, _normalVDir);

	// _normalVDirをそのままキャラの向きにする
	if (VSize(_normalVDir) > 0.f) {// 移動していない時は無視するため
		_vDir = VNorm(_normalVDir);
	}

}

void CharacterBase::EnemyMove() {
	_status = STATUS::DASH;
	_vDir = VNorm(VSub(_target, _vPos));
	_vPos = VAdd(_vPos, Math::VMulti(_vDir, _spd));
}

void CharacterBase::Roll() {
	_status = STATUS::ROLL;
}

void CharacterBase::RollFront() {
	_status = STATUS::ROLLFRONT;
}

void CharacterBase::Attack() {
	_status = STATUS::ATTACK;
	OBB atkobb;
	atkobb.CreateObb(VAdd(VGet(_vPos.x, _vPos.y + _CharacterCenterHight, _vPos.z), Math::VMulti(VNorm(_vDir), 50))
		, VGet(20, 0, 0), VGet(0, 20, 0), VGet(0, 0, 20));
	atkobb.SetExistTime(40); 
	atkobb.SetEffectiveTime(_attackStartupFrame);
	atkobb.SetObbType(OBB::Type::DAMAGE);
	_obbs.push_back(atkobb);
}

void CharacterBase::ClassAttackLow() {
	_status = STATUS::CLASSATTACKLOW;
}

void CharacterBase::ClassAttackHigh() {
	_status = STATUS::CLASSATTACKHIGH;
}

void CharacterBase::Damage() {
	if (_nohittime) {
		return;
	}
	_status = STATUS::DAMAGE;
	_hit = true;
}

void CharacterBase::Render() {

	for (auto iteAnim = _vAnim.begin(); iteAnim != _vAnim.end(); ++iteAnim) {
		MV1SetAttachAnimTime(_handle, (*iteAnim)->_attach_index, (*iteAnim)->_play_time);
	}

	// 位置
	MV1SetPosition(_handle, _vPos);
	// 向きからY軸回転を算出
	VECTOR vRot = { 0,0,0 };
	vRot.y = atan2(_vDir.x * -1, _vDir.z * -1);		// モデルが標準でどちらを向いているかで式が変わる(これは-zを向いている場合)
	MV1SetRotationXYZ(_handle, vRot);

	// 描画
	if (_nohittime) {
		if ((gGlobal._time / 5) % 2) {
			MV1DrawModel(_handle);
		}
	}
	else {
		MV1DrawModel(_handle);
	};

	for(auto e : _obbs) {
		e.Render();
	}
}

void CharacterBase::AnimationSet(bool interruption) {
	// アタッチされているアニメーションに、close時間を設ける
	for (auto iteAnim = _vAnim.begin(); iteAnim != _vAnim.end(); ++iteAnim) {
		if ((*iteAnim)->_close_time == 0.f) {
			(*iteAnim)->_close_time = 10.f;		// ブレンドするフレーム数
			(*iteAnim)->_close_total_time = (*iteAnim)->_close_time;
		}
	}
	// 新しいアニメーションを追加
	CharacterBase::Animation* anim = new CharacterBase::Animation();
	// ステータスに合わせてアニメーションのアタッチ
	anim->_nohitAnime = false;
	anim->_anim_spd = _motionSpd;
	switch (_status) {
	case STATUS::NONE:
		anim->_attach_index = MV1AttachAnim(_handle, 0, _motionHandleIdle, FALSE);
		break;
	case STATUS::IDLE:
		anim->_attach_index = MV1AttachAnim(_handle, 0, _motionHandleIdle, FALSE);
		break;
	case STATUS::WALK:
		anim->_attach_index = MV1AttachAnim(_handle, 0, _motionHandleWalk, FALSE);
		break;
	case STATUS::DASH:
		anim->_attach_index = MV1AttachAnim(_handle, 0, _motionHandleDash, FALSE);
		break;
	case STATUS::DAMAGE:
		anim->_attach_index = MV1AttachAnim(_handle, 0, _motionHandleDamage, FALSE);
		anim->_nohitAnime = true;
		break;
	case STATUS::DEAD:
		anim->_attach_index = MV1AttachAnim(_handle, 0, _motionHandleDead, FALSE);
		break;
	case STATUS::ROLL:
		anim->_attach_index = MV1AttachAnim(_handle, 0, _motionHandleRoll, FALSE);
		anim->_anim_spd = 0.8;
		anim->_nohitAnime = true;
		break;
	case STATUS::ROLLFRONT:
		anim->_attach_index = MV1AttachAnim(_handle, 0, _motionHandleRollFront, FALSE);
		anim->_anim_spd = 0.8;
		anim->_nohitAnime = true;
		break;
	case STATUS::ATTACK:
		anim->_attach_index = MV1AttachAnim(_handle, 0, _motionHandleAttack, FALSE);
		break;
	case STATUS::CLASSATTACKLOW:
		anim->_attach_index = MV1AttachAnim(_handle, 0, _motionHandleClassAtkLow, FALSE);
		break;
	case STATUS::CLASSATTACKHIGH:
		anim->_attach_index = MV1AttachAnim(_handle, 0, _motionHandleClassAtkHigh, FALSE);
		break;
	}

	// アタッチしたアニメーションの総再生時間を取得する
	anim->_total_time = MV1GetAttachAnimTotalTime(_handle, anim->_attach_index);
	// 再生時間を初期化
	anim->_close_time = 0.0f;
	anim->_play_time = 0.0f;

	anim->_noInterruption = interruption;
	if (anim->_nohitAnime) {
		_nohittime = anim->_total_time;
	}

	// アニメーション追加
	_vAnim.push_back(anim);

}

void CharacterBase::AnimationPlay() {
	// アニメーション時間処理
	for (auto iteAnim = _vAnim.begin(); iteAnim != _vAnim.end(); ) {
		if ((*iteAnim)->_close_time == 0.f) {
			// 再生時間を進める
			(*iteAnim)->_play_time += (*iteAnim)->_anim_spd;
			// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
			if ((*iteAnim)->_play_time >= (*iteAnim)->_total_time) {
				(*iteAnim)->_play_time = 0.0f;
				if ((*iteAnim)->_noInterruption) {
					(*iteAnim)->_noInterruption = false;
				}
			}
		}
		else {
			// 閉じ時間を減らす
			(*iteAnim)->_close_time -= (*iteAnim)->_anim_spd;

			// 閉じ時間が終わったか？
			if ((*iteAnim)->_close_time <= 0.f) {
				// アニメーションがアタッチされていたら、デタッチする
				MV1DetachAnim(_handle, (*iteAnim)->_attach_index);
				// このアニメーションを削除
				delete (*iteAnim);
				iteAnim = _vAnim.erase(iteAnim);
				continue;
			}
			// ブレンド率を変更する
			MV1SetAttachAnimBlendRate(_handle, (*iteAnim)->_attach_index, (*iteAnim)->_close_time / (*iteAnim)->_close_total_time);
		}
		++iteAnim;
	}
}

bool CharacterBase::AnimationNoInterruption() {
	for (auto iteAnim = _vAnim.begin(); iteAnim != _vAnim.end(); ) {
		if ((*iteAnim)->_noInterruption) {
			return true;
		}
		++iteAnim;
	}
	return false;
}

void CharacterBase::SetAnimeNoInterruption(bool interruption) {
	auto iteAnim = _vAnim.begin();
	(*iteAnim)->_noInterruption = interruption;
}
