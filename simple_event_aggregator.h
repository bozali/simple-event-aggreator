#pragma once

#include <unordered_map>
#include <functional>
#include <typeinfo>
#include <memory>
#include <vector>

#ifdef _WIN32

#include <rpc.h>

#pragma comment(lib, "Rpcrt4.lib")

#else
#endif // _WIN32


namespace simple::ea {

namespace internal {

template <typename T>
inline static constexpr size_t GetHashCode() {
    return typeid(T).hash_code();
}

}

class SubscriptionToken {
public:
    inline SubscriptionToken() {
        UuidCreate(&uuid);
    }

    inline void Unsubscribe() {
        if (deleter != nullptr) {
            deleter();
        }
    }

    UUID uuid;
    std::function<void()> deleter;
};


class PubSubEventBase {
public:
    virtual ~PubSubEventBase() { }
};


template <typename TPayload>
class PubSubEvent : public PubSubEventBase {
public:
    using FnCallback = std::function<void(TPayload)>;

    /** TODO We need a function with variadic args of callback so if we register them we can unsubscribe them with just one token. */
    inline std::shared_ptr<SubscriptionToken> Subscribe(const FnCallback& callback) {
        auto token = std::make_shared<SubscriptionToken>();

        token->deleter = [=]() {
            /* Search for pair that contains the subscription uuid as the one we want to delete. */
            auto it = std::remove_if(
                subscriptions_.begin(),
                subscriptions_.end(),
                [=, this](std::pair<std::shared_ptr<SubscriptionToken>, FnCallback> pair) { return token->uuid == pair.first->uuid; });

            if (it != subscriptions_.end()) {
                subscriptions_.erase(it);
            }
        };

        subscriptions_.push_back(std::make_pair(token, callback));
        return token;
    }

    inline void Publish(TPayload payload) {
        for (std::pair<std::shared_ptr<SubscriptionToken>, FnCallback>& pair : subscriptions_) {
            pair.second(payload);
        }
    }

private:
    std::vector<std::pair<std::shared_ptr<SubscriptionToken>, FnCallback>> subscriptions_;
};


class EventAggregator
{
public:
    template <typename TEventType>
    inline std::shared_ptr<TEventType> GetEvent() {
        const size_t hash_code = internal::GetHashCode<TEventType>();

        auto found = events_.find(hash_code);

        if (found == events_.end()) {
            auto event = std::make_shared<TEventType>();
            events_[hash_code] = event;
            return event;
        }

        return std::dynamic_pointer_cast<TEventType>(found->second);
    }

private:
    std::unordered_map<size_t, std::shared_ptr<PubSubEventBase>> events_;
};

}
