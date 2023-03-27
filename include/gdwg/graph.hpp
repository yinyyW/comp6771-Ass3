#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

// This will not compile straight away
namespace gdwg {
	template<typename N, typename E>
	class graph {
	public:
		class iterator;
		// ########### constructors ###########
		graph() noexcept
		: nodes_{std::set<std::shared_ptr<N>, node_comparator>()}
		, edges_{std::map<std::shared_ptr<N>, std::set<edge, edge_comparator>, edge_list_comparator>()} {
		}

		graph(std::initializer_list<N> i_list)
		: graph(i_list.begin(), i_list.end()) {}

		template<typename InputIt>
		graph(InputIt first, InputIt last) {
			std::for_each(first, last, [this](auto const& n) { insert_node(n); });
		}

		graph(graph&& other) noexcept
		: nodes_{std::exchange(other.nodes_, std::set<std::shared_ptr<N>, node_comparator>())}
		, edges_{std::exchange(
		     other.edges_,
		     std::map<std::shared_ptr<N>, std::set<edge, edge_comparator>, edge_list_comparator>())} {}

		auto operator=(graph&& other) noexcept -> graph& {
			std::swap(nodes_, other.nodes_);
			std::swap(edges_, other.edges_);

			other.nodes_ = std::set<std::shared_ptr<N>, node_comparator>();
			other.edges_ =
			   std::map<std::shared_ptr<N>, std::set<edge, edge_comparator>, edge_list_comparator>();

			return *this;
		}

		graph(graph const& other)
		: nodes_{std::set<std::shared_ptr<N>, node_comparator>()}
		, edges_{std::map<std::shared_ptr<N>, std::set<edge, edge_comparator>, edge_list_comparator>()} {
			// copy nodes
			std::for_each(other.nodes_.cbegin(), other.nodes_.cend(), [this](auto const& n) {
				insert_node(*n);
			});

			// copy edges
			std::for_each(other.edges_.cbegin(), other.edges_.cend(), [this](auto const& other_edge_list) {
				auto src = *(other_edge_list.first);
				std::for_each(other_edge_list.second.begin(),
				              other_edge_list.second.end(),
				              [src, this](auto const& other_edge) {
					              auto dst_it = nodes_.find(*(other_edge.to));
					              auto edge_list_it = edges_.find(src);
					              auto new_edge = edge{*dst_it, other_edge.weight};
					              edge_list_it->second.emplace(new_edge);
				              });
			});
		}

		auto operator=(graph const& other) -> graph& {
			graph(other).swap(*this);
			return *this;
		}

		// ########### Modifiers ###########
		auto insert_node(N const& value) -> bool {
			if (is_node(value)) {
				return false;
			}
			auto node = std::make_shared<N>(value);
			nodes_.emplace(node);
			edges_.emplace(node, std::set<edge, edge_comparator>());
			return true;
		}

