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

        struct System : public Node {
            struct Hardware : public StringNode {
                [[nodiscard]] Optional<String> vendor() const noexcept { return String{node->first_attribute("vendor")}; }
                [[nodiscard]] Optional<String> version() const noexcept { return String{node->first_attribute("version")}; }
                [[nodiscard]] Optional<String> serial() const noexcept { return String{node->first_attribute("serial")}; }
                [[nodiscard]] Uuid uuid() const noexcept { return Uuid{node->first_attribute("uuid")}; }
            };
            struct Firmware : public Node {
                [[nodiscard]] Optional<String> vendor() const noexcept { return String{node->first_attribute("vendor")}; }
                [[nodiscard]] Optional<String> version() const noexcept { return String{node->first_attribute("version")}; }
                [[nodiscard]] Optional<String> release_date() const noexcept { return String{node->first_attribute("release_date")}; }
            };

            [[nodiscard]] Optional<String> product() const noexcept { return String{node->first_attribute("product")}; }
        };
        struct PciDev : public Node {
            enum class Type { pci };

            struct Product : public Node {
                [[nodiscard]] Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                [[nodiscard]] Optional<gsl::czstring<>> value() const noexcept { return node->value(); }
            };
            struct Vendor : public Node {
                [[nodiscard]] Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                [[nodiscard]] Optional<gsl::czstring<>> value() const noexcept { return node->value(); }
            };
            struct Capability : public Node {
                enum class CapType {
                    phys_function,
                    virt_functions,
                    pci_bridge,
                    cardbus_bridge,
                    mdev_types,
                };

                struct Address : public Node {
                    [[nodiscard]] Optional<String> domain() const noexcept { return String{node->first_attribute("domain")}; }
                    [[nodiscard]] Optional<String> bus() const noexcept { return String{node->first_attribute("bus")}; }
                    [[nodiscard]] Optional<String> slot() const noexcept { return String{node->first_attribute("slot")}; }
                    [[nodiscard]] Optional<String> function() const noexcept { return String{node->first_attribute("function")}; }
                };
                struct ECapType : public Node {
                    enum class DeviceAPI { vfio_pci };

                    [[nodiscard]] String id() const noexcept { return String{node->first_attribute("id")}; }
                    [[nodiscard]] Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
                    [[nodiscard]] DeviceAPI device_api() const noexcept {
                        return *magic_enum::enum_cast<DeviceAPI>(node->first_attribute("deviceAPI")->value());
                    };
                    [[nodiscard]] Integral available_instances() const noexcept { return Integral{node->first_attribute("availableInstances")}; }
                };
                struct IommuGroup : public Node {
                    [[nodiscard]] Integral iommu_group() const noexcept { return Integral{node->first_attribute("iommuGroup")}; }
                    [[nodiscard]] Integral address() const noexcept { return Integral{node->first_attribute("address")}; }
                };
                struct Numa : public Node {
                    [[nodiscard]] Optional<Integral> node_() const noexcept { return Integral{node->first_attribute("node")}; }
                };
                struct PciExpress : public Node {
                    struct Link : public Node {
                        enum class Validity { cap, sta };

                        [[nodiscard]] Validity validity() const noexcept {
                            return *magic_enum::enum_cast<Validity>(node->first_attribute("node")->value());
                        }
                        [[nodiscard]] Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
                        [[nodiscard]] Optional<Integral> speed() const noexcept { return Integral{node->first_attribute("speed")}; }
                        [[nodiscard]] Integral width() const noexcept { return Integral{node->first_attribute("width")}; }
                    };
                };

                [[nodiscard]] Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
                [[nodiscard]] std::optional<CapType> type() const noexcept {
                    const auto attr = node->first_attribute("type");
                    if (!attr)
                        return std::nullopt;
                    return magic_enum::enum_cast<CapType>(attr->value());
                }
                [[nodiscard]] Optional<Integral> max_count() const noexcept { return Integral{node->first_attribute("maxCount")}; }
            };

            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] Optional<Integral> class_() const noexcept { return Integral{node->first_attribute("class")}; }
            [[nodiscard]] Integral domain() const noexcept { return Integral{node->first_attribute("vendor")}; }
            [[nodiscard]] Integral bus() const noexcept { return Integral{node->first_attribute("bus")}; }
            [[nodiscard]] Integral slot() const noexcept { return Integral{node->first_attribute("slot")}; }
            [[nodiscard]] Integral function() const noexcept { return Integral{node->first_attribute("function")}; }
        };
        struct UsbDev : public Node {
            enum class Type { usb_device };

            struct Product : public Node {
                [[nodiscard]] Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                [[nodiscard]] Optional<gsl::czstring<>> value() const noexcept { return node->value(); }
            };
            struct Vendor : public Node {
                [[nodiscard]] Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                [[nodiscard]] Optional<gsl::czstring<>> value() const noexcept { return node->value(); }
            };

            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] Integral bus() const noexcept { return Integral{node->first_attribute("bus")}; }
            [[nodiscard]] Integral device() const noexcept { return Integral{node->first_attribute("device")}; }
        };
        struct UsbInterface : public Node {
            enum class Type { usb };

            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] Integral number() const noexcept { return Integral{node->first_attribute("number")}; }
            [[nodiscard]] Integral class_() const noexcept { return Integral{node->first_attribute("class")}; }
            [[nodiscard]] Integral subclass() const noexcept { return Integral{node->first_attribute("subclass")}; }
            [[nodiscard]] Integral protocol() const noexcept { return Integral{node->first_attribute("protocol")}; }
            [[nodiscard]] Optional<String> description() const noexcept { return String{node->first_attribute("description")}; }
        };
        struct Net : public Node {
            enum class Type { net };

            struct Link : public Node {
                enum class States { unknown, notpresent, down, lowerlayerdown, testing, dormant, up };

                [[nodiscard]] Optional<Integral> speed() const noexcept { return Integral{node->first_attribute("speed")}; }
                [[nodiscard]] States state() const noexcept { return *magic_enum::enum_cast<States>(node->first_attribute("state")->value()); }
            };
            struct Feature : public Node {
                [[nodiscard]] String name() const noexcept { return String{node->first_attribute("name")}; }
            };
            struct Capability : public Node {
                [[nodiscard]] Integral type() const noexcept { return Integral{node->first_attribute("type")}; }
            };

            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] String interface() const noexcept { return String{node->first_attribute("interface")}; }
            [[nodiscard]] Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
            [[nodiscard]] NamedSpan<Feature> features() const noexcept { return NamedSpan<Feature>{"feature", node}; }
            [[nodiscard]] NamedSpan<Capability> capabilities() const noexcept { return NamedSpan<Capability>{"capability", node}; }
        };
        struct CapScsiHost : public Node {
            enum class Type { scsi_host };

            struct Capability : public Node {
                enum class Type { fc_host, vports_ops };

                [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                [[nodiscard]] Integral wwnn() const noexcept { return Integral{node->first_attribute("wwnn")}; }
                [[nodiscard]] Integral wwpn() const noexcept { return Integral{node->first_attribute("wwpn")}; }
                [[nodiscard]] Optional<Integral> fabric_wwn() const noexcept { return Integral{node->first_attribute("fabric_wwn")}; }
                [[nodiscard]] Integral max_vports() const noexcept { return Integral{node->first_attribute("max_vports")}; }
                [[nodiscard]] Integral vports() const noexcept { return Integral{node->first_attribute("vports")}; }
            };

            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] Integral host() const noexcept { return Integral{node->first_attribute("host")}; }
            [[nodiscard]] Optional<Integral> unique_id() const noexcept { return Integral{node->first_attribute("type")}; }
            [[nodiscard]] NamedSpan<Capability> capabilities() const noexcept { return NamedSpan<Capability>{"capability", node}; }
        };
        struct CapsScsiTarget : public Node {
            enum class Type { scsi_target };

            struct Capability : public Node {
                enum class Type { fc_remote_port };

                [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                [[nodiscard]] String rport() const noexcept { return String{node->first_attribute("rport")}; }
                [[nodiscard]] Integral wwpn() const noexcept { return Integral{node->first_attribute("wwpn")}; }
            };

            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] String target() const noexcept { return String{node->first_attribute("target")}; }
            [[nodiscard]] Optional<Capability> capability() const noexcept { return Capability{node->first_node("capability")}; };
        };
        struct CapScsi : public Node {
            //            enum class Type { scsi };

            //            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] Integral host() const noexcept { return Integral{node->first_node("host")}; }
            [[nodiscard]] Integral bus() const noexcept { return Integral{node->first_node("bus")}; }
            [[nodiscard]] Integral target() const noexcept { return Integral{node->first_node("target")}; }
            [[nodiscard]] Integral lun() const noexcept { return Integral{node->first_node("lun")}; }
            [[nodiscard]] String type() const noexcept { return String{node->first_node("type")}; }
        };
        struct CapStorage : public Node {
            enum class Type { storage };

            struct Capability : public Node {
                enum class Type { removable };

                [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                [[nodiscard]] bool media_available() const noexcept { return node->first_node("media_available")->value() == "1"sv; }
                [[nodiscard]] Integral media_size() const noexcept { return Integral{node->first_node("media_size")}; }
                [[nodiscard]] Optional<String> media_label() const noexcept { return String{node->first_node("media_label")}; }
            };

            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] String block() const noexcept { return String{node->first_node("block")}; }
            [[nodiscard]] Optional<String> bus() const noexcept { return String{node->first_node("bus")}; }
            [[nodiscard]] Optional<String> drive_type() const noexcept { return String{node->first_node("drive_type")}; }
            [[nodiscard]] Optional<String> model() const noexcept { return String{node->first_node("model")}; }
            [[nodiscard]] Optional<String> vendor() const noexcept { return String{node->first_node("vendor")}; }
            [[nodiscard]] Optional<String> serial() const noexcept { return String{node->first_node("serial")}; }
            [[nodiscard]] Optional<Capability> capability() const noexcept { return Capability{node->first_node("capability")}; }
            [[nodiscard]] Optional<Integral> logical_block_size() const noexcept { return Integral{node->first_node("logical_block_size")}; }
            [[nodiscard]] Optional<Integral> num_blocks() const noexcept { return Integral{node->first_node("num_blocks")}; }
            [[nodiscard]] Integral size() const noexcept { return Integral{node->first_node("size")}; }
        };
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
}; // namespace
} // namespace
}