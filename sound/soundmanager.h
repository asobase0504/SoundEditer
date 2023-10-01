//*****************************************************************************
//
// �����}�l�[�W���[
// Author::TAKANO
// Author YudaKaito
//
//*****************************************************************************
#ifndef _SOUND_MANAGER_H_
#define _SOUND_MANAGER_H_

//=============================================================================
// include
//=============================================================================
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "xaudio2.h"
#include <unordered_map>

//=============================================================================
// �O���錾
//=============================================================================
class CSound;
class CSoundDistance;

//=============================================================================
// �N���X��`
//=============================================================================
class CSoundManager
{
public:
	static float MASTER_VOLUME;
	static float BGM_VOLUME;
	static float SE_VOLUME;
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
	CSoundDistance* PlayDistance(std::string tag,const D3DXVECTOR3& inPos);	// �Đ�

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
};
#endif // !_SOUND_MANAGER_H
