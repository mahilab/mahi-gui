#include <mahi/Coroutine.hpp>

namespace mahi::gui {

//==============================================================================
// YieldInstruction
//==============================================================================

YieldInstruction::YieldInstruction() {
}

YieldInstruction::~YieldInstruction() {
}

bool YieldInstruction::isOver() {
   return true;
}

WaitForSeconds::WaitForSeconds(float duration) :
   YieldInstruction(),
   m_duration(duration),
   m_elapsedTime(0.0f)
{
}

bool WaitForSeconds::isOver() {
    // m_elapsedTime += Engine::deltaTime(); TODO!
    return m_elapsedTime >= m_duration;
}

//==============================================================================
// PromiseType
//==============================================================================

PromiseType::PromiseType() :
   m_instruction(nullptr)
{
}

PromiseType::~PromiseType() {
}

SuspendAlawys PromiseType::initial_suspend() {
   return SuspendAlawys{}; // changing this to change start
}

SuspendAlawys PromiseType::final_suspend() {
   return SuspendAlawys{};
}

Enumerator PromiseType::get_return_object() {
    auto h = std::experimental::coroutine_handle<PromiseType>::from_promise(*this);
    std::shared_ptr<Coroutine> coro(new Coroutine(h));
    return Enumerator(coro);
}

void PromiseType::unhandled_exception() {
   std::exit(1);
}

SuspendNever PromiseType::return_void() {
   return SuspendNever{};
}

SuspendAlawys PromiseType::yield_value(YieldInstruction* value) {
   m_instruction = std::shared_ptr<YieldInstruction>(value);
   return SuspendAlawys{};
}

SuspendAlawys PromiseType::yield_value(std::shared_ptr<YieldInstruction> value) {
    m_instruction = value;
    return SuspendAlawys{};
}

//==============================================================================
// Coroutine
//==============================================================================

Coroutine::Coroutine(std::experimental::coroutine_handle<PromiseType> coroutine) :
    YieldInstruction(),
    m_coroutine(coroutine),
    m_stop(false)
{
}

Coroutine::Coroutine(Coroutine &&other) :
    m_coroutine(other.m_coroutine)
{
    other.m_coroutine = nullptr;
}

Coroutine::~Coroutine() {
    if (m_coroutine) {
        m_coroutine.destroy();
    }
}

bool Coroutine::isOver() {
    return m_coroutine.done();
}

void Coroutine::stop() {
    m_stop = true;
}


//==============================================================================
// Enumerator
//==============================================================================

bool Enumerator::moveNext() {
    if (m_ptr->m_stop) // coroutine has request stop
        return false;
    auto instruction = m_ptr->m_coroutine.promise().m_instruction;
    if (instruction) { // there is an instruction
        if (instruction->isOver()) { // yield instruction is over
            m_ptr->m_coroutine.resume();
            return !m_ptr->m_coroutine.done();
        }
        else { // yield instruction is not over
            return true;
        }
    }
    else { // no yield instruction
        m_ptr->m_coroutine.resume();
        return !m_ptr->m_coroutine.done();
    }
}

Enumerator::Enumerator(std::shared_ptr<Coroutine> h) :
    m_ptr(h)
{
}

Enumerator::~Enumerator() {

}

Enumerator::Enumerator(Enumerator &&e) :
    m_ptr(std::move(e.m_ptr))
{
    e.m_ptr = nullptr;
};

std::shared_ptr<Coroutine> Enumerator::getCoroutine() {
    return m_ptr;
}

} // namespac mahi::gui

