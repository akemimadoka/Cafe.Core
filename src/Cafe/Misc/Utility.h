#ifndef CAFE_CORE_MISC_UTILITY_H
#define CAFE_CORE_MISC_UTILITY_H

#include "TypeTraits.h"
#include <cassert>

namespace Cafe::Core::Misc
{
	namespace Detail
	{
		template <typename Tuple, typename Callable, std::size_t... I>
		constexpr auto ForEachImpl(Tuple&& tuple, Callable&& callable,
		                           std::index_sequence<I...>) /*noexcept(noexcept(std::tuple{ [&] {
		  if constexpr (!std::is_void_v<std::invoke_result_t<
		                    Callable&&, decltype(std::get<I>(std::forward<Tuple>(tuple)))>>)
		  {
		    return std::forward<Callable>(callable)(std::get<I>(std::forward<Tuple>(tuple)));
		  }
		  else
		  {
		    std::forward<Callable>(callable)(std::get<I>(std::forward<Tuple>(tuple)));
		    return Identity<void>{};
		  }
		}()... }))*/
		{
			return std::tuple{ [&] {
				if constexpr (!std::is_void_v<std::invoke_result_t<
				                  Callable&&, decltype(std::get<I>(std::forward<Tuple>(tuple)))>>)
				{
					return std::forward<Callable>(callable)(
					    std::get<I>(std::forward<Tuple>(tuple)));
				}
				else
				{
					std::forward<Callable>(callable)(std::get<I>(std::forward<Tuple>(tuple)));
					return Identity<void>{};
				}
			}()... };
		}
	} // namespace Detail

