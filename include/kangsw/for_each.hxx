#pragma once
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include "counter.hxx"

namespace kangsw {

/**
 * @see https://stackoverflow.com/questions/1198260/how-can-you-iterate-over-the-elements-of-an-stdtuple
 * Iterate over tempalte
 */
template <std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
tuple_for_each(std::tuple<Tp...>&, FuncT) // Unused arguments are given no names.
{}

template <std::size_t I = 0, typename FuncT, typename... Tp>
  inline typename std::enable_if < I<sizeof...(Tp), void>::type
  tuple_for_each(std::tuple<Tp...>& t, FuncT f) {
    if constexpr (std::is_invocable_v<FuncT, decltype(std::get<I>(t)), size_t>) {
        f(std::get<I>(t), I);
    }
    else {
        f(std::get<I>(t));
    }
    tuple_for_each<I + 1, FuncT, Tp...>(t, f);
}

template <std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
tuple_for_each(std::tuple<Tp...> const&, FuncT) // Unused arguments are given no names.
{}

template <std::size_t I = 0, typename FuncT, typename... Tp>
  inline typename std::enable_if < I<sizeof...(Tp), void>::type
  tuple_for_each(std::tuple<Tp...> const& t, FuncT f) {
    if constexpr (std::is_invocable_v<FuncT, decltype(std::get<I>(t)), size_t>) {
        f(std::get<I>(t), I);
    }
    else {
        f(std::get<I>(t));
    }
    tuple_for_each<I + 1, FuncT, Tp...>(t, f);
}

/**
 * Executes for_each with given parallel execution policy. However, it provides current partition index within given callback.
 * It is recommended to set num_partitions as same as current thread count.
 */
template <typename It_, typename Fn_, typename ExPo_>
void for_each_partition(ExPo_&&, It_ first, It_ last, Fn_&& cb, size_t num_partitions = std::thread::hardware_concurrency()) {
    if (first == last) { throw std::invalid_argument("Zero argument"); }
    if (num_partitions == 0) { throw std::invalid_argument("Invalid partition size"); }
    size_t num_elems = std::distance(first, last);
    size_t steps = std::max<size_t>(1, num_elems / num_partitions);
    num_partitions = std::min(num_elems, num_partitions);
    counter_range partitions(num_partitions);

    std::for_each(
      ExPo_{},
      partitions.begin(),
      partitions.end(),
      [num_elems, num_partitions, steps, &cb, &first](size_t partition_index) {
          It_ it = first, end;
          size_t current_index = steps * partition_index;
          std::advance(it, current_index);
          std::advance(end = it, partition_index + 1 == num_partitions ? num_elems - current_index : steps);

          for (; it != end; ++it) {
              if constexpr (std::is_invocable_v<Fn_, decltype(*it)>) {
                  cb(*it);
              }
              else if constexpr (std::is_invocable_v<Fn_, decltype(*it), size_t /*partition*/>) {
                  cb(*it, partition_index);
              }
              else {
                  static_assert(false, "given callback has invalid signature");
              }
          }
      });
}

/**
 * Executes for_each with given parallel execution policy. However, it provides current partition index within given callback.
 * It is recommended to set num_partitions as same as current thread count.
 */
template <typename Range_, typename Fn_, typename ExPo_>
void for_each_range(ExPo_&&, Range_&& range, Fn_&& cb, size_t num_partitions = std::thread::hardware_concurrency()) {
    auto first = std::begin(range);
    auto last = std::end(range);
    using It_ = decltype(first);

    if (first == last) { throw std::invalid_argument("Zero argument"); }
    if (num_partitions == 0) { throw std::invalid_argument("Invalid partition size"); }
    size_t num_elems = std::distance(first, last);
    size_t steps = std::max<size_t>(1, num_elems / num_partitions);
    num_partitions = std::min(num_elems, num_partitions);
    counter_range partitions(num_partitions);

    std::for_each(
      ExPo_{},
      partitions.begin(),
      partitions.end(),
      [num_elems, num_partitions, steps, &cb, &first](size_t partition_index) {
          It_ it = first, end;
          size_t current_index = steps * partition_index;
          std::advance(it, current_index);
          std::advance(end = it, partition_index + 1 == num_partitions ? num_elems - current_index : steps);

          for (; it != end; ++it) {
              if constexpr (std::is_invocable_v<Fn_, decltype(*it)>) {
                  cb(*it);
              }
              else if constexpr (std::is_invocable_v<Fn_, decltype(*it), size_t /*partition*/>) {
                  cb(*it, partition_index);
              }
              else {
                  static_assert(false, "given callback has invalid signature");
              }
          }
      });
}

/**
 * convenient helper method for for_reach_partition
 */
template <typename ExPo_, typename Fn_>
void for_each_indexes(ExPo_&&, int64_t begin, int64_t end, Fn_&& cb, size_t num_partitions = std::thread::hardware_concurrency()) {
    if (begin < end) { throw std::invalid_argument("end precedes begin"); }

    counter_range range(begin, end);
    for_each_partition(ExPo_{}, range.begin(), range.end(), std::forward<Fn_>(cb), num_partitions);
}

template <typename ExPo_, typename Fn_>
void for_each_indexes(int64_t begin, int64_t end, Fn_&& cb) {
    counter_range range(begin, end);
    std::for_each(range.begin(), range.end(), std::forward<Fn_>(cb));
}

namespace impl__ {
enum class recurse_policy_base {
    preorder,
    postorder,
};
template <impl__::recurse_policy_base Val_>
using recurse_policy_v = std::integral_constant<impl__::recurse_policy_base, Val_>;

} // namespace impl__

namespace recurse {
constexpr impl__::recurse_policy_v<impl__::recurse_policy_base::preorder> preorder;
constexpr impl__::recurse_policy_v<impl__::recurse_policy_base::postorder> postorder;
} // namespace recurse

/**
 * 재귀적으로 작업을 수행합니다.
 * @param root 루트가 되는 노드입니다.
 * @param recurse Ty_로부터 하위 노드를 추출합니다. void(Ty_& parent, void (emplacer)(Ty_&)) 시그니쳐를 갖는 콜백으로, parent의 자손 노드를 iterate해 각각의 노드에 대해 emplacer(node)를 호출하여 재귀적인 작업을 수행할 수 있습니다.
 * 
 */
template <
  typename Ref_, typename Recurse_,
  impl__::recurse_policy_base Policy_ = impl__::recurse_policy_base::preorder>
decltype(auto) recurse_for_each(
  Ref_ root, Recurse_&& recurse,
  std::integral_constant<impl__::recurse_policy_base, Policy_> = {}) {
    if constexpr (Policy_ == impl__::recurse_policy_base::preorder) {
        std::vector<std::pair<Ref_, size_t>> stack;
        stack.emplace_back(root, 0);

        while (!stack.empty()) {
            auto ref = stack.back();
            stack.pop_back();

            if constexpr (std::is_invocable_v<Recurse_, Ref_, void(Ref_)>) {
                recurse(ref.first, [&stack, n = ref.second + 1](Ref_ arg) { stack.emplace_back(arg, n); });
            }
            else if constexpr (std::is_invocable_v<Recurse_, Ref_, size_t, void(Ref_)>) {
                recurse(ref.first, ref.second, [&stack, n = ref.second + 1](Ref_ arg) { stack.emplace_back(arg, n); });
            }
            else {
                static_assert(false);
            }
        }
    }
    else if constexpr (Policy_ == impl__::recurse_policy_base::postorder) {
        static_assert(false); // do it later
    }
    else {
        static_assert(false);
    }
}

} // namespace kangsw
