#include "Player.h"
#include "ModeGame.h"
#include "AppFrame.h"
#include "ApplicationGlobal.h"

/**
* @file Player.cpp
* @brief �v���C���[�����삷��L�����̓����A�A�j���[�V�����A�M�~�b�N�Ƃ̓����蔻��
* @author ���a��
*/

// �v�Z�p�}�N��
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
	_attach_index = -1;		// �A�j���[�V�����A�^�b�`�͂���Ă��Ȃ�
	// �X�e�[�^�X���u�����v�ɐݒ�
	_status = STATUS::NONE;
	// �Đ����Ԃ̏�����
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
	// ���ʒu�̐ݒ�
	_colSubY = 40.f;
	_colSubSide = 15.f;
	_damage = LoadEffekseerEffect("res/Effect/PlayerEffect/ef_damage/ef_damage.efkefc", 50.0f);
	_sliding = LoadEffekseerEffect("res/Effect/PlayerEffect/ef_sliding/ef_sldiing.efkefc", 50.0f);
	_doublejump = LoadEffekseerEffect("res/Effect/PlayerEffect/ef_doublejump/ef_doublejump.efkefc", 50.0f);
	_die = LoadEffekseerEffect("res/Effect/PlayerEffect/ef_die/ef_die.efkefc", 50.0f);

	_obb.CreateObb(MV1GetFramePosition(_handle, 8) , VGet(5, 0, 0), VGet(0, 5, 0), VGet(0, 0, 5));

};

