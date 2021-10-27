#include <iostream>

#include "simple_event_aggregator.h"

class RandomEvent : public simple::ea::PubSubEvent<int32_t> { };


void HandleRandom(int32_t random) {
	std::cout << "Random: " << random << "\n";
}


void HandleRandom2(int32_t random) {
	std::cout << "Random2: " << random << "\n";
}


int main()
{
	simple::ea::EventAggregator aggregator;
	auto event = aggregator.GetEvent<RandomEvent>();
	auto token = event->Subscribe(HandleRandom);
	event->Subscribe(HandleRandom2);

	token->Unsubscribe();

	aggregator.GetEvent<RandomEvent>()->Publish(10);

	return 0;
}
