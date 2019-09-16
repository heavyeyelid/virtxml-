//
// Created by hugo on 15.09.19.
//

#pragma once

#include <magic_enum.hpp>
#include "generic.hpp"
#include "xmlval.hpp"

namespace virtxml {
inline namespace {

struct Device : public Node {
    struct DevNode : public StringNode {
        enum class Type { dev, link };
        [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
    };
    struct Parent : public Node {
        [[nodiscard]] Optional<Integral> wwnn() const noexcept { return Integral{node->first_attribute("wwnn")}; }
        [[nodiscard]] Optional<Integral> wwpn() const noexcept { return Integral{node->first_attribute("wwpn")}; }
        [[nodiscard]] Optional<Integral> fabric_wwn() const noexcept { return Integral{node->first_attribute("fabric_wwn")}; }
    };
    struct Driver : public StringNode {
        [[nodiscard]] Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
    };
    struct Capability : public Node {
        enum class Type { system, pci, usb_device, usb, net, scsi_host, scsi_target, scsi, storage, drm, mdev, ccw };
        enum class DrmType { primary, constrol, render };

        struct MdevType : public Node {
            [[nodiscard]] String id() const noexcept { return String{node->first_attribute("id")}; }
        };
        struct IommuGroup : public Node {
            [[nodiscard]] Integral number() const noexcept { return Integral{node->first_attribute("number")}; }
        };

        [[nodiscard]] Optional<MdevType> mdev_type() const noexcept { return MdevType{node->first_node("type")}; }
        [[nodiscard]] DrmType drm_type() const noexcept { return *magic_enum::enum_cast<DrmType>(node->first_attribute("type")->value()); }
    };

    [[nodiscard]] String name() const noexcept { return String{node->first_attribute("name")}; }
    [[nodiscard]] Optional<String> path() const noexcept { return String{node->first_attribute("path")}; }
};

} // namespace
}