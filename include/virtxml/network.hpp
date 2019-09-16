#pragma once
#include <algorithm>
#include <string_view>
#include <type_traits>
#include <magic_enum.hpp>
#include "generic.hpp"
#include "xmlval.hpp"

using namespace std::literals;

namespace virtxml {
inline namespace {

enum class MacTableManager {
    kernel,
    libvirt,
};

template <class CRTP, template <class> class Optional = std::void_t>
struct HasMacTableManager : public HasMore<CRTP, HasMacTableManager<CRTP, Optional>> {
    [[nodiscard]] std::optional<MacTableManager> mode() const noexcept {
        if constexpr (std::is_void_v<Optional<void>>)
            *magic_enum::enum_cast<MacTableManager>(this->get_node()->first_attribute("macTableManager")->value());
        const auto attr = this->get_node()->first_attribute("macTableManager");
        return attr ? magic_enum::enum_cast<MacTableManager>(attr->value()) : std::nullopt;
    }
};

struct VirtualPort : public Node {
    constexpr static std::array types{
        "802.1Qbg"sv,
        "802.1Qbh"sv,
        "openvswitch"sv,
        "midonet"sv,
    };

    struct Parameters : public Node {
        [[nodiscard]] Optional<Integral> manager_id() const noexcept { return Integral{node->first_attribute("managerid")}; }
        [[nodiscard]] Optional<Integral> type_id() const noexcept { return Integral{node->first_attribute("typeid")}; }
        [[nodiscard]] Optional<Integral> type_id_version() const noexcept { return Integral{node->first_attribute("typeidversion")}; }
        [[nodiscard]] Optional<Uuid> instance_id() const noexcept { return Uuid{node->first_attribute("instanceid")}; }
        [[nodiscard]] Optional<String> profile_id() const noexcept { return String{node->first_attribute("profileid")}; }
        [[nodiscard]] Optional<Uuid> interface_id() const noexcept { return Uuid{node->first_attribute("interfaceid")}; }
    };

    [[nodiscard]] std::optional<typename decltype(types)::const_iterator> type() const noexcept {
        const auto attr = node->first_attribute("type");
        return attr ? std::optional{std::find(types.cbegin(), types.cend(), attr->value())} : std::nullopt;
    }
    [[nodiscard]] Optional<Parameters> parameters() const noexcept { return Parameters{node->first_node("parameters")}; }
};

} // namespace
}