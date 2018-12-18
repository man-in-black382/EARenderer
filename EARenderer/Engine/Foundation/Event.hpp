//
//  Event.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Event_hpp
#define Event_hpp

#include <unordered_map>
#include <string>
#include <vector>

#include "BitwiseEnum.hpp"

namespace EARenderer {

#pragma mark - Event

    template<class PublisherT, class KeyT, class DelegateT>
    class Event;

    template<class PublisherT, class KeyT, class RetT, class... ArgTs>
    class Event<PublisherT, KeyT, RetT(ArgTs...)> {

        friend PublisherT;

    public:

        using Delegate = std::function<RetT(ArgTs...)>;

        struct Binding {
            KeyT key;
            Delegate delegate;

            Binding(KeyT key, Delegate delegate)
                    :
                    key(key),
                    delegate(delegate) {
            }

            template<class T>
            Binding(KeyT key, T *target, RetT(T::*funcPtr)(ArgTs...))
                    :
                    key(key),
                    delegate([target, funcPtr](ArgTs... args) {
                        return (target->*funcPtr)(args...);
                    }) {
            }
        };

        void subscribe(const Binding &binding) {
            mContainer.bindings[binding.key] = binding.delegate;
        }

        void unsubscribe(KeyT key) {
            mContainer.bindings.erase(key);
        }

        void clear() {
            mContainer.bindings.clear();
        }

        size_t size() const {
            return mContainer.bindings.size();
        }

        template<typename T>
        Event &operator+=(T &&binding) {
            subscribe(std::forward<T>(binding));
            return *this;
        }

        Event &operator+=(const Binding &binding) {
            subscribe(binding);
            return *this;
        }

        template<typename T>
        Event &operator-=(T &&key) {
            unsubscribe(std::forward<T>(key));
            return *this;
        }

    private:
        // Hide bindings map from publisher (PublisherT)
        struct BindingContainer {
        private:
            friend Event;
            std::unordered_map<KeyT, Delegate> bindings;
        };

        BindingContainer mContainer;

        void raise(ArgTs... args) {
            for (auto &binding : mContainer.bindings) {
                binding.second(args...);
            }
        }

        void raise(std::vector<RetT> &results, ArgTs... args) {
            for (auto &binding : mContainer.bindings) {
                results.emplace_back(binding.second(args...));
            }
        }

        template<typename... T>
        void operator()(T &&... args) {
            raise(std::forward<T>(args)...);
        }

        template<typename... T>
        void operator()(std::vector<RetT> &results, T &&... args) {
            raise(results, std::forward<T>(args)...);
        }

    };

#pragma mark - MultiEvent

    template<class PublisherT, class IdxT, class KeyT, class DelegateT>
    class MultiEvent;

    template<class PublisherT, class IdxT, class KeyT, class RetT, class... ArgTs>
    class MultiEvent<PublisherT, IdxT, KeyT, RetT(ArgTs...)> {

    public:
        friend PublisherT;
        using EventType = Event<PublisherT, KeyT, RetT(ArgTs...)>;

    private:
        // Make events map inaccessible to publisher (PublisherT)
        struct EventsContainer {
        private:
            friend MultiEvent;
            std::unordered_map<IdxT, EventType> events;
        };

        EventsContainer mEventsContainer;

    public:
        EventType &at(IdxT index) {
            return mEventsContainer.events[index];
        }

        EventType &operator[](IdxT index) {
            return mEventsContainer.events[index];
        }
    };

}

#endif /* Event_hpp */
