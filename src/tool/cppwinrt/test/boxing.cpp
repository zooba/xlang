#include "pch.h"

using namespace winrt;
using namespace Windows::Foundation;

namespace
{
    struct Stringable : implements<Stringable, IStringable>
    {
        hstring m_value;

        explicit Stringable(hstring const& value) : m_value(value)
        {
        }

        hstring ToString()
        {
            return m_value;
        }
    };

    boxed_value test(boxed_value const& value)
    {
        REQUIRE(value == L"value");
        hstring const text;
        return { text };
    }
}

TEST_CASE("boxing")
{
    {
        boxed_value boxed{ L"one" };
        REQUIRE(boxed);
        hstring value = boxed;
        REQUIRE(value == L"one");
        boxed = { L"two" };
        REQUIRE(boxed);
        value = boxed;
        REQUIRE(value == L"two");
        boxed = nullptr;
        REQUIRE(!boxed);
        value = L"three";
        boxed = { value };
        REQUIRE(boxed);
        REQUIRE(boxed == L"three");
    }
    {
        boxed_value boxed{ 1 };
        REQUIRE(boxed);
        int32_t value = boxed;
        REQUIRE(value == 1);
        boxed = { 2 };
        REQUIRE(boxed);
        value = boxed;
        REQUIRE(value == 2);
        boxed = nullptr;
        REQUIRE(!boxed);
        value = 3;
        boxed = { value };
        REQUIRE(boxed);
        REQUIRE(static_cast<int32_t>(boxed) == 3);
    }
    {
        boxed_value boxed{ make<Stringable>(L"one") };
        REQUIRE(boxed);
        IStringable value = boxed;
        REQUIRE(value.ToString() == L"one");
        boxed = { make<Stringable>(L"two") };
        REQUIRE(boxed);
        value = boxed;
        REQUIRE(value.ToString() == L"two");
        boxed = nullptr;
        REQUIRE(!boxed);
        value = make<Stringable>(L"three");
        boxed = { value };
        REQUIRE(boxed);
        value = nullptr;
        value = boxed;
        REQUIRE(value.ToString() == L"three");
    }
}
