//*****************************************************************************
//
// �����}�l�[�W���[
// Author::TAKANO
// Author Yuda Kaito
//
//*****************************************************************************
//=============================================================================
// include
//=============================================================================
#include "../application.h"
#include <assert.h>
#include "soundmanager.h"
#include "sound.h"
#include "sound_distance.h"

// �t�@�C���Ǘ�
#include <fstream>
#include "../letter.h"
#include "../file.h"

//=============================================================================
// �ÓI�ϐ��̐錾
//=============================================================================
CSoundManager* CSoundManager::m_instance = nullptr;
float CSoundManager::MASTER_VOLUME = 1.0f;
float CSoundManager::BGM_VOLUME = 1.0f;
float CSoundManager::SE_VOLUME = 1.0f;

//-----------------------------------------------------------------------------
// �V���O���g���̌Ăяo��
//-----------------------------------------------------------------------------
CSoundManager* CSoundManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new CSoundManager;
		m_instance->Init();
	}

	return m_instance;
}

//-----------------------------------------------------------------------------
//�R���X�g���N�^
//-----------------------------------------------------------------------------
CSoundManager::CSoundManager()
{
	m_pXAudio2 = nullptr;				// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	m_pMasteringVoice = nullptr;		// �}�X�^�[�{�C�X
}

//-----------------------------------------------------------------------------
//�f�X�g���N�^
//-----------------------------------------------------------------------------
CSoundManager::~CSoundManager()
{
}

//-----------------------------------------------------------------------------
// ������
//-----------------------------------------------------------------------------
HRESULT CSoundManager::Init()
{
	//--------------------------------------
	// COM���C�u�����̏�����
	//--------------------------------------
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// XAudio2�I�u�W�F�N�g�̍쐬
	if (FAILED(XAudio2Create(&m_pXAudio2, 0)))
	{
		// COM���C�u�����̏I������
		CoUninitialize();

		assert(false);

		return E_FAIL;
	}
	
	// �}�X�^�[�{�C�X�̐���
	if (FAILED(m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice)))
	{
		if (m_pXAudio2)
		{
			// XAudio2�I�u�W�F�N�g�̊J��
			m_pXAudio2->Release();
			m_pXAudio2 = nullptr;
		}

		// COM���C�u�����̏I������
		CoUninitialize();

		assert(false);

		return E_FAIL;
	}

	LoadPath();		//���̓Ǎ�

	return S_OK;
}

//-----------------------------------------------------------------------------
//�I��
//-----------------------------------------------------------------------------
void CSoundManager::Uninit()
{
	// �}�X�^�[�{�C�X�̔j��
	m_pMasteringVoice->DestroyVoice();
	m_pMasteringVoice = nullptr;

	if (m_pXAudio2 != nullptr)
	{
		// XAudio2�I�u�W�F�N�g�̊J��
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
	}

	// COM���C�u�����̏I������
	CoUninitialize();
}

//-----------------------------------------------------------------------------
// �Đ�(����)
//-----------------------------------------------------------------------------
CSound* CSoundManager::Play(std::string inTag)
{
	IXAudio2SourceVoice* sourceVoice;

	HRESULT result = m_pXAudio2->CreateSourceVoice(&sourceVoice, &(m_soundData[inTag].format));

	assert(SUCCEEDED(result));

	// �I�[�f�B�I�o�b�t�@�̓o�^
	sourceVoice->SubmitSourceBuffer(&m_soundData[inTag].buffer);

	CSound* sound = new CSound;
	sound->Init();
	sound->SetSourceVoice(sourceVoice);
	sound->Play();

	return sound;
}

CSoundDistance * CSoundManager::PlayDistance(std::string inTag, const D3DXVECTOR3& inPos)
{
	IXAudio2SourceVoice* sourceVoice;

	HRESULT result = m_pXAudio2->CreateSourceVoice(&sourceVoice, &(m_soundData[inTag].format));

	assert(SUCCEEDED(result));

	// �I�[�f�B�I�o�b�t�@�̓o�^
	sourceVoice->SubmitSourceBuffer(&m_soundData[inTag].buffer);

	CSoundDistance* sound = new CSoundDistance;
	sound->Init();
	sound->SetSourceVoice(sourceVoice);
	sound->Play(inPos);

	return sound;
}

