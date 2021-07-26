#pragma once

#include "../def.h"
#include "../atomic.h"

namespace co {

/**
 * co::Mutex is a mutex lock for coroutines
 *   - It is similar to Mutex for threads.
 *   - Users SHOULD use co::Mutex in coroutine environments only.
 */
class Mutex {
  public:
    Mutex();
    ~Mutex();

    Mutex(Mutex&& m) : _p(m._p) { m._p = 0; }

    Mutex(const Mutex& m) : _p(m._p) {
        atomic_inc(_p);
    }

    void operator=(const Mutex&) = delete;

    /**
     * acquire the lock
     *   - It MUST be called in a coroutine.
     *   - It will block until the lock was acquired by the calling coroutine.
     */
    void lock();

    /**
     * release the lock
     *   - It SHOULD be called in the coroutine that holds the lock.
     */
    void unlock();

    /**
     * try to acquire the lock
     *   - It SHOULD be called in a coroutine.
     *   - If no coroutine holds the lock, the calling coroutine will get the lock.
     *
     * @return  true if the lock was acquired by the calling coroutine, otherwise false
     */
    bool try_lock();

  private:
    uint32* _p;
};

/**
 * guard to release the mutex lock
 *   - lock() is called in the constructor.
 *   - unlock() is called in the destructor.
 */
class MutexGuard {
  public:
    explicit MutexGuard(co::Mutex& lock) : _lock(lock) {
        _lock.lock();
    }

    explicit MutexGuard(co::Mutex* lock) : _lock(*lock) {
        _lock.lock();
    }

    ~MutexGuard() {
        _lock.unlock();
    }

  private:
    co::Mutex& _lock;
    DISALLOW_COPY_AND_ASSIGN(MutexGuard);
};

} // co
