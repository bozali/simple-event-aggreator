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
        UuidCreate(&uuid_);
    }

    inline ~SubscriptionToken() {
        this->Unsubscribe();
    }

    inline void Unsubscribe() {
        if (deleter_ != nullptr) {
            deleter_();
        }
    }

private:
    UUID uuid_;
    std::function<void()> deleter_;

    template <typename> friend class PubSubEvent;
};


class PubSubEventBase {
public:
    virtual ~PubSubEventBase() { }
};


template <typename TPayload>
class Subscription {
public:
    inline Subscription(const std::function<void(TPayload)>& callback) {
        callback_ = callback;
        token_ = std::make_shared<SubscriptionToken>();
    }

    inline Subscription(const std::function<void(TPayload)>& callback,
                 const std::function<bool(TPayload)>& filter)
        : Subscription(callback)
    { }

private:
    std::function<bool(TPayload)> filter_;
    std::function<void(TPayload)> callback_;
    std::shared_ptr<SubscriptionToken> token_;

    template <typename> friend class PubSubEvent;
};


template <typename TPayload>
class PubSubEvent : public PubSubEventBase {
public:
    inline std::shared_ptr<SubscriptionToken> Subscribe(const std::function<void(TPayload)>& callback, const std::function<bool(TPayload)>& filter = nullptr) {

        Subscription<TPayload> new_subscription(callback);
        new_subscription.filter_ = filter;

        new_subscription.token_->deleter_ = [=]() {
            /* Search for pair that contains the subscription uuid as the one we want to delete. */
            auto it = std::remove_if(
                subscriptions_.begin(),
                subscriptions_.end(),
                [=, this](Subscription<TPayload> subscription) { return new_subscription.token_->uuid_ == subscription.token_->uuid_; });

            if (it != subscriptions_.end()) {
                subscriptions_.erase(it);
            }
        };

        subscriptions_.push_back(new_subscription);

        return new_subscription.token_;
    }

    inline void Publish(TPayload payload) {
        for (Subscription<TPayload>& subscription : subscriptions_) {
            if (subscription.filter_ != nullptr && !subscription.filter_(payload)) {
                continue;
            }
            
            subscription.callback_(payload);
        }
    }

private:
    std::vector<Subscription<TPayload>> subscriptions_;
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
