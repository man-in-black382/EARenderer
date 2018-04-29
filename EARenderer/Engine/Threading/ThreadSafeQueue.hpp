//
//  DispatchQueue.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 4/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef DispatchQueue_hpp
#define DispatchQueue_hpp

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

namespace EARenderer {

    template <class T>
    class ThreadSafeQueue {
    private:
        std::atomic_bool mValid = true;
        mutable std::mutex mMutex;
        std::queue<T> mQueue;
        std::condition_variable mCondition;

    public:
        /**
         * Destructor.
         */
        ~ThreadSafeQueue(void) {
            invalidate();
        }

        /**
         * Attempt to get the first value in the queue.
         * Returns true if a value was successfully written to the out parameter, false otherwise.
         */
        bool tryPop(T& out) {
            std::lock_guard<std::mutex> lock(mMutex);
            if(mQueue.empty() || !mValid) {
                return false;
            }
            out = std::move(mQueue.front());
            mQueue.pop();
            return true;
        }

        /**
         * Get the first value in the queue.
         * Will block until a value is available unless clear is called or the instance is destructed.
         * Returns true if a value was successfully written to the out parameter, false otherwise.
         */
        bool waitPop(T& out) {
            std::unique_lock<std::mutex> lock(mMutex);
            mCondition.wait(lock, [this]() {
                return !mQueue.empty() || !mValid;
            });
            /*
             * Using the condition in the predicate ensures that spurious wakeups with a valid
             * but empty queue will not proceed, so only need to check for validity before proceeding.
             */
            if(!mValid) {
                return false;
            }
            out = std::move(mQueue.front());
            mQueue.pop();
            return true;
        }

        /**
         * Push a new value onto the queue.
         */
        void push(T value) {
            std::lock_guard<std::mutex> lock(mMutex);
            mQueue.push(std::move(value));
            mCondition.notify_one();
        }

        /**
         * Check whether or not the queue is empty.
         */
        bool empty(void) const {
            std::lock_guard<std::mutex> lock(mMutex);
            return mQueue.empty();
        }

        /**
         * Clear all items from the queue.
         */
        void clear(void) {
            std::lock_guard<std::mutex> lock(mMutex);
            while(!mQueue.empty()) {
                mQueue.pop();
            }
            mCondition.notify_all();
        }

        /**
         * Invalidate the queue.
         * Used to ensure no conditions are being waited on in waitPop when
         * a thread or the application is trying to exit.
         * The queue is invalid after calling this method and it is an error
         * to continue using a queue after this method has been called.
         */
        void invalidate(void) {
            std::lock_guard<std::mutex> lock(mMutex);
            mValid = false;
            mCondition.notify_all();
        }

        /**
         * Returns whether or not this queue is valid.
         */
        bool isValid(void) const {
            std::lock_guard<std::mutex> lock(mMutex);
            return mValid;
        }
    };

}

#endif /* DispatchQueue_hpp */
