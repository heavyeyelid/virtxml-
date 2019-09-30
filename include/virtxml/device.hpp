//
// Created by hugo on 15.09.19.
//

#pragma once

#include <string_view>
#include <gsl/gsl>
#include <magic_enum.hpp>
#include "generic.hpp"
#include "xmlspan.hpp"
#include "xmlval.hpp"

using namespace std::literals;

namespace virtxml {
inline namespace {

struct Device : public Node {
    struct DevNode : public StringNode {
        enum class Type { dev, link };

        [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
    };
    struct Parent : public Node {
        [[nodiscard]] inline Optional<Integral> wwnn() const noexcept { return Integral{node->first_attribute("wwnn")}; }
        [[nodiscard]] inline Optional<Integral> wwpn() const noexcept { return Integral{node->first_attribute("wwpn")}; }
        [[nodiscard]] inline Optional<Integral> fabric_wwn() const noexcept { return Integral{node->first_attribute("fabric_wwn")}; }
    };
    struct Driver : public StringNode {
        [[nodiscard]] inline Optional<String> name() const noexcept { return String{node->first_node("name")}; }
    };
    struct Capability : public Node {
        enum class Type { system, pci, usb_device, usb, net, scsi_host, scsi_target, scsi, storage, drm, mdev, ccw };
        enum class DrmType { primary, constrol, render };

        struct Hardware : public StringNode {
            [[nodiscard]] inline Optional<String> vendor() const noexcept { return String{node->first_node("vendor")}; }
            [[nodiscard]] inline Optional<String> version() const noexcept { return String{node->first_node("version")}; }
            [[nodiscard]] inline Optional<String> serial() const noexcept { return String{node->first_node("serial")}; }
            [[nodiscard]] inline Uuid uuid() const noexcept { return Uuid{node->first_node("uuid")}; }
        };
        struct Firmware : public Node {
            [[nodiscard]] inline Optional<String> vendor() const noexcept { return String{node->first_attribute("vendor")}; }
            [[nodiscard]] inline Optional<String> version() const noexcept { return String{node->first_attribute("version")}; }
            [[nodiscard]] inline Optional<String> release_date() const noexcept { return String{node->first_attribute("release_date")}; }
        };

        struct Product : public Node {
            [[nodiscard]] inline Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
            [[nodiscard]] inline Optional<gsl::czstring<>> value() const noexcept { return node->value(); }
        };
        struct Vendor : public Node {
            [[nodiscard]] inline Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
            [[nodiscard]] inline Optional<gsl::czstring<>> value() const noexcept { return node->value(); }
        };
        struct SubCapability : public Node {
            enum class Type { fc_host, vports_ops, removable, fc_remote_port, phys_function,
                virt_functions,
                pci_bridge,
                cardbus_bridge,
                mdev_types, };

            struct Address : public Node {
                [[nodiscard]] inline Optional<String> domain() const noexcept { return String{node->first_attribute("domain")}; }
                [[nodiscard]] inline Optional<String> bus() const noexcept { return String{node->first_attribute("bus")}; }
                [[nodiscard]] inline Optional<String> slot() const noexcept { return String{node->first_attribute("slot")}; }
                [[nodiscard]] inline Optional<String> function() const noexcept { return String{node->first_attribute("function")}; }
            };
            struct ECapType : public Node {
                enum class DeviceAPI { vfio_pci };

                [[nodiscard]] inline String id() const noexcept { return String{node->first_attribute("id")}; }
                [[nodiscard]] inline Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
                [[nodiscard]] inline DeviceAPI device_api() const noexcept {
                    return *magic_enum::enum_cast<DeviceAPI>(node->first_attribute("deviceAPI")->value());
                };
                [[nodiscard]] inline Integral available_instances() const noexcept { return Integral{node->first_attribute("availableInstances")}; }
            };
            struct IommuGroup : public Node {
                [[nodiscard]] inline Integral iommu_group() const noexcept { return Integral{node->first_attribute("iommuGroup")}; }
                [[nodiscard]] inline Integral address() const noexcept { return Integral{node->first_attribute("address")}; }
            };
            struct Numa : public Node {
                [[nodiscard]] inline Optional<Integral> node_() const noexcept { return Integral{node->first_attribute("node")}; }
            };
            struct PciExpress : public Node {
                struct Link : public Node {
                    enum class Validity { cap, sta };

                    [[nodiscard]] inline Validity validity() const noexcept {
                        return *magic_enum::enum_cast<Validity>(node->first_attribute("node")->value());
                    }
                    [[nodiscard]] inline Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
                    [[nodiscard]] inline Optional<Integral> speed() const noexcept { return Integral{node->first_attribute("speed")}; }
                    [[nodiscard]] inline Integral width() const noexcept { return Integral{node->first_attribute("width")}; }
                };
            };

            [[nodiscard]] inline Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
            [[nodiscard]] inline std::optional<Type> type() const noexcept {
                const auto attr = node->first_attribute("type");
                if (!attr)
                    return std::nullopt;
                return magic_enum::enum_cast<Type>(attr->value());
            }
            [[nodiscard]] inline bool media_available() const noexcept { return node->first_node("media_available")->value() == "1"sv; }
            [[nodiscard]] inline Integral media_size() const noexcept { return Integral{node->first_node("media_size")}; }
            [[nodiscard]] inline Optional<String> media_label() const noexcept { return String{node->first_node("media_label")}; }
            [[nodiscard]] inline Integral wwnn() const noexcept { return Integral{node->first_attribute("wwnn")}; }
            [[nodiscard]] inline Integral wwpn() const noexcept { return Integral{node->first_attribute("wwpn")}; }
            [[nodiscard]] inline Optional<Integral> fabric_wwn() const noexcept { return Integral{node->first_attribute("fabric_wwn")}; }
            [[nodiscard]] inline Integral max_vports() const noexcept { return Integral{node->first_attribute("max_vports")}; }
            [[nodiscard]] inline Integral vports() const noexcept { return Integral{node->first_attribute("vports")}; }
            [[nodiscard]] inline Optional<Integral> max_count() const noexcept { return Integral{node->first_attribute("maxCount")}; }
            [[nodiscard]] inline String rport() const noexcept { return String{node->first_attribute("rport")}; }
        };
        struct Link : public Node {
            enum class States { unknown, notpresent, down, lowerlayerdown, testing, dormant, up };

            [[nodiscard]] inline Optional<Integral> speed() const noexcept { return Integral{node->first_attribute("speed")}; }
            [[nodiscard]] inline States state() const noexcept { return *magic_enum::enum_cast<States>(node->first_attribute("state")->value()); }
        };
        struct Feature : public Node {
            [[nodiscard]] inline String name() const noexcept { return String{node->first_attribute("name")}; }
        };
        struct MdevType : public Node {
            [[nodiscard]] inline String id() const noexcept { return String{node->first_attribute("id")}; }
        };
        struct IommuGroup : public Node {
            [[nodiscard]] inline Integral number() const noexcept { return Integral{node->first_attribute("number")}; }
        };

        [[nodiscard]] inline Optional<MdevType> mdev_type() const noexcept { return MdevType{node->first_node("type")}; }
        [[nodiscard]] inline DrmType drm_type() const noexcept { return *magic_enum::enum_cast<DrmType>(node->first_attribute("type")->value()); }
        [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
        [[nodiscard]] inline Optional<Integral> class_() const noexcept { return Integral{node->first_node("class")}; }
        [[nodiscard]] inline Integral domain() const noexcept { return Integral{node->first_attribute("vendor")}; }
        [[nodiscard]] inline Integral bus() const noexcept { return Integral{node->first_attribute("bus")}; }
        [[nodiscard]] inline Integral slot() const noexcept { return Integral{node->first_attribute("slot")}; }
        [[nodiscard]] inline Integral function() const noexcept { return Integral{node->first_attribute("function")}; }
        [[nodiscard]] inline Integral device() const noexcept { return Integral{node->first_attribute("device")}; }
        [[nodiscard]] inline Integral number() const noexcept { return Integral{node->first_attribute("number")}; }
        [[nodiscard]] inline Integral subclass() const noexcept { return Integral{node->first_attribute("subclass")}; }
        [[nodiscard]] inline Integral protocol() const noexcept { return Integral{node->first_attribute("protocol")}; }
        [[nodiscard]] inline Optional<String> description() const noexcept { return String{node->first_attribute("description")}; }
        [[nodiscard]] inline String interface() const noexcept { return String{node->first_attribute("interface")}; }
        [[nodiscard]] inline Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
        [[nodiscard]] inline NamedSpan<Feature> features() const noexcept { return NamedSpan<Feature>{"feature", node}; }
        [[nodiscard]] inline NamedSpan<Capability> capabilities() const noexcept { return NamedSpan<Capability>{"capability", node}; }
        [[nodiscard]] inline Integral host() const noexcept { return Integral{node->first_attribute("host")}; }
        [[nodiscard]] inline Optional<Integral> unique_id() const noexcept { return Integral{node->first_attribute("type")}; }
        [[nodiscard]] inline Integral target() const noexcept { return Integral{node->first_node("target")}; }
        [[nodiscard]] inline Integral lun() const noexcept { return Integral{node->first_node("lun")}; }
        [[nodiscard]] inline Optional<Capability> capability() const noexcept { return Capability{node->first_node("capability")}; };
        [[nodiscard]] inline String block() const noexcept { return String{node->first_node("block")}; }
        [[nodiscard]] inline Optional<String> drive_type() const noexcept { return String{node->first_node("drive_type")}; }
        [[nodiscard]] inline Optional<String> model() const noexcept { return String{node->first_node("model")}; }
        [[nodiscard]] inline Optional<String> vendor() const noexcept { return String{node->first_node("vendor")}; }
        [[nodiscard]] inline Optional<String> serial() const noexcept { return String{node->first_node("serial")}; }
        [[nodiscard]] inline Optional<Integral> logical_block_size() const noexcept { return Integral{node->first_node("logical_block_size")}; }
        [[nodiscard]] inline Optional<Integral> num_blocks() const noexcept { return Integral{node->first_node("num_blocks")}; }
        [[nodiscard]] inline Integral size() const noexcept { return Integral{node->first_node("size")}; }
    };

    [[nodiscard]] inline String name() const noexcept { return String{node->first_node("name")}; }
    [[nodiscard]] inline Optional<String> path() const noexcept { return String{node->first_node("path")}; }
    [[nodiscard]] inline Optional<DevNode> dev_node() const noexcept { return DevNode{node->first_node("devnode")}; }
    [[nodiscard]] inline Optional<Parent> parent() const noexcept { return Parent{node->first_node("parent")}; }
    [[nodiscard]] inline Optional<Capability> capability() const noexcept { return Capability{node->first_node("capability")}; }
    [[nodiscard]] inline Optional<String> product() const noexcept { return String{node->first_node("product")}; }
}; // namespace
} // namespace
}