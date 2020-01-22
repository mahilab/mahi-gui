#pragma once

#include <memory>
#include <functional>
#include <list>
#include <vector>
#include <algorithm>

namespace mahi::gui
{

namespace detail
{

/// ProtoEvent is the template implementation for callback list.
template <typename, typename>
class ProtoEvent; // undefined

/// CollectorInvocation invokes event handlers differently depending on return type.
template <typename, typename>
struct CollectorInvocation;

/// CollectorLast returns the result of the last event handler from a event emission.
template <typename Result>
struct CollectorLast
{
    using CollectorResult = Result;
    explicit CollectorLast() : last_() {}
    inline bool operator()(Result r)
    {
        last_ = r;
        return true;
    }
    CollectorResult result() { return last_; }

private:
    Result last_;
};

/// CollectorDefault implements the default event handler collection behaviour.
template <typename Result>
struct CollectorDefault : CollectorLast<Result>
{
};

/// CollectorDefault specialisation for events with void return type.
template <>
struct CollectorDefault<void>
{
    using CollectorResult = void;
    void result() {}
    inline bool operator()(void) { return true; }
};

/// CollectorInvocation specialisation for regular events.
template <class Collector, class R, class... Args>
struct CollectorInvocation<Collector, R(Args...)>
{
    inline bool
    invoke(Collector &collector, const std::function<R(Args...)> &cbf, Args... args) const
    {
        return collector(cbf(args...));
    }
};

/// CollectorInvocation specialisation for events with void return type.
template <class Collector, class... Args>
struct CollectorInvocation<Collector, void(Args...)>
{
    inline bool
    invoke(Collector &collector, const std::function<void(Args...)> &cbf, Args... args) const
    {
        cbf(args...);
        return collector();
    }
};

/// ProtoEvent template specialised for the callback signature and collector.
template <class Collector, class R, class... Args>
class ProtoEvent<R(Args...), Collector> : private CollectorInvocation<Collector, R(Args...)>
{
protected:
    using CbFunction = std::function<R(Args...)>;
    using Result = typename CbFunction::result_type;
    using CollectorResult = typename Collector::CollectorResult;

private:
    /*copy-ctor*/ ProtoEvent(const ProtoEvent &) = delete;
    ProtoEvent &operator=(const ProtoEvent &) = delete;

    using CallbackSlot = std::shared_ptr<CbFunction>;
    using CallbackList = std::list<CallbackSlot>;
    CallbackList callback_list_;

    size_t add_cb(const CbFunction &cb)
    {
        callback_list_.emplace_back(std::make_shared<CbFunction>(cb));
        return size_t(callback_list_.back().get());
    }

    bool remove_cb(size_t id)
    {
        auto it = std::remove_if(begin(callback_list_), end(callback_list_),
                                 [id](const CallbackSlot &slot) { return size_t(slot.get()) == id; });
        bool const removed = it != end(callback_list_);
        callback_list_.erase(it, end(callback_list_));
        return removed;
    }

public:
    /// ProtoEvent constructor, connects default callback if non-nullptr.
    ProtoEvent(const CbFunction &method)
    {
        if (method)
            add_cb(method);
    }
    /// ProtoEvent destructor releases all resources associated with this event.
    ~ProtoEvent()
    {
    }

    /// Operator to add a new function or lambda as event handler, returns a handler connection ID.
    size_t connect(const CbFunction &cb) { return add_cb(cb); }

    template <class Class, class RR, class... Args2>
    size_t connect(Class *object, RR (Class::*method)(Args2...))
    {
        auto f = [object, method](Args2... args) { return (object->*method)(args...); };
        return connect(f);
    }

    template <class Instance, class Class, class RR, class... Args2>
    size_t connect(Instance &object, RR (Class::*method)(Args2...))
    {
        auto f = [&object, method](Args2... args) { return (object.*method)(args...); };
        return connect(f);
    }

    /// Operator to remove a event handler through it connection ID, returns if a handler was removed.
    bool disconnect(size_t connection)
    {
        return remove_cb(connection);
    }

