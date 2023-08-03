/**
 * \file
 * \author Christien Alden (34119981)
 * \brief AEngine defined types
*/
#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>

namespace AEngine
{
//--------------------------------------------------------------------------------
// Numeric
//--------------------------------------------------------------------------------
    using Uint8 = std::uint8_t;
    using Uint16 = std::uint16_t;
    using Uint32 = std::uint32_t;
    using Uint64 = std::uint64_t;
    using Size_t = std::size_t;
    using Intptr_t = std::intptr_t;
    using Ptrdiff_t = std::ptrdiff_t;

//--------------------------------------------------------------------------------
// Pointers
//--------------------------------------------------------------------------------
    template<typename T>
	using UniquePtr = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr UniquePtr<T> MakeUnique(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

    template <typename T>
    using SharedPtr = std::shared_ptr<T>;
    template <typename T, typename... Args>
    constexpr SharedPtr<T> MakeShared(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    using WeakPtr = std::weak_ptr<T>;
    template <typename T>
    WeakPtr<T> MakeWeak(SharedPtr<T> shared_ptr)
    {
        return std::move(WeakPtr<T>(shared_ptr));
    }
}
