#include <numeric>

#include "fiber_timer.h"

bool FiberTimer::Comparator::operator()(const FiberTimer::ptr &lhs, const FiberTimer::ptr &rhs) const {
    if (!lhs && !rhs) {
        return false;
    }
    if (!lhs) {
        return true;
    }
    if (!rhs) {
        return false;
    }
    if (lhs->m_next < rhs->m_next) {
        return true;
    }
    if (lhs->m_next > rhs->m_next) {
        return false;
    }
    return lhs.get() < rhs.get();
}


FiberTimer::FiberTimer(uint64_t ms, FiberTimer::Callback cb, bool recurring, FiberTimerManager *manager)
        : m_recurring(recurring),
          m_ms(ms),
          m_next(FiberGetCurrentTimeMs() + m_ms),
          m_cb(std::move(cb)),
          m_manager(manager) {

}

FiberTimer::FiberTimer(uint64_t next) : m_next(next) {

}

bool FiberTimer::cancel() {
    std::unique_lock wlock(m_manager->m_rw_lock);
    if (m_cb) {
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        if (it != m_manager->m_timers.end()) {
            m_manager->m_timers.erase(it);
        }
        return true;
    }
    return false;
}

bool FiberTimer::refresh() {
    std::unique_lock wlock(m_manager->m_rw_lock);
    if (!m_cb) {
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
        return false;
    }
    m_manager->m_timers.erase(it);
    m_next = FiberGetCurrentTimeMs() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}


bool FiberTimer::reset(uint64_t ms, bool from_now) {
    if (ms == m_ms && !from_now) {
        return true;
    }
    std::unique_lock wlock(m_manager->m_rw_lock);
    if (!m_cb) {
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
        return false;
    }
    m_manager->m_timers.erase(it);
    uint64_t start = 0;
    if (from_now) {
        start = FiberGetCurrentTimeMs();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;
    m_manager->addTimer(shared_from_this(), wlock);
    return true;
}

FiberTimerManager::FiberTimerManager() : m_last_execute_time(FiberGetCurrentTimeMs()) {

}

FiberTimerManager::~FiberTimerManager() {

}

uint64_t FiberTimerManager::getNextTimer() {
    std::shared_lock rlock(m_rw_lock);
    m_tickled = false;
    if (m_timers.empty()) {
        return std::numeric_limits<uint64_t>::max();
    }

    const FiberTimer::ptr &next = *m_timers.begin();
    uint64_t now_ms = FiberGetCurrentTimeMs();
    if (now_ms >= next->m_next) {
        return 0;
    } else {
        return next->m_next - now_ms;
    }
}

bool FiberTimerManager::hasTimer() {
    std::shared_lock rlock(m_rw_lock);
    return !m_timers.empty();
}

void FiberTimerManager::listExpiredCb(std::vector<Callback> &cbs) {
    uint64_t now_ms = FiberGetCurrentTimeMs();
    std::vector<FiberTimer::ptr> expired;

    std::unique_lock wlock(m_rw_lock);
    if (m_timers.empty()) {
        return;
    }
    bool rollover = detectClockRollover(now_ms);
    if (!rollover && ((*m_timers.begin())->m_next > now_ms)) {
        return;
    }

    FiberTimer::ptr now_timer(new FiberTimer(now_ms));
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    while (it != m_timers.end() && (*it)->m_next == now_ms) {
        ++it;
    }
    expired.insert(expired.begin(),
                   std::make_move_iterator(m_timers.begin()),
                   std::make_move_iterator(it));
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());

    for (auto &timer: expired) {
        cbs.push_back(timer->m_cb);
        if (timer->m_recurring) {
            timer->m_next = now_ms + timer->m_ms;
            m_timers.insert(timer);
        } else {
            timer->m_cb = nullptr;
        }
    }
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
    std::shared_ptr<void> tmp = weak_cond.lock();
    if (tmp) {
        cb();
    }
}

template<typename LockType>
void FiberTimerManager::addTimer(FiberTimer::ptr timer, LockType &lock) {
    auto it = m_timers.insert(std::move(timer)).first;
    bool at_front = (it == m_timers.begin()) && !m_tickled;
    if (at_front) {
        m_tickled = true;
    }
    lock.unlock();

    if (at_front) {
        onTimerInsertedAtFront();
    }
}

FiberTimer::ptr FiberTimerManager::addTimer(uint64_t ms, FiberTimerManager::Callback cb, bool recurring) {
    FiberTimer::ptr timer(new FiberTimer(ms, cb, recurring, this));
    std::unique_lock rlock(m_rw_lock);
    addTimer(timer, rlock);
    return timer;
}

FiberTimer::ptr
FiberTimerManager::addConditionTimer(uint64_t ms, FiberTimerManager::Callback cb, std::weak_ptr<void> weak_cond,
                                     bool recurring) {
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

bool FiberTimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if (now_ms < m_last_execute_time &&
        now_ms < (m_last_execute_time - 60 * 60 * 1000)) {
        rollover = true;
    }
    m_last_execute_time = now_ms;
    return rollover;
}