void Player::Process() {
	//�����Ńv���C���[�𓮂���
	_obb.RotateObb(VGet(0, 0, 0));
	int key = ApplicationMain::GetInstance()->GetKey();
	int trg = ApplicationMain::GetInstance()->GetTrg();
	int rel = ApplicationMain::GetInstance()->GetRel();
	
	VECTOR oldv = _v;
	VECTOR oldvPos = _vPos;
	STATUS oldStatus = _status;
	_v = { 0,0,0 }; 

	_g -= 1;
	//���n���Ă邩
	//�󒆂Ńq�b�g���Ă����ꍇ���n�܂ő���s�\�A�������[�V������
	if (_land) {
		_airJumpCnt = 1;
		_wallJumpCnt = 1;
		_g = 0;
		_hit = false;
	}
	else if (_hit) {
		_status = STATUS::FALL;
	}

	//��_�����̃m�b�N�o�b�N����
	auto dir = VMulti(VNorm(_vDir), -1);
	if (_nohittime != 0) {//�q�b�g���Ȃ�
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
			// �G�t�F�N�g���Đ�����B
			VECTOR dir = _vDir;
			VECTOR vx, vz;
			vx = VGet(dir.x, dir.y, dir.z);//dir;
			vz = VGet(0, 0, -1);
			float rady = acos(VDot(vx, vz) / (VSize(vx) * VSize(vz)));
			//����O�ρA�E���E�����̔���ɂǂ��g����������
			VECTOR gisk = VCross(vx, vz);
			SetRotationPlayingEffekseer3DEffect(_playingEffectDamage, 0, rady, 0);
		}
		// �Đ����̃G�t�F�N�g���ړ�����B
		SetPosPlayingEffekseer3DEffect(_playingEffectDamage, _vPos.x, _vPos.y, _vPos.z);
	}
	else {
		StopEffekseer3DEffect(_playingEffectDamage);
		_playingEffectDamage = 0;
	}

	//�`�[�g���s���������Ȃ��Ȃ�
	if (_modegame.GetDebugUseColloion()) {
		_vPos.y += _g;
	}

	//�q�b�g���łȂ����J�������؂�ւ�����Ă��Ȃ���΃L��������
	if (_hit == false) {
		int InputX, InputY;
		GetJoypadAnalogInput(&InputX, &InputY, DX_INPUT_KEY_PAD1);

		if (key & PAD_INPUT_DOWN) {_v.x = 1 * InputY;}
		if (key & PAD_INPUT_UP) {_v.x = 1 * InputY;}
		if (key & PAD_INPUT_LEFT) { _v.z = 1 * InputX; }
		if (key & PAD_INPUT_RIGHT){_v.z = 1 * InputX;}

			//�X���C�f�B���O���͕�����ς��Ȃ�
			if (_status == STATUS::SLIDHING) {
				_v = _slidhingV;
			}
			//�ǃW�����v����X����ς��Ȃ�
			if (_status == STATUS::JUMP_WALL) {
				_v.z = _wallJumpDir;
			}

		//�W�����v�{�^���������ꂽ��
		if (trg & PAD_INPUT_1 ) {

			if (_land == false && _stick == true && _wallJumpCnt >= 1) {//�󒆂ŕǂɐG��Ă����ԂȂ�
				_g = _airJumpHeight;
				_status = STATUS::JUMP_WALL;
				--_wallJumpCnt;
				_wallJumpDir = _vDir.x / abs(_vDir.x) * -1;
			}
			else if (_land == false && _airJumpCnt >= 1) {//�󒆂ɂ��ăW�����v�񐔂������
				_g = _airJumpHeight;
				_status = STATUS::JUMP_AIR;
				--_airJumpCnt;
				_playingEffect = PlayEffekseer3DEffect(_doublejump);
			}
			if (_land == true) {//�n�ʂȂ�W�����v����
				_g = _jumpHeight;
				_vPos.y += _g;
				_status = STATUS::JUMP;
				_input = false;
				_land = false;
			}
		}
		
		//������
		if (_vPos.y < oldvPos.y && _status != STATUS::JUMP_AIR && _status != STATUS::JUMP_WALL) {//�W�����v���[�V�����̂ݗ������ł��D�悵�čĐ�
			_status = STATUS::FALL;
		}

		//���n���Ă�Ȃ�
		if (_land == true) {
			
			if (VSize(_v) > 0.f) {		
				//����
				_status = STATUS::WALK;
				if (oldStatus != STATUS::SLIDHING) {
					_spd = _walkSpd;
				}

				//�_�b�V��
				if (key & PAD_INPUT_6 && oldStatus != STATUS::SLIDHING && oldStatus != STATUS::SQUAT
					&& oldStatus != STATUS::SQUAT_WALK) {
					_spd = _dashSpd;
					_status = STATUS::DASH;
				}

				//���Ⴊ�݈ړ�
				if (key & PAD_INPUT_3) {
					//���������Ɉ�莞�ԑ����Ă�����X���C�f�B���O
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
				//���Ⴊ��
				if (key & PAD_INPUT_3) {
					_status = STATUS::SQUAT;
					_height = _squatHeight;
				}
				else {
					_height = _standardHeight;
				}
				//���n
				if (oldStatus == STATUS::FALL || oldStatus == STATUS::LANDING) {
					_status = STATUS::LANDING;
				}
			}
		}

		length = 0.f;
		if (VSize(_v) > 0.f) { 
			length = _spd;
			//�_�b�V�����Ă�����_�b�V���̍ő呬�x���グ��
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

		//�L�����̈ړ�����ʂ��v�Z
		rad = atan2(_v.z, _v.x);
		_v.x = cos(Math::ToRadians(-90) + rad) * length;
		_v.z = sin(Math::ToRadians(-90) + rad) * length;

	};

	// �ړ��O�̈ʒu��ۑ�
	oldv = _v; 
	oldvPos = _vPos;

	// �R���W��������ň��������������ɁAescapeTbl[]���Ɋp�x��ς��ĉ�������݂�
	float escapeTbl[] = {
		0, -10, 10, -20, 20, -30, 30, -40, 40, -50, 50, -60, 60, -70, 70, -80, 80,
	};
	for (int i = 0; i < sizeof(escapeTbl) / sizeof(escapeTbl[0]); i++) {
		// escapeTbl[i]�̕������ړ���v��]
		float escape_rad = DEG2RAD(escapeTbl[i]);
		if (_hit == false) {
			_v.x = cos(rad +Math::ToRadians(-90)+ escape_rad) * length;
			_v.z = sin(rad + Math::ToRadians(-90) + escape_rad) * length;
		}
		// v�̕��ړ�
		_vPos = VAdd(_vPos, _v);
		// �ړ�������ŃR���W��������
		
		MV1_COLL_RESULT_POLY hitPoly;
		bool land = false;//���̒��n���

		// ��l���̍��ʒu���牺�����ւ̒���
		VECTOR LineStart = VAdd(_vPos, VGet(0, _colSubY, 0));
		VECTOR LineEnd = VAdd(_vPos, VGet(0, -_colSubY, 0));

		//��l���̑�������g������ɐL�΂�����
		VECTOR UpLineStart = VAdd(_vPos, VGet(0, _height, 0));
		

		
		_land = land;
		//�n��
		if (_land == false) { 
			hitPoly = MV1CollCheck_Line(_modegame.GetStageFloorCollision(), 0,
				LineStart, LineEnd);
			_land = HitCheckFloor(hitPoly, oldvPos);
		}
		break;
	}

	// �ړ��ʂ����̂܂܃L�����̌����ɂ���
	if (VSize(_v) > 0.f) {// �ړ����Ă��Ȃ����͖������邽��
		_vDir = _v;
	}
	if (_nohittime != 0) {
		_vDir = VMulti(_vDir, -1);
	}


	//�L�����̍�����O�����ɐL�΂������œ����蔻��
	//����Ɏg�������̎n�܂�ƏI���
	VECTOR a = VGet(_vPos.x, _vPos.y + _colSubY, _vPos.z);
	VECTOR b = VAdd(a, VMulti(_vDir, _colSubSide));
	//���ɂ��L�΂�
	a = VAdd(a, VMulti(_vDir, -_colSubSide));
	bool stick = false;
	if (_modegame.GetDebugUseColloion()) {

		for (auto e : _modegame.GetStageWallCollision()) {

			//�ǂꂩ�̕ǂɓ������Ă����画����ȗ�
			if (stick) {
				break;
			}
			stick = HitCheckWall(a, b, oldvPos, oldv, e, stick);
		}
	}
	
	_obb.SetPos_W(MV1GetFramePosition(_handle, 8));
	if (Getnohittime() == 0) {
		for (auto e : _modegame._bishop.GetObbs()) {
			if (_obb.ColOBBs(_obb, e)) {
				//Damage(3, 1);
			}
		}
	}
		
	
	
	_stick = stick;

	//�����蔻��J�v�Z���̕ό`
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
	//�ҋ@���[�V�����̐ݒ�
	if (_standbyInterval == 200) {
		_standbytype = GetRand(3);
		waitmotion = true;
		_standbyInterval = 0;
	}

	// �X�e�[�^�X���ς���Ă��Ȃ����H
	if (oldStatus != _status || waitmotion) {
		_standbyInterval = 0;
		AnimationSet();
	}
	else if (_status == STATUS::WAIT) {
		_standbyInterval++;
	}

	// �A�j���[�V�������ԏ���
	for (auto iteAnim = _vAnim.begin(); iteAnim != _vAnim.end(); ) {
		if ((*iteAnim)->_close_time == 0.f) {
			// �Đ����Ԃ�i�߂�
			(*iteAnim)->_play_time += (*iteAnim)->_anim_spd;
			// �Đ����Ԃ������_���ɗh�炪����
			switch (_status) {
			case Player::STATUS::WAIT:
				(*iteAnim)->_play_time += (float)(rand() % 10) / 100.f;	// 0.00 �` 0.09 �̗h�炬�B�ώZ����Ƃ��ꂪ�N����
				break;
			}
			// �Đ����Ԃ��A�j���[�V�����̑��Đ����ԂɒB������Đ����Ԃ��O�ɖ߂�
			if ((*iteAnim)->_play_time >= (*iteAnim)->_total_time) {
				(*iteAnim)->_play_time = 0.0f;

				//���胂�[�V�����͈�x�Đ�������X�e�[�^�X��ς���
				//���n���[�V������͑ҋ@��
				if (_status == STATUS::LANDING) {
					_status = STATUS::WAIT;
					AnimationSet();
				}
				//��i�W�����v���[�V������͗�����
				if (_status == STATUS::JUMP_AIR || _status == STATUS::JUMP_WALL) {
					_status = STATUS::FALL;
					AnimationSet();
				}
				//�ҋ@���[�V������͒ʏ�̑ҋ@��
				if (_standbytype != -1) {
					_standbytype = -1;
					AnimationSet();
				}
			}
		}
		else {
			// �����Ԃ����炷
			(*iteAnim)->_close_time -= (*iteAnim)->_anim_spd;

			// �����Ԃ��I��������H
			if ((*iteAnim)->_close_time <= 0.f) {
				// �A�j���[�V�������A�^�b�`����Ă�����A�f�^�b�`����
				MV1DetachAnim(_handle, (*iteAnim)->_attach_index);
				// ���̃A�j���[�V�������폜
				delete (*iteAnim);
				iteAnim = _vAnim.erase(iteAnim);
				continue;
			}
			// �u�����h����ύX����
			MV1SetAttachAnimBlendRate(_handle, (*iteAnim)->_attach_index, (*iteAnim)->_close_time / (*iteAnim)->_close_total_time);
		}
		++iteAnim;
	}

	//���������ɑ����Ă�������Z
	if (_status != STATUS::SLIDHING) {
		if (VSize(VSub(oldv, _v)) == 0 && _status == STATUS::DASH) {
			++_dashTime;
		}
		else {
			_dashTime = 0;
		}
	}
	// �Đ����̃G�t�F�N�g���ړ�����B
	SetPosPlayingEffekseer3DEffect(_playingEffect, _vPos.x, _vPos.y, _vPos.z);
}

void Player::Damage(int knockbacktime, int damage) {
	
	_nohittime = knockbacktime;
	//�����_���[�W���󂯂��疳�G���Ԃ𖳌��ɂ��Ă���
	if (_life == damage) {
		_nohittime = 999999;
	}
	_life -= damage;

}

bool Player::HitCheckFloor(MV1_COLL_RESULT_POLY hitPoly, VECTOR oldvPos) {

	if (hitPoly.HitFlag &&
		_status != Player::STATUS::JUMP) {
		// ��������
		// ��������Y�ʒu���L�������W�ɂ���

		// �L�������㉺�Ɉړ������ʂ����A�ړ��ʂ��C��
		_v.y += _vPos.y - oldvPos.y;
		if (_vPos.y < hitPoly.HitPosition.y) {
			_vPos.y = hitPoly.HitPosition.y + 1.0f;
		}
		// ���[�vi���甲����
		return true;
	}

	return false;
};

bool Player::HitCheckWall(VECTOR a, VECTOR b, VECTOR oldvPos, VECTOR oldv, int e, bool stick) {


	auto n = MV1GetPosition(e);
	// ��l���̍��ʒu����O�����ւ̒���
	MV1_COLL_RESULT_POLY hitPolySide;
	hitPolySide = MV1CollCheck_Line(e, 0,
		a, b);

	//�R���W�����Ɛ����������������A���������ꏊ�ƃv���C���[�̋����Ŕ����ς���
	auto c = VSize(VSub(_vPos, hitPolySide.HitPosition));

	if (hitPolySide.HitFlag) {
		if (c < 90) {
			//�ǂɐG��Ă���
			stick = true;
		}
		else {
			stick = false;
		}

		if (c < 45) {
			//�ǂ𒴂��悤�Ƃ��Ă���
			//������-20�͕ǂ��������x�ɂ���ĕς��
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

	// �A�^�b�`����Ă���A�j���[�V�����ɁAclose���Ԃ�݂���
	for (auto iteAnim = _vAnim.begin(); iteAnim != _vAnim.end(); ++iteAnim) {
		if ((*iteAnim)->_close_time == 0.f) {
			(*iteAnim)->_close_time = 2.f;		// �u�����h����t���[����
			(*iteAnim)->_close_total_time = (*iteAnim)->_close_time;
		}
	}
	// �V�����A�j���[�V������ǉ�
	Player::Animation* anim = new Player::Animation();
	// �X�e�[�^�X�ɍ��킹�ăA�j���[�V�����̃A�^�b�`
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

	// �A�^�b�`�����A�j���[�V�����̑��Đ����Ԃ��擾����
	anim->_total_time = MV1GetAttachAnimTotalTime(_handle, anim->_attach_index);
	// �Đ����Ԃ�������
	anim->_play_time = 0.0f;
	anim->_close_time = 0.0f;
	// �Đ��J�n���Ԃ������_���ɂ��炷
	switch (_status) {
	case Player::STATUS::WAIT:
		anim->_play_time += rand() % 30;	// 30�͓K��
		break;
	}
	// �A�j���[�V�����ǉ�
	_vAnim.push_back(anim);
}

void Player::Render() {


	for (auto iteAnim = _vAnim.begin(); iteAnim != _vAnim.end(); ++iteAnim) {
		MV1SetAttachAnimTime(_handle, (*iteAnim)->_attach_index, (*iteAnim)->_play_time);
	}

	// �ʒu
	MV1SetPosition(_handle, _vPos);
	// ��������Y����]���Z�o
	VECTOR vRot = { 0,0,0 };
	vRot.y = atan2(_vDir.x * -1, _vDir.z * -1);		// ���f�����W���łǂ���������Ă��邩�Ŏ����ς��(�����-z�������Ă���ꍇ)
	MV1SetRotationXYZ(_handle, vRot);

	// �`��
	MV1DrawModel(_handle);
	auto color = GetColor(rand() % 255, rand() % 255, rand() % 255);

	if (_modegame.GetDebugViewColloion()) {
		//DrawCapsule3D(_senterpos, _capsulepos_end, _r, 16, color, color, FALSE);
	}

	//�ǔ���p�̐���
	auto a = VGet(_vPos.x, _vPos.y + _colSubY, _vPos.z);
	auto b = VAdd(a, VMulti(_vDir, _colSubSide));

	auto Matrix = MGetRotY(Math::ToRadians(90));
	b = VTransformSR(b, Matrix);

	_obb.Render();
}

void Player::StopEffect() {
	StopEffekseer3DEffect(_playingEffectDamage);
	StopEffekseer3DEffect(_playingEffect);
}




