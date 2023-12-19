#pragma once
#include <string>
#include "ModeMenu.h"
#include <memory>
#include"ObjectBase.h"
#include"Player.h"
#include <vector> 
#include <unordered_map>

#include"Bishop.h"
#include"King.h"
#include"CharacterBase.h"

// �v�Z�p�}�N��
#define	PI	(3.1415926535897932386f)
#define	DEG2RAD(x)			( ((x) / 180.0f ) * PI )
#define	RAD2DEG(x)			( ((x) * 180.0f ) / PI )

// �J����
class Camera {
public:
	VECTOR	_vPos;					// �ʒu
	VECTOR	_vTarget;				// �J�������ǂ��^�[�Q�b�g���W
	float	_clipNear, _clipFar;	// �N���b�v
	VECTOR	_vPosDifference;        //�^�[�Q�b�g�ƃJ�����̋���
	float  _angleX;  //�����p�x
	float  _angleY;  //�����p�x

	float CamV = 0;
	float CamH = 0;

	//�J������]�Ɏg���ɍ��W�̌v�Z
	void PolarCoords(float angle, float length, std::pair<float, float> centerPos,
		float &posX, float &posY) {
		//�ɍ��W�ŉ~�ł̈ʒu�����߂�
		float m_Length = length;	// ���a�̒���
		float m_CenterX = centerPos.first;	// ���S���WX
		float m_CenterY = centerPos.second;	// ���S���WY
		// ���S���W�Ɋp�x�ƒ������g�p�����~�̈ʒu�����Z����
		// �x���@�̊p�x���ʓx�@�ɕϊ�
		float radius = Math::ToRadians(angle);

		// �O�p�֐����g�p���A�~�̈ʒu������o���B
		float add_x = cos(radius) * m_Length;
		float add_y = sin(radius) * m_Length;

		// ���ʂłł��ʒu�𒆐S�ʒu�ɉ��Z�������W��Ԃ�
		posX = m_CenterX + add_x;
		posY = m_CenterY + add_y;
	}

	void CameraRotation();
};
// ���[�h
class ModeGame : public ModeBase
{
	typedef ModeBase base;
public:

	std::shared_ptr<CharacterBase> _player;
	std::shared_ptr<CharacterBase> _enemy;

	bool _freeCamMode = false;

	ModeGame(int stn,int conte);
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

	bool GetDebugViewColloion() { return _bViewCollision; }
	void SetDebugViewColloion(bool b) { _bViewCollision = b; }
	bool GetDebugUseColloion() { return _bUseCollision; }
	void SetDebugUseColloion(bool b) { _bUseCollision = b; }
	bool GetDebugViewCameraInfo() { return _bViewCameraInfo; }
	void SetDebugViewCameraInfo(bool b) { _bViewCameraInfo = b; }

	bool GetDebugCup() { return _bViewCollision; }

	int GetStageFloorCollision() { return _stageCollisionFloor; };
	std::vector<int> GetStageWallCollision() { return _stageWallCollisions; }//�f�o�b�O

	//���[�U�[�̏��
	struct LaserData {
		std::vector<VECTOR> _vPoses;  //���[�U�[�̎n�܂�ƏI���̍��W
		int _type;  //���[�U�[�̎��
		std::vector<int> _trapNums; //�N��������g���b�v�̔ԍ�
		int _form;
		int _stageNum;
		bool _move;//������
		std::vector<VECTOR> _movedVPoses;
		bool _switch;//�X�C�b�`���邩
		std::vector<VECTOR> _switchVPoses;
		std::vector<VECTOR> _switchMovedVPoses;
		//�~�`�p
		float _delay;     //�X�e�[�W�X�^�[�g����N������܂ł̎���
		float _spd;//�g��̑���

	};
	std::vector<LaserData> _laserData;

	//�g���b�v�̏��
	struct TrapData {
		VECTOR _vPos;
		int _type;
		int _stageNum;
		VECTOR _movedVPos;
		std::vector<int> _switchLaserNum;
		int _spd;
	};
	std::vector<TrapData> _trapData;

	//�v���C���[�̏��
	Player::PlayerData _playerData;

protected:

	// �J����
	Camera	_cam;
	//Enemy  _enemy;
	//std::vector<base> objects;
	// 3D���f���`��p
	int _handle;

	std::vector<std::shared_ptr<ObjectBase>> objects;

	bool _input;
	float _charaSpeed;
	int _score;

	//json�ǂݍ���
	//json�t�@�C����ǂݍ���Ŕz��ɏ����܂Ƃ߂�
	void LoadJsonfile(); 

	int _attach_index;
	float _total_time;
	float _play_time;
	VECTOR _vPos;	// �ʒu
	//int _g;//�d��

	VECTOR _vDir;	// ����
	
	// �f�o�b�O�p
	bool	_bViewCollision;
	bool	_bUseCollision;
	bool	_bViewCameraInfo;

	// �}�b�v�p
	int _handleMap;
	int _handleSkySphere;
	int _frameMapCollision;
	int _wallGraphHandle_front;//�ǂ̊G
	int _wallGraphHandle_side;//�ǂ̊G

	//���̓����蔻��𖳗�������
	std::vector<int> _stageWallCollisions;//�f�o�b�O
	int _stColl;//�f�o�b�O�A�X�e�[�W�̃R���W����
	int _stColl2;//�f�o�b�O�A�X�e�[�W�̃R���W����
	int _stageCollisionFloor;

	//����
	int _light;
	// �V���h�E�}�b�v�p
	int _handleShadowMap;
	int _handleShadowMap_2;

	//UI�N���X
	std::vector<int> _hp;
	std::unordered_map<int, int> _displayTimes;
	std::unordered_map<int, int> _remain;

	int _screenBright = 0;//��ʂ̖��邳

	int _conte;//�R���e�B�j���[��
}; 


