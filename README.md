# Simple EventAggregator

This small project was heavily inspired by https://prismlibrary.com/docs/event-aggregator.html and just for testing purposes but can be used in different kind of projects.

The EventAggregator allows to subscribe to events and publish these events with payloads.


## Example 1: Creating a custom event, subscribing and publishing

In this example we will create an event and subscribe to it. After the subscription we will notify each subscriber of the event with a payload.

```c++

// A simple event that will have a string as a payload.
class MessageEvent : public simple::ea::PubSubEvent<std::string> { }

void HandleMessage(std::string message) {
	std::cout << "Message: " << message << "\n";
}

int main()
{
    simple::ea::EventAggregator aggregator;
    
    // Get the event type and subscribe to it with our handler.
    aggregator.GetEvent<MessageEvent>()->Subscribe(HandleMessage);
    
    // Publish message with payload "Published message"
    aggregator.GetEvent<MessageEvent>()->Publish("Published message...");
    
    return 0;
}

```


## Example 2: Creating a custom event, subscribing and unsubscribing

In this example we will subscribe to an event and unsubscribe to it with a subscription token.

```c++

// A simple event that will have a string as a payload.
class MessageEvent : public simple::ea::PubSubEvent<std::string> { }

void HandleMessage(std::string message) {
	std::cout << "Message: " << message << "\n";
}

int main()
{
    simple::ea::EventAggregator aggregator;
    
    // Get the event type and subscribe to it with our handler.
    auto token = aggregator.GetEvent<MessageEvent>()->Subscribe(HandleMessage);
    
    // Unsubscribe the event
    token->Unsubscribe();
    
    return 0;
}

```

## Example 3: Subscription filter

Filters allow to check if a subscriber should execute to an event. In this example only published events with payload over 10 will be output.

```c++

class IntegerEvent : public simple::ea::PubSubEvent<int> { };


void HandleInteger(int value) {
	std::cout << "Value: " << value << "\n";
}


int main()
{
	simple::ea::EventAggregator aggregator;
	aggregator.GetEvent<IntegerEvent>()->Subscribe(HandleInteger, [](int value) { return value > 10; });

	aggregator.GetEvent<IntegerEvent>()->Publish(5);
	aggregator.GetEvent<IntegerEvent>()->Publish(15);

	return 0;
}

```


## Example 4: Real world example

```c++

struct BroadcastMessageArgs {
    Client* const sender;
    std::string message;
};

class BroadcastMessageEvent : public simple::ea::PubSubEvent<BroadcastMessageArgs> { };

class Client {
public:
    Client(std::shared_ptr<EventAggregator> aggregator) {
        token_ = aggregator_->GetEvent<BroadcastMessageEvent>()->Subscribe(std::bind(&Client::HandleBroadcast, this, std::placeholders::_1))
    }
    
    void SendMessage(std::string message) {
        aggregator_->GetEvent<BroadcastMessageEvent>()->Publish(BroadcastMessageArgs{ this, message });
    }
    
    void HandleBroadcast(BroadcastMessageArgs args) {
        // ...
    }
    
private:
    int32_t id_;
    
    std::shared_ptr<SubscriptionToken> token_;
    std::shared_ptr<EventAggregator> aggregator_;
};

```

