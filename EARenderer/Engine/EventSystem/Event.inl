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

namespace EARenderer {
    
    template <class PublisherT, class KeyT, class DelegateT>
    class Event;
    
    template <class PublisherT, class KeyT, class RetT, class... ArgTs>
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
            delegate(delegate)
            { }
            
            template <class T>
            Binding(KeyT key, T* target, RetT(T::*funcPtr)(ArgTs...))
            :
            key(key),
            delegate([target, funcPtr](ArgTs... args) {
                         return (target->*funcPtr)(args...);
                     })
            { }
        };
        
        void subscribe(const Binding& binding) {
            mContainer.bindings[binding.key] = binding.delegate;
        }
        
        void unsubscribe(const Binding& binding) {
            mContainer.bindings.erase(binding.key);
        }
        
        template <typename T>
        Event& operator +=(T&& binding) {
            subscribe(std::forward<T>(binding));
            return *this;
        }
        
        template <typename T>
        Event& operator -=(T&& binding) {
            unsubscribe(std::forward<T>(binding));
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
        
        void operator()(ArgTs... args) {
            for (auto& binding : mContainer.bindings) {
                binding.second(args...);
            }
        }
        
        void operator()(std::vector<RetT>& results, ArgTs... args) {
            for (auto& binding : mContainer.bindings) {
                results.emplace_back(binding.second(args...));
            }
        }
        
    };
    
}

#endif /* Event_hpp */
