// BASS stream for loopback recording in Windows Vista.
// Copyright (c) 2008 Andrew Firsov.

#include "stdafx.h"
#include "BASS_VistaLoopback.h"

#define EIF(x) if (FAILED(hr=(x))) { goto Exit; }	// Exit If Failed.

////////////////////////////////////////////////////////////////////////////////
DWORD CALLBACK BassVistaLoopback::LoopbackStreamProc(HSTREAM a_handle,
	void* a_buffer, DWORD a_length, void* a_user)
{
	BassVistaLoopback* l_this = (BassVistaLoopback *)a_user;
	ASSERT(l_this);

	BYTE*  l_dst_buffer = (BYTE*)a_buffer;
	UINT32 l_dst_length = a_length;

	// NOTE: we fill the required buffer completely
	while (l_dst_length)
	{
		// 1. Getting new chunk
		// 2. If we don't have any sounds, then just making the remaining part
		//      of the buffer silent and exit
		// 3. Counting data length for copying
		// 4. Filling destination buffer
		// 5. Updating variables
		// 6. If l_dst_length, then repeat

		BYTE*   l_src_buffer = NULL;
		DWORD   l_src_flags = 0;
		UINT32  l_src_frames_available = 0;
		UINT32& l_src_offset = l_this->m_src_offset;

		HRESULT hr = l_this->m_capture_client->GetBuffer(&l_src_buffer,
			&l_src_frames_available, &l_src_flags, NULL, NULL);

		if (!l_src_frames_available)
		{
			ZeroMemory(l_dst_buffer, l_dst_length);
			hr = l_this->m_capture_client->ReleaseBuffer(l_src_frames_available);
			return a_length;
		}

		UINT32 l_src_bytes_available = l_src_frames_available * l_this->m_wfx->nBlockAlign;
		UINT32 l_bytes_4copy = min(l_dst_length, l_src_bytes_available - l_src_offset);

		memcpy(l_dst_buffer, l_src_buffer + l_src_offset, l_bytes_4copy);

		l_dst_length -= l_bytes_4copy;
		l_dst_buffer += l_bytes_4copy;
		l_src_offset += l_bytes_4copy;

		ASSERT(l_src_offset <= l_src_bytes_available);
		if (l_src_offset == l_src_bytes_available)
			l_src_offset = 0;	// clearing offset if all copied

		// Asking for same chunk (0) if it wasn't fully copied
		hr = l_this->m_capture_client->ReleaseBuffer(
			(l_src_offset) ? 0 : l_src_frames_available);
	}
	return a_length;
}

////////////////////////////////////////////////////////////////////////////////
BassVistaLoopback::BassVistaLoopback()
	:m_wfx(NULL)
	,m_loopback_stream(0)
	,m_src_offset(0)
{
	HRESULT hr;
	EIF(GetDefaultDevice(eRender, &m_audio_client));
	EIF(m_audio_client->GetMixFormat(&m_wfx));

	EIF(m_audio_client->Initialize(
		AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK,
		5000000 /*0.5 second in 100-ns*/, 0, m_wfx, NULL));

	EIF(m_audio_client->GetService(__uuidof(IAudioCaptureClient),
		(void**)&m_capture_client));
	EIF(m_audio_client->Start());

	m_loopback_stream = BASS_StreamCreate(
		m_wfx->nSamplesPerSec,
		m_wfx->nChannels,
		BASS_SAMPLE_FLOAT | BASS_STREAM_DECODE,
		LoopbackStreamProc,
		this);
	
	// Better behavior of stream buffer ?
	BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 5);

Exit:
	return;
}

////////////////////////////////////////////////////////////////////////////////
BassVistaLoopback::~BassVistaLoopback()
{
	HRESULT hr = m_audio_client->Stop();
	BASS_StreamFree(m_loopback_stream);
	CoTaskMemFree(m_wfx);
}

////////////////////////////////////////////////////////////////////////////////
HSTREAM BassVistaLoopback::GetLoopbackStream() const
{
	return m_loopback_stream;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT BassVistaLoopback::GetDefaultDevice(EDataFlow a_flow, IAudioClient **a_client)
{
	CComPtr<IMMDeviceEnumerator> l_enumerator;
	CComPtr<IMMDevice> l_device;

	HRESULT hr;
	EIF(l_enumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator)));
	EIF(l_enumerator->GetDefaultAudioEndpoint(a_flow, eConsole, &l_device));
	EIF(l_device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER,
		NULL, reinterpret_cast<void**>(a_client)));
Exit:
	return hr;
}