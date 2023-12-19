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

	//�U���󂯂����
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

		//�A�j���[�V������ݒ�
	// �X�e�[�^�X���ς���Ă��Ȃ����H
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

	//�g���I�����obb�̃C�e���[�^�̏�������������
	//Math�̃t�@�C��������菜���āAMath�̐����AMath�ɍD���Ɋ֐���ǉ��ł���悤��

	//�̂�obb�Y���ĉ�]����悤��
	//����������
	//�L�����ǉ�King�A���f���̓ǂݍ��݂���
	//�K�v�Ȃ�EnemyProcess�̓L�������ɏ���

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
		//�A�N�V�������łȂ����
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

	//�A�j���[�V������ݒ�
	// �X�e�[�^�X���ς���Ă��Ȃ����H
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

	//�A�N�V�������łȂ��Ȃ�
	if (!_action) {
		_movementInAction = 0;
		//�C���^�[�o����������΃A�N�V����
		if (_enemyActionInterval <= 0) {
			_vDir = VNorm(VSub(_target, _vPos));
			//�^�[�Q�b�g�Ƃ̋����ōs����ς���
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

	//�A�j���[�V������ݒ�
	// �X�e�[�^�X���ς���Ă��Ȃ����H
	if (_oldStatus != _status) {
		AnimationSet(_action);
	}
	else {
		SetAnimeNoInterruption(_action);
	}

	//���̃A�N�V�����܂ł̊Ԋu�ɁA�Đ����̃��[�V�����̎��Ԃ𑫂�
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

		// v��rad����]������
		_normalVDir.x = cos(_rad + _cameraVDir) * spd;
		_normalVDir.z = sin(_rad + _cameraVDir) * spd;
	}
	else {
		_status = STATUS::IDLE;
	}

	_vPos = VAdd(_vPos, _normalVDir);

	// _normalVDir�����̂܂܃L�����̌����ɂ���
	if (VSize(_normalVDir) > 0.f) {// �ړ����Ă��Ȃ����͖������邽��
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

	// �ʒu
	MV1SetPosition(_handle, _vPos);
	// ��������Y����]���Z�o
	VECTOR vRot = { 0,0,0 };
	vRot.y = atan2(_vDir.x * -1, _vDir.z * -1);		// ���f�����W���łǂ���������Ă��邩�Ŏ����ς��(�����-z�������Ă���ꍇ)
	MV1SetRotationXYZ(_handle, vRot);

	// �`��
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
	// �A�^�b�`����Ă���A�j���[�V�����ɁAclose���Ԃ�݂���
	for (auto iteAnim = _vAnim.begin(); iteAnim != _vAnim.end(); ++iteAnim) {
		if ((*iteAnim)->_close_time == 0.f) {
			(*iteAnim)->_close_time = 10.f;		// �u�����h����t���[����
			(*iteAnim)->_close_total_time = (*iteAnim)->_close_time;
		}
	}
	// �V�����A�j���[�V������ǉ�
	CharacterBase::Animation* anim = new CharacterBase::Animation();
	// �X�e�[�^�X�ɍ��킹�ăA�j���[�V�����̃A�^�b�`
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

	// �A�^�b�`�����A�j���[�V�����̑��Đ����Ԃ��擾����
	anim->_total_time = MV1GetAttachAnimTotalTime(_handle, anim->_attach_index);
	// �Đ����Ԃ�������
	anim->_close_time = 0.0f;
	anim->_play_time = 0.0f;

	anim->_noInterruption = interruption;
	if (anim->_nohitAnime) {
		_nohittime = anim->_total_time;
	}

	// �A�j���[�V�����ǉ�
	_vAnim.push_back(anim);

}

void CharacterBase::AnimationPlay() {
	// �A�j���[�V�������ԏ���
	for (auto iteAnim = _vAnim.begin(); iteAnim != _vAnim.end(); ) {
		if ((*iteAnim)->_close_time == 0.f) {
			// �Đ����Ԃ�i�߂�
			(*iteAnim)->_play_time += (*iteAnim)->_anim_spd;
			// �Đ����Ԃ��A�j���[�V�����̑��Đ����ԂɒB������Đ����Ԃ��O�ɖ߂�
			if ((*iteAnim)->_play_time >= (*iteAnim)->_total_time) {
				(*iteAnim)->_play_time = 0.0f;
				if ((*iteAnim)->_noInterruption) {
					(*iteAnim)->_noInterruption = false;
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
