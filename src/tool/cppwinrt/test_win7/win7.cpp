#include "pch.h"
#include <windows.h>

// Note: this implementation intentionally avoids using any C++ standard library features like new/malloc/std::atomic
// to ensure that the runtime behavior  is ABI stable.

namespace
{
    struct hstring_handle
    {
        uint32_t size{};
        uint32_t m_references{ 1 };

        void add_ref() noexcept
        {
            InterlockedIncrement(&m_references);
        }

        void release() noexcept
        {
            if (0 == InterlockedDecrement(&m_references))
            {
                delete this;
            }
        }
    };
}

int32_t WINRT_CALL WINRT_GetRestrictedErrorInfo(void** info) noexcept
{
    *info = nullptr;
    return 0;
}

int32_t WINRT_CALL WINRT_RoGetActivationFactory(void* classId, winrt::guid const& iid, void** factory) noexcept
{
    return 0;
}

int32_t WINRT_CALL WINRT_RoInitialize(uint32_t type) noexcept
{
    return CoInitializeEx(0, type == 0 ? COINIT_APARTMENTTHREADED : COINIT_MULTITHREADED);
}

int32_t WINRT_CALL WINRT_RoOriginateLanguageException(int32_t error, void* message, void* exception) noexcept
{
    return 0;
}

int32_t WINRT_CALL WINRT_WindowsCreateString(wchar_t const* value, uint32_t length, void** string) noexcept
{
    if (length == 0)
    {
        *string = nullptr;
        return 0;
    }

    *string = VirtualAlloc(nullptr, sizeof(hstring_handle) + (length + 1) * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (*string == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    new (*string) hstring_handle();
    *static_cast<uint32_t*>(*string) = length;
    memcpy(static_cast<hstring_handle*>(*string) + 1, value, (length + 1) * sizeof(wchar_t));
    return 0;
}

int32_t WINRT_CALL WINRT_WindowsDeleteString(void* string) noexcept
{
    if (string)
    {
        static_cast<hstring_handle*>(string)->release();
    }

    return 0;
}

wchar_t const* WINRT_CALL WINRT_WindowsGetStringRawBuffer(void* string, uint32_t* length) noexcept
{
    if (!string)
    {
        return L"";
    }

    if (length)
    {
        *length = static_cast<hstring_handle*>(string)->size;
    }


    return 0;
}
