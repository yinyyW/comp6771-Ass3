#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <string>
#include <vector>

TEST_CASE("Insert Node") {
	auto graph1 = gdwg::graph<std::string, int>{};

	REQUIRE(graph1.insert_node("a"));
	REQUIRE(graph1.insert_node("b"));

	auto const nodes = std::vector<std::string>{"a", "b"};
	CHECK(graph1.nodes() == nodes);
	CHECK(!graph1.insert_node("a"));
}

TEST_CASE("Insert Edge: Throw exception if src or dst does not exist") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	REQUIRE(graph1.insert_edge("a", "b", 1));
	REQUIRE(graph1.insert_edge("b", "a", 2));
	REQUIRE(graph1.insert_edge("a", "c", 3));

	REQUIRE_THROWS_WITH(graph1.insert_edge("a", "x", 3),
	                    "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node "
	                    "does not exist");
}

TEST_CASE("Insert Edge: Check Edges") {
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

TEST_CASE("Insert Edge: Fail to insert more edges") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	CHECK(!graph1.insert_edge("a", "b", 1));
	CHECK(!graph1.insert_edge("b", "a", 2));
	CHECK(!graph1.insert_edge("a", "c", 3));
}

TEST_CASE("Replace Node: Throw exception if old node does not exist") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	REQUIRE_THROWS_WITH(graph1.replace_node("x", "y"),
	                    "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
}

TEST_CASE("Replace Node: Check nodes and edges.") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	CHECK(graph1.replace_node("a", "x"));
	CHECK(!graph1.replace_node("b", "c"));

	auto const new_nodes = std::vector<std::string>{"b", "c", "x"};
	CHECK(graph1.nodes() == new_nodes);
	CHECK(graph1.is_connected("x", "b"));
	CHECK(graph1.is_connected("b", "x"));
	CHECK(graph1.is_connected("x", "c"));
}

TEST_CASE("merge_replace_node: Throw exception if old node or new node does not exist") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	REQUIRE_THROWS_WITH(graph1.merge_replace_node("a", "y"),
	                    "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if "
	                    "they don't exist in the graph");

	REQUIRE_THROWS_WITH(graph1.merge_replace_node("y", "b"),
	                    "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if "
	                    "they don't exist in the graph");
}

TEST_CASE("merge_replace_node: Check nodes and edges") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	graph1.merge_replace_node("a", "c");

	auto const nodes = std::vector<std::string>{"b", "c"};
	CHECK(graph1.nodes() == nodes);
	CHECK(graph1.is_connected("c", "b"));
	CHECK(graph1.is_connected("b", "c"));
	CHECK(graph1.is_connected("c", "c"));
}

TEST_CASE("erase node") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	CHECK(!graph1.erase_node("x"));
	CHECK(graph1.erase_node("b"));

	auto const nodes = std::vector<std::string>{"a", "c"};
	auto const expected_conn = std::vector<std::string>{"c"};
	auto const actual_conn = graph1.connections("a");
	CHECK(graph1.nodes() == nodes);
	CHECK(actual_conn == expected_conn);
}

TEST_CASE("erase edge (src, dst, weight): Throw exception if src or dst does not exist") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	REQUIRE_THROWS_WITH(graph1.erase_edge("a", "x", 1),
	                    "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't "
	                    "exist in the graph");

	REQUIRE_THROWS_WITH(graph1.erase_edge("x", "c", 1),
	                    "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't "
	                    "exist in the graph");
}

TEST_CASE("erase edge (src, dst, weight): Check edges") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	CHECK(graph1.erase_edge("a", "b", 1));
	CHECK(!graph1.erase_edge("b", "a", 1));

	auto const expected_conns = std::vector<std::string>{"c"};
	auto const actual_conns = graph1.connections("a");

	CHECK(expected_conns == actual_conns);
}

TEST_CASE("erase edge (iterator): Check edges") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	auto g_it = graph1.begin();
	++g_it;

	g_it = graph1.erase_edge(g_it);
	CHECK((*g_it).from == "b");
	CHECK((*g_it).to == "a");
	CHECK((*g_it).weight == 2);
	CHECK(!graph1.is_connected("a", "c"));
}

TEST_CASE("erase edge (iterator): erase begin iterator") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("a", "c", 3);

	auto g_it = graph1.begin();
	g_it = graph1.erase_edge(g_it);
	CHECK((*g_it).from == "a");
	CHECK((*g_it).to == "c");
	CHECK((*g_it).weight == 3);
	CHECK(!graph1.is_connected("a", "b"));
}

TEST_CASE("erase edge (iterator): erase end iterator") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("a", "c", 3);

	auto g_it = graph1.end();
	--g_it;
	g_it = graph1.erase_edge(g_it);

	CHECK(g_it == graph1.end());
	CHECK(!graph1.is_connected("a", "c"));
}

TEST_CASE("erase edge (iterator from, iterator to): erase end iterator") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	auto g_it = graph1.begin();
	++g_it;
	g_it = graph1.erase_edge(g_it, graph1.end());

	CHECK(g_it == graph1.end());
	CHECK(!graph1.is_connected("a", "c"));
	CHECK(!graph1.is_connected("b", "a"));
}

TEST_CASE("clear") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};

	graph1.insert_edge("a", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("a", "c", 3);

	graph1.clear();

	CHECK(graph1.empty());
}