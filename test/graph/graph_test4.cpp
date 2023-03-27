#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <string>
#include <vector>

TEST_CASE("Begin/End iterator test") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};
	auto graph2 = gdwg::graph<std::string, int>{"a"};
	graph1.insert_edge("b", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("b", "c", 3);

	auto begin_it = graph1.begin();

	CHECK((*begin_it).from == "b");
	CHECK((*begin_it).to == "a");
	CHECK((*begin_it).weight == 2);

	CHECK(graph2.begin() == graph2.end());
}

TEST_CASE("Next Iterator test") {
	auto graph1 = gdwg::graph<int, int>{1, 7, 12, 14, 19, 21, 31, 67};

	graph1.insert_edge(7, 21, 13);
	graph1.insert_edge(12, 19, 16);
	graph1.insert_edge(14, 14, 0);
	graph1.insert_edge(19, 1, 3);
	graph1.insert_edge(19, 21, 2);
	graph1.insert_edge(21, 14, 23);
	graph1.insert_edge(21, 31, 14);
	graph1.insert_edge(1, 7, 4);
	graph1.insert_edge(1, 12, 3);
	graph1.insert_edge(1, 21, 12);

	auto const expected_from = std::vector<int>{1, 1, 1, 7, 12, 14, 19, 19, 21, 21};
	auto const expected_to = std::vector<int>{7, 12, 21, 21, 19, 14, 1, 21, 14, 31};
	auto const expected_weight = std::vector<int>{4, 3, 12, 13, 16, 0, 3, 2, 23, 14};
	auto expected_from_it = expected_from.begin();
	auto expected_to_it = expected_to.begin();
	auto expected_weight_it = expected_weight.begin();

	for (auto g_it : graph1) {
		CHECK(g_it.from == *(expected_from_it));
		CHECK(g_it.to == *(expected_to_it));
		CHECK(g_it.weight == *(expected_weight_it));
		++expected_from_it;
		++expected_to_it;
		++expected_weight_it;
	}
}

TEST_CASE("Previous Iterator test") {
	auto graph1 = gdwg::graph<int, int>{1, 7, 12, 14, 19, 21, 31, 67};

	graph1.insert_edge(7, 21, 13);
	graph1.insert_edge(12, 19, 16);
	graph1.insert_edge(14, 14, 0);
	graph1.insert_edge(19, 1, 3);
	graph1.insert_edge(19, 21, 2);
	graph1.insert_edge(21, 14, 23);
	graph1.insert_edge(21, 31, 14);
	graph1.insert_edge(1, 7, 4);
	graph1.insert_edge(1, 12, 3);
	graph1.insert_edge(1, 21, 12);

	auto const expected_from = std::vector<int>{1, 1, 1, 7, 12, 14, 19, 19, 21, 21};
	auto const expected_to = std::vector<int>{7, 12, 21, 21, 19, 14, 1, 21, 14, 31};
	auto const expected_weight = std::vector<int>{4, 3, 12, 13, 16, 0, 3, 2, 23, 14};

	auto expected_from_it = expected_from.end();
	auto expected_to_it = expected_to.end();
	auto expected_weight_it = expected_weight.end();

	auto g_it = graph1.end();
	--g_it;
	--expected_from_it;
	--expected_to_it;
	--expected_weight_it;

	for (; g_it != graph1.begin(); --g_it) {
		CHECK((*g_it).from == *(expected_from_it));
		CHECK((*g_it).to == *(expected_to_it));
		CHECK((*g_it).weight == *(expected_weight_it));
		--expected_from_it;
		--expected_to_it;
		--expected_weight_it;
	}
}

TEST_CASE("iterator test: empty graph") {
	auto graph1 = gdwg::graph<std::string, int>{};
	auto graph2 = gdwg::graph<std::string, int>{};
	auto g1_it = graph1.begin();
	auto g2_it = graph1.begin();
	++g1_it;
	--g2_it;

	CHECK(g1_it == graph1.end());
	CHECK(g1_it == graph1.begin());
	CHECK(g2_it == graph2.begin());
	CHECK(g2_it == graph2.end());
	CHECK(graph1.begin() == graph1.end());
}