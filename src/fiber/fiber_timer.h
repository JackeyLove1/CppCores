#include <memory>
#include <utility>
#include <vector>
#include <set>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <chrono>

class FiberTimerManager;

static inline uint64_t FiberGetCurrentTimeMs() {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    return now_ms.time_since_epoch().count();
}

class FiberTimer : public std::enable_shared_from_this<FiberTimer> {
    friend class FiberTimerManager;

public:
    typedef std::shared_ptr<FiberTimer> ptr;
    typedef std::function<void()> Callback;

    bool cancel();

    bool refresh();

    bool reset(uint64_t ms, bool from_now);

private:
    explicit FiberTimer(uint64_t ms, Callback cb,
                        bool recurring, FiberTimerManager *manager);

    explicit FiberTimer(uint64_t next);

private:
    /// is always running
    bool m_recurring{false};
    /// time cycle
    uint64_t m_ms{0};
    /// execute time
    uint64_t m_next{0};
    /// callback
    Callback m_cb{nullptr};
    /// timer manager
    FiberTimerManager *m_manager{nullptr};

private:
    struct Comparator {
        bool operator()(const FiberTimer::ptr &lhs, const FiberTimer::ptr &rhs) const;
    };
};

class FiberTimerManager {
    friend class FiberTimer;

public:
    using Callback = FiberTimer::Callback;

    FiberTimerManager();

    virtual ~FiberTimerManager();

    FiberTimer::ptr addTimer(uint64_t ms, Callback cb, bool recurring = false);

    FiberTimer::ptr addConditionTimer(uint64_t ms, Callback cb, std::weak_ptr<void> weak_cond, bool recurring = false);

    uint64_t getNextTimer();

    void listExpiredCb(std::vector<Callback> &cbs);

    bool hasTimer();

protected:
    /**
     * @brief when new timer insert into the front, run this func
     */
    virtual void onTimerInsertedAtFront() = 0;

    /**
     * @brief add timer to manager
     */
    template<typename LockType>
    void addTimer(FiberTimer::ptr val, LockType &lock);

private:
    /**
     * @brief check the machine timer has been lay down
     */
    bool detectClockRollover(uint64_t now_ms);

private:
    /// Shared Mutex
    std::shared_mutex m_rw_lock;
    /// timer collections
    std::set<FiberTimer::ptr, FiberTimer::Comparator> m_timers;
    /// trigger onTimerInsertedAtFront
    bool m_tickled = false;
    /// last execute time
    uint64_t m_last_execute_time = 0;
};