	template <typename Tuple, typename Callable>
	constexpr auto
	ForEach(Tuple&& tuple, Callable&& callable) noexcept(noexcept(Detail::ForEachImpl(
	    std::forward<Tuple>(tuple), std::forward<Callable>(callable),
	    std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{})))
	{
		return Detail::ForEachImpl(
		    std::forward<Tuple>(tuple), std::forward<Callable>(callable),
		    std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
	}

	namespace Detail
	{
		template <typename Tuple, typename Callable, std::size_t... I>
		constexpr void UncheckedRuntimeGetImpl(std::size_t index, Tuple&& tuple,
		                                       Callable&& callable, std::index_sequence<I...>)
		{
			constexpr auto size = std::tuple_size_v<std::remove_cvref_t<Tuple>>;
			assert(index < size);
			constexpr auto visitors = std::array{ +[](Tuple&& tuple_, Callable&& callable_) {
				std::forward<Callable>(callable_)(std::get<I>(std::forward<Tuple>(tuple_)));
			}... };
			visitors[index](std::forward<Tuple>(tuple), std::forward<Callable>(callable));
		}

		template <typename Tuple, typename Callable, std::size_t... I>
		constexpr bool RuntimeGetImpl(std::size_t index, Tuple&& tuple, Callable&& callable,
		                              std::index_sequence<I...>)
		{
			constexpr auto size = std::tuple_size_v<std::remove_cvref_t<Tuple>>;
			if (index >= size)
			{
				return false;
			}
			UncheckedRuntimeGetImpl(index, std::forward<Tuple>(tuple),
			                        std::forward<Callable>(callable), std::index_sequence<I...>{});
			return true;
		}
	} // namespace Detail

	template <typename Tuple, typename Callable>
	constexpr bool RuntimeGet(std::size_t index, Tuple&& tuple, Callable&& callable)
	{
		return Detail::RuntimeGetImpl(
		    index, std::forward<Tuple>(tuple), std::forward<Callable>(callable),
		    std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>{});
	}

	template <typename Tuple, typename Callable>
	constexpr void UncheckedRuntimeGet(std::size_t index, Tuple&& tuple, Callable&& callable)
	{
		Detail::UncheckedRuntimeGetImpl(
		    index, std::forward<Tuple>(tuple), std::forward<Callable>(callable),
		    std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>{});
	}

	template <typename T, typename... Args>
	constexpr T Construct(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
	{
		return T(std::forward<Args>(args)...);
	}

	template <typename T, typename Mapper, typename... Args>
	constexpr T MapConstruct(Mapper&& mapper, Args&&... args) noexcept(
	    noexcept(Construct<T>(std::forward<Mapper>(mapper)(std::forward<Args>(args))...)))
	{
		return Construct<T>(std::forward<Mapper>(mapper)(std::forward<Args>(args))...);
	}

	template <typename T>
	struct ConvertMapperType
	{
		constexpr ConvertMapperType() noexcept = default;

		template <typename U>
		constexpr T operator()(U&& arg) noexcept(std::is_nothrow_constructible_v<T, U&&>)
		{
			return T(std::forward<U>(arg));
		}
	};

	template <typename T>
	constexpr ConvertMapperType<T> ConvertMapper{};

	template <typename T, typename TConv, typename... Args>
	constexpr T ConvertConstruct(Args&&... args) noexcept(
	    noexcept(MapConstruct<T>(ConvertMapper<TConv>, std::forward<Args>(args)...)))
	{
		return MapConstruct<T>(ConvertMapper<TConv>, std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	constexpr void
	Initialize(T& obj, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
	{
		new (static_cast<void*>(std::addressof(obj))) T(std::forward<Args>(args)...);
	}

	namespace Detail
	{
		template <typename T, typename Tuple, std::size_t... I>
		constexpr void
		InitializeWithTupleImpl(T& obj, Tuple&& args, std::index_sequence<I...>) noexcept(
		    noexcept(Initialize(obj, std::get<I>(std::forward<Tuple>(args))...)))
		{
			Initialize(obj, std::get<I>(std::forward<Tuple>(args))...);
		}
	} // namespace Detail

	template <typename T, typename Tuple>
	constexpr void
	InitializeWithTuple(T& obj, Tuple&& args) noexcept(noexcept(Detail::InitializeWithTupleImpl(
	    obj, std::forward<Tuple>(args),
	    std::make_index_sequence<std::tuple_size_v<RemoveCvRef<Tuple>>>{})))
	{
		Detail::InitializeWithTupleImpl(
		    obj, std::forward<Tuple>(args),
		    std::make_index_sequence<std::tuple_size_v<RemoveCvRef<Tuple>>>{});
	}

	/// @brief  用于对一组非 void 的表达式进行无顺序保证计算，也可以用于检查合法性
	struct UnseqEvaluatorType
	{
		constexpr UnseqEvaluatorType() noexcept = default;

		template <typename... T>
		constexpr void operator()(T&&...) noexcept
		{
		}
	};

	constexpr UnseqEvaluatorType UnseqEvaluator{};

	/// @brief  测试枚举或 BitMask 类型中 test 是否设置了 mask 中包含的任何一位 flag
	/// @remark 若 mask 为全 0，结果为 false
	template <typename T>
	constexpr bool HasAnyFlagsSet(T const& test, T const& mask) noexcept
	{
		if constexpr (std::is_enum_v<T>)
		{
			using UnderlyingType = std::underlying_type_t<T>;
			return HasAnyFlagsSet(static_cast<UnderlyingType>(test),
			                      static_cast<UnderlyingType>(mask));
		}
		else
		{
			return (test & mask) != T{};
		}
	}

	/// @brief  测试枚举或 BitMask 类型中 test 是否设置了 mask 中包含的所有 flag
	/// @remark 若 mask 为全 0，结果为 true
	template <typename T>
	constexpr bool HasAllFlagsSet(T const& test, T const& mask) noexcept
	{
		if constexpr (std::is_enum_v<T>)
		{
			using UnderlyingType = std::underlying_type_t<T>;
			return HasAllFlagsSet(static_cast<UnderlyingType>(test),
			                      static_cast<UnderlyingType>(mask));
		}
		else
		{
			return (test & mask) == mask;
		}
	}

	template <typename T>
	constexpr T SetFlags(T const& src, T const& mask) noexcept
	{
		if constexpr (std::is_enum_v<T>)
		{
			using UnderlyingType = std::underlying_type_t<T>;
			return SetFlags(static_cast<UnderlyingType>(src), static_cast<UnderlyingType>(mask));
		}
		else
		{
			return src | mask;
		}
	}

	template <typename T>
	constexpr T ClearFlags(T const& src, T const& mask) noexcept
	{
		if constexpr (std::is_enum_v<T>)
		{
			using UnderlyingType = std::underlying_type_t<T>;
			return ClearFlags(static_cast<UnderlyingType>(src), static_cast<UnderlyingType>(mask));
		}
		else
		{
			return src & ~mask;
		}
	}

	template <typename... T>
	constexpr auto ComposeFlags(T const&... flags) noexcept
	{
		static_assert(sizeof...(flags) > 0,
		              "Cafe::Core::Misc::ComposeFlags requires one or more arguments.");

		using FirstType = std::tuple_element_t<0, std::tuple<T...>>;
		if constexpr (std::is_enum_v<FirstType>)
		{
			return ComposeFlags(static_cast<std::underlying_type_t<FirstType>>(flags)...);
		}
		else
		{
			return (FirstType{} | ... | flags);
		}
	}

	template <typename Seq>
	struct SequenceToArray;

	template <template <typename T, T...> typename SeqTemplate, typename T, T... Value>
	struct SequenceToArray<SeqTemplate<T, Value...>>
	{
		static constexpr T Array[]{ Value... };
	};

	template <typename T, std::size_t N>
	struct Array
	{
		T Content[N];

		static constexpr std::size_t Size = N;

		constexpr Array() = default;

		constexpr Array(const T (&array)[N])
		{
			std::copy(std::begin(array), std::end(array), Content);
		}

		constexpr Array(T(&&array)[N])
		{
			std::move(std::begin(array), std::end(array), Content);
		}
	};

	template <typename T>
	struct ArrayBinder
	{
		template <std::size_t N>
		using Result = Array<T, N>;
	};

	template <typename Seq, typename Mapper>
	struct MapSequenceTrait;

	template <template <typename T, T...> typename SeqTemplate, typename T, T... Value,
	          typename Mapper>
	struct MapSequenceTrait<SeqTemplate<T, Value...>, Mapper>
	{
		using Result = SeqTemplate<T, Mapper::Map(Value)...>;
	};

	template <typename Seq, typename Mapper>
	using MapSequence = typename MapSequenceTrait<Seq, Mapper>::Result;

	template <typename Tuple, template <typename> class Mapper>
	struct MapTupleTrait;

	template <template <typename...> class TupleTemplate, typename... T,
	          template <typename> class Mapper>
	struct MapTupleTrait<TupleTemplate<T...>, Mapper>
	{
		using Result = TupleTemplate<typename Mapper<T>::type...>;
	};

	template <typename Tuple, template <typename> class Mapper>
	using MapTuple = typename MapTupleTrait<Tuple, Mapper>::Result;

	template <typename T>
	struct ResultToTypeWrapper
	{
		using type = typename T::Result;
	};
} // namespace Cafe::Core::Misc

#endif
