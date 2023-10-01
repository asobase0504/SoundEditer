//---------------------------------------------------------
//
//���y�}�l�[�W���[
//Author::TAKANO
//
//---------------------------------------------------------
#ifndef _SOUND_MANAGER_H
#define _SOUND_MANAGER_H

//----------------------------------------
//�C���N���[�h�t�@�C��
//----------------------------------------
#include "sound.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "xaudio2.h"
#include <unordered_map>

//----------------------------------------
//�N���X��`
//�w�i(�I�u�W�F�N�g����h��)
//----------------------------------------
class CSoundManager
{
private:
	CSoundManager();
	static CSoundManager* m_instance;
public:
	static CSoundManager* GetInstance();
	~CSoundManager();
public:
	struct SSoundData
	{
		XAUDIO2_BUFFER buffer;
		WAVEFORMATEX format;
	};

	HRESULT Init();		// ������
	void Uninit();		// �I��

	CSound* Play(std::string tag);	// �Đ�
	void AllStop();	// �S�Ă̒�~

/* �����o�[�֐� */
private:

	// �Ǎ���
	void LoadPath();	// �p�X���[�h
	HRESULT LoadSound(std::string tag,std::string path,bool loop);

	// ���(�`�����N)�̊m�F
	HRESULT CheckChunk(HANDLE hFile,
		DWORD format,
		DWORD *pChunkSize,
		DWORD *pChunkDataPosition);


	// ���(�`�����N)�̓ǂݍ���
	HRESULT ReadChunkData(HANDLE hFile,
		void *pBuffer,
		DWORD dwBuffersize,
		DWORD dwBufferoffset);

/* �����o�[�ϐ� */
private:
	IXAudio2 *m_pXAudio2;	// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	IXAudio2MasteringVoice *m_pMasteringVoice;	// �}�X�^�[�{�C�X

	// Sound
	std::unordered_map<std::string, SSoundData> m_soundData;
	std::vector<CSound*> m_playSound;
};
#endif // !_SOUND_MANAGER_H