//-----------------------------------------------------------------------------
// Path�Ǎ���
//-----------------------------------------------------------------------------
void CSoundManager::LoadPath()
{
	std::ifstream ifs("Data/JSON/SoundPath.json");
	nlohmann::json Loadfile;
	ifs >> Loadfile;

	if (ifs)
	{
		for (int i = 0; i < (int)Loadfile["SOUND"].size(); i++)
		{
			LoadSound(Loadfile["SOUND"][i]["TAG"], UTF8toSjis(Loadfile["SOUND"][i]["PATH"]), Loadfile["SOUND"][i]["LOOP"]);
		}
	}
}

//-----------------------------------------------------------------------------
// �����̃f�[�^�Ǎ���
//-----------------------------------------------------------------------------
HRESULT CSoundManager::LoadSound(std::string tag, std::string path,bool loop)
{
	HRESULT hr;

	HANDLE hFile;
	DWORD dwChunkSize = 0;
	DWORD dwChunkPosition = 0;
	DWORD dwFiletype;
	WAVEFORMATEXTENSIBLE wfx;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̃N���A
	memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

	// �T�E���h�f�[�^�t�@�C���̐���
	hFile = CreateFile((LPCSTR)path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		assert(false);
		return HRESULT_FROM_WIN32(GetLastError());
	}
	if (SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^��擪�Ɉړ�
		assert(false);
		return HRESULT_FROM_WIN32(GetLastError());
	}

	// WAVE�t�@�C���̃`�F�b�N
	hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		//�v���O�������ً}�Ŏ~�߂�
		assert(false);
		return S_FALSE;
	}
	hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
	if (FAILED(hr))
	{
		//�v���O�������ً}�Ŏ~�߂�
		assert(false);
		return S_FALSE;
	}
	if (dwFiletype != 'EVAW')
	{
		//�v���O�������ً}�Ŏ~�߂�
		assert(false);
		return S_FALSE;
	}

	// �t�H�[�}�b�g�`�F�b�N
	hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		//�v���O�������ً}�Ŏ~�߂�
		assert(false);
		return S_FALSE;
	}
	hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	if (FAILED(hr))
	{
		//�v���O�������ً}�Ŏ~�߂�
		assert(false);
		return S_FALSE;
	}

	BYTE* pDataAudio;
	DWORD sizeAudio;

	// �I�[�f�B�I�f�[�^�ǂݍ���
	hr = CheckChunk(hFile, 'atad', &sizeAudio, &dwChunkPosition);
	if (FAILED(hr))
	{
		//�v���O�������ً}�Ŏ~�߂�
		assert(false);
		return S_FALSE;
	}

	pDataAudio = (BYTE*)malloc(sizeAudio);
	hr = ReadChunkData(hFile, pDataAudio, sizeAudio, dwChunkPosition);
	if (FAILED(hr))
	{
		//�v���O�������ً}�Ŏ~�߂�
		assert(false);
		return S_FALSE;
	}

	// �t�@�C�����N���[�Y
	CloseHandle(hFile);

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = sizeAudio;
	buffer.pAudioData = pDataAudio;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	if (loop)
	{
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	else
	{
		buffer.LoopCount = 0;
	}

	m_soundData[tag].buffer = buffer;
	m_soundData[tag].format = wfx.Format;

	return S_OK;
}

//-----------------------------------------------------------------------------
// ���̊m�F
//-----------------------------------------------------------------------------
HRESULT CSoundManager::CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^��擪�Ɉړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, nullptr) == 0)
		{// �`�����N�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, nullptr) == 0)
		{// �`�����N�f�[�^�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, nullptr) == 0)
			{// �t�@�C���^�C�v�̓ǂݍ���
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, nullptr, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// �t�@�C���|�C���^���`�����N�f�[�^���ړ�
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// ���̓ǂݎ��
//--------------------------------------
HRESULT CSoundManager::ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^���w��ʒu�܂ňړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, nullptr) == 0)
	{// �f�[�^�̓ǂݍ���
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}
