#pragma once
#include <magic_enum.hpp>
#include "xmlval.hpp"

namespace virtxml {

struct Adapter : public Node {
    enum class Type {
        scsi_host,
        fc_host,
    };

    struct ParentAddr : public Node {
        struct Address : public Node {
            [[nodiscard]] Optional<Integral> domain() const noexcept { return Integral{node->first_attribute("domain")}; }
            [[nodiscard]] Optional<Integral> bus() const noexcept { return Integral{node->first_attribute("bus")}; }
            [[nodiscard]] Optional<String> function() const noexcept { return String{node->first_attribute("domain")}; }
            [[nodiscard]] std::optional<bool> multifunction() const noexcept {
                const auto mf_attr = node->first_attribute("multifunction");
                return mf_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(mf_attr->value()))} : std::nullopt;
            }
        };

        [[nodiscard]] Optional<Integral> unique_id() const noexcept { return Integral{node->first_attribute("unique_id")}; }
        [[nodiscard]] Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
    };

    [[nodiscard]] std::optional<Type> protocol() const noexcept {
        const auto attr = node->first_attribute("protocol");
        return attr ? magic_enum::enum_cast<Type>(attr->value()) : std::nullopt;
    }
    [[nodiscard]] Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
    [[nodiscard]] Optional<ParentAddr> parent_addr() const noexcept { return ParentAddr{node->first_node("parent_addr")}; }
    [[nodiscard]] Optional<String> parent() const noexcept { return String{node->first_attribute("parent")}; }
    [[nodiscard]] std::optional<bool> managed() const noexcept {
        const auto attr = node->first_attribute("managed");
        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
    }
    [[nodiscard]] Optional<Integral> parent_wwnn() const noexcept { return Integral{node->first_attribute("parent_wwnn")}; }
    [[nodiscard]] Optional<Integral> parent_wwpn() const noexcept { return Integral{node->first_attribute("parent_wwpn")}; }
    [[nodiscard]] Optional<Integral> parent_fabric_wwn() const noexcept { return Integral{node->first_attribute("parent_fabric_wwn")}; }
    [[nodiscard]] Optional<Integral> wwpn() const noexcept { return Integral{node->first_attribute("wwpn")}; }
    [[nodiscard]] Optional<Integral> wwnn() const noexcept { return Integral{node->first_attribute("wwnn")}; }
};

}