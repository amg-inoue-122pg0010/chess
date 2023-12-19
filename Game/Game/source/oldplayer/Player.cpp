#include "Player.h"
#include "ModeGame.h"
#include "AppFrame.h"
#include "ApplicationGlobal.h"

/**
* @file Player.cpp
* @brief プレイヤーが操作するキャラの動き、アニメーション、ギミックとの当たり判定
* @author 井上和哉
*/

// 計算用マクロ
#define	PI	(3.1415926535897932386f)
#define	DEG2RAD(x)			( ((x) / 180.0f ) * PI )
#define	RAD2DEG(x)			( ((x) * 180.0f ) / PI )

Player::Player(ModeGame& modegame)
	:_modegame(modegame)
{
	Init();
}


void Player::SetStatus(PlayerData data) {

	_jumpHeight = data._jumpHeight;
	_airJumpHeight = data._airJumpHeight;
	_walkSpd = data._walkSpd;
	_dashSpd = data._dashSpd;
	_minDashSpd = _dashSpd;
	_maxDashSpd = data._maxDashSpd;
	_accelerateTime = data._accelerateTime;
	_acceleration = (_maxDashSpd - _minDashSpd) / _accelerateTime;
	_squatSpd = data._squatSpd;
	_dashTimeToSlidhing = data._dashTimeToSlidhing;
}

void Player::Init() {
	ObjectBase::Init();
	_life = 3;
	_handle = MV1LoadModel("res/PlayerModel/Player_11_data_multimotion0324.mv1");
	_camerarequest = false;
	_attach_index = -1;		// アニメーションアタッチはされていない
	// ステータスを「無し」に設定
	_status = STATUS::NONE;
	// 再生時間の初期化
	_play_time = 0.0f;
	_vDir = VGet(0, 0, -1);
	_land = false;
	_r = 20;
	_nohittime = 0;
	_hit = false;
	_standardHeight = 170;
	_squatHeight = 70;
	_height = _standardHeight;
	_standbyInterval = 0;
	_standbytype = -1;
	// 腰位置の設定
	_colSubY = 40.f;
	_colSubSide = 15.f;
	_damage = LoadEffekseerEffect("res/Effect/PlayerEffect/ef_damage/ef_damage.efkefc", 50.0f);
	_sliding = LoadEffekseerEffect("res/Effect/PlayerEffect/ef_sliding/ef_sldiing.efkefc", 50.0f);
	_doublejump = LoadEffekseerEffect("res/Effect/PlayerEffect/ef_doublejump/ef_doublejump.efkefc", 50.0f);
	_die = LoadEffekseerEffect("res/Effect/PlayerEffect/ef_die/ef_die.efkefc", 50.0f);
};

