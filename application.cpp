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

	m_pSoundManager->Init(hWnd);

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
