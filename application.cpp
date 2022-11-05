//--------------------------------------------------------------------------------------
//
// �}�l�[�W���[�A�v���P�[�V����
// Author::TAKANO
//
//--------------------------------------------------------------------------------------

//--------------------------------------
//�C���N���[�h�t�@�C��
//--------------------------------------
#include "renderer.h"
#include "input.h"
#include "application.h"
#include "soundmanager.h"
#include <assert.h>

//--------------------------------------
//�O���[�o���ϐ�
//--------------------------------------
CRenderer *CApplication::m_aRenderer;
CInput* CApplication::m_pInput;
CSoundManager *CApplication::m_pSoundManager;

//--------------------------------------
//�R���X�g���N�^
//--------------------------------------
CApplication::CApplication()
{
	
}

//--------------------------------------
//�f�X�g���N�^
//--------------------------------------
CApplication::~CApplication()
{
}

//--------------------------------------
//������
//--------------------------------------
HRESULT CApplication::Init(HINSTANCE hInctance, HWND hWnd, bool bWindow)
{
	//�|�C���^�ɃS�~�������Ă��Ȃ����`�F�b�N
	ReleseDustCheck();
	
	//���I�m��
	m_aRenderer = new CRenderer;
	m_pInput = new CInput;
	m_pSoundManager = new CSoundManager;

	//������
	m_aRenderer->Init(hWnd, bWindow);

	m_pInput->Initkeyboard(hInctance,hWnd);

	m_pSoundManager->Init();
	m_Switch = 0;	//�Z���N�g���Ɏg�p����ϐ��̏�����

	return S_OK;
}

//--------------------------------------
//�I��
//--------------------------------------
void CApplication::Uninit()
{
	//�S�~�������Ă��Ȃ����m�F
	ReleseDustCheck();
}

//--------------------------------------
//�X�V
//--------------------------------------
void CApplication::Update()
{
	if (m_aRenderer != nullptr)
	{//�����_���[�̃|�C���^�ɒl�������Ă����Ƃ�
		m_aRenderer->Update();						//�����_���[�̍X�V
	}

	if (m_pInput != nullptr)
	{//�L�[�{�[�h�̃|�C���^�ɒl�������Ă����Ƃ�
		m_pInput->Updatekeyboard();			//�L�[�{�[�h�̍X�V
	}

	if (m_pSoundManager != nullptr)
	{
		m_pSoundManager->Update();
	}

	ChangeSound();
	SoundParameter();
}

//--------------------------------------
//�`��
//--------------------------------------
void CApplication::Draw()
{
	if (m_aRenderer != nullptr)
	{
		//�����_���[�̕`��
		m_aRenderer->Draw();
	}
}

//--------------------------------------
//�I�������ƃ_�X�g�`�F�b�N�p�֐�
//--------------------------------------
void CApplication::ReleseDustCheck()
{
	if (m_aRenderer != nullptr)
	{//�����_���[�̃|�C���^�ɉ�������̒l�������Ă�����
		m_aRenderer->Uninit();					//�I������
		delete m_aRenderer;						//�����_���[�|�C���^�����
		m_aRenderer = nullptr;					//�k������
	}

	if (m_pInput != nullptr)
	{//�L�[�{�[�h�̃|�C���^�ɉ�������̒l�������Ă�����
		m_pInput->Uninitkeyboard();		//�I������
		delete m_pInput;				//�L�[�{�[�h�̃|�C���^�����
		m_pInput = nullptr;				//�k������
	}

	if (m_pSoundManager != nullptr)
	{
		m_pSoundManager->Uninit();
	}
}

//---------------------------------------
//�����_���[���擾
//---------------------------------------
CRenderer * CApplication::GetRenderer()
{
	return m_aRenderer;
}

//---------------------------------------
//�R���g���[���̏��擾
//---------------------------------------
CInput * CApplication::GetInput()
{
	return m_pInput;
}

//---------------------------------------
//���[�h���擾
//---------------------------------------
CSoundManager * CApplication::GetSoundManager()
{
	return m_pSoundManager;
}

void CApplication::SoundVolume()
{
	//if (CApplication::GetInput()->GetkeyboardPress(DIK_UP))
	//{
	//	m_pSound[type]->volUp(UP_DOWN_TRACK);
	//	m_pSoundManager->SetVolume(m_type, m_pSound[type]->Getvol());
	//}

	//if (CApplication::GetInput()->GetkeyboardPress(DIK_DOWN))
	//{
	//	m_pSound[type]->volDown(UP_DOWN_TRACK);
	//	m_pSoundManager->SetVolume("", m_pSound[type]->Getvol());
	//}

	//if (CApplication::GetInput()->GetkeyboardTrigger(DIK_LEFT))
	//{
	//	m_Fadetype = TYPE_IN;
	//	m_Fade_Ok = false;
	//}

	//if (CApplication::GetInput()->GetkeyboardTrigger(DIK_RIGHT))
	//{
	//	m_Fadetype = TYPE_OUT;
	//	m_Fade_Ok = false;
	//}

	//if (CApplication::GetInput()->GetkeyboardPress(DIK_O))
	//{
	//	m_pSound[type]->PitchUp(TRACK_PITH);
	//	m_pSoundManager->SetRate(m_type, m_pSound[type]->Getrate());
	//}

	//if (CApplication::GetInput()->GetkeyboardPress(DIK_P))
	//{
	//	m_pSound[type]->PitchDown(TRACK_PITH);
	//	m_pSoundManager->SetRate(m_type, m_pSound[type]->Getrate());
	//}

}

void CApplication::ShiftRate()
{
}

void CApplication::SoundParameter()
{
	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_N))
	{
		m_pSoundManager->Save();
	}

	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_M))
	{
		m_pSoundManager->Load();
	}
}

void CApplication::ChangeSound()
{
	if (CApplication::GetInput()->GetkeyboardTrigger(DIK_L))
	{ //�Ȃ̓���ւ�

		m_Switch = (m_Switch + 1) % 12;

		Select();

		if (m_Switch >= 11)
		{
			m_Switch = 0;
		}
	}
}

void CApplication::Select()
{
	std::string tag;

	switch (m_Switch)
	{
	case 0:

		/*�ύX�Ȃ�*/

		break;

	case 1:
		tag = "BATTLEBAN";
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 2:
		tag = "LAST_MEETING";
		m_pSoundManager->Stop();
		m_pSoundManager->Play("LAST_MEETING");
		break;

	case 3:
		tag = "RESULT_BGM";
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 4:
		tag = "VIRTUAL";
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 5:
		tag = "BATTLEMEETING_VER_2";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);

		break;

	case 6:
		tag = "NO_NAME";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 7:
		tag = "CHALLENGE_TO_TOMORROW";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 8:
		tag = "BATTLEMEETING";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 9:
		tag = "HEART";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 10:
		tag = "DRUM_VOICE";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	case 11:
		tag = "RANKING";
		m_pSoundManager->SetFadeIn(tag);
		m_pSoundManager->Stop();
		m_pSoundManager->Play(tag);
		break;

	default:
		assert(false);
		break;
	}
}
