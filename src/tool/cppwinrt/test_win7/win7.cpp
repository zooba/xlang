#include "pch.h"

int32_t WINRT_CALL WINRT_RoInitialize(uint32_t type) noexcept
{
    return 0;
}

int32_t WINRT_CALL WINRT_GetRestrictedErrorInfo(void** info) noexcept
{
    *info = nullptr;
    return 0;
}

int32_t WINRT_CALL WINRT_RoOriginateLanguageException(int32_t error, void* message, void* exception) noexcept
{
    return 0;
}

int32_t WINRT_CALL WINRT_WindowsCreateString(wchar_t const* sourceString, uint32_t length, void** string) noexcept
{
    return 0;
}

int32_t WINRT_CALL WINRT_WindowsDeleteString(void* string) noexcept
{
    return 0;
}

wchar_t const* WINRT_CALL WINRT_WindowsGetStringRawBuffer(void* string, uint32_t* length) noexcept
{
    return 0;
}
