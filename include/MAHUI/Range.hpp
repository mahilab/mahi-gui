#pragma once

#include <experimental/coroutine>
#include <variant>
#include <stdexcept>

namespace carnot {

    template <typename T>
    struct generator
    {
        struct promise_type
        {
            std::variant<T const*, std::exception_ptr> value;

            promise_type& get_return_object()
            {
                return *this;
            }

            std::experimental::suspend_always initial_suspend()
            {
                return {};
            }

            std::experimental::suspend_always final_suspend()
            {
                return {};
            }

            std::experimental::suspend_always yield_value(T const& other)
            {
                value = std::addressof(other);
                return {};
            }

            void return_void()
            {
            }

            template <typename Expression>
            Expression&& await_transform(Expression&& expression)
            {
                static_assert(sizeof(expression) == 0, "co_await is not supported in coroutines of type generator");
                return std::forward<Expression>(expression);
            }

            void unhandled_exception()
            {
                value = std::move(std::current_exception());
            }

            void rethrow_if_failed()
            {
                if (value.index() == 1)
                {
                    std::rethrow_exception(std::get<1>(value));
                }
            }
        };

        using handle_type = std::experimental::coroutine_handle<promise_type>;

        handle_type handle{ nullptr };

        struct iterator
        {
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = ptrdiff_t;
            using pointer = T const*;
            using reference = T const&;

            handle_type handle;

            iterator(std::nullptr_t) : handle(nullptr)
            {
            }

            iterator(handle_type handle_type) : handle(handle_type)
            {
            }

            iterator &operator++()
            {
                handle.resume();

                if (handle.done())
                {
                    promise_type& promise = handle.promise();
                    handle = nullptr;
                    promise.rethrow_if_failed();
                }

                return *this;
            }

            iterator operator++(int) = delete;

            bool operator==(iterator const& other) const
            {
                return handle == other.handle;
            }

            bool operator!=(iterator const& other) const
            {
                return !(*this == other);
            }

            T const& operator*() const
            {
                return *std::get<0>(handle.promise().value);
            }

            T const* operator->() const
            {
                return std::addressof(operator*());
            }
        };

        iterator begin()
        {
            if (!handle)
            {
                return nullptr;
            }

            handle.resume();

            if (handle.done())
            {
                handle.promise().rethrow_if_failed();
                return nullptr;
            }

            return handle;
        }

        iterator end()
        {
            return nullptr;
        }

        generator(promise_type& promise) :
            handle(handle_type::from_promise(promise))
        {
        }

        generator() = default;
        generator(generator const&) = delete;
        generator &operator=(generator const&) = delete;

        generator(generator&& other) : handle(other.handle)
        {
            other.handle = nullptr;
        }

        generator &operator=(generator&& other)
        {
            if (this != &other)
            {
                handle = other.handle;
                other.handle = nullptr;
            }

            return *this;
        }

        ~generator()
        {
            if (handle)
            {
                handle.destroy();
            }
        }
    };

    template <typename T>
    generator<int> range(T first, T last)
    {
        while (first != last)
        {
            co_yield first++;
        }
    }

    generator<std::size_t> range(std::size_t last);

} // namespace carnot
