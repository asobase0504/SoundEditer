//*****************************************************************************
//
//�T�E���h
//Author::TAKANO
//Author::YudaKaito
//
//*****************************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_

//=============================================================================
// include
//=============================================================================
#include <Windows.h>
#include <mfapi.h>
#include "xaudio2.h"

/*
// Object�̔h���ɂ��Ė��t���[��Update��ʂ��ĉ������B
// Sound�p��priority������ƊǗ��y����
*/

//=============================================================================
// �N���X��`
//=============================================================================
class CSound
{
public:

	CSound();		// �R���X�g���N�^
	~CSound();		// �f�X�g���N�^
	HRESULT Init();	// ������
	void Update();	// �X�V
	void Uninit();	// �I��

	// �Đ�
	HRESULT Play();

	// ��~
	void Stop();

	// ����
	void SetVolume(const float inVolume);
	void AddVolume(const float inVolume) { SetVolume(inVolume + m_volume); }
	float GetVolume() { return m_volume; }

	// �s�b�`
	void SetPitch(const float inPitch);
	void AddPitch(const float inPitch) { SetPitch(inPitch + m_pitch); }
	float GetPitch() { return m_pitch; }

	// ���f�[�^
	void SetSourceVoice(IXAudio2SourceVoice* inVoice) { m_apSourceVoice = inVoice; }

private:
	IXAudio2SourceVoice* m_apSourceVoice;	// �\�[�X�{�C�X

	float m_volume;
	float m_pitch;
	int m_loop;
};

#endif // !_SOUND_H_
