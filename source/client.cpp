#include <gdwg/graph.hpp>

#include <iostream>
#include <string>

auto main() -> int {
	// This will not compile straight away
	auto graph1 = gdwg::graph<std::string, int>{};
	graph1.insert_node("hello");
	graph1.insert_node("how");
	graph1.insert_node("are");
	graph1.insert_node("you?");

	graph1.insert_edge("hello", "how", 5);
	graph1.insert_edge("hello", "are", 8);
	graph1.insert_edge("hello", "are", 2);

	graph1.insert_edge("how", "you?", 1);
	graph1.insert_edge("how", "hello", 4);

	graph1.insert_edge("are", "you?", 3);

	std::cout << graph1;

	// This is a structured binding.
	// https://en.cppreference.com/w/cpp/language/structured_binding
	// It allows you to unpack your tuple.

	graph1.erase_edge(graph1.begin(), graph1.end());
	for (auto const& [from, to, weight] : graph1) {
		std::cout << from << " -> " << to << " (weight " << weight << ")\n";
	}
}
