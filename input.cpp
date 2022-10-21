//-----------------------------------------------------------------------------------------------------------
//
//�L�[�{�[�h�A�Q�[���p�b�h���͏���
//Author::TAKANO
//
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------
//�C���N���[�h�t�@�C��
//-----------------------------------------------------------
#include "input.h"
#include "application.h"

//-----------------------------------------------------------
//�R���X�g���N�^
//-----------------------------------------------------------
CInput::CInput()
{
	m_pInput = nullptr;
	m_pDevkeyboard = nullptr;
}

//-----------------------------------------------------------
//�f�X�g���N�^
//-----------------------------------------------------------
CInput::~CInput()
{
}

//-----------------------------------------------------------
//�L�[�{�[�h�̏�����
//-----------------------------------------------------------
HRESULT CInput::Initkeyboard(HINSTANCE hInstance, HWND hWnd)
{
	//Directinput�̐���
	if (FAILED(DirectInput8Create(hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)& m_pInput,NULL)))
	{
		return E_FAIL;
	}

	//���̓f�o�C�X�̐���
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard,&m_pDevkeyboard,NULL)))
	{
		return E_FAIL;
	}

	//�f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(m_pDevkeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	//�������[�h�̐ݒ�
	if (FAILED(m_pDevkeyboard->SetCooperativeLevel(hWnd,(DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	//�L�[�{�[�h�̃A�N�Z�X�����l��
	m_pDevkeyboard->Acquire();

	return S_OK;
}

//-----------------------------------------------------------
//�Q�[���p�b�h�̏�����
//-----------------------------------------------------------
HRESULT CInput::InitJoypad(void)
{
	//�������̃N���A
	memset(&m_JoykeyState, 0, sizeof(XINPUT_STATE));

	//Xinput�̃X�e�[�g�̐ݒ�i�L�����p�j
	XInputEnable(true);

	return S_OK;
}

//-----------------------------------------------------------
//�L�[�{�[�h�̏I��
//-----------------------------------------------------------
void CInput::Uninitkeyboard(void)
{
	//���̓f�o�C�X�̔j��
	if (m_pDevkeyboard != NULL)
	{
		m_pDevkeyboard->Unacquire();
		m_pDevkeyboard->Release();
		m_pDevkeyboard = NULL;
	}

	//DirectInput�I�u�W�F�N�g�̔j��
	if (m_pInput != NULL)
	{
		m_pInput->Release();
		m_pInput = NULL;
	}
}

//-----------------------------------------------------------
//�Q�[���p�b�h�̏I��
//-----------------------------------------------------------
void CInput::UninitJoypad(void)
{
	//Xinput�̃X�e�[�g�̐ݒ�
	XInputEnable(false);
}

//-----------------------------------------------------------
//�L�[�{�[�h�̍X�V
//-----------------------------------------------------------
void CInput::Updatekeyboard(void)
{
	BYTE akeystate[NUM_KEY_MAX];				//�L�[�{�[�h�̓��͏��
	int nCntkey;

	//���̓f�o�C�X����f�[�^���擾
	if (SUCCEEDED(m_pDevkeyboard->GetDeviceState(sizeof(akeystate), &akeystate[0])))
	{
			for (nCntkey = 0; nCntkey < NUM_KEY_MAX; nCntkey++)
			{
				m_akeyStateTrigger[nCntkey] = (m_akeyState[nCntkey] & akeystate[nCntkey]) ^ akeystate[nCntkey];			//�g���K�[����						//�L�[�{�[�h�̃g���K�[�����擾
				m_akeyState[nCntkey] = akeystate[nCntkey];																//�v���X����						//�L�[�{�[�h�̃v���X�����擾
			}
	}
	else
	{
		m_pDevkeyboard->Acquire();					//�L�[�{�[�h�̃A�N�Z�X�����l��
	}
}

//-----------------------------------------------------------
//�Q�[���p�b�h�X�V
//-----------------------------------------------------------
void CInput::UpdateJoypad(void)
{
	XINPUT_STATE joykeystate;

	//�V���C�p�b�g�̏�Ԃ��擾
	if (XInputGetState(0,&joykeystate) == ERROR_SUCCESS)
	{
		m_JoykeyStateTrigger.Gamepad.wButtons = (m_JoykeyState.Gamepad.wButtons & joykeystate.Gamepad.wButtons) ^ joykeystate.Gamepad.wButtons;			//�g���K�[����	
		m_JoykeyState = joykeystate;
	}
}

//-----------------------------------------------------------
//�L�[�{�[�h�̏����擾(����)
//-----------------------------------------------------------
bool CInput::GetkeyboardPress(int nkey)
{
	return(m_akeyState[nkey] & 0x80) ? true : false;
}

//-----------------------------------------------------------
//�L�[�{�[�h�̏����擾
//-----------------------------------------------------------
bool CInput::GetkeyboardTrigger(int nkey)
{
	return(m_akeyStateTrigger[nkey] & 0x80) ? true : false;
}

//-----------------------------------------------------------
//�Q�[���p�b�h�̏����擾(����)
//-----------------------------------------------------------
bool CInput::GetJoypadPress(XINPUT_TYPE key)
{
	return (m_JoykeyState.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//-----------------------------------------------------------
//�Q�[���p�b�h�̏����擾
//-----------------------------------------------------------
bool CInput::GetJoypadTrigger(XINPUT_TYPE key)
{
	return (m_JoykeyStateTrigger.Gamepad.wButtons & (0x01 << key)) ? true : false;
}