		auto insert_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src "
				                         "or dst node does not exist");
			}

			if (find(src, dst, weight) != end()) {
				return false;
			}

			// insert new edges
			auto dst_it = nodes_.find(dst);
			auto edge_list_it = edges_.find(src);
			auto new_edge = edge{*dst_it, weight};
			edge_list_it->second.emplace(new_edge);
			return true;
		}

		auto replace_node(N const& old_data, N const& new_data) -> bool {
			if (!is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node that "
				                         "doesn't exist");
			}
			if (is_node(new_data)) {
				return false;
			}

			// insert new node
			insert_node(new_data);

			// replace edges
			auto old_edge_list_it = edges_.find(old_data);
			for_each(old_edge_list_it->second.begin(),
			         old_edge_list_it->second.end(),
			         [old_data, new_data, this](auto graph_edge) {
				         if (*(graph_edge.to) == old_data) {
					         insert_edge(new_data, new_data, graph_edge.weight);
				         }
				         else {
					         insert_edge(new_data, *(graph_edge.to), graph_edge.weight);
				         }
			         });
			old_edge_list_it->second.clear();
			edges_.erase(old_edge_list_it);

			for (auto edge_list_it = edges_.begin(); edge_list_it != edges_.end(); ++edge_list_it) {
				for (auto edge_it = edge_list_it->second.begin(); edge_it != edge_list_it->second.end();)
				{
					if (*(edge_it->to) == old_data) {
						insert_edge(*(edge_list_it->first), new_data, edge_it->weight);
						edge_it = edge_list_it->second.erase(edge_it);
					}
					else {
						++edge_it;
					}
				}
			}

			// remove old node
			auto old_data_it = nodes_.find(old_data);
			nodes_.erase(old_data_it);

			return true;
		}

		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (!is_node(old_data) || !is_node(new_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or "
				                         "new data if they don't exist in the graph");
			}

			// replace edges
			auto old_edge_list_it = edges_.find(old_data);
			for_each(old_edge_list_it->second.begin(),
			         old_edge_list_it->second.end(),
			         [old_data, new_data, this](auto graph_edge) {
				         if (*(graph_edge.to) == old_data) {
					         insert_edge(new_data, new_data, graph_edge.weight);
				         }
				         else {
					         insert_edge(new_data, *(graph_edge.to), graph_edge.weight);
				         }
			         });
			old_edge_list_it->second.clear();
			edges_.erase(old_edge_list_it);

			for (auto edge_list_it = edges_.begin(); edge_list_it != edges_.end(); ++edge_list_it) {
				for (auto edge_it = edge_list_it->second.begin(); edge_it != edge_list_it->second.end();)
				{
					if (*(edge_it->to) == old_data) {
						insert_edge(*(edge_list_it->first), new_data, edge_it->weight);
						edge_it = edge_list_it->second.erase(edge_it);
					}
					else {
						++edge_it;
					}
				}
			}

			// remove old node
			auto old_data_it = nodes_.find(old_data);
			nodes_.erase(old_data_it);
		}

		auto erase_node(N const& value) -> bool {
			if (!is_node(value)) {
				return false;
			}

			// remove edges
			auto node_to_erase_it = nodes_.find(value);
			for (auto edge_list_it = edges_.begin(); edge_list_it != edges_.end();) {
				auto node = *(edge_list_it->first);

				if (node == value) {
					// remove outcoming edges of node
					edge_list_it->second.clear();
					edge_list_it = edges_.erase(edge_list_it);
				}
				else {
					// remove incoming edge of node
					for (auto edge_it = edge_list_it->second.begin();
					     edge_it != edge_list_it->second.end();) {
						if (*(edge_it->to) == value) {
							edge_it = edge_list_it->second.erase(edge_it);
						}
						else {
							++edge_it;
						}
					}
					++edge_list_it;
				}
			}

			// remvoe node
			nodes_.erase(node_to_erase_it);
			return true;
		}

		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if "
				                         "they don't exist in the graph");
			}

			// find edge list of src
			auto edge_list_it = edges_.find(src);

			// find edge in edge list
			auto dst_it = nodes_.find(dst);
			auto find_edge = edge{*dst_it, weight};
			auto edge_it = edge_list_it->second.find(find_edge);

			// if edge does not exist
			if (edge_it == edge_list_it->second.end()) {
				return false;
			}

			// remove edge
			edge_list_it->second.erase(edge_it);
			return true;
		}

		auto erase_edge(iterator graph_it) -> iterator;

		auto erase_edge(iterator it_from, iterator it_to) -> iterator;

		auto clear() noexcept -> void {
			nodes_.clear();
			edges_.clear();
		}

		// ########### Accessors  ###########
		[[nodiscard]] auto is_node(N const& value) -> bool {
			auto node = nodes_.find(value);
			return node != nodes_.end();
		}

		[[nodiscard]] auto empty() -> bool {
			return nodes_.empty();
		}

		[[nodiscard]] auto is_connected(N const& src, N const& dst) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				return false;
			}

			auto edge_list_it = edges_.find(src);
			for (auto edge_it = edge_list_it->second.cbegin(); edge_it != edge_list_it->second.cend();
			     ++edge_it)
			{
				if (*(edge_it->to) == dst) {
					return true;
				}
			}
			return false;
		}

		[[nodiscard]] auto nodes() -> std::vector<N> {
			auto nodes_vec = std::vector<N>{};
			std::for_each(nodes_.cbegin(), nodes_.cend(), [&nodes_vec](auto const& n) {
				nodes_vec.push_back(*n);
			});
			return nodes_vec;
		}

		[[nodiscard]] auto weights(N const& src, N const& dst) -> std::vector<E> {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node "
				                         "don't exist in the graph");
			}

			auto weights_vec = std::vector<E>{};
			auto edge_list_it = edges_.find(src);

			for_each(edge_list_it->second.cbegin(),
			         edge_list_it->second.cend(),
			         [dst, &weights_vec](auto const& graph_edge) {
				         if (*(graph_edge.to) == dst) {
					         weights_vec.push_back(graph_edge.weight);
				         }
			         });

			return weights_vec;
		}

		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) -> iterator {
			if (!is_node(src) || !is_node(dst)) {
				return end();
			}

			// find edge list of src
			auto edge_list_it = edges_.find(src);
			if (edge_list_it->second.empty()) {
				return end();
			}

			// find edge
			auto dst_it = nodes_.find(dst);
			auto find_edge = edge{*dst_it, weight};
			auto res = edge_list_it->second.find(find_edge);
			if (res == edge_list_it->second.end()) {
				return end();
			}

			return iterator(edge_list_it, edges_.begin(), edges_.end(), res);
		}

		[[nodiscard]] auto connections(N const& src) -> std::vector<N> {
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't "
				                         "exist in the graph");
			}

			auto conn_vec = std::vector<N>{};
			auto edge_list_it = edges_.find(src);

			for_each(edge_list_it->second.cbegin(),
			         edge_list_it->second.cend(),
			         [&conn_vec](auto const& graph_edge) {
				         if (std::find(conn_vec.begin(), conn_vec.end(), *(graph_edge.to))
				             == conn_vec.end()) {
					         conn_vec.push_back(*(graph_edge.to));
				         }
			         });

			return conn_vec;
		}

		// ########### Iterator access ###########
		[[nodiscard]] auto begin() const -> iterator {
			auto edge_list_begin_it = edges_.begin();
			while (edge_list_begin_it != edges_.end() && edge_list_begin_it->second.empty()) {
				++edge_list_begin_it;
			}
			if (edge_list_begin_it == edges_.end()) {
				return iterator(edge_list_begin_it,
				                edges_.begin(),
				                edges_.end(),
				                std::set<edge, edge_comparator>().cend());
			}
			return iterator(edge_list_begin_it,
			                edges_.begin(),
			                edges_.end(),
			                edge_list_begin_it->second.begin());
		}

		[[nodiscard]] auto end() const -> iterator {
			return iterator(edges_.end(),
			                edges_.begin(),
			                edges_.end(),
			                std::set<edge, edge_comparator>().cend());
		}

		// ########### Comparisons ###########
		[[nodiscard]] auto operator==(graph const& other) const -> bool {
			// compare nodes
			auto is_equal = std::equal(
			   nodes_.cbegin(),
			   nodes_.cend(),
			   other.nodes_.cbegin(),
			   [](auto const& node, auto const& other_node) { return *node == *other_node; });

			if (!is_equal) {
				return false;
			}

			// compare edges
			auto other_edge_list_it = other.edges_.cbegin();
			for (auto edge_list_it = edges_.cbegin(); edge_list_it != edges_.cend(); ++edge_list_it) {
				if (*(edge_list_it->first) != *(other_edge_list_it->first)
				    || edge_list_it->second.size() != other_edge_list_it->second.size())
				{
					return false;
				}
				is_equal = std::equal(edge_list_it->second.cbegin(),
				                      edge_list_it->second.cend(),
				                      other_edge_list_it->second.begin(),
				                      [](auto const& graph_edge, auto const& other_graph_edge) {
					                      return *(graph_edge.to) == *(other_graph_edge.to)
					                             && graph_edge.weight == other_graph_edge.weight;
				                      });
				if (!is_equal) {
					return false;
				}
				++other_edge_list_it;
			}
			return is_equal;
		}

		// ########### Extractor ###########
		friend auto operator<<(std::ostream& ost, graph const& obj) -> std::ostream& {
			for (auto edge_list_it = obj.edges_.cbegin(); edge_list_it != obj.edges_.cend();
			     ++edge_list_it) {
				ost << *(edge_list_it->first) << " (\n";
				for (auto edge_it = edge_list_it->second.cbegin(); edge_it != edge_list_it->second.cend();
				     ++edge_it)
				{
					ost << "  " << *edge_it->to << " | " << edge_it->weight << "\n";
				}
				ost << ")\n";
			}

			return ost;
		}

		// iterator: value_type
		struct value_type {
			N from;
			N to;
			E weight;
		};

	private:
		struct edge {
			std::shared_ptr<N> to;
			E weight;
		};

		struct node_comparator {
			using is_transparent = void;

			auto operator()(std::shared_ptr<N> const& lhs, std::shared_ptr<N> const& rhs) const -> bool {
				return *lhs < *rhs;
			}

			auto operator()(std::shared_ptr<N> const& lhs, N const& rhs) const -> bool {
				return *lhs < rhs;
			}

			auto operator()(N const& lhs, std::shared_ptr<N> const& rhs) const -> bool {
				return lhs < *rhs;
			}
		};

		struct edge_comparator {
			auto operator()(const edge& lhs, const edge& rhs) const -> bool {
				if (*lhs.to == *rhs.to) {
					return lhs.weight < rhs.weight;
				}
				return *lhs.to < *rhs.to;
			}
		};

		struct edge_list_comparator {
			using is_transparent = void;

			auto operator()(std::shared_ptr<N> const& lhs, std::shared_ptr<N> const& rhs) const -> bool {
				return *lhs < *rhs;
			}

			auto operator()(std::shared_ptr<N> const& lhs, N const& rhs) const -> bool {
				return *lhs < rhs;
			}

			auto operator()(N const& lhs, std::shared_ptr<N> const& rhs) const -> bool {
				return lhs < *rhs;
			}
		};

		std::set<std::shared_ptr<N>, node_comparator> nodes_;
		std::map<std::shared_ptr<N>, std::set<edge, edge_comparator>, edge_list_comparator> edges_;

		auto swap(graph& other) -> void {
			std::swap(nodes_, other.nodes_);
			std::swap(edges_, other.edges_);
		}
	};

	template<typename N, typename E>
	class graph<N, E>::iterator {
		using graph_const_iterator =
		   typename std::map<std::shared_ptr<N>, std::set<edge, edge_comparator>, edge_list_comparator>::const_iterator;
		using edge_const_iterator = typename std::set<edge, edge_comparator>::const_iterator;

	public:
		using value_type = graph<N, E>::value_type;
		using reference = value_type;
		using pointer = void;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::bidirectional_iterator_tag;

		// Iterator constructor
		iterator() = default;

		// Iterator source
		auto operator*() -> reference {
			auto src = *(graph_it_->first);
			auto dst = *(edge_it_->to);
			auto weight = edge_it_->weight;
			return value_type{src, dst, weight};
		}

		// Iterator traversal
		auto operator++() -> iterator& {
			// end iterator
			if (graph_it_ == graph_end_) {
				return *this;
			}

			// next iterator
			++edge_it_;
			if (edge_it_ == graph_it_->second.end()) {
				// find next node
				++graph_it_;
				while (graph_it_ != graph_end_ && graph_it_->second.empty()) {
					++graph_it_;
				}

				// find next edge
				if (graph_it_ != graph_end_) {
					edge_it_ = graph_it_->second.begin();
				}
			}
			return *this;
		}

		auto operator++(int) -> iterator {
			auto copy = *this;
			++(*this);
			return copy;
		}

		auto operator--() -> iterator& {
			// empty graph case
			if (graph_begin_ == graph_end_) {
				return *this;
			}

			// first iterator case
			if (graph_it_ == graph_begin_ && edge_it_ == graph_begin_->second.begin()) {
				return *this;
			}

			// find valid previous node
			while (graph_it_ == graph_end_ || graph_it_->second.empty()) {
				--graph_it_;
				edge_it_ = graph_it_->second.end();
			}

			// previous edge
			if (edge_it_ == graph_it_->second.begin()) {
				--graph_it_;
				while (graph_it_->second.empty()) {
					--graph_it_;
				}
				edge_it_ = graph_it_->second.end();
			}
			--edge_it_;

			return *this;
		}

		auto operator--(int) -> iterator {
			auto copy = *this;
			--(*this);
			return copy;
		}

		// Iterator comparison
		auto operator==(iterator const& other) const -> bool {
			if (graph_it_ == graph_end_) {
				return other.graph_it_ == other.graph_end_;
			}
			return graph_it_ == other.graph_it_ && edge_it_ == other.edge_it_;
		}

	private:
		explicit iterator(graph_const_iterator graph_it,
		                  graph_const_iterator graph_begin,
		                  graph_const_iterator graph_end,
		                  edge_const_iterator edge_it)
		: graph_it_(graph_it)
		, graph_begin_(graph_begin)
		, graph_end_(graph_end)
		, edge_it_(edge_it) {}
		graph_const_iterator graph_it_;
		graph_const_iterator graph_begin_;
		graph_const_iterator graph_end_;
		edge_const_iterator edge_it_;
		friend class graph;
	};

	template<typename N, typename E>
	auto graph<N, E>::erase_edge(iterator graph_it) -> iterator {
		if (graph_it == end()) {
			return end();
		}

		auto curr_graph_it = graph_it.graph_it_;
		auto curr_edge_it = graph_it.edge_it_;
		auto next_it = graph_it;
		++next_it;

		edges_[curr_graph_it->first].erase(curr_edge_it);

		return next_it;
	}

	template<typename N, typename E>
	auto graph<N, E>::erase_edge(iterator it_from, iterator it_to) -> iterator {
		auto g_it = it_from;
		while (g_it != it_to) {
			g_it = erase_edge(g_it);
		}
		return g_it;
	}

} // namespace gdwg

#endif // GDWG_GRAPH_HPP
