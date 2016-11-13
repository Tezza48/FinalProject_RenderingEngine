#pragma once

#include <Windows.h>
#include <exception>

namespace DX
{
	// Default throw if failed,
	// saves me from having to handle
	// failed HRESULTS each time
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