    /// Emit a event, i.e. invoke all its callbacks and collect return types with the Collector.
    CollectorResult
    emit(Args... args) const
    {
        Collector collector;
        for (auto &slot : callback_list_)
        {
            if (slot)
            {
                const bool continue_emission = this->invoke(collector, *slot, args...);
                if (!continue_emission)
                    break;
            }
        }
        return collector.result();
    }
    // Number of connected slots.
    std::size_t size() const
    {
        return callback_list_.size();
    }
};

} // namespace detail
// namespace mahi::gui

/**
 * Event is a template type providing an interface for arbitrary callback lists.
 * A event type needs to be declared with the function signature of its callbacks,
 * and optionally a return result collector class type.
 * Event callbacks can be added with operator+= to a event and removed with operator-=, using
 * a callback connection ID return by operator+= as argument.
 * The callbacks of a event are invoked with the emit() method and arguments according to the signature.
 * The result returned by emit() depends on the event collector class. By default, the result of
 * the last callback is returned from emit(). Collectors can be implemented to accumulate callback
 * results or to halt a running emissions in correspondance to callback results.
 * The event implementation is safe against recursion, so callbacks may be removed and
 * added during a event emission and recursive emit() calls are also safe.
 * The overhead of an unused event is intentionally kept very low, around the size of a single pointer.
 * Note that the Event template types is non-copyable.
 */
template <typename EventSignature, class Collector = detail::CollectorDefault<typename std::function<EventSignature>::result_type>>
struct Event /*final*/ : detail::ProtoEvent<EventSignature, Collector>
{
    using ProtoEvent = detail::ProtoEvent<EventSignature, Collector>;
    using CbFunction = typename ProtoEvent::CbFunction;
    /// Event constructor, supports a default callback as argument.
    Event(const CbFunction &method = CbFunction()) : ProtoEvent(method) {}
};

template <typename EventSignature, class Friend, class Collector = detail::CollectorDefault<typename std::function<EventSignature>::result_type>>
struct ProtectedEvent /*final*/ : detail::ProtoEvent<EventSignature, Collector>
{
    using ProtoEvent = detail::ProtoEvent<EventSignature, Collector>;
    using CbFunction = typename ProtoEvent::CbFunction;
    /// ProtectedEvent constructor, supports a default callback as argument.
    ProtectedEvent(const CbFunction &method = CbFunction()) : ProtoEvent(method) {}
protected:
    friend Friend;
    using detail::ProtoEvent<EventSignature, Collector>::emit;
    // using detail::ProtoEvent<EventSignature, Collector>::invoke;
};



/// This function creates a std::function by binding @a object to the member function pointer @a method.
template <class Instance, class Class, class R, class... Args>
std::function<R(Args...)>
slot(Instance &object, R (Class::*method)(Args...))
{
    return [&object, method](Args... args) { return (object.*method)(args...); };
}

/// This function creates a std::function by binding @a object to the member function pointer @a method.
template <class Class, class R, class... Args>
std::function<R(Args...)>
slot(Class *object, R (Class::*method)(Args...))
{
    return [object, method](Args... args) { return (object->*method)(args...); };
}

/// Keep event emissions going while all handlers return !0 (true).
template <typename Result>
struct CollectorUntil0
{
    using CollectorResult = Result;
    explicit CollectorUntil0() : result_() {}
    const CollectorResult &result() { return result_; }
    inline bool
    operator()(Result r)
    {
        result_ = r;
        return result_ ? true : false;
    }

private:
    CollectorResult result_;
};

/// Keep event emissions going while all handlers return 0 (false).
template <typename Result>
struct CollectorWhile0
{
    using CollectorResult = Result;
    explicit CollectorWhile0() : result_() {}
    const CollectorResult &result() { return result_; }
    inline bool
    operator()(Result r)
    {
        result_ = r;
        return result_ ? false : true;
    }

private:
    CollectorResult result_;
};

/// CollectorVector returns the result of the all event handlers from a event emission in a std::vector.
template <typename Result>
struct CollectorVector
{
    using CollectorResult = std::vector<Result>;
    const CollectorResult &result() { return result_; }
    inline bool
    operator()(Result r)
    {
        result_.push_back(r);
        return true;
    }

private:
    CollectorResult result_;
};

} // namespace mahi::gui