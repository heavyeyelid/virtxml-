#pragma once

#include <rapidxml_ns.hpp>

namespace virtxml {
using namespace rapidxml_ns;

template <typename T> using Optional = T;

template <class, class> struct HasMore;

struct Node {
    constexpr Node(xml_node<>* node) : node(node) {}
#if 1 /* With functions */
    constexpr explicit operator bool() const noexcept { return node != nullptr; }
#else /* With paramexpr */
    using operator bool(this s) = s.item != nullptr;
#endif

    template <class, class> friend struct HasMore;

  protected:
    xml_node<>* node;
};

template <class FCRTP, class CRTP> struct HasMore {
    [[nodiscard]] inline const xml_node<>* get_node() const noexcept {
        return static_cast<const Node&>(static_cast<const FCRTP&>(static_cast<const CRTP&>(*this))).node;
    }
};
} // namespace virtxml