#include "pch.h"
#include <windows.h>
#include <roapi.h>

// Note: this implementation intentionally avoids using any C++ standard library features like new/malloc/atomic
// to ensure that the runtime behavior is ABI stable.

namespace
{
    enum class hstring_flags
    {
        none,
        reference,
    };

    DEFINE_ENUM_FLAG_OPERATORS(hstring_flags)

    struct hstring_reference
    {
        hstring_flags flags;
        uint32_t size;
        uint32_t padding1;
        uint32_t padding2;
        wchar_t const* value;
    };

    struct hstring_handle
    {
        hstring_flags flags{};
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
                VirtualFree(this, 0, MEM_RELEASE);
            }
        }

        static auto create(wchar_t const* value, uint32_t size)
        {
            auto handle = static_cast<hstring_handle*>(VirtualAlloc(
                nullptr,
                sizeof(hstring_handle) + (size + 1) * sizeof(wchar_t),
                MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

            if (handle)
            {
                new (handle) hstring_handle();
                handle->size = size;
                memcpy(handle + 1, value, (size + 1) * sizeof(wchar_t));
            }

            return handle;
        }
    };

    hstring_reference* get_hstring_reference(void* string) noexcept
    {
        auto reference = static_cast<hstring_reference*>(string);

        if ((reference->flags & hstring_flags::reference) == hstring_flags::reference)
        {
            return reference;
        }

        return nullptr;
    }
}

int32_t WINRT_CALL WINRT_GetRestrictedErrorInfo(void** info) noexcept
{
    *info = nullptr;
    return 0;
}

int32_t WINRT_CALL WINRT_RoGetActivationFactory(void* classId, winrt::guid const& iid, void** factory) noexcept
{
    return RoGetActivationFactory(static_cast<HSTRING>(classId), reinterpret_cast<GUID const&>(iid), factory);
}

int32_t WINRT_CALL WINRT_RoInitialize(uint32_t const type) noexcept
{
    return CoInitializeEx(0, type == 0 ? COINIT_APARTMENTTHREADED : COINIT_MULTITHREADED);
}

int32_t WINRT_CALL WINRT_RoOriginateLanguageException(int32_t error, void* message, void* exception) noexcept
{
    return 0;
}

int32_t WINRT_CALL WINRT_WindowsCreateString(wchar_t const* const value, uint32_t const size, void** result) noexcept
{
    if (size == 0)
    {
        *result = nullptr;
        return 0;
    }

    *result = hstring_handle::create(value, size);
    return *result ? 0 : E_OUTOFMEMORY;
}

int32_t WINRT_CALL WINRT_WindowsDuplicateString(void* value, void** result) noexcept
{
    if (!value)
    {
        *result = nullptr;
        return 0;
    }

    if (auto reference = get_hstring_reference(value))
    {
        return WINRT_WindowsCreateString(reference->value, reference->size, result);
    }

    *result = value;
    static_cast<hstring_handle*>(value)->add_ref();
    return 0;
}

int32_t WINRT_CALL WINRT_WindowsCreateStringReference(wchar_t const* const value, uint32_t const size, void* header, void** result) noexcept
{
    if (size == 0)
    {
        *result = nullptr;
        return S_OK;
    }

    auto reference = static_cast<hstring_reference*>(header);
    reference->flags = hstring_flags::reference;
    reference->size = size;
    reference->value = value;
    *result = reference;
    return S_OK;
}

int32_t WINRT_CALL WINRT_WindowsDeleteString(void* string) noexcept
{
    if (string)
    {
        static_cast<hstring_handle*>(string)->release();
    }

    return 0;
}

wchar_t const* WINRT_CALL WINRT_WindowsGetStringRawBuffer(void* string, uint32_t* size) noexcept
{
    if (!string)
    {
        if (size)
        {
            *size = 0;
        }

        return L"";
    }

    auto handle = static_cast<hstring_handle*>(string);

    if (size)
    {
        *size = handle->size;
    }

    if (auto reference = get_hstring_reference(string))
    {
        return reference->value;
    }

    return reinterpret_cast<wchar_t const*>(handle + 1);
}
