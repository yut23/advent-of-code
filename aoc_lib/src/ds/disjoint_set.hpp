#ifndef DISJOINT_SET_HPP_M2XYFQAC
#define DISJOINT_SET_HPP_M2XYFQAC

#include <cstddef> // for size_t
#include <map>     // for map
#include <utility> // for swap

namespace aoc::ds {

/**
 * Implementation: union-by-size with path halving
 */
class disjoint_set {
  public:
    using id_t = std::size_t;
    struct node_t {
        node_t *parent;
        std::size_t size;
        id_t id;

        explicit node_t(id_t id) : parent(this), size(1), id(id) {}
    };

    [[nodiscard]] node_t *new_set();
    node_t *find(node_t *node);
    node_t *find(id_t id);
    /// returns true if two distinct sets were merged
    bool merge(node_t *x, node_t *y);
    bool merge(id_t x, id_t y);

    std::size_t set_count() const { return m_set_count; }

  private:
    std::map<std::size_t, node_t> m_nodes;
    id_t m_next_id = 0;
    std::size_t m_set_count = 0;
};

disjoint_set::node_t *disjoint_set::new_set() {
    auto it = m_nodes.emplace_hint(m_nodes.end(), m_next_id, m_next_id);
    ++m_next_id;
    ++m_set_count;
    return &it->second;
}

// path halving
disjoint_set::node_t *disjoint_set::find(node_t *node) {
    while (node->parent != node) {
        node->parent = node->parent->parent;
        node = node->parent;
    }
    return node;
}
disjoint_set::node_t *disjoint_set::find(id_t id) {
    return find(&m_nodes.at(id));
}

// union-by-size
bool disjoint_set::merge(node_t *x, node_t *y) {
    // replace nodes by roots
    x = find(x);
    y = find(y);

    if (x == y) {
        // x and y are already in the same set
        return false;
    }

    // if necessary, swap variables to ensure that x has at least as many
    // descendants as y
    if (x->size < y->size) {
        std::swap(x, y);
    }

    // make x the new root
    y->parent = x;
    // update the size of x
    x->size += y->size;
    --m_set_count;
    return true;
}
bool disjoint_set::merge(id_t x, id_t y) {
    return merge(&m_nodes.at(x), &m_nodes.at(y));
}

} // namespace aoc::ds

#endif /* end of include guard: DISJOINT_SET_HPP_M2XYFQAC */
