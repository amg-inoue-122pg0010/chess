#pragma once
#include "DxLib.h"
#include <functional>
#include "Math.h"
struct CharacterBody{

	VECTOR _headTop;
	VECTOR _headUnder;

	VECTOR _bodyTop;//obb�̏�ʂ̒��S(��[)�̍��W
	VECTOR _bodyUnder;//obb�̒�ʂ̒��S(���[)�̍��W
	int _bodyTopFrameIndex;//obb��Y�킹�郂�f���̃t���[���ԍ�
	int _bodyUnderFrameIndex;
	VECTOR _bodyObbSize;//obb�̑傫��
	VECTOR _bodyTilt;//�̂̌X��

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
	VECTOR m_Pos;              // �ʒu
	VECTOR m_Direct[3];        //�@�ӂ̌����ƒ����Ax,y,z
	VECTOR m_NormaDirect[3];   // �����x�N�g��,X,Y,Z
	float m_fLength[3];        // �e�������̒���
	bool _hit;                 //OBB���m���Ԃ����Ă��邩
	int _existTime;            //obb��������܂ł̎���
	bool _del;                 //����obb���폜���邩�ǂ���
	int  _effectiveTime;       //����obb���L�����ǂ����A�L���ɂ���܂ł̎���
	std::function<VECTOR(VECTOR, VECTOR, float)> _trajectory;   //�������U���̋O��

	
public:
	
	void CreateObb(VECTOR vpos, VECTOR x, VECTOR y, VECTOR z);//�󂯎�����x�N�g����OBB���쐬
	void CreateObb(VECTOR vpos, VECTOR size);//���ꂼ��̃x�N�g�������A���A�����ɕ��s��OBB���쐬����ꍇ

	void RotateObb(VECTOR rotate);//���W�A���Ŋp�x���󂯎��AOBB��x��y��z�̏��ɉ�]������
	void RotateObb(MATRIX matrix);//�󂯎������]�s��ʂ�ɉ�

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

	VECTOR GetDirect(int elem) { return m_NormaDirect[elem]; };    // �w�莲�ԍ��̕����x�N�g�����擾
	float GetLen_W(int elem) { return m_fLength[elem]; };          // �w�莲�����̒������擾
	VECTOR GetPos_W() { return m_Pos; };             // �ʒu���擾
	void SetPos_W(VECTOR pos) { m_Pos = pos; };      //�ʒu���w��
	float LenSegOnSeparateAxis(VECTOR Sep, VECTOR e1, VECTOR e2, VECTOR* e3 =0);
	bool ColOBBs(OBB& obb1, OBB& obb2);

};



