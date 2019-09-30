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
            [[nodiscard]] inline Optional<Integral> domain() const noexcept { return Integral{node->first_attribute("domain")}; }
            [[nodiscard]] inline Optional<Integral> bus() const noexcept { return Integral{node->first_attribute("bus")}; }
            [[nodiscard]] inline Optional<String> function() const noexcept { return String{node->first_attribute("domain")}; }
            [[nodiscard]] inline std::optional<bool> multifunction() const noexcept {
                const auto mf_attr = node->first_attribute("multifunction");
                return mf_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(mf_attr->value()))} : std::nullopt;
            }
        };

        [[nodiscard]] inline Optional<Integral> unique_id() const noexcept { return Integral{node->first_attribute("unique_id")}; }
        [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
    };

    [[nodiscard]] inline std::optional<Type> protocol() const noexcept {
        const auto attr = node->first_attribute("protocol");
        return attr ? magic_enum::enum_cast<Type>(attr->value()) : std::nullopt;
    }
    [[nodiscard]] inline Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
    [[nodiscard]] inline Optional<ParentAddr> parent_addr() const noexcept { return ParentAddr{node->first_node("parent_addr")}; }
    [[nodiscard]] inline Optional<String> parent() const noexcept { return String{node->first_attribute("parent")}; }
    [[nodiscard]] inline std::optional<bool> managed() const noexcept {
        const auto attr = node->first_attribute("managed");
        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
    }
    [[nodiscard]] inline Optional<Integral> parent_wwnn() const noexcept { return Integral{node->first_attribute("parent_wwnn")}; }
    [[nodiscard]] inline Optional<Integral> parent_wwpn() const noexcept { return Integral{node->first_attribute("parent_wwpn")}; }
    [[nodiscard]] inline Optional<Integral> parent_fabric_wwn() const noexcept { return Integral{node->first_attribute("parent_fabric_wwn")}; }
    [[nodiscard]] inline Optional<Integral> wwpn() const noexcept { return Integral{node->first_attribute("wwpn")}; }
    [[nodiscard]] inline Optional<Integral> wwnn() const noexcept { return Integral{node->first_attribute("wwnn")}; }
};

}