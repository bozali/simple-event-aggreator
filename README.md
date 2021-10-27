# Simple EventAggregator

This small project was heavily inspired by https://prismlibrary.com/docs/event-aggregator.html and just for testing purposes but can be used in different kind of projects.

The EventAggregator allows to subscribe to events and publish these events with payloads.


## Example 1: Creating a custom event, subscribing and publishing

This example represents how to create an event and and subscribe to it. After subscription it will notify each subscriber to the event type with the specified payload.

```c++

// A simple event that will have a string as a payload.
class MessageEvent : public simple::ea::PubSubEvent<std::string> { }

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

This example shows how we can unsubscribe a event type with a subscription token

```c++

// A simple event that will have a string as a payload.
class MessageEvent : public simple::ea::PubSubEvent<std::string> { }

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

