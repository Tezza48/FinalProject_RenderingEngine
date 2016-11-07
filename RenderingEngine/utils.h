#pragma once

#include <Windows.h>
#include <exception>

namespace DX
{

//#ifndef ThrowIfFailed
//#define ThrowIfFailed(x)		\
//	HRESULT hr = x;				\
//	if(FAILED(hr))				\
//	{							\
//		throw std::exception;	\
//	}
//
//#endif

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			MessageBox(0, L"HRESULT failed somewhere.", L"ERROR", MB_OK);
			throw std::exception();
		}
	}

}