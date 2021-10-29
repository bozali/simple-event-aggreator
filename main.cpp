#include <iostream>
#include <string>

#include "simple_event_aggregator.h"

class MessageEvent : public simple::ea::PubSubEvent<std::string> { };


void HandleMessage(std::string message) {
	std::cout << "Message []: "  << message << "\n";
}


int main()
{
	simple::ea::EventAggregator aggregator;
	aggregator.GetEvent<MessageEvent>()->Subscribe(HandleMessage, [](std::string message) { return message == "Something"; });

	aggregator.GetEvent<MessageEvent>()->Publish("Published message...");
	aggregator.GetEvent<MessageEvent>()->Publish("Something");

	return 0;
}
