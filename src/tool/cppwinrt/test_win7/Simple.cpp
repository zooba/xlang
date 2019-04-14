#include "pch.h"

using namespace winrt;

TEST_CASE("Simple")
{
	hstring value = L"hello";
	REQUIRE(value == L"hello");
}
