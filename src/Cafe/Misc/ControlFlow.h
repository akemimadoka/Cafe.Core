#ifndef CAFE_CORE_MISC_CONTROL_FLOW_H
#define CAFE_CORE_MISC_CONTROL_FLOW_H

#include "TypeTraits.h"
#include <concepts>
#include <variant>
#include <optional>

namespace Cafe::Core::Misc
{
    struct ControlFlowBase
    {
    };

    template <typename T>
    concept ControlFlow = std::is_base_of_v<ControlFlowBase, T> && !std::same_as<T, ControlFlowBase>;

    struct ContinueType : ControlFlowBase
    {
    };

    template <typename T>
    struct IsContinue : std::false_type
    {
    };

    template <>
    struct IsContinue<ContinueType> : std::true_type
    {
    };

    template <typename T>
    concept Continue = IsContinue<T>::value;

    template <typename T = Detail::EmptyStruct>
    struct BreakType : ControlFlowBase
    {
        [[no_unique_address]] T Value;
    };

    template <typename T>
    struct IsBreak : std::false_type
    {
    };

    template <typename T>
    struct IsBreak<BreakType<T>> : std::true_type
    {
    };

    template <typename T>
    concept Break = IsBreak<T>::value;

    template <typename T = Detail::EmptyStruct>
    struct ControlFlowVariant : std::variant<ContinueType, BreakType<T>>
    {
        using std::variant<ContinueType, BreakType<T>>::variant;

        constexpr bool IsContinue() const noexcept
        {
            return std::holds_alternative<ContinueType>(*this);
        }

        constexpr bool IsBreak() const noexcept
        {
            return std::holds_alternative<BreakType<T>>(*this);
        }

        constexpr T& GetBreakValue() noexcept
        {
            return std::get<BreakType<T>>(*this).Value;
        }
    };

    template <typename T>
    struct IsControlFlowVariant : std::false_type
    {
    };

    template <typename T>
    struct IsControlFlowVariant<ControlFlowVariant<T>> : std::true_type
    {
    };

    template <typename Callable, typename... Args>
    struct IsCallableReturningControlFlow : std::false_type
    {
    };

    template <typename Callable, typename... Args> requires(IsControlFlowVariant<std::invoke_result_t<Callable, Args...>>::value)
    struct IsCallableReturningControlFlow<Callable, Args...> : std::true_type
    {
    };
}

#endif
