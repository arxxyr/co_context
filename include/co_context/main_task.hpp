#pragma once

#include <coroutine>
#include <variant>
#include <concepts>
#include <exception>
#include "task_info.hpp"
#include <memory>
#include <cassert>

namespace co_context {

class main_task {
  public:
    struct promise_type {
        detail::task_info io_info{detail::task_info::task_type::co_spawn};

        inline constexpr std::suspend_always initial_suspend() const noexcept {
            return {};
        }

        inline constexpr std::suspend_never final_suspend() const noexcept {
            return {};
        }

        inline main_task get_return_object() noexcept {
            auto handle =
                std::coroutine_handle<promise_type>::from_promise(*this);
            io_info.handle = handle;
            return main_task{handle};
        }

        inline void unhandled_exception() const {
            std::rethrow_exception(std::current_exception());
        }
    };

    main_task(std::coroutine_handle<promise_type> handle) noexcept
        : handle(handle) {
    }

    main_task(const main_task &) noexcept = default;

/* 
    main_task(const main_task &) = delete;

    main_task(main_task &&other) noexcept
        : handle(other.handle) {
        other.handle = nullptr;
    }

    main_task &operator=(const main_task &other) = delete;

    main_task &operator=(main_task &&other) noexcept {
        if (this != &other) {
            handle = other.handle;
            other.handle = nullptr;
        }
    }
 */

    inline detail::task_info* get_io_info_ptr() const noexcept {
        return &handle.promise().io_info;
    }

  private:
    std::coroutine_handle<promise_type> handle;
};

} // namespace co_context