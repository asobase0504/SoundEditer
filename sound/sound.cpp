//-----------------------------------------------------------------------------
//
//�T�E���h
//Author::TAKANO
//
//-----------------------------------------------------------------------------

//--------------------------------------
//�C���N���[�h�t�@�C��
//--------------------------------------
#include "sound.h"
#include "../application.h"
#include "soundmanager.h"
#include <assert.h>

//--------------------------------------
//�萔��`
//--------------------------------------
const float VOLUME = 11.0f;
const float PITCH = 11.0f;
const float PROVISION_ZERO = 0.0f;

//--------------------------------------
//�R���X�g���N�^
//--------------------------------------
CSound::CSound()
{
	m_volume = 0.0f;
	m_pitch = 0.0f;
}

//--------------------------------------
//�f�X�g���N�^
//--------------------------------------
CSound::~CSound()
{
}

//--------------------------------------
//������
//--------------------------------------
HRESULT CSound::Init()
{
	m_volume = 0.0f;
	m_pitch = 1.0f;

	return S_OK;
}

void CSound::Update()
{
	XAUDIO2_VOICE_STATE xa2state;
	m_apSourceVoice->GetState(&xa2state);
	if (xa2state.BuffersQueued == 0)
	{
		Uninit();
	}
}

//--------------------------------------
//�I��
//--------------------------------------
void CSound::Uninit()
{
	if (m_apSourceVoice != nullptr)
	{
		// �ꎞ��~
		m_apSourceVoice->Stop(0);

		// �\�[�X�{�C�X�̔j��
		m_apSourceVoice->DestroyVoice();
		m_apSourceVoice = nullptr;

	}

	if (m_apDataAudio != nullptr)
	{
		// �I�[�f�B�I�f�[�^�̊J��
		free(m_apDataAudio);
		m_apDataAudio = nullptr;
	}
}

HRESULT CSound::Play()
{
	// �Đ�
	m_apSourceVoice->Start(0);

	return S_OK;
}

void CSound::Stop()
{
	m_apSourceVoice->Stop(0);
}

void CSound::SetVolume(const float inVolume)
{
	m_volume = inVolume;
	m_apSourceVoice->SetVolume(inVolume);
}

void CSound::SetPitch(const float inPitch)
{
	m_pitch = inPitch;
	m_apSourceVoice->SetFrequencyRatio(inPitch);
}
