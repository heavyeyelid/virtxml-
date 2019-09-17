#pragma once
#include <algorithm>
#include <string_view>
#include <type_traits>
#include <magic_enum.hpp>
#include "generic.hpp"
#include "xmlspan.hpp"
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
struct Bandwidth : public Node {
    struct Attributes : public Node {
        [[nodiscard]] Optional<Integral> average() const noexcept { return Integral{node->first_attribute("average")}; }
        [[nodiscard]] Optional<Integral> peak() const noexcept { return Integral{node->first_attribute("peak")}; }
        [[nodiscard]] Optional<Integral> floor() const noexcept { return Integral{node->first_attribute("floor")}; }
        [[nodiscard]] Optional<Integral> burst() const noexcept { return Integral{node->first_attribute("bur  st")}; }
    };

    [[nodiscard]] Optional<Attributes> inbound() const noexcept { return Attributes{node->first_node("inbound")}; }
    [[nodiscard]] Optional<Attributes> outbound() const noexcept { return Attributes{node->first_node("outbound")}; }
};
struct Vlan : public Node {
    struct Tag : public Node {
        enum class NativeMode {
            tagged,
            untagged,
        };
        [[nodiscard]] Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
        [[nodiscard]] std::optional<NativeMode> native_mode() const noexcept {
            const auto attr = node->first_attribute("nativeMode");
            return attr ? magic_enum::enum_cast<NativeMode>(attr->value()) : std::nullopt;
        }
    };
    [[nodiscard]] std::optional<bool> trunk() const noexcept {
        const auto attr = node->first_attribute("trunk");
        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
    }
    [[nodiscard]] NamedSpan<Tag> tags() const noexcept { return NamedSpan<Tag>{"tag", node}; }
};

} // namespace
}