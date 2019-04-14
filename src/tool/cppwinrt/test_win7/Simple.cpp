#include "pch.h"

using namespace winrt;
using namespace Windows::Foundation;

TEST_CASE("Simple")
{
    hstring a = L"hello";
    assert(a == L"hello");

    hstring b = a;
    assert(b == L"hello");
}
