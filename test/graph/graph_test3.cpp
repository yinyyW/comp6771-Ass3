#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <string>
#include <vector>

TEST_CASE("is_node test") {
	auto graph1 = gdwg::graph<std::string, int>{};
	graph1.insert_node("a");
	graph1.insert_node("b");
	graph1.insert_node("c");

	CHECK(graph1.is_node("a"));
	CHECK(graph1.is_node("b"));
	CHECK(graph1.is_node("c"));
	CHECK(!graph1.is_node("x"));
}

TEST_CASE("empty test") {
	auto graph1 = gdwg::graph<std::string, int>{};
	CHECK(graph1.empty());

	graph1.insert_node("a");
	CHECK(!graph1.empty());
}

TEST_CASE("is_connected test") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	CHECK(graph1.is_connected("a", "b"));
	CHECK(graph1.is_connected("a", "c"));
	CHECK(graph1.is_connected("b", "a"));

	CHECK(!graph1.is_connected("b", "c"));
	CHECK(!graph1.is_connected("c", "a"));
	CHECK(!graph1.is_connected("c", "b"));
}

TEST_CASE("nodes test") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};
	auto graph2 = gdwg::graph<std::string, int>{};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	auto const nodes1 = std::vector<std::string>{"a", "b", "c"};
	auto const nodes2 = std::vector<std::string>{};

	CHECK(graph1.nodes() == nodes1);
	CHECK(graph2.nodes() == nodes2);
}

TEST_CASE("weights test") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};
	auto graph2 = gdwg::graph<std::string, int>{};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("a", "b", 10);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	auto const expected_weights1 = std::vector<int>{1, 10};
	auto const actual_weights1 = graph1.weights("a", "b");

	auto const expected_weights2 = std::vector<int>{};
	auto const actual_weights2 = graph1.weights("b", "b");

	CHECK(expected_weights1 == actual_weights1);
	CHECK(expected_weights2 == actual_weights2);
}

TEST_CASE("find test") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	auto const find1 = graph1.find("a", "b", 1);
	auto const find2 = graph1.find("b", "a", 2);
	auto const find3 = graph1.find("a", "b", 10);

	CHECK(find1 != graph1.end());
	CHECK(find2 != graph1.end());
	CHECK(find3 == graph1.end());
}

TEST_CASE("connections test") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	auto const expected_conn_a = std::vector<std::string>{"b", "c"};
	auto const expected_conn_b = std::vector<std::string>{"a"};
	auto const expected_conn_c = std::vector<std::string>{};

	CHECK(graph1.connections("a") == expected_conn_a);
	CHECK(graph1.connections("b") == expected_conn_b);
	CHECK(graph1.connections("c") == expected_conn_c);
}