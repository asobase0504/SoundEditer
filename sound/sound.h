//---------------------------------------------------------
//
//�T�E���h
//Author::TAKANO
//Author::YudaKaito
//
//---------------------------------------------------------
#ifndef _SOUND_H_
#define _SOUND_H_

//--------------------------------------
//�C���N���[�h�t�@�C��
//--------------------------------------
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "xaudio2.h"

//--------------------------------------
// �N���X��`
// 
// Object�̔h���ɂ��Ė��t���[��Update��ʂ��ĉ������B
//--------------------------------------
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

	void SetPitch(const float inPitch);
	void AddPitch(const float inPitch) { SetPitch(inPitch + m_pitch); }
	float GetPitch() { return m_pitch; }

	void SetSourceVoice(IXAudio2SourceVoice* inVoice) { m_apSourceVoice = inVoice; }
	IXAudio2SourceVoice* GetSourceVoice() { return m_apSourceVoice; }

private:
	IXAudio2SourceVoice* m_apSourceVoice;	// �\�[�X�{�C�X
	BYTE* m_apDataAudio;					// �I�[�f�B�I�f�[�^
	DWORD m_aSizeAudio;						// �I�[�f�B�I�f�[�^�T�C�Y

	float m_volume;
	float m_pitch;
	int m_loop;
};

#endif // !_SOUND_H_
