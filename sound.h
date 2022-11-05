//---------------------------------------------------------
//
//�T�E���h
//Author::TAKANO
//
//---------------------------------------------------------
#ifndef _SOUND_H_
#define _SOUND_H_

//--------------------------------------
//�C���N���[�h�t�@�C��
//--------------------------------------
#include <Windows.h>

//--------------------------------------
//�N���X��`
//�T�E���h�֘A
//--------------------------------------
class CSound
{
public:

	CSound();										// �R���X�g���N�^
	~CSound();										// �f�X�g���N�^
	HRESULT Init();									// ������
	void Uninit();									// �I��
	void volUp(float Up);							// �v���O������ō�p����p�̉��ʂ̏グ�ϐ�
	void volDown(float Down);						// �v���O�����ō�p����p�̉��ʂ̉����ϐ�
	void PitchUp(float rateUp);						// �v���O������ō�p���鉹�K�グ�p�̕ϐ�
	void PitchDown(float rateDown);					// �v���O������ō�p���鉹�K�����p�̕ϐ�
	void Setvol(float vol) { fvol = vol; }			// ���ʏグ
	void SetPitch(float rate) { frate = rate; }		// �s�b�`���グ���K���グ��

	float Getvol() { return fvol; }
	float Getrate() { return frate; }
	IXAudio2SourceVoice* GetSourceVoice() { return m_apSourceVoice; }

private:
	IXAudio2SourceVoice* m_apSourceVoice;	// �\�[�X�{�C�X
	BYTE* m_apDataAudio;					// �I�[�f�B�I�f�[�^
	DWORD m_aSizeAudio;						// �I�[�f�B�I�f�[�^�T�C�Y
	float fvol;
	float frate;
};

#endif // !_SOUND_H_
