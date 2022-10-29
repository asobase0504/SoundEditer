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
#include"d3dx9.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "xaudio2.h"
#include <map>

//--------------------------------------
//�N���X��`
//�T�E���h�֘A
//--------------------------------------
class CSound
{
public:
	struct PARAM
	{//�T�E���h�p�����[�^�[�̍\����
		char *m_pFilename;	// �t�@�C����
		int m_nCntLoop;		// ���[�v�J�E���g
	};

	enum SOUND_TYPE
	{
		SOUND_NONE = -1,					// �g�p����Ă��Ȃ����
		SOUND_BGM_BATTLEBAN,				// BGM 0
		SOUND_BGM_LAST_MEETING,				// BGM 1
		SOUND_BGM_RESULT_BGM,				// BGM 2
		SOUND_BGM_VIRTUAL,					// BGM 3
		SOUND_BGM_BATTLEMEETING_VER_2,		// BGM 4
		SOUND_BGM_NO_NAME,					// BGM 5
		SOUND_BGM_CHALLENGE_TO_TOMORROW,	// BGM 6
		SOUND_BGM_BATTLEMEETING,			// BGM 7
		SOUND_BGM_HEART,					// BGM 8
		SOUND_BGM_DRUM_VOICE,				// BGM 9
		SOUND_BGM_RANKING,					// BGM 10
		SOUND_SE_EXPOSION,					// SE1
		SOUND_SE_HIT,						// SE2
		SOUND_SE_SHOT,						// SE3
		SOUND_MAX,
	};

	CSound();											// �R���X�g���N�^
	~CSound();											// �f�X�g���N�^
	HRESULT Init(HWND hWnd);							// ������
	void Uninit();										// �I��
	HRESULT Play(std::string inName);					// �Đ�
	HRESULT Play(SOUND_TYPE type);						// �Đ�
	void SetVolume(SOUND_TYPE type, float fVolume);		// ����
	void SetRate(SOUND_TYPE type, float rate);			// �s�b�`����
	void Stop(SOUND_TYPE type);							// �ꕔ��~
	void Stop();										// �S��~

private:
	HRESULT CheckChunk(HANDLE hFile,					//���(�`�����N)�̊m�F
					   DWORD format, 
					   DWORD *pChunkSize, 
					   DWORD *pChunkDataPosition);


	HRESULT ReadChunkData(HANDLE hFile,					//���(�`�����N)�̓ǂݍ���
						  void *pBuffer, 
						  DWORD dwBuffersize, 
						  DWORD dwBufferoffset);

private:

private:
	IXAudio2 *m_pXAudio2;								// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	IXAudio2MasteringVoice *m_pMasteringVoice;			// �}�X�^�[�{�C�X
	IXAudio2SourceVoice *m_apSourceVoice[SOUND_MAX];	// �\�[�X�{�C�X
	BYTE *m_apDataAudio[SOUND_MAX];						// �I�[�f�B�I�f�[�^
	DWORD m_aSizeAudio[SOUND_MAX];						// �I�[�f�B�I�f�[�^�T�C�Y
	int m_nCountFade;									// �t�F�[�h�p�J�E���^�[
};

#endif // !_SOUND_H_
