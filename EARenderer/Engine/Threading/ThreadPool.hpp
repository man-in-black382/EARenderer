//
//  ThreadPool.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 4/26/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef ThreadPool_hpp
#define ThreadPool_hpp

#include "ThreadSafeQueue.hpp"

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

// http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/

namespace EARenderer {

    class ThreadPool {
    private:

#pragma mark - Thread Task Interface

        class IThreadTask {
        public:
            IThreadTask(void) = default;

            virtual ~IThreadTask(void) = default;

            IThreadTask(const IThreadTask &rhs) = delete;

            IThreadTask &operator=(const IThreadTask &rhs) = delete;

            IThreadTask(IThreadTask &&other) = default;

            IThreadTask &operator=(IThreadTask &&other) = default;

            virtual void execute() = 0;
        };

#pragma mark - Thread Task

        template<typename Func>
        class ThreadTask : public IThreadTask {
        private:
            Func mFunc;

        public:
            ThreadTask(Func &&func) : mFunc(std::move(func)) {
            }

            ~ThreadTask(void) override = default;

            ThreadTask(const ThreadTask &rhs) = delete;

            ThreadTask &operator=(const ThreadTask &rhs) = delete;

            ThreadTask(ThreadTask &&other) = default;

            ThreadTask &operator=(ThreadTask &&other) = default;

            void execute() override {
                mFunc();
            }
        };

    public:

#pragma mark - Task Future

        /**
         * A wrapper around a std::future that adds the behavior of futures returned from std::async.
         * Specifically, this object will block and wait for execution to finish before going out of scope.
         */
        template<typename T>
        class TaskFuture {
        private:
            std::future<T> mFuture;

        public:
            TaskFuture(std::future<T> &&future) : mFuture(std::move(future)) {
            }

            TaskFuture(const TaskFuture &rhs) = delete;

            TaskFuture &operator=(const TaskFuture &rhs) = delete;

            TaskFuture(TaskFuture &&other) = default;

            TaskFuture &operator=(TaskFuture &&other) = default;

            ~TaskFuture() {
                if (mFuture.valid()) {
                    mFuture.get();
                }
            }

            auto get() {
                return mFuture.get();
            }
        };

#pragma mark - Thread Pool Member Variables

    private:
        std::atomic_bool mDone = false;
        ThreadSafeQueue<std::unique_ptr<IThreadTask>> mTaskQueue;
        std::vector<std::thread> mThreads;

    public:

#pragma mark - Thread Pool Lifecycle

        static ThreadPool &Default() {
            static ThreadPool defaultPool;
            return defaultPool;
        }

        ThreadPool()
                :
                ThreadPool(std::max(std::thread::hardware_concurrency(), 2u) - 1u) {
            /*
             * Always create at least one thread.  If hardware_concurrency() returns 0,
             * subtracting one would turn it to UINT_MAX, so get the maximum of
             * hardware_concurrency() and 2 before subtracting 1.
             */
        }

        explicit ThreadPool(const std::uint32_t numThreads) {
            try {
                for (std::uint32_t i = 0; i < numThreads; ++i) {
                    mThreads.emplace_back(&ThreadPool::worker, this);
                }
            }
            catch (...) {
                destroy();
                throw;
            }
        }

        ThreadPool(const ThreadPool &rhs) = delete;

        ThreadPool &operator=(const ThreadPool &rhs) = delete;

        ~ThreadPool() {
            destroy();
        }

#pragma mark - Thread Pool Job Submission

        /**
         * Submit a job to be run by the thread pool.
         */
        template<typename Func, typename... Args>
        auto submit(Func &&func, Args &&... args) {
            auto boundTask = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            using ResultType = std::result_of_t<decltype(boundTask)()>;
            using PackagedTask = std::packaged_task<ResultType()>;
            using TaskType = ThreadTask<PackagedTask>;

            PackagedTask task(std::move(boundTask));
            TaskFuture<ResultType> result(task.get_future());
            mTaskQueue.push(std::make_unique<TaskType>(std::move(task)));
            return result;
        }

    private:

#pragma mark - Thread Pool Private Heplers

        /**
         * Constantly running function each thread uses to acquire work items from the queue.
         */
        void worker() {
            while (!mDone) {
                std::unique_ptr<IThreadTask> pTask(nullptr);
                if (mTaskQueue.waitPop(pTask)) {
                    pTask->execute();
                }
            }
        }

        /**
         * Invalidates the queue and joins all running threads.
         */
        void destroy() {
            mDone = true;
            mTaskQueue.invalidate();

            for (auto &thread : mThreads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
        }
    };
}

#endif /* ThreadPool_hpp */