void Player::Process() {
	//ここでプレイヤーを動かす
 
	int key = ApplicationMain::GetInstance()->GetKey();
	int trg = ApplicationMain::GetInstance()->GetTrg();
	int rel = ApplicationMain::GetInstance()->GetRel();
	
	VECTOR oldv = _v;
	VECTOR oldvPos = _vPos;
	STATUS oldStatus = _status;
	_v = { 0,0,0 }; 

	_g -= 1;
	//着地してるか
	//空中でヒットしていた場合着地まで操作不能、落下モーションに
	if (_land) {
		_airJumpCnt = 1;
		_wallJumpCnt = 1;
		_g = 0;
		_hit = false;
	}
	else if (_hit) {
		_status = STATUS::FALL;
	}

	//被ダメ時のノックバック処理
	auto dir = VMulti(VNorm(_vDir), -1);
	if (_nohittime != 0) {//ヒット中なら
		_hit = true;
		_nohittime -= 1;
		_g = 0;

		switch (_land)
		{
		case true:
			_status = STATUS::DAMAGE;
			break;
		case false:
			_status = STATUS::DAMAGE_AIR;
		}
		
		_v = VAdd(_v, dir);

		if (_nohittime == 0) {
			_status = STATUS::NONE;
			_input = true;
		}

		if ( _playingEffectDamage == 0) {
			
			if (_life <= 0) {
				_playingEffectDamage = PlayEffekseer3DEffect(_die);
				SetSpeedPlayingEffekseer3DEffect(_playingEffectDamage, 1.0f);
			}
			else {
				_playingEffectDamage = PlayEffekseer3DEffect(_damage);
			}
			// エフェクトを再生する。
			VECTOR dir = _vDir;
			VECTOR vx, vz;
			vx = VGet(dir.x, dir.y, dir.z);//dir;
			vz = VGet(0, 0, -1);
			float rady = acos(VDot(vx, vz) / (VSize(vx) * VSize(vz)));
			//これ外積、右回り・左回りの判定にどう使うか聞こう
			VECTOR gisk = VCross(vx, vz);
			SetRotationPlayingEffekseer3DEffect(_playingEffectDamage, 0, rady, 0);
		}
		// 再生中のエフェクトを移動する。
		SetPosPlayingEffekseer3DEffect(_playingEffectDamage, _vPos.x, _vPos.y, _vPos.z);
	}
	else {
		StopEffekseer3DEffect(_playingEffectDamage);
		_playingEffectDamage = 0;
	}
	_vPos.y += _g;

	//ヒット中でないかつカメラが切り替えされていなければキャラ操作
	if (_hit == false) {
			if (key & PAD_INPUT_DOWN) { _v.x = 1; }
			if (key & PAD_INPUT_UP) { _v.x = -1; }
			if (key & PAD_INPUT_LEFT) { _v.z = -1; }
			if (key & PAD_INPUT_RIGHT) { _v.z = 1; }

			//スライディング中は方向を変えない
			if (_status == STATUS::SLIDHING) {
				_v = _slidhingV;
			}
			//壁ジャンプ中はX軸を変えない
			if (_status == STATUS::JUMP_WALL) {
				_v.z = _wallJumpDir;
			}

		//ジャンプボタンが押された時
		if (trg & PAD_INPUT_1 ) {

			if (_land == false && _stick == true && _wallJumpCnt >= 1) {//空中で壁に触れている状態なら
				_g = _airJumpHeight;
				_status = STATUS::JUMP_WALL;
				--_wallJumpCnt;
				PlaySoundMem(gGlobal._pv["jumpwall"], DX_PLAYTYPE_BACK, true);
				_wallJumpDir = _vDir.x / abs(_vDir.x) * -1;
			}
			else if (_land == false && _airJumpCnt >= 1) {//空中にいてジャンプ回数があれば
				_g = _airJumpHeight;
				_status = STATUS::JUMP_AIR;
				--_airJumpCnt;
				_playingEffect = PlayEffekseer3DEffect(_doublejump);
				PlaySoundMem(gGlobal._pv["jump2nd"], DX_PLAYTYPE_BACK, true);
			}
			if (_land == true) {//地面ならジャンプする
				_g = _jumpHeight;
				_vPos.y += _g;
				_status = STATUS::JUMP;
				_input = false;
				_land = false;
				int rand = GetRand(1);
				if (rand == 0) {
					PlaySoundMem(gGlobal._pv["jump1st1"], DX_PLAYTYPE_BACK, true);
				}
				if (rand == 1) {
					PlaySoundMem(gGlobal._pv["jump1st2"], DX_PLAYTYPE_BACK, true);
				}
			}
		}
		
		//落下中
		if (_vPos.y < oldvPos.y && _status != STATUS::JUMP_AIR && _status != STATUS::JUMP_WALL) {//ジャンプモーションのみ落下中でも優先して再生
			_status = STATUS::FALL;
		}

		//着地してるなら
		if (_land == true) {
			
			if (VSize(_v) > 0.f) {		
				//歩き
				_status = STATUS::WALK;
				if (oldStatus != STATUS::SLIDHING) {
					_spd = _walkSpd;
				}

				//ダッシュ
				if (key & PAD_INPUT_6 && oldStatus != STATUS::SLIDHING && oldStatus != STATUS::SQUAT
					&& oldStatus != STATUS::SQUAT_WALK) {
					_spd = _dashSpd;
					_status = STATUS::DASH;
					PlaySoundMem(gGlobal._se["Sprint"], DX_PLAYTYPE_LOOP, true);
				}
				else {
					StopSoundMem(gGlobal._se["Sprint"]);
				}

				//しゃがみ移動
				if (key & PAD_INPUT_3) {
					//同じ方向に一定時間走っていたらスライディング
					if (_spd > _squatSpd && _dashTime > _dashTimeToSlidhing) {
						_status = STATUS::SLIDHING;
						_spd -= 0.1f;
						_slidhingV = _v;
					}
					else {
						_status = STATUS::SQUAT_WALK;
						_spd = _squatSpd;
					}
					_height = _squatHeight;
				}
				else {
					_height = _standardHeight;
				}
			}
			else {
				_status = STATUS::WAIT;
				//しゃがみ
				if (key & PAD_INPUT_3) {
					_status = STATUS::SQUAT;
					_height = _squatHeight;
				}
				else {
					_height = _standardHeight;
				}
				//着地
				if (oldStatus == STATUS::FALL || oldStatus == STATUS::LANDING) {
					_status = STATUS::LANDING;
				}
			}
		}

		length = 0.f;
		if (VSize(_v) > 0.f) { 
			length = _spd;
			//ダッシュしていたらダッシュの最大速度を上げる
			if (_status == STATUS::DASH) {
				_dashSpd += _acceleration;
			}
			else {
				_dashSpd -= _acceleration;
			}
		}
		else {
			_dashSpd -= _acceleration;
		}
		_dashSpd = Math::Clamp(_dashSpd, _minDashSpd, _maxDashSpd);

		//キャラの移動する量を計算
		rad = atan2(_v.z, _v.x);
		_v.x = cos(Math::ToRadians(-90) + rad) * length;
		_v.z = sin(Math::ToRadians(-90) + rad) * length;

	};

	// 移動前の位置を保存
	oldv = _v; 
	oldvPos = _vPos;

	// コリジョン判定で引っかかった時に、escapeTbl[]順に角度を変えて回避を試みる
	float escapeTbl[] = {
		0, -10, 10, -20, 20, -30, 30, -40, 40, -50, 50, -60, 60, -70, 70, -80, 80,
	};
	for (int i = 0; i < sizeof(escapeTbl) / sizeof(escapeTbl[0]); i++) {
		// escapeTbl[i]の分だけ移動量v回転
		float escape_rad = DEG2RAD(escapeTbl[i]);
		if (_hit == false) {
			_v.x = cos(rad +Math::ToRadians(-90)+ escape_rad) * length;
			_v.z = sin(rad + Math::ToRadians(-90) + escape_rad) * length;
		}
		// vの分移動
		_vPos = VAdd(_vPos, _v);
		// 移動した先でコリジョン判定
		
		MV1_COLL_RESULT_POLY hitPoly;
		bool land = false;//仮の着地状態

		// 主人公の腰位置から下方向への直線
		VECTOR LineStart = VAdd(_vPos, VGet(0, _colSubY, 0));
		VECTOR LineEnd = VAdd(_vPos, VGet(0, -_colSubY, 0));

		//主人公の足元から身長分上に伸ばした線
		VECTOR UpLineStart = VAdd(_vPos, VGet(0, _height, 0));
		

		//ギミックとプレイヤーのY軸線での判定
		for (auto e : _modegame.GetGimmickCollision()) {

			//キャラの上部分、キャラから上に伸ばした線とギミックが当たったか確認
			if (e->GetCollDamege() != -1) {
				//ギミックのダメージ部分と当たったか
				hitPoly = MV1CollCheck_Line(e->GetCollDamege(), 0,
					_vPos, UpLineStart);
				if (hitPoly.HitFlag) {
					//当たったら即死
					Damage(0, _life);
				}
			}

			//ダメージではない部分と当たったら落下させる
			for (auto i : e->GetCollisions()) {
				hitPoly = MV1CollCheck_Line(i, 0,
					_vPos, UpLineStart);
				if (hitPoly.HitFlag) {
					_g = -1;
				}
			}

			hitPoly = MV1CollCheck_Line(e->GetCollFloor(), 0,
				_vPos, UpLineStart);
			if (hitPoly.HitFlag) {
				_g = -1;
			}
			
			//キャラの下部分、キャラから下に伸ばした線とギミックが当たったか確認
			//ギミックの上に当たったか
			if (land == false) {
				hitPoly = MV1CollCheck_Line(e->GetCollFloor(), 0,
					LineStart, LineEnd);
				land = HitCheckFloor(hitPoly, oldvPos);
				//当たったらギミックごとのアクションを呼ぶ
				if (land) {
					e->HitAction();
				}
			}
		}
		_land = land;
		//地面
		if (_land == false) { 
			hitPoly = MV1CollCheck_Line(_modegame.GetStageFloorCollision(), 0,
				LineStart, LineEnd);
			_land = HitCheckFloor(hitPoly, oldvPos);
		}
		break;
	}

	// 移動量をそのままキャラの向きにする
	if (VSize(_v) > 0.f) {// 移動していない時は無視するため
		_vDir = _v;
	}
	if (_nohittime != 0) {
		_vDir = VMulti(_vDir, -1);
	}


	//キャラの腰から前方向に伸ばした線で当たり判定
	//判定に使う線分の始まりと終わり
	VECTOR a = VGet(_vPos.x, _vPos.y + _colSubY, _vPos.z);
	VECTOR b = VAdd(a, VMulti(_vDir, _colSubSide));
	//後ろにも伸ばす
	a = VAdd(a, VMulti(_vDir, -_colSubSide));
	bool stick = false;

	for (auto e : _modegame.GetStageWallCollision()) {

		//どれかの壁に当たっていたら判定を省略
		if (stick) {
			break;
		}
		stick = HitCheckWall(a, b, oldvPos, oldv, e, stick);
	}
	//ギミックとプレイヤーのZ軸線での当たり判定
	for (auto e : _modegame.GetGimmickCollision()) {

		for (auto i : e->GetCollisions()) {
			//どれかの壁に当たっていたら判定を省略
			if (stick) {
				bool stuck = false;
				stuck = HitCheckWall(a, b, oldvPos, oldv, i, stick);
				if (stuck) {
					Damage(0, _life);
				}
			}
			else {
				stick = HitCheckWall(a, b, oldvPos, oldv, i, stick);
			}
		}
	}

	_stick = stick;

	//判定の形がスフィアのギミックと当たり判定
	for (auto e : _modegame.GetGimmickCollision()) {
		if (e->GetGimmickType() ==
			GimmickBase::GimmickType::HAMMER||
			e->GetGimmickType() ==
			GimmickBase::GimmickType::APPROACHINGWALL||
			e->GetGimmickType() ==
			GimmickBase::GimmickType::APPROACHINGFLOOR)
		{
			VECTOR vpos = e->GetvPos();
			if (HitCheck_Sphere_Capsule(e->GetSphereVPos(), e->Getrad(),
				_senterpos, _capsulepos_end, _r))
			{
				Damage(0, _life);
			}
		}
	}


	//当たり判定カプセルの変形
	_senterpos = GetvPos();
	_senterpos.y += _r;
	_capsulepos_end = _senterpos;
	_capsulepos_end.y += 130;

	if (_status == STATUS::SLIDHING) {
		VECTOR dir = VNorm(_vDir);
		_capsulepos_end.y -= 100;
		_capsulepos_end.x += (dir.x * -1) * 130;
		_capsulepos_end.z += (dir.z * -1) * 130;
	}
	
	if (_status == STATUS::SQUAT|| _status == STATUS::SQUAT_WALK) {
		_capsulepos_end.y -= 100;
	}

	bool waitmotion = false;
	//待機モーションの設定
	if (_standbyInterval == 200) {
		_standbytype = GetRand(3);
		waitmotion = true;
		_standbyInterval = 0;
		PlaySoundMem(gGlobal._pv["stay"], DX_PLAYTYPE_BACK, true);
	}

	// ステータスが変わっていないか？
	if (oldStatus != _status || waitmotion) {
		_standbyInterval = 0;
		AnimationSet();
	}
	else if (_status == STATUS::WAIT) {
		_standbyInterval++;
	}

	// アニメーション時間処理
	for (auto iteAnim = _vAnim.begin(); iteAnim != _vAnim.end(); ) {
		if ((*iteAnim)->_close_time == 0.f) {
			// 再生時間を進める
			(*iteAnim)->_play_time += (*iteAnim)->_anim_spd;
			// 再生時間をランダムに揺らがせる
			switch (_status) {
			case Player::STATUS::WAIT:
				(*iteAnim)->_play_time += (float)(rand() % 10) / 100.f;	// 0.00 ～ 0.09 の揺らぎ。積算するとずれが起きる
				break;
			}
			// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
			if ((*iteAnim)->_play_time >= (*iteAnim)->_total_time) {
				(*iteAnim)->_play_time = 0.0f;

				//特定モーションは一度再生したらステータスを変える
				//着地モーション後は待機に
				if (_status == STATUS::LANDING) {
					_status = STATUS::WAIT;
					AnimationSet();
				}
				//二段ジャンプモーション後は落下に
				if (_status == STATUS::JUMP_AIR || _status == STATUS::JUMP_WALL) {
					_status = STATUS::FALL;
					AnimationSet();
				}
				//待機モーション後は通常の待機に
				if (_standbytype != -1) {
					_standbytype = -1;
					AnimationSet();
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

	//同じ方向に走っていたら加算
	if (_status != STATUS::SLIDHING) {
		if (VSize(VSub(oldv, _v)) == 0 && _status == STATUS::DASH) {
			++_dashTime;
		}
		else {
			_dashTime = 0;
		}
	}
	// 再生中のエフェクトを移動する。
	SetPosPlayingEffekseer3DEffect(_playingEffect, _vPos.x, _vPos.y, _vPos.z);
}

void Player::Damage(int knockbacktime, int damage) {
	
	_nohittime = knockbacktime;
	//即死ダメージを受けたら無敵時間を無限にしておく
	if (_life == damage) {
		_nohittime = 999999;
	}
	_life -= damage;

	//音声
	PlaySoundMem(gGlobal._se["laser_damage"], DX_PLAYTYPE_BACK, true);
	int rand = GetRand(1);
	if (_life >= 1) {
		if (rand == 0) {
			PlaySoundMem(gGlobal._pv["damege1"], DX_PLAYTYPE_BACK, true);
		}
		if (rand == 1) {
			PlaySoundMem(gGlobal._pv["damege2"], DX_PLAYTYPE_BACK, true);
		}
	}
	else {
		PlaySoundMem(gGlobal._pv["dead"], DX_PLAYTYPE_BACK, true);
	}
}

bool Player::HitCheckFloor(MV1_COLL_RESULT_POLY hitPoly, VECTOR oldvPos) {

	if (hitPoly.HitFlag &&
		_status != Player::STATUS::JUMP) {
		// 当たった
		// 当たったY位置をキャラ座標にする

		// キャラが上下に移動した量だけ、移動量を修正
		_v.y += _vPos.y - oldvPos.y;
		if (_vPos.y < hitPoly.HitPosition.y) {
			_vPos.y = hitPoly.HitPosition.y + 1.0f;
		}
		// ループiから抜ける
		return true;
	}

	return false;
};

bool Player::HitCheckWall(VECTOR a, VECTOR b, VECTOR oldvPos, VECTOR oldv, int e, bool stick) {


	auto n = MV1GetPosition(e);
	// 主人公の腰位置から前方向への直線
	MV1_COLL_RESULT_POLY hitPolySide;
	hitPolySide = MV1CollCheck_Line(e, 0,
		a, b);

	//コリジョンと線分が当たった時、当たった場所とプレイヤーの距離で判定を変える
	auto c = VSize(VSub(_vPos, hitPolySide.HitPosition));

	if (hitPolySide.HitFlag) {
		if (c < 90) {
			//壁に触れている
			stick = true;
		}
		else {
			stick = false;
		}

		if (c < 45) {
			//壁を超えようとしている
			//ここの-20は壁が動く速度によって変わる
			_vPos = VAdd(hitPolySide.HitPosition, VMulti(VNorm(_vDir), -20));
			_vPos.y = oldvPos.y;
			_v = oldv;
		}
	}
	else {
		stick = false;
	}
	return stick;
}

void Player::AnimationSet() {

	// アタッチされているアニメーションに、close時間を設ける
	for (auto iteAnim = _vAnim.begin(); iteAnim != _vAnim.end(); ++iteAnim) {
		if ((*iteAnim)->_close_time == 0.f) {
			(*iteAnim)->_close_time = 2.f;		// ブレンドするフレーム数
			(*iteAnim)->_close_total_time = (*iteAnim)->_close_time;
		}
	}
	// 新しいアニメーションを追加
	Player::Animation* anim = new Player::Animation();
	// ステータスに合わせてアニメーションのアタッチ
	switch (_status) {
	case Player::STATUS::WAIT:
		switch (_standbytype) {
		case 0:
			anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_idol_dust"), -1, FALSE);
			break;
		case 1:
			anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_idol_gymnastics"), -1, FALSE);
			break;
		case 2:
			anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_idol_tonton"), -1, FALSE);
			break;
		case 3:
			anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_idol_stretch"), -1, FALSE);
			break;
		default:
			anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_idol"), -1, FALSE);
			break;
		}
		break;
	case Player::STATUS::WALK:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_wark"), -1, FALSE);
		break;
	case Player::STATUS::DASH:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_run"), -1, FALSE);
		break;
	case Player::STATUS::DAMAGE:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_damage"), -1, FALSE);
		if (_life <= 0) {
		anim->_anim_spd = 0.08f;
		}
		break;
	case Player::STATUS::DAMAGE_AIR:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_damage"), -1, FALSE);
		break;
	case Player::STATUS::SLIDHING:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_sliding"), -1, FALSE);
		_playingEffect = PlayEffekseer3DEffect(_sliding);
		break;
	case Player::STATUS::JUMP:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_jump"), -1, FALSE);
		anim->_anim_spd = 1.f;
		break;
	case Player::STATUS::JUMP_AIR:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_jump_second"), -1, FALSE);
		anim->_anim_spd = 1.f;
		break;
	case Player::STATUS::JUMP_WALL:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_walljump_left"), -1, FALSE);
		break;
	case Player::STATUS::SQUAT:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_crawling_idol"), -1, FALSE);
		break;
	case Player::STATUS::SQUAT_WALK:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_crawling"), -1, FALSE);
		break;
	case Player::STATUS::FALL:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_stay_air"), -1, FALSE);
		break;
	case Player::STATUS::LANDING:
		anim->_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "motion_landing"), -1, FALSE);
		break;
	}

	// アタッチしたアニメーションの総再生時間を取得する
	anim->_total_time = MV1GetAttachAnimTotalTime(_handle, anim->_attach_index);
	// 再生時間を初期化
	anim->_play_time = 0.0f;
	anim->_close_time = 0.0f;
	// 再生開始時間をランダムにずらす
	switch (_status) {
	case Player::STATUS::WAIT:
		anim->_play_time += rand() % 30;	// 30は適当
		break;
	}
	// アニメーション追加
	_vAnim.push_back(anim);
}

void Player::Render() {


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
	MV1DrawModel(_handle);
	auto color = GetColor(rand() % 255, rand() % 255, rand() % 255);

	if (_modegame.GetDebugViewColloion()) {
		DrawCapsule3D(_senterpos, _capsulepos_end, _r, 16, color, color, FALSE);
	}

	//壁判定用の線分
	auto a = VGet(_vPos.x, _vPos.y + _colSubY, _vPos.z);
	auto b = VAdd(a, VMulti(_vDir, _colSubSide));

	auto Matrix = MGetRotY(Math::ToRadians(90));
	b = VTransformSR(b, Matrix);


}

void Player::StopEffect() {
	StopEffekseer3DEffect(_playingEffectDamage);
	StopEffekseer3DEffect(_playingEffect);
}




