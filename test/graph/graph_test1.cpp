// graph_test_1: Constructors tests
// graph_test_2: Modifiers tests
// graph_test_3: Accessors tests
// graph_test_4: Iterators tests
// graph_test_5: Comparisons tests and extractors test

// ############## Constructors test ##############
// graph() test: test empty graph.
// graph(std::initializer_list<N> il): test initialized nodes.
// graph(InputIt first, InputIt last): test initialized nodes.
// move constructor/move assignment: test nodes and edges in
// original graph and new graph.
// copy constructor/move assignment: test nodes and edges in
// original graph and new graph.

// ############## Modifiers test ##############
// insert_node: test nodes in graph after inserting nodes.
// Fail to insert node if node already exists.
// insert_edge: test edges in graph after inserting edges.
// Throw exception if src or dst does not exist.
// Fail to insert edge if edge already exists.
// replace_node: test nodes and edges after replacing nodes.
// Throw exception if node does not exists.
// Fail to replace node if new node already exists.
// merge_replace_node: test nodes and edges after
// merge_replace_node.
// Throw exception if old_data or new_data does not exist.
// erase_node: Check nodes and edges in graph after removing node
// Fail to erase node if node does not exist.
// erase_edge(N const& src, N const& dst, E const& weight):
// Throw exception if src or dst does not exist
// Fail to remove edge if edge does not exist
// Check edges after removing edge
// erase_edge(iterator): Check the graph after removing edge
// Check the returned iterator after removing edge
// erase_edge(iterator from, itertor to): Check the graph after
// removing edge. Check the returned iterator after removing edge

// ############## Accessors test ##############
// is_node: Check node in graph
// empty: Check empty graph
// is_connected: Check connected edges
// nodes: Check nodes in graph
// weights: Check all weights from src to dst
// find: Check the the iterator returned by find function
// connections: Check all dst from src

// ############## Iterator test ##############
// iterator begin/end: test value_type of begin/end iterator
// iterator ++/--: test value_type of iterator after ++/--

// ############## Comparisons test and Extractors test ##############
// Comparison test: test two graphs with the same nodes and edges.
// Exractors test: test output string of graph.

#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <string>
#include <vector>

TEST_CASE("constructor: no arguments") {
	auto graph1 = gdwg::graph<std::string, int>{};
	CHECK(graph1.empty());
}

TEST_CASE("constructor argument: initializer_list") {
	auto graph1 = gdwg::graph<std::string, int>{"a"};
	auto graph2 = gdwg::graph<std::string, int>{"a", "b", "c"};

	auto const nodes_1 = std::vector<std::string>{"a"};
	auto const nodes_2 = std::vector<std::string>{"a", "b", "c"};

	CHECK(graph1.nodes() == nodes_1);
	CHECK(graph2.nodes() == nodes_2);
}

TEST_CASE("constructor argument: InputIt first, InputIt last") {
	auto const vec1 = std::vector<std::string>{"a"};
	auto const vec2 = std::vector<std::string>{"a", "b", "c"};

	auto graph1 = gdwg::graph<std::string, int>(vec1.begin(), vec1.end());
	auto graph2 = gdwg::graph<std::string, int>(vec2.begin(), vec2.end());

	auto const nodes_1 = std::vector<std::string>{"a"};
	auto const nodes_2 = std::vector<std::string>{"a", "b", "c"};

	CHECK(graph1.nodes() == nodes_1);
	CHECK(graph2.nodes() == nodes_2);
}

TEST_CASE("move constructor") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};
	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "c", 2);

	auto graph2 = gdwg::graph<std::string, int>(std::move(graph1));

	auto const nodes_2 = std::vector<std::string>{"a", "b", "c"};

	CHECK(graph1.empty());
	CHECK(graph2.nodes() == nodes_2);
	CHECK(graph2.is_connected("a", "b"));
	CHECK(graph2.is_connected("b", "c"));
}

TEST_CASE("move assignment") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};
	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "c", 2);

	auto graph2 = std::move(graph1);

	auto const nodes_2 = std::vector<std::string>{"a", "b", "c"};

	CHECK(graph1.empty());
	CHECK(graph2.nodes() == nodes_2);
	CHECK(graph2.is_connected("a", "b"));
	CHECK(graph2.is_connected("b", "c"));
}

TEST_CASE("copy constructor") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};
	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "c", 2);

	auto graph2 = gdwg::graph<std::string, int>(graph1);

	auto const nodes = std::vector<std::string>{"a", "b", "c"};

	CHECK(graph1.nodes() == nodes);
	CHECK(graph2.nodes() == nodes);

	CHECK(graph1.is_connected("a", "b"));
	CHECK(graph1.is_connected("b", "c"));
	CHECK(graph2.is_connected("a", "b"));
	CHECK(graph2.is_connected("b", "c"));
}

TEST_CASE("copy assignment") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};
	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "c", 2);

	auto graph2 = graph1;

	auto const nodes = std::vector<std::string>{"a", "b", "c"};

	CHECK(graph1.nodes() == nodes);
	CHECK(graph2.nodes() == nodes);

	CHECK(graph1.is_connected("a", "b"));
	CHECK(graph1.is_connected("b", "c"));
	CHECK(graph2.is_connected("a", "b"));
	CHECK(graph2.is_connected("b", "c"));
}
