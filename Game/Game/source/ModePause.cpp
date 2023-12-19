#include "ModePause.h"
#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeStageSelect.h"
#include "ModeTitle.h"
#include "ApplicationGlobal.h"

ModePause::ModePause(ModeGame* modegame) 
:_modeGame(modegame) 
{
	_cgBack = ResourceServer::LoadGraph("res/InGame/pause.png");
	_animeMax = 5;
	_cgCursor = ResourceServer::LoadGraph("res/title/yajirusi.png");
	_animeCnt = 0;

	_keyConfig = ResourceServer::LoadGraph("res/Ui/config.png");
	_drawConfig = 0;
	_configAlfa = 0;
}

bool ModePause::Process() {
	base::Process();
	int trg = ApplicationMain::GetInstance()->GetTrg();

	// ���̃��[�h��艺�̃��C���[��Process()���Ă΂Ȃ�
	ModeServer::GetInstance()->SkipProcessUnderLayer();

	// ESC�L�[�Ń��j���[�����
	bool close = false;
	_animeCnt++;

		//���ڂ�I��
		if (trg & PAD_INPUT_UP) {
			_menuNum--; _animeCnt = 0;
			PlaySoundMem(gGlobal._se["Select"], DX_PLAYTYPE_BACK, true);
		}
		if (trg & PAD_INPUT_DOWN) {
			_menuNum++; _animeCnt = 0;
			PlaySoundMem(gGlobal._se["Select"], DX_PLAYTYPE_BACK, true);
		}
		_menuNum = Math::Clamp(_menuNum, 1, 4);

		//����
		if (trg & PAD_INPUT_2) {
			PlaySoundMem(gGlobal._se["Dicision"], DX_PLAYTYPE_BACK, true);
			switch (_menuNum) {
			case 1:
				//�Q�[���ɖ߂�
				close = true;				
				break;
			case 2:
				//�X�e�[�W���X�^�[�g
				_modeGame->SetDebugReStage(!_modeGame->GetDebugReStage());
				close = true;
				break;
			case 3:
				//�L�[�R���t�B�O
				_drawConfig = true;
				break;
			case 4:
				//�^�C�g��
				ModeServer::GetInstance()->Del(_modeGame);
				ModeServer::GetInstance()->Add(new ModeTitle(), 1, "title");
				close = true;
				break;
			default:
				break;
			}
		}

		if (_drawConfig) {
			_configAlfa = 255;
		}
		else {
			_configAlfa = 0;
		}

		if (_drawConfig == true) {
			if (trg & PAD_INPUT_1) {
				_drawConfig = false;
			}
		}

		// ���j���[�����
		if (close || _configAlfa == 0 && trg & PAD_INPUT_1) {
			// ���̃��[�h���폜����
			ModeServer::GetInstance()->Del(this);
		}

	return true;
}

bool ModePause::Render() {

	DrawGraph(0, 0, _cgBack, true);
	
	if (_animeCnt / 16 % 2 == 0) {
	DrawRotaGraph(730, 655 + (_menuNum * 65), 0.7, 0, _cgCursor, true, false);
	}
	DrawGraph(0, 200, _cgMenu, true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, _configAlfa);
	DrawGraph(0, 0, _keyConfig, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, _configAlfa);
	return true;
}