#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <sstream>
#include <string>

TEST_CASE("Comparison Test") {
	auto graph1 = gdwg::graph<std::string, int>{"a", "b", "c"};
	auto graph2 = gdwg::graph<std::string, int>{"a", "b", "c"};
	auto graph3 = gdwg::graph<std::string, int>{};
	auto graph4 = gdwg::graph<std::string, int>();

	graph1.insert_edge("b", "b", 1);
	graph1.insert_edge("b", "a", 2);
	graph1.insert_edge("b", "c", 3);

	graph2.insert_edge("b", "b", 1);
	graph2.insert_edge("b", "a", 2);
	graph2.insert_edge("b", "c", 3);

	CHECK(graph1 == graph2);
	CHECK(graph3 == graph4);
}

TEST_CASE("Extractor Test") {
	auto graph1 = gdwg::graph<int, int>{1, 2, 3, 4, 5, 6, 64};
	graph1.insert_edge(1, 5, -1);
	graph1.insert_edge(2, 1, 1);
	graph1.insert_edge(2, 4, 2);
	graph1.insert_edge(3, 2, 2);
	graph1.insert_edge(3, 6, -8);
	graph1.insert_edge(4, 1, -4);
	graph1.insert_edge(4, 5, 3);
	graph1.insert_edge(5, 2, 7);
	graph1.insert_edge(6, 2, 5);
	graph1.insert_edge(6, 3, 10);

	auto const expected_output = std::string_view(R"(1 (
  5 | -1
)
2 (
  1 | 1
  4 | 2
)
3 (
  2 | 2
  6 | -8
)
4 (
  1 | -4
  5 | 3
)
5 (
  2 | 7
)
6 (
  2 | 5
  3 | 10
)
64 (
)
)");
	auto out = std::ostringstream{};
	out << graph1;
	CHECK(out.str() == expected_output);
}