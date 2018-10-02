//
// Created by benoit on 18/09/22.
//

#ifndef QUIZZBOT_EVENT_QUEUE_H
#define QUIZZBOT_EVENT_QUEUE_H

#include <queue>
#include <mutex>
#include <utility>

namespace quizzbot {
    template<typename T>
    class event_queue {

    public:

       // void push(T element);
        void push(T&& element);
        // void pop(T& element);

        /// Will empty queue and return a new queue with all elmements
        std::queue<T> empty();

        size_t size() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.size();
        }
    private:
        std::queue<T> queue_;
        mutable std::mutex mutex_;
    };

    template<typename T>
    void event_queue<T>::push(T&& element) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(element));
    }

    template<typename T>
    std::queue<T> event_queue<T>::empty() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::queue<T> out_queue;
        std::swap(queue_, out_queue);
        return out_queue;
    }
}
#endif //QUIZZBOT_EVENT_QUEUE_H
