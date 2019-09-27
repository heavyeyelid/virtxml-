#pragma once

#include <string_view>
#include <gsl/gsl>
#include <magic_enum.hpp>
#include <rapidxml_ns.hpp>
#include "cpu_types.hpp"
#include "basic.hpp"
#include "generic.hpp"
#include "network.hpp"
#include "storage.hpp"
#include "xmlspan.hpp"
#include "xmlval.hpp"

namespace virtxml {
inline namespace {

enum class StartupPolicy {
    mandatory,
    requisite,
    optional,
};

enum class SgIO {
    filtered,
    unfiltered,
};

struct Boot : public Node {
    [[nodiscard]] Integral order() const noexcept { return Integral{node->first_attribute("order")}; }
    [[nodiscard]] Optional<String> loadparm() const noexcept { return String{node->first_attribute("loadparm")}; }
};

struct Rom : public Node {
    [[nodiscard]] std::optional<bool> bar() const noexcept {
        const auto attr = node->first_attribute("bar");
        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
    }
    [[nodiscard]] Optional<String> file() const noexcept { return String{node->first_attribute("file")}; }
};
struct Reconnect : public Node {
    [[nodiscard]] bool enabled() const noexcept { return static_cast<bool>(*magic_enum::enum_cast<YesNo>(node->first_attribute("enabled"))); }
    [[nodiscard]] Optional<Integral> timeout() const noexcept { return Integral{node->first_attribute("timeout")}; }
};

struct Address : public Node {
    enum class Type {
        pci,
        drive,
        virtio_serial,
        ccid,
        usb,
        spapr_vio,
        virtio_mmio,
        ccw,
        isa,
        dimm,
    };

    [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
    [[nodiscard]] Optional<Integral> domain() const noexcept { return Integral{node->first_attribute("domain")}; }
    [[nodiscard]] Optional<Integral> bus() const noexcept { return Integral{node->first_attribute("bus")}; }
    [[nodiscard]] Optional<String> function() const noexcept { return String{node->first_attribute("domain")}; }
    [[nodiscard]] std::optional<bool> multifunction() const noexcept {
        const auto mf_attr = node->first_attribute("multifunction");
        return mf_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(mf_attr->value()))} : std::nullopt;
    }
    [[nodiscard]] Optional<Integral> controller() const noexcept { return Integral{node->first_attribute("controller")}; }
    [[nodiscard]] Optional<Integral> target() const noexcept { return Integral{node->first_attribute("target")}; }
    [[nodiscard]] Optional<Integral> unit() const noexcept { return Integral{node->first_attribute("unit")}; }
    [[nodiscard]] Optional<Integral> drive_controller() const noexcept { return Integral{node->first_attribute("controller")}; }
    [[nodiscard]] Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
    [[nodiscard]] Optional<Integral> reg() const noexcept { return Integral{node->first_attribute("reg")}; }
    [[nodiscard]] Optional<Integral> cssid() const noexcept { return Integral{node->first_attribute("cssid")}; }
    [[nodiscard]] Optional<Integral> ssid() const noexcept { return Integral{node->first_attribute("ssid")}; }
    [[nodiscard]] Optional<Integral> devno() const noexcept { return Integral{node->first_attribute("devno")}; }
    [[nodiscard]] Optional<Integral> iobase() const noexcept { return Integral{node->first_attribute("iobase")}; }
    [[nodiscard]] Optional<Integral> irq() const noexcept { return Integral{node->first_attribute("irq")}; }
    [[nodiscard]] Optional<Integral> slot() const noexcept { return Integral{node->first_attribute("slot")}; }
    [[nodiscard]] Optional<Integral> base() const noexcept { return Integral{node->first_attribute("base")}; }
};

struct Domain : private Node {
  public:
    enum class Type {
        qemu,
        kqemu,
        kvm,
        xen,
        lxc,
        uml,
        openvz,
        test,
        vmware,
        hyperv,
        vbox,
        phyp,
        vz,
        bhyve,
    };
    enum class OffOption {
        destroy,
        restart,
        preserve,
        rename_restart, // -
    };
    enum class CrashOption {
        destroy,
        restart,
        preserve,
        rename_restart,
        coredump_destroy, // -
        coredump_restart, // -
    };
    enum class LockFailureOption {
        poweroff,
        restart,
        pause,
        ignore,
    };

    struct Cpu : public Node, public cpu::HasMode<Cpu, Optional>, public cpu::HasMatch<Cpu, Optional>, public cpu::HasCheck<Cpu, Optional> {
        [[nodiscard]] Optional<cpu::Model> model() const noexcept { return cpu::Model{node->first_node("model")}; }
        [[nodiscard]] Optional<String> vendor() const noexcept { return String{node->first_node("vendor")}; }
        [[nodiscard]] Optional<cpu::Topology> topology() const noexcept { return cpu::Topology{node->first_node("topology")}; }
        [[nodiscard]] NamedSpan<cpu::Feature> features() const noexcept { return NamedSpan<cpu::Feature>{"feature", node}; }
        [[nodiscard]] Optional<NamedSpan<cpu::NumaCell>> numa() const noexcept { return NamedSpan<cpu::NumaCell>{"cell", node->first_node("numa")}; }
        [[nodiscard]] Optional<cpu::Cache> cache() const noexcept { return cpu::Cache{node->first_node("cache")}; }
    };
    struct Sysinfo : public Node {
        struct BiosEntry : public StringNode {
            enum class Name {
                vendor,
                version,
                date,
                release,
            };

            [[nodiscard]] Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
        };
        struct SystemEntry : public StringNode {
            enum class Name {
                manufacturer,
                product,
                version,
                serial,
                uuid,
                sku,
                family,
            };

            [[nodiscard]] Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
        };
        struct BaseboardEntry : public StringNode {
            enum class Name {
                manufacturer,
                product,
                version,
                serial,
                asset,
                location,
            };

            [[nodiscard]] Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
        };

        [[nodiscard]] std::string_view type() const noexcept { return "smbios"; } // Attribute always has this value
        [[nodiscard]] Optional<NamedSpan<BiosEntry>> bios() const noexcept { return NamedSpan<BiosEntry>{"entry", node->first_node("bios")}; }
        [[nodiscard]] Optional<NamedSpan<SystemEntry>> system() const noexcept { return NamedSpan<SystemEntry>{"entry", node->first_node("system")}; }
        [[nodiscard]] NamedSpan<BaseboardEntry> baseboard() const noexcept {
            return NamedSpan<BaseboardEntry>{"entry", node->first_node("baseBoard")};
        }
    };
    struct Os : public Node {
        enum class TypeValue {
            xen,
        // Who thought it was a good idea to name a compiler-wide macro called "linux" ?
#ifdef linux
#define linux_def linux
#undef linux
#endif
            linux,
#ifdef linux_def
#define linux linux_def
#undef linux_def
#endif
            hvm,
            exe,
        };

        struct Type : public Node {
            [[nodiscard]] std::optional<cpu::Arch> arch() const noexcept {
                const auto arch_attr = node->first_attribute("arch");
                return arch_attr ? std::optional{*magic_enum::enum_cast<cpu::Arch>(arch_attr->value())} : std::nullopt;
            }
            [[nodiscard]] Optional<String> machine() const noexcept { return String{node->first_attribute("machine")}; }
            explicit operator TypeValue() const noexcept { return *magic_enum::enum_cast<TypeValue>(node->value()); }
        };
        struct HvmLoader : public StringNode {
            enum class Type {
                rom,
                pflash,
            };

            [[nodiscard]] std::optional<bool> readonly() const noexcept {
                const auto ro_attr = node->first_attribute("readonly");
                return ro_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(ro_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] std::optional<bool> secure() const noexcept {
                const auto ro_attr = node->first_attribute("secure");
                return ro_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(ro_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] std::optional<Type> type() const noexcept {
                const auto ro_attr = node->first_attribute("type");
                return ro_attr ? std::optional{*magic_enum::enum_cast<Type>(ro_attr->value())} : std::nullopt;
            }
        };
        struct HvmNvram : public StringNode {
            [[nodiscard]] Optional<String> template_() const noexcept { return String{node->first_attribute("template")}; }
        };
        struct Boot : public Node {
            enum class Device { hd, fd, cdrom, network };

            [[nodiscard]] Device dev() const noexcept { return *magic_enum::enum_cast<Device>(node->first_attribute("dev")->value()); }
        };
        struct HvmBootMenu : public Node {
            [[nodiscard]] bool enable() const noexcept { return static_cast<bool>(*magic_enum::enum_cast<YesNo>(node->first_attribute("enable"))); }

            [[nodiscard]] Optional<Integral> timeout() const noexcept { return Integral{node->first_attribute("timeout")}; }
        };
        struct HvmSMBIOS : public Node {
            enum class Mode {
                emulate,
                host,
                sysinfo,
            };

            [[nodiscard]] Mode mode() const noexcept { return *magic_enum::enum_cast<Mode>(node->first_attribute("mode")->value()); }
        };
        struct HvmBIOS : public Node {
            [[nodiscard]] std::optional<bool> use_serial() const noexcept {
                const auto us_attr = node->first_attribute("useserial");
                return us_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(us_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] Optional<Integral> reboot_timeout() const noexcept { return Integral{node->first_attribute("rebootTimeout")}; }
        };
        struct HvmAcpiTable : public StringNode {
            [[nodiscard]] constexpr std::string_view type() const noexcept { return "slic"; }
        };
        struct ExeInit : public StringNode {};
        struct ExeInitArg : public StringNode {};
        struct ExeInitEnv : public StringNode {
            [[nodiscard]] String name() const noexcept { return String{node->first_attribute("name")}; }
        };
        struct ExeInitDir : public StringNode {};
        struct ExeInitUser : public StringNode {};
        struct ExeInitGroup : public StringNode {};

        [[nodiscard]] Type type() const noexcept { return Type{node->first_node("type")}; }
        [[nodiscard]] Optional<HvmLoader> hvm_loader() const noexcept { return HvmLoader{node->first_node("loader")}; }
        [[nodiscard]] Optional<HvmNvram> hvm_nvram() const noexcept { return HvmNvram{node->first_node("nvram")}; }
        [[nodiscard]] Optional<HvmBootMenu> hvm_boot_menu() const noexcept { return HvmBootMenu{node->first_node("bootmenu")}; }
        [[nodiscard]] Optional<HvmSMBIOS> hvm_smbios() const noexcept { return HvmSMBIOS{node->first_node("smbios")}; }
        [[nodiscard]] Optional<HvmBIOS> hvm_bios() const noexcept { return HvmBIOS{node->first_node("bios")}; }
        [[nodiscard]] Optional<NamedSpan<HvmAcpiTable>> hvm_acpi() const noexcept {
            return NamedSpan<HvmAcpiTable>{"table", node->first_node("acpi")};
        }
        [[nodiscard]] Optional<String> kernel() const noexcept { return String{node->first_node("kernel")}; }
        [[nodiscard]] Optional<String> initrd() const noexcept { return String{node->first_node("initrd")}; }
        [[nodiscard]] Optional<String> root() const noexcept { return String{node->first_node("root")}; }
        [[nodiscard]] Optional<String> cmdline() const noexcept { return String{node->first_node("cmdline")}; }
        [[nodiscard]] Optional<String> dtb() const noexcept { return String{node->first_node("dtb")}; }
        [[nodiscard]] NamedSpan<Boot> boots() const noexcept { return NamedSpan<Boot>{"boot", node}; }
        [[nodiscard]] Optional<ExeInit> exe_init() const noexcept { return ExeInit{node->first_node("init")}; }
        [[nodiscard]] NamedSpan<ExeInitArg> exe_init_args() const noexcept { return NamedSpan<ExeInitArg>{"initarg", node}; }
        [[nodiscard]] NamedSpan<ExeInitEnv> exe_init_env() const noexcept { return NamedSpan<ExeInitEnv>{"initenv", node}; }
        [[nodiscard]] Optional<ExeInitDir> exe_init_dir() const noexcept { return ExeInitDir{node->first_node("initdir")}; }
        [[nodiscard]] Optional<ExeInitUser> exe_init_user() const noexcept { return ExeInitUser{node->first_node("inituser")}; }
        [[nodiscard]] Optional<ExeInitGroup> exe_init_group() const noexcept { return ExeInitGroup{node->first_node("initgroup")}; }
    };
    struct Bootloader : public StringNode {};
    struct BootloaderArgs : public StringNode {};
    struct Clock : public Node {
        enum class Offset {
            localtime,
            utc,
            timezone,
            variable,
        };
        enum class Basis {
            utc,
            localtime,
        };

        struct Timer : public Node {
            enum class Name {
                platform,
                rtc,
                tsc,
                hpet,
                pit,
                kvmclock,
                hypervclock,
            };
            enum class Track {
                boot,
                guest,
                wall,
            };
            enum class TickPolicy {
                delay,
                merge,
                discard,
                catchup,
            };
            enum class Mode {
                auto_,
                native,
                emulate,
                paravirt,
                smpsafe,
            };

            struct Catchup : public Node {
                [[nodiscard]] Optional<Integral> threshold() const noexcept { return Integral{node->first_attribute("threshold")}; }
                [[nodiscard]] Optional<Integral> slew() const noexcept { return Integral{node->first_attribute("slew")}; }
                [[nodiscard]] Optional<Integral> limit() const noexcept { return Integral{node->first_attribute("limit")}; }
            };

            [[nodiscard]] Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
            [[nodiscard]] std::optional<Track> track() const noexcept {
                const auto track_attr = node->first_attribute("track");
                return track_attr ? magic_enum::enum_cast<Track>(track_attr->value()) : std::nullopt;
            }
            [[nodiscard]] std::optional<TickPolicy> tick_policy() const noexcept {
                const auto track_attr = node->first_attribute("tickpolicy");
                return track_attr ? magic_enum::enum_cast<TickPolicy>(track_attr->value()) : std::nullopt;
            }
            [[nodiscard]] Optional<Catchup> catchup() const noexcept { return Catchup{node->first_node("catchup")}; }
            [[nodiscard]] Optional<Integral> frequency() const noexcept { return Integral{node->first_attribute("frequency")}; }
            [[nodiscard]] std::optional<Mode> mode() const noexcept {
                const auto mode_attr = node->first_attribute("mode");
                return mode_attr ? magic_enum::enum_cast<Mode>(mode_attr->value()) : std::nullopt;
            }
            [[nodiscard]] std::optional<bool> present() const noexcept {
                const auto pr_attr = node->first_attribute("present");
                return pr_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(pr_attr->value()))} : std::nullopt;
            }
        };

        [[nodiscard]] Offset offset() const noexcept { return *magic_enum::enum_cast<Offset>(node->first_attribute("offset")->value()); }
        [[nodiscard]] Optional<String> adjustment() const noexcept { return String{node->first_attribute("adjustment")}; }
        [[nodiscard]] Optional<String> timezone() const noexcept { return String{node->first_attribute("timezone")}; }
        [[nodiscard]] std::optional<Basis> basis() const noexcept {
            const auto basis_attr = node->first_attribute("basis");
            return basis_attr ? magic_enum::enum_cast<Basis>(basis_attr->value()) : std::nullopt;
        }

        [[nodiscard]] NamedSpan<Timer> timers() const noexcept { return NamedSpan<Timer>{"timer", node}; }
    };
    struct Perf : public Node {
        struct Event : public Node {
            enum class Name {
                cmt,
                mbmt,
                mbml,
                cpu_cycles,
                instructions,
                cache_references,
                cache_misses,
                branch_instructions,
                branch_misses,
                bus_cycles,
                stalled_cycles_frontend,
                stalled_cycles_backend,
                ref_cpu_cycles,
                cpu_clock,
                task_clock,
                page_faults,
                context_switches,
                cpu_migrations,
                page_faults_min,
                page_faults_maj,
                alignment_faults,
                emulation_faults,
            };

            [[nodiscard]] Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
            [[nodiscard]] bool enabled() const noexcept {
                return static_cast<bool>(*magic_enum::enum_cast<YesNo>(node->first_attribute("enabled")->value()));
            }
        };

        [[nodiscard]] NamedSpan<Event> events() const noexcept { return NamedSpan<Event>{"event", node}; }
    };

    struct Devices : public Node {
        enum class QemuCharDevType {
            dev,
            file,
            pipe,
            unix_,
            tcp,
            udp,
            null,
            stdio,
            vc,
            pty,
            spicevmc,
            spiceport,
            nmdm,
        };

        struct Emulator : public StringNode {};
        struct SecLabel : public Node {
            [[nodiscard]] Optional<String> model() const noexcept { return String{node->first_attribute("model")}; }
            [[nodiscard]] std::optional<bool> relabel() const noexcept {
                const auto rlb_attr = node->first_attribute("relabel");
                return rlb_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(rlb_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] std::optional<bool> labelskip() const noexcept {
                const auto lbs_attr = node->first_attribute("labelskip");
                return lbs_attr ? std::optional{static_cast<bool>(magic_enum::enum_cast<YesNo>(lbs_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] NamedSpan<StringNode> labels() const noexcept { return NamedSpan<StringNode>{"label", node}; }
        };
        struct Disk : public Node {
            enum class Device {
                floppy,
                disk,
                cdrom,
                lun,
            };
            enum class Type {
                file,
                block,
                dir,
                network,
                volume,
            };
            struct Auth : public Node {
                struct Secret : public Node {
                    enum class Type {
                        ceph,
                        iscsi,
                    };

                    [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                    [[nodiscard]] Optional<Uuid> uuid() const noexcept { return Uuid{node->first_attribute("uuid")}; }
                    [[nodiscard]] String usage() const noexcept { return String{node->first_attribute("usage")}; }
                };

                [[nodiscard]] String username() const noexcept { return String{node->first_attribute("username")}; }
                [[nodiscard]] Secret secret() const noexcept { return Secret{node->first_node("secret")}; }
            };
            struct Snapshot : public Node {
                [[nodiscard]] String name() const noexcept { return String{node->first_attribute("name")}; }
            };
            struct Source : public Node {
                enum class Mode {
                    host,
                    direct,
                };
                enum class Protocol {
                    rbd,
                    iscsi,
                    http,
                    https,
                    sheepdog,
                    ftp,
                    ftps,
                    tftp,
                    nbd,
                    gluster,
                    vxhs,
                };

                struct Host : public Node {
                    enum class Transport {
                        tcp,
                        rdma,
                        unix_,
                    };

                    [[nodiscard]] std::optional<Transport> transport() const noexcept {
                        const auto transport_attr = node->first_attribute("transport");
                        return transport_attr ? magic_enum::enum_cast<Transport>(transport_attr->value()) : std::nullopt;
                    }
                    [[nodiscard]] Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
                    [[nodiscard]] Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
                    [[nodiscard]] Optional<String> socket() const noexcept { return String{node->first_attribute("socket")}; }
                };
                struct Config : public Node {
                    [[nodiscard]] String file() const noexcept { return String{node->first_attribute("file")}; }
                };

                [[nodiscard]] Optional<String> file() const noexcept { return String{node->first_attribute("file")}; }
                [[nodiscard]] Optional<String> dev() const noexcept { return String{node->first_attribute("dev")}; }
                [[nodiscard]] Optional<String> dir() const noexcept { return String{node->first_attribute("dir")}; }
                [[nodiscard]] Optional<String> pool() const noexcept { return String{node->first_attribute("pool")}; }
                [[nodiscard]] Optional<String> volume() const noexcept { return String{node->first_attribute("volume")}; }
                [[nodiscard]] std::optional<Mode> mode() const noexcept {
                    const auto mode_attr = node->first_attribute("mode");
                    return mode_attr ? magic_enum::enum_cast<Mode>(mode_attr->value()) : std::nullopt;
                }
                [[nodiscard]] std::optional<Protocol> protocol() const noexcept {
                    const auto proto_attr = node->first_attribute("protocol");
                    return proto_attr ? magic_enum::enum_cast<Protocol>(proto_attr->value()) : std::nullopt;
                }
                [[nodiscard]] Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
                [[nodiscard]] std::optional<bool> tls() const noexcept {
                    const auto tls_attr = node->first_attribute("tls");
                    return tls_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(tls_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] NamedSpan<Host> hosts() const noexcept { return NamedSpan<Host>{"host", node}; }
                [[nodiscard]] Optional<Snapshot> snapshot() const noexcept { return Snapshot{node->first_node("snapshot")}; }
                [[nodiscard]] Optional<Config> config() const noexcept { return Config{node->first_node("config")}; }
                [[nodiscard]] Optional<Auth> auth() const noexcept { return Auth{node->first_node("auth")}; }
                [[nodiscard]] Optional<storage::Encryption> encryption() const noexcept {
                    return storage::Encryption{node->first_node("encryption")};
                }
                [[nodiscard]] NamedSpan<SecLabel> seclabels() const noexcept { return NamedSpan<SecLabel>{"seclabel", node}; }
            };
            struct BackingStore : public Node {
                struct Format : public Node {
                    [[nodiscard]] std::optional<storage::Format> type() const noexcept {
                        const auto type_attr = node->first_attribute("type");
                        return type_attr ? magic_enum::enum_cast<storage::Format>(type_attr->value()) : std::nullopt;
                    }
                };
                [[nodiscard]] Optional<Integral> index() const noexcept { return Integral{node->first_attribute("index")}; }
                [[nodiscard]] Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
                [[nodiscard]] Optional<BackingStore> next() const noexcept { return BackingStore{node->first_node("backingStore")}; }
                [[nodiscard]] Optional<Format> format() const noexcept { return Format{node->first_node("format")}; }
            };

            [[nodiscard]] std::optional<Device> device() const noexcept {
                const auto dev_attr = node->first_attribute("device");
                return dev_attr ? magic_enum::enum_cast<Device>(dev_attr->value()) : std::nullopt;
            }
            [[nodiscard]] std::optional<Type> type() const noexcept {
                const auto type_attr = node->first_attribute("type");
                return type_attr ? magic_enum::enum_cast<Type>(type_attr->value()) : std::nullopt;
            }
            [[nodiscard]] Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
            [[nodiscard]] Optional<BackingStore> backing_store() const noexcept { return BackingStore{node->first_node("backingStore")}; }
            [[nodiscard]] std::optional<bool> raw_io() const noexcept {
                const auto rio_attr = node->first_attribute("raw_io");
                return rio_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(rio_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] std::optional<SgIO> sg_io() const noexcept {
                const auto sgio_attr = node->first_attribute("sgio");
                return sgio_attr ? magic_enum::enum_cast<SgIO>(sgio_attr->value()) : std::nullopt;
            }
            [[nodiscard]] Optional<Snapshot> snapshot() const noexcept { return Snapshot{node->first_node("snapshot")}; }
        };
        struct Ip : public Node {
            [[nodiscard]] String address() const noexcept { return String{node->first_attribute("address")}; }
            [[nodiscard]] Optional<String> family() const noexcept { return String{node->first_attribute("family")}; }
            [[nodiscard]] Optional<Integral> prefix() const noexcept { return Integral{node->first_attribute("prefix")}; }
            [[nodiscard]] Optional<String> peer() const noexcept { return String{node->first_attribute("peer")}; }
        };
        struct Route : public Node {
            [[nodiscard]] Optional<String> family() const noexcept { return String{node->first_attribute("family")}; }
            [[nodiscard]] String address() const noexcept { return String{node->first_attribute("address")}; }
            [[nodiscard]] Optional<String> netmask() const noexcept { return String{node->first_attribute("netmask")}; }
            [[nodiscard]] Optional<Integral> prefix() const noexcept { return Integral{node->first_attribute("prefix")}; }
            [[nodiscard]] String gateway() const noexcept { return String{node->first_attribute("gateway")}; }
            [[nodiscard]] Optional<Integral> metric() const noexcept { return Integral{node->first_attribute("metric")}; }
        };
        struct Controller : public Node {
            enum class Type {
                fdc,
                sata,
                ccid,
                scsi,
                usb,
                ide,
                pci,
                virtio_serial,
            };
            enum class Model {
                /* scsi */
                auto_,
                buslogic,
                lsilogic,
                lsisas1068,
                vmpvscsi,
                ibmvscsi,
                virtio_scsi,
                lsisas1078,

                /* usb */
                piix3_uhci,
                piix4_uhci,
                ehci,
                ich9_ehci1,
                ich9_uhci1,
                ich9_uhci2,
                ich9_uhci3,
                vt82c686b_uhci,
                pci_ohci,
                nec_xhci,
                none,
                qusb1,
                qusb2,
                qemu_xhci,

                /* ide */
                piix3,
                piix4,
                ich6,

                /* pci */
                spapr_pci_host_bridge,
                pci_bridge,
                i82801b11_bridge,
                ioh3420,
                pcie_root_port,
                x3130_upstream,
                xio3130_downstream,
                pxb,
                pxb_pcie,
                // pci-root
                pci_root,
                pcie_root,
                // other pci
                dmi_to_pci_bridge,
                pcie_switch_upstream_port,
                pcie_switch_downstream_port,
                pci_expander_bus,
                pcie_expander_bus,
            };

            struct UsbMaster : public Node {
                [[nodiscard]] Optional<Integral> start_port() const noexcept { return Integral{node->first_attribute("startport")}; }
            };
            struct PciTarget : public Node {
                [[nodiscard]] Optional<Integral> chassis_nr() const noexcept { return Integral{Node::node->first_attribute("chassisNr")}; }
                [[nodiscard]] Optional<Integral> chassis() const noexcept { return Integral{Node::node->first_attribute("chassis")}; }
                [[nodiscard]] Optional<Integral> port() const noexcept { return Integral{Node::node->first_attribute("port")}; }
                [[nodiscard]] Optional<Integral> bus_nr() const noexcept { return Integral{Node::node->first_attribute("busNr")}; }
                [[nodiscard]] Optional<Integral> index() const noexcept { return Integral{Node::node->first_attribute("index")}; }
                [[nodiscard]] Optional<Integral> node() const noexcept { return Integral{Node::node->first_node("node")}; }
            };
            struct PciHole64 : public Node {
                [[nodiscard]] Optional<String> unit() const noexcept { return String{node->first_attribute("unit")}; }
                [[nodiscard]] Integral value() const noexcept { return Integral{node}; }
            };
            struct Driver : public Node {
                [[nodiscard]] Optional<Integral> queues() const noexcept { return Integral{node->first_attribute("queues")}; }
                [[nodiscard]] Optional<Integral> cmd_per_lun() const noexcept { return Integral{node->first_attribute("cmd_per_lun")}; }
                [[nodiscard]] Optional<Integral> max_sectors() const noexcept { return Integral{node->first_attribute("max_sectors")}; }
                [[nodiscard]] std::optional<bool> ioeventfd() const noexcept {
                    const auto ioefd_attr = node->first_attribute("ioeventfd");
                    return ioefd_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ioefd_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] Optional<Integral> iothread() const noexcept { return Integral{node->first_attribute("iothread")}; }
                [[nodiscard]] std::optional<bool> iommu() const noexcept {
                    const auto iommu_attr = node->first_attribute("iommu");
                    return iommu_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(iommu_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] std::optional<bool> ats() const noexcept {
                    const auto ats_attr = node->first_attribute("ats");
                    return ats_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ats_attr->value()))} : std::nullopt;
                }
            };

            [[nodiscard]] Optional<Integral> index() const noexcept { return Integral{node->first_attribute("index")}; }
            [[nodiscard]] Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] std::optional<Model> model() const noexcept {
                const auto model_attr = node->first_attribute("model");
                if (!model_attr)
                    return std::nullopt;
                std::string in{model_attr->value()};
                std::replace(in.begin(), in.end(), '-', '_');
                return magic_enum::enum_cast<Model>(in);
            }
            [[nodiscard]] Optional<UsbMaster> usb_master() const noexcept { return UsbMaster{node->first_node("address")}; }
            [[nodiscard]] Optional<Integral> ports() const noexcept { return Integral{node->first_attribute("ports")}; }
            [[nodiscard]] Optional<PciTarget> pci_target() const noexcept { return PciTarget{node->first_node("target")}; }
            [[nodiscard]] Optional<PciHole64> pci_hole64() const noexcept { return PciHole64{node->first_node("pcihole64")}; }
            [[nodiscard]] Optional<Integral> vio_vectors() const noexcept { return Integral{node->first_attribute("vectors")}; }
            [[nodiscard]] Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
        };
        struct Lease : public Node {
            struct Target : public Node {
                [[nodiscard]] String path() const noexcept { return String{node->first_attribute("path")}; }
                [[nodiscard]] Optional<Integral> offset() const noexcept { return Integral{node->first_attribute("offset")}; }
            };

            [[nodiscard]] String lockspace() const noexcept { return String{node->first_node("lockspace")}; }
            [[nodiscard]] String key() const noexcept { return String{node->first_node("key")}; }
            [[nodiscard]] Target target() const noexcept { return Target{node->first_node("target")}; }
        };
        struct Filesystem : public Node {
            enum class Type {
                file,
                block,
                mount,
                bind,
                template_,
                ram,
            };
            enum class AccessMode {
                passthrough,
                mapped,
                squash,
            };

            struct Driver : public Node, storage::HasFormat<Driver, Optional> {
                enum class Type {
                    path,
                    handle,
                    loop,
                    nbd,
                    ploop,
                };

                [[nodiscard]] std::optional<Type> type() const noexcept {
                    const auto type_attr = node->first_attribute("type");
                    return type_attr ? magic_enum::enum_cast<Type>(type_attr->value()) : std::nullopt;
                }
                [[nodiscard]] String wrpolicy() const noexcept { return String{node->first_attribute("wrpolicy")}; }
                [[nodiscard]] std::optional<bool> iommu() const noexcept {
                    const auto iommu_attr = node->first_attribute("iommu");
                    return iommu_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(iommu_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] std::optional<bool> ats() const noexcept {
                    const auto ats_attr = node->first_attribute("ats");
                    return ats_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ats_attr->value()))} : std::nullopt;
                }
            };
            struct Source : public Node {
                [[nodiscard]] Optional<String> file() const noexcept { return String{node->first_attribute("file")}; }
                [[nodiscard]] Optional<String> dev() const noexcept { return String{node->first_attribute("dev")}; }
                [[nodiscard]] Optional<String> dir() const noexcept { return String{node->first_attribute("dir")}; }
                [[nodiscard]] Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
                [[nodiscard]] Optional<Integral> usage() const noexcept { return Integral{node->first_attribute("usage")}; }
                [[nodiscard]] Optional<String> units() const noexcept { return String{node->first_attribute("units")}; }
            };
            struct Target : public Node {
                [[nodiscard]] String dir() const noexcept { return String{node->first_attribute("dir")}; }
            };
            struct SpaceLimit : public Node {
                [[nodiscard]] Optional<String> unit() const noexcept { return String{node->first_attribute("unit")}; }
                [[nodiscard]] Integral value() const noexcept { return Integral{node}; }
            };

            [[nodiscard]] std::optional<Type> type() const noexcept {
                const auto type_attr = node->first_attribute("type");
                return type_attr ? magic_enum::enum_cast<Type>(type_attr->value()) : std::nullopt;
            }
            [[nodiscard]] Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
            [[nodiscard]] Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
            [[nodiscard]] Target target() const noexcept { return Target{node->first_node("target")}; }
            [[nodiscard]] std::optional<AccessMode> access_mode() const noexcept {
                const auto am_attr = node->first_attribute("accessmode");
                return am_attr ? magic_enum::enum_cast<AccessMode>(am_attr->value()) : std::nullopt;
            }
            [[nodiscard]] bool readonly() const noexcept { return node->first_node("readonly") != nullptr; }
            [[nodiscard]] Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
            [[nodiscard]] Optional<SpaceLimit> hard_space_limit() const noexcept { return SpaceLimit{node->first_node("hard_space_limit")}; }
            [[nodiscard]] Optional<SpaceLimit> soft_space_limit() const noexcept { return SpaceLimit{node->first_node("soft_space_limit")}; }
        };
        struct Interface : public Node {
            enum class Type {
                bridge,
                ethernet,
                vhostuser,
                network,
                direct,
                user,
                internal,
                mcast,
                client,
                udp,
                server,
                hostdev,
                pci,
                usb,
            };

            struct Source : public Node, public HasMacTableManager<Source, Optional> {
                enum class VhostUserType {
                    unix_,
                };
                enum class VHostUserMode {
                    server,
                    client,
                };
                enum class BridgeMode {
                    vepa,
                    bridge,
                    private_,
                    passthrough,
                };

                struct UdpLocal : public Node {
                    [[nodiscard]] Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
                    [[nodiscard]] Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
                };
                struct Vendor : public Node {
                    [[nodiscard]] Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                };
                struct Product : public Node {
                    [[nodiscard]] Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                };
                struct HostdevAddress : public Node {
                    enum class Type {
                        pci,
                        usb,
                    };

                    [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                    [[nodiscard]] Optional<Integral> usb_bus() const noexcept { return Integral{node->first_attribute("bus")}; }
                    [[nodiscard]] Optional<Integral> usb_device() const noexcept { return Integral{node->first_attribute("device")}; }
                    [[nodiscard]] Optional<Integral> pci_domain() const noexcept { return Integral{node->first_attribute("domain")}; }
                    [[nodiscard]] Optional<Integral> pci_bus() const noexcept { return Integral{node->first_attribute("bus")}; }
                    [[nodiscard]] Optional<String> pci_function() const noexcept { return String{node->first_attribute("domain")}; }
                    [[nodiscard]] std::optional<bool> pci_multifunction() const noexcept {
                        const auto mf_attr = node->first_attribute("multifunction");
                        return mf_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(mf_attr->value()))} : std::nullopt;
                    }
                };

                [[nodiscard]] Optional<String> bridge() const noexcept { return String{node->first_attribute("bridge")}; }
                [[nodiscard]] Optional<String> network() const noexcept { return String{node->first_attribute("network")}; }
                [[nodiscard]] Optional<String> port_group() const noexcept { return String{node->first_attribute("portgroup")}; }
                [[nodiscard]] NamedSpan<Ip> ethernet_ips() const noexcept { return NamedSpan<Ip>{"ip", node}; }
                [[nodiscard]] NamedSpan<Route> ethernet_routes() const noexcept { return NamedSpan<Route>{"route", node}; }
                [[nodiscard]] std::optional<VhostUserType> vhostuser_type() const noexcept {
                    const auto type_attr = node->first_attribute("type");
                    return type_attr ? magic_enum::enum_cast<VhostUserType>(type_attr->value()) : std::nullopt;
                }
                [[nodiscard]] Optional<String> vhostuser_path() const noexcept { return String{node->first_attribute("path")}; }
                [[nodiscard]] std::optional<VHostUserMode> vhostuser_mode() const noexcept {
                    const auto mode_attr = node->first_attribute("mode");
                    return mode_attr ? magic_enum::enum_cast<VHostUserMode>(mode_attr->value()) : std::nullopt;
                }
                [[nodiscard]] Optional<Reconnect> vhostuser_reconnect() const noexcept { return Reconnect{node->first_node("reconnect")}; }
                [[nodiscard]] Optional<String> direct_dev() const noexcept { return String{node->first_attribute("dev")}; }
                [[nodiscard]] std::optional<BridgeMode> direct_mode() const noexcept {
                    const auto mode_attr = node->first_attribute("mode");
                    return mode_attr ? magic_enum::enum_cast<BridgeMode>(mode_attr->value()) : std::nullopt;
                }
                [[nodiscard]] Optional<String> internal_name() const noexcept { return String{node->first_attribute("name")}; }
                [[nodiscard]] Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
                [[nodiscard]] Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
                [[nodiscard]] Optional<UdpLocal> udp_local() const noexcept { return UdpLocal{node->first_node("local")}; }
                [[nodiscard]] std::optional<bool> hostdev_missing() const noexcept {
                    const auto attr = node->first_attribute("missing");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] Optional<Vendor> hostdev_vendor() const noexcept { return Vendor{node->first_node("vendor")}; }
                [[nodiscard]] Optional<Product> hostdev_product() const noexcept { return Product{node->first_node("product")}; }
                [[nodiscard]] Optional<HostdevAddress> hostdev_address() const noexcept { return HostdevAddress{node->first_node("address")}; }
            };
            struct Link : public Node {
                enum class State : bool {
                    down = false,
                    up = true,
                };
                [[nodiscard]] State state() const noexcept { return *magic_enum::enum_cast<State>(node->first_attribute("state")->value()); }
            };
            struct Mtu : public Node {
                [[nodiscard]] Integral size() const noexcept { return Integral{node->first_attribute("size")}; }
            };
            struct Coalesce : public Node {
                struct Rx : public Node {
                    struct Frames : public Node {
                        [[nodiscard]] Optional<Integral> max() const noexcept { return Integral{node->first_attribute("max")}; }
                    };
                    [[nodiscard]] Optional<Frames> frames() const noexcept { return Frames{node->first_node("frames")}; }
                };
                [[nodiscard]] Optional<Rx> rx() const noexcept { return Rx{node->first_node("rx")}; }
            };
            struct Target : public Node {
                [[nodiscard]] String dev() const noexcept { return String{node->first_attribute("dev")}; }
            };
            struct Guest : public Node {
                [[nodiscard]] Optional<String> dev() const noexcept { return String{node->first_attribute("dev")}; }
                [[nodiscard]] Optional<String> actual() const noexcept { return String{node->first_attribute("actual")}; }
            };
            struct Mac : public Node {
                [[nodiscard]] String address() const noexcept { return String{node->first_attribute("address")}; }
            };
            struct Script : public Node {
                [[nodiscard]] String path() const noexcept { return String{node->first_attribute("path")}; }
            };
            struct BackendDomain : public Node {
                [[nodiscard]] String name() const noexcept { return String{node->first_attribute("name")}; }
            };
            struct Model : public Node {
                [[nodiscard]] String type() const noexcept { return String{node->first_attribute("type")}; }
            };
            struct Backend : public Node {
                [[nodiscard]] Optional<String> tap() const noexcept { return String{node->first_attribute("tap")}; }
                [[nodiscard]] Optional<String> vhost() const noexcept { return String{node->first_attribute("vhost")}; }
            };
            struct Driver : public Node {
                enum class Name {
                    kvm,
                    vfio,
                    xen,
                    qemu,
                    vhost,
                };
                enum class TxMode {
                    iothread,
                    timer,
                };

                struct Guest : public Node {
                    [[nodiscard]] std::optional<bool> csum() const noexcept {
                        const auto attr = node->first_attribute("csum");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                    [[nodiscard]] std::optional<bool> tso4() const noexcept {
                        const auto attr = node->first_attribute("tso4");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                    [[nodiscard]] std::optional<bool> tso6() const noexcept {
                        const auto attr = node->first_attribute("tso6");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                    [[nodiscard]] std::optional<bool> ecn() const noexcept {
                        const auto attr = node->first_attribute("ecn");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                    [[nodiscard]] std::optional<bool> ufo() const noexcept {
                        const auto attr = node->first_attribute("ufo");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                };
                struct Host : public Guest {
                    [[nodiscard]] std::optional<bool> gso() const noexcept {
                        const auto attr = node->first_attribute("gso");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                    [[nodiscard]] std::optional<bool> mrg_rxbuf() const noexcept {
                        const auto attr = node->first_attribute("mrg_rxbuf");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                };

                [[nodiscard]] std::optional<Name> name() const noexcept {
                    const auto track_attr = node->first_attribute("name");
                    return track_attr ? magic_enum::enum_cast<Name>(track_attr->value()) : std::nullopt;
                }
                [[nodiscard]] Optional<Integral> queues() const noexcept { return Integral{node->first_attribute("queues")}; }
                [[nodiscard]] Optional<Integral> rx_queue_size() const noexcept { return Integral{node->first_attribute("rx_queue_size")}; }
                [[nodiscard]] Optional<Integral> tx_queue_size() const noexcept { return Integral{node->first_attribute("tx_queue_size")}; }
                [[nodiscard]] std::optional<TxMode> tx_mode() const noexcept {
                    const auto track_attr = node->first_attribute("txmode");
                    return track_attr ? magic_enum::enum_cast<TxMode>(track_attr->value()) : std::nullopt;
                }
                [[nodiscard]] std::optional<bool> ioeventfd() const noexcept {
                    const auto attr = node->first_attribute("ioeventfd");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] std::optional<bool> event_idx() const noexcept {
                    const auto attr = node->first_attribute("event_idx");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] std::optional<bool> iommu() const noexcept {
                    const auto iommu_attr = node->first_attribute("iommu");
                    return iommu_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(iommu_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] std::optional<bool> ats() const noexcept {
                    const auto ats_attr = node->first_attribute("ats");
                    return ats_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ats_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] Optional<Host> host() const noexcept { return Host{node->first_node("host")}; }
                [[nodiscard]] Optional<Guest> guest() const noexcept { return Guest{node->first_node("guest")}; }
            };
            struct Tune : public Node {
                [[nodiscard]] Optional<Integral> sndbuf() const noexcept { return Integral{node->first_node("sndbuf")}; }
            };
            struct FilterRef : public Node {
                struct Parameter : public Node {
                    [[nodiscard]] String name() const noexcept { return String{node->first_attribute("name")}; }
                    [[nodiscard]] String value() const noexcept { return String{node->first_attribute("value")}; }
                };

                [[nodiscard]] String nc_name() const noexcept { return String{node->first_attribute("NCName")}; }
                [[nodiscard]] NamedSpan<Parameter> parameters() const noexcept { return NamedSpan<Parameter>{"parameter", node}; }
            };

            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
            [[nodiscard]] std::optional<bool> hostdev_managed() const noexcept {
                const auto attr = node->first_attribute("managed");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
            [[nodiscard]] Optional<VirtualPort> virtual_port() const noexcept { return VirtualPort{node->first_node("virtualport")}; }
            [[nodiscard]] Optional<Link> link() const noexcept { return Link{node->first_node("link")}; }
            [[nodiscard]] Optional<Mtu> mtu() const noexcept { return Mtu{node->first_node("mtu")}; }
            [[nodiscard]] Optional<Coalesce> coalesce() const noexcept { return Coalesce{node->first_node("coalesce")}; }
            [[nodiscard]] Optional<Target> target() const noexcept { return Target{node->first_node("target")}; }
            [[nodiscard]] Optional<Guest> guest() const noexcept { return Guest{node->first_node("guest")}; }
            [[nodiscard]] Optional<Mac> mac() const noexcept { return Mac{node->first_node("mac")}; }
            [[nodiscard]] NamedSpan<Ip> ips() const noexcept { return NamedSpan<Ip>{"ip", node}; }
            [[nodiscard]] Optional<Script> script() const noexcept { return Script{node->first_node("script")}; }
            [[nodiscard]] Optional<BackendDomain> backend_domain() const noexcept { return BackendDomain{node->first_node("backenddomain")}; }
            [[nodiscard]] Optional<Model> model() const noexcept { return Model{node->first_node("model")}; }
            [[nodiscard]] Optional<Backend> backend() const noexcept { return Backend{node->first_node("backend")}; }
            [[nodiscard]] Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
            [[nodiscard]] Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
            [[nodiscard]] Optional<FilterRef> filter_ref() const noexcept { return FilterRef{node->first_node("filterref")}; }
            [[nodiscard]] Optional<Tune> tune() const noexcept { return Tune{node->first_node("tune")}; }
            [[nodiscard]] Optional<Boot> boot() const noexcept { return Boot{node->first_node("boot")}; }
            [[nodiscard]] Optional<Rom> rom() const noexcept { return Rom{node->first_node("rom")}; }
            [[nodiscard]] Optional<Bandwidth> bandwidth() const noexcept { return Bandwidth{node->first_node("bandwidth")}; }
            [[nodiscard]] Optional<Vlan> vlan() const noexcept { return Vlan{node->first_node("vlan")}; }
            [[nodiscard]] std::optional<bool> trust_guest_rx_filters() const noexcept {
                const auto attr = node->first_attribute("trustGuestRxFilters");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
        };
        struct Input : public Node {
            enum class Type {
                tablet,
                mouse,
                keyboard,
                passthrough,
            };
            enum class Bus {
                ps2,
                usb,
                xen,
                virtio,
            };

            struct Driver : public Node {
                [[nodiscard]] std::optional<bool> iommu() const noexcept {
                    const auto iommu_attr = node->first_attribute("iommu");
                    return iommu_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(iommu_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] std::optional<bool> ats() const noexcept {
                    const auto ats_attr = node->first_attribute("ats");
                    return ats_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ats_attr->value()))} : std::nullopt;
                }
            };
            struct Source : public Node {
                [[nodiscard]] String evdev() const noexcept { return String{node->first_attribute("evdev")}; }
            };

            [[nodiscard]] Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] Bus bus() const noexcept { return *magic_enum::enum_cast<Bus>(node->first_attribute("bus")->value()); }
            [[nodiscard]] Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
            [[nodiscard]] Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct Sound : public Node {
            enum class Model {
                sb16,
                es1370,
                pcspk,
                ac97,
                ich6,
                ich9,
                usb,
            };

            struct Codec : public Node {
                enum class Type {
                    duplex,
                    mirco,
                };

                [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            };

            [[nodiscard]] Model model() const noexcept { return *magic_enum::enum_cast<Model>(node->first_attribute("model")->value()); }
            [[nodiscard]] Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
            [[nodiscard]] NamedSpan<Codec> codecs() const noexcept { return NamedSpan<Codec>{"codec", node}; }
        };
        struct HostDev : public Node {
            enum class Mode {
                subsystem,
                capabilities,
            };
            enum class Type {
                pci,
                usb,
                scsi,
                scsi_host,
                mdev,
                storage,
                misc,
                net,
            };
            enum class Model {
                vfio_pci,
                vfio_ap,
            };

            struct Driver : public Node {
                enum class Name {
                    kvm,
                    vfio,
                    xen,
                };

                [[nodiscard]] Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
            };
            struct Source : public Node {
                enum class Protocol {
                    adapter,
                    iscsi,
                };

                struct PciAddress : public Node {
                    [[nodiscard]] Optional<Integral> domain() const noexcept { return Integral{node->first_attribute("domain")}; }
                    [[nodiscard]] Optional<Integral> bus() const noexcept { return Integral{node->first_attribute("bus")}; }
                    [[nodiscard]] Optional<String> function() const noexcept { return String{node->first_attribute("domain")}; }
                    [[nodiscard]] std::optional<bool> multifunction() const noexcept {
                        const auto mf_attr = node->first_attribute("multifunction");
                        return mf_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(mf_attr->value()))} : std::nullopt;
                    }
                };
                struct Vendor : public Node {
                    [[nodiscard]] Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                };
                struct Product : public Node {
                    [[nodiscard]] Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                };
                struct UsbScsiAddress : public Node {
                    enum class Type {
                        pci,
                        usb,
                    };

                    [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                    [[nodiscard]] Optional<Integral> bus() const noexcept { return Integral{node->first_attribute("bus")}; }
                    [[nodiscard]] Optional<Integral> usb_device() const noexcept { return Integral{node->first_attribute("device")}; }
                    [[nodiscard]] Optional<Integral> scsi_target() const noexcept { return Integral{node->first_attribute("target")}; }
                    [[nodiscard]] Optional<String> scsi_unit() const noexcept { return String{node->first_attribute("uint")}; }
                };
                struct IscsiHost : public Node {
                    [[nodiscard]] String name() const noexcept { return String{node->first_attribute("name")}; }
                    [[nodiscard]] Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
                };
                struct MdevAddress : public Node {
                    [[nodiscard]] Optional<Uuid> uuid() const noexcept { return Uuid{node->first_node("uuid")}; }
                };

                [[nodiscard]] std::optional<StartupPolicy> startup_policy() const noexcept {
                    const auto attr = node->first_attribute("startupPolicy");
                    return attr ? magic_enum::enum_cast<StartupPolicy>(attr->value()) : std::nullopt;
                }
                [[nodiscard]] Optional<PciAddress> pci_address() const noexcept { return PciAddress{node->first_node("address")}; }
                [[nodiscard]] Optional<Vendor> vendor() const noexcept { return Vendor{node->first_node("vendor")}; }
                [[nodiscard]] Optional<Product> product() const noexcept { return Product{node->first_node("product")}; }
                [[nodiscard]] Optional<UsbScsiAddress> usb_address() const noexcept { return UsbScsiAddress{node->first_node("address")}; }
                [[nodiscard]] Optional<UsbScsiAddress> scsi_address() const noexcept { return UsbScsiAddress{node->first_node("address")}; }
                [[nodiscard]] NamedSpan<IscsiHost> iscsi_hosts() const noexcept { return NamedSpan<IscsiHost>{"host", node}; }
                [[nodiscard]] Optional<Disk::Auth> iscsi_auth() const noexcept { return Disk::Auth{node->first_node("auth")}; }
                [[nodiscard]] Optional<String> vhost_wwpn() const noexcept { return String{node->first_attribute("wwpn")}; }
                [[nodiscard]] Optional<MdevAddress> mdev_address() const noexcept { return MdevAddress{node->first_node("address")}; }
                [[nodiscard]] Optional<String> storage_block() const noexcept { return String{node->first_attribute("block")}; }
                [[nodiscard]] Optional<String> misc_char() const noexcept { return String{node->first_attribute("char")}; }
                [[nodiscard]] Optional<String> net_interface() const noexcept { return String{node->first_attribute("interface")}; }
            };

            [[nodiscard]] std::optional<Mode> mode() const noexcept {
                const auto attr = node->first_attribute("mode");
                return attr ? magic_enum::enum_cast<Mode>(attr->value()) : std::nullopt;
            }
            [[nodiscard]] std::optional<bool> managed() const noexcept {
                const auto attr = node->first_attribute("managed");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
            [[nodiscard]] Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
            [[nodiscard]] std::optional<SgIO> sg_io() const noexcept {
                const auto sgio_attr = node->first_attribute("sgio");
                return sgio_attr ? magic_enum::enum_cast<SgIO>(sgio_attr->value()) : std::nullopt;
            }
            [[nodiscard]] std::optional<bool> raw_io() const noexcept {
                const auto rio_attr = node->first_attribute("raw_io");
                return rio_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(rio_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] Optional<Adapter> adapter() const noexcept { return Adapter{node->first_node("adapter")}; }
            [[nodiscard]] Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
            [[nodiscard]] std::optional<Model> model() const noexcept {
                const auto track_attr = node->first_attribute("model");
                if (!track_attr)
                    return std::nullopt;
                std::string in{track_attr->value()};
                std::replace(in.begin(), in.end(), '-', '_');
                return magic_enum::enum_cast<Model>(in);
            }
            [[nodiscard]] NamedSpan<Ip> net_ips() const noexcept { return NamedSpan<Ip>{"ip", node}; }
            [[nodiscard]] NamedSpan<Route> net_routes() const noexcept { return NamedSpan<Route>{"route", node}; }
            [[nodiscard]] Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] Optional<Boot> boot() const noexcept { return Boot{node->first_node("boot")}; }
            [[nodiscard]] Optional<Rom> rom() const noexcept { return Rom{node->first_node("rom")}; }
            [[nodiscard]] Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
            [[nodiscard]] bool readonly() const noexcept { return node->first_node("readonly") != nullptr; }
            [[nodiscard]] bool shareable() const noexcept { return node->first_node("shareable") != nullptr; }
        };
        struct Graphics : public Node {
            enum class Type {
                sdl,
                vnc,
                spice,
                rdp,
                desktop,
            };
            enum class SharePolicy {
                allow_exclusive,
                force_shared,
                ignore,
            };
            enum class Connected {
                fail,
                disconnect,
                keep,
            };
            enum class Mode {
                any,
                secure,
                insecure,
            };

            struct Listen : public Node {
                enum class Type {
                    address,
                    network,
                    socket,
                    none,
                };

                [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                [[nodiscard]] Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
                [[nodiscard]] Optional<String> socket() const noexcept { return String{node->first_attribute("socket")}; }
                [[nodiscard]] Optional<String> network() const noexcept { return String{node->first_attribute("network")}; }
            };
            struct Channel : public Node {
                enum class Name {
                    main,
                    display,
                    inputs,
                    cursor,
                    playback,
                    record,
                    smartcard,
                    usbredir,
                };

                [[nodiscard]] Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
                [[nodiscard]] Mode mode() const noexcept { return *magic_enum::enum_cast<Mode>(node->first_attribute("mode")->value()); }
            };
            struct Image : public Node {
                enum class Compression {
                    auto_glz,
                    auto_lz,
                    quic,
                    glz,
                    lz,
                    off,
                };

                [[nodiscard]] Compression compression() const noexcept {
                    return *magic_enum::enum_cast<Compression>(node->first_attribute("compression")->value());
                }
            };
            struct JpegZlib : public Node {
                enum class Compression {
                    auto_,
                    never,
                    always,
                };

                [[nodiscard]] Compression compression() const noexcept {
                    return *magic_enum::enum_cast<Compression>(node->first_attribute("compression")->value());
                }
            };
            struct Playback : public Node {
                [[nodiscard]] bool compression() const noexcept {
                    return static_cast<bool>(*magic_enum::enum_cast<OnOff>(node->first_attribute("compression")->value()));
                }
            };
            struct Streaming : public Node {
                enum class Mode {
                    filter,
                    all,
                    off,
                };

                [[nodiscard]] Mode mode() const noexcept { return *magic_enum::enum_cast<Mode>(node->first_attribute("mode")->value()); }
            };
            struct Clipboard : public Node {
                [[nodiscard]] bool copypaste() const noexcept {
                    return static_cast<bool>(*magic_enum::enum_cast<OnOff>(node->first_attribute("copypaste")->value()));
                }
            };
            struct Mouse : public Node {
                enum class Mode {
                    server,
                    client,
                };

                [[nodiscard]] Mode mode() const noexcept { return *magic_enum::enum_cast<Mode>(node->first_attribute("mode")->value()); }
            };
            struct EnableC : public Node {
                [[nodiscard]] bool enable() const noexcept {
                    return static_cast<bool>(*magic_enum::enum_cast<YesNo>(node->first_attribute("copypaste")->value()));
                }
            };
            struct Gl : public EnableC {
                [[nodiscard]] Optional<String> render_node() const noexcept { return String{node->first_attribute("rendernode")}; }
            };

            [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] Optional<String> display() const noexcept { return String{node->first_attribute("display")}; }
            [[nodiscard]] Optional<String> xauth() const noexcept { return String{node->first_attribute("xauth")}; }
            [[nodiscard]] std::optional<bool> fullscreen() const noexcept {
                const auto attr = node->first_attribute("fullscreen");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
            [[nodiscard]] Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
            [[nodiscard]] std::optional<bool> autoport() const noexcept {
                const auto attr = node->first_attribute("autoport");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
            [[nodiscard]] Optional<Integral> websocket() const noexcept { return Integral{node->first_attribute("websocket")}; }
            [[nodiscard]] Optional<String> listen() const noexcept { return String{node->first_attribute("listen")}; }
            [[nodiscard]] std::optional<SharePolicy> share_policy() const noexcept {
                const auto track_attr = node->first_attribute("sharePolicy");
                if (!track_attr)
                    return std::nullopt;
                std::string in{track_attr->value()};
                std::replace(in.begin(), in.end(), '-', '_');
                return magic_enum::enum_cast<SharePolicy>(in);
            }
            [[nodiscard]] Optional<String> socket() const noexcept { return String{node->first_attribute("socket")}; }
            [[nodiscard]] Optional<String> passwd() const noexcept { return String{node->first_attribute("passwd")}; }
            [[nodiscard]] Optional<String> keymap() const noexcept { return String{node->first_attribute("keymap")}; }
            [[nodiscard]] Optional<String> passwd_valid_to() const noexcept { return String{node->first_attribute("passwdValidTo")}; }
            [[nodiscard]] Optional<Integral> tls_port() const noexcept { return Integral{node->first_attribute("tlsPort")}; }
            [[nodiscard]] std::optional<Connected> connected() const noexcept {
                const auto attr = node->first_attribute("connected");
                return attr ? magic_enum::enum_cast<Connected>(attr->value()) : std::nullopt;
            }
            [[nodiscard]] std::optional<Mode> default_mode() const noexcept {
                const auto attr = node->first_attribute("defaultMode");
                return attr ? magic_enum::enum_cast<Mode>(attr->value()) : std::nullopt;
            }
            [[nodiscard]] NamedSpan<Listen> listens() const noexcept { return NamedSpan<Listen>{"listen", node}; }
            [[nodiscard]] NamedSpan<Channel> channels() const noexcept { return NamedSpan<Channel>{"channel", node}; }
            [[nodiscard]] Optional<Image> image() const noexcept { return Image{node->first_node("image")}; }
            [[nodiscard]] Optional<JpegZlib> jpeg() const noexcept { return JpegZlib{node->first_node("jpeg")}; }
            [[nodiscard]] Optional<JpegZlib> zlib() const noexcept { return JpegZlib{node->first_node("zlib")}; }
            [[nodiscard]] Optional<Playback> playback() const noexcept { return Playback{node->first_node("playback")}; }
            [[nodiscard]] Optional<Streaming> streaming() const noexcept { return Streaming{node->first_node("streaming")}; }
            [[nodiscard]] Optional<Clipboard> clipboard() const noexcept { return Clipboard{node->first_node("clipboard")}; }
            [[nodiscard]] Optional<Mouse> mouse() const noexcept { return Mouse{node->first_node("mouse")}; }
            [[nodiscard]] Optional<EnableC> filetransfer() const noexcept { return EnableC{node->first_node("filetransfer")}; }
            [[nodiscard]] Optional<Gl> gl() const noexcept { return Gl{node->first_node("gl")}; }
            [[nodiscard]] std::optional<bool> replace_user() const noexcept {
                const auto attr = node->first_attribute("replaceUser");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
            [[nodiscard]] std::optional<bool> multi_user() const noexcept {
                const auto attr = node->first_attribute("multiUser");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
        };
        struct Video : public Node {
            struct Driver : public Node {
                enum class VgaConf {
                    io,
                    on,
                    off,
                };

                [[nodiscard]] std::optional<bool> iommu() const noexcept {
                    const auto iommu_attr = node->first_attribute("iommu");
                    return iommu_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(iommu_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] std::optional<bool> ats() const noexcept {
                    const auto ats_attr = node->first_attribute("ats");
                    return ats_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ats_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] std::optional<VgaConf> vga_conf() const noexcept {
                    const auto ats_attr = node->first_attribute("vgaconf");
                    return ats_attr ? magic_enum::enum_cast<VgaConf>(ats_attr->value()) : std::nullopt;
                }
            };
            struct Model : public Node {
                enum class Type {
                    vga,
                    cirrus,
                    vmvga,
                    xen,
                    vbox,
                    virtio,
                    gop,
                    qxl,
                };

                struct Acceleration : public Node {
                    [[nodiscard]] std::optional<bool> accel3d() const noexcept { return bool_wrap_attr<YesNo, Optional>(node, "accel3d"); }
                    [[nodiscard]] std::optional<bool> accel2d() const noexcept { return bool_wrap_attr<YesNo, Optional>(node, "accel2d"); }
                };

                [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                [[nodiscard]] Optional<Integral> qxl_ram() const noexcept { return Integral{node->first_attribute("ram")}; }
                [[nodiscard]] Optional<Integral> qxl_vgamem() const noexcept { return Integral{node->first_attribute("vgamem")}; }
                [[nodiscard]] Optional<Integral> qxl_vram64() const noexcept { return Integral{node->first_attribute("vram64")}; }
                [[nodiscard]] Optional<Integral> vram() const noexcept { return Integral{node->first_attribute("vram")}; }
                [[nodiscard]] Optional<Integral> heads() const noexcept { return Integral{node->first_attribute("heads")}; }
                [[nodiscard]] std::optional<bool> primary() const noexcept {
                    const auto attr = node->first_attribute("primary");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] Optional<Acceleration> acceleration() const noexcept { return Acceleration{node->first_node("acceleration")}; }
            };

            [[nodiscard]] Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
            [[nodiscard]] Optional<Model> model() const noexcept { return Model{node->first_node("model")}; }
            [[nodiscard]] Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct QemuCharDev : public Node {
            struct Source : public Node {
                [[nodiscard]] Optional<String> mode() const noexcept { return String{node->first_attribute("mode")}; }
                [[nodiscard]] Optional<String> path() const noexcept { return String{node->first_attribute("path")}; }
                [[nodiscard]] Optional<String> host() const noexcept { return String{node->first_attribute("host")}; }
                [[nodiscard]] Optional<String> service() const noexcept { return String{node->first_attribute("service")}; }
                [[nodiscard]] Optional<String> channel() const noexcept { return String{node->first_attribute("channel")}; }
                [[nodiscard]] Optional<String> master() const noexcept { return String{node->first_attribute("master")}; }
                [[nodiscard]] Optional<String> slave() const noexcept { return String{node->first_attribute("slave")}; }
                [[nodiscard]] std::optional<bool> append() const noexcept {
                    const auto attr = node->first_attribute("append");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] std::optional<bool> tls() const noexcept {
                    const auto attr = node->first_attribute("tls");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] Optional<Reconnect> reconnect() const noexcept { return Reconnect{node->first_node("reconnect")}; }
                [[nodiscard]] NamedSpan<SecLabel> seclabels() const noexcept { return NamedSpan<SecLabel>{"seclabel", node}; }
            };
            struct Protocol : public Node {
                enum class Type {
                    raw,
                    telnet,
                    telnets,
                    tls,
                };
                [[nodiscard]] Type type() const noexcept { return enum_wrap_attr<Type>(node, "type"); }
            };
            struct Log : public Node {
                [[nodiscard]] String file() const noexcept { return String{node->first_attribute("file")}; }
                [[nodiscard]] std::optional<bool> append() const noexcept { return bool_wrap_attr<OnOff, Optional>(node, "append"); }
            };
            struct Target : public Node {
                enum class Type {
                    xen,
                    serial,
                    uml,
                    virtio,
                    lxc,
                    openvz,
                    sclp,
                    sclplm,
                    isa_serial,
                    usb_serial,
                    pci_serial,
                    spapr_vio_serial,
                    system_serial,
                    sclp_serial,
                };

                struct Model : public Node {
                    enum class Name {
                        isa_serial,
                        usb_serial,
                        pci_serial,
                        spapr_vty,
                        pl011,
                        sclpconsole,
                        sclplmconsole,
                    };
                    [[nodiscard]] Name name() const noexcept {
                        std::string in{node->first_attribute("name")->value()};
                        std::replace(in.begin(), in.end(), '-', '_');
                        return *magic_enum::enum_cast<Name>(in);
                    }
                };

                [[nodiscard]] std::optional<Type> type() const noexcept {
                    const auto attr = node->first_attribute("type");
                    if (!attr)
                        return std::nullopt;
                    std::string in{attr->value()};
                    std::replace(in.begin(), in.end(), '-', '_');
                    return magic_enum::enum_cast<Type>(in);
                }
                [[nodiscard]] Optional<String> port() const noexcept { return String{node->first_attribute("port")}; }
                [[nodiscard]] Optional<Model> model() const noexcept { return Model{node->first_node("model")}; }
            };
            [[nodiscard]] QemuCharDevType type() const noexcept {
                return *magic_enum::enum_cast<QemuCharDevType>(node->first_attribute("type")->value());
            }
            [[nodiscard]] Optional<String> tty() const noexcept { return String{node->first_attribute("tty")}; }
            [[nodiscard]] NamedSpan<Source> sources() const noexcept { return NamedSpan<Source>{"source", node}; }
            [[nodiscard]] Optional<Protocol> protocol() const noexcept { return Protocol{node->first_node("protocol")}; }
            [[nodiscard]] Optional<Log> log() const noexcept { return Log{node->first_node("log")}; }
            [[nodiscard]] Optional<Target> target() const noexcept { return Target{node->first_node("target")}; }
            [[nodiscard]] Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct Console : public QemuCharDev {};
        struct Parallel : public QemuCharDev {};
        struct Serial : public QemuCharDev {};
        struct Channel : public Node {
            struct Target : public Node {
                enum class Type {
                    guestfwd,
                    virtio,
                    xen,
                };
                enum class State : bool {
                    connected = true,
                    disconnected = false,
                };

                [[nodiscard]] Type type() const noexcept { return enum_wrap_attr<Type>(node, "type"); }
                [[nodiscard]] Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
                [[nodiscard]] Optional<String> port() const noexcept { return String{node->first_attribute("port")}; }
                [[nodiscard]] Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
                [[nodiscard]] std::optional<State> state() const noexcept { return enum_wrap_attr<State, Optional>(node, "state"); }
            };

            [[nodiscard]] QemuCharDevType type() const noexcept {
                return *magic_enum::enum_cast<QemuCharDevType>(node->first_attribute("type")->value());
            }
            [[nodiscard]] NamedSpan<QemuCharDev::Source> sources() const noexcept { return NamedSpan<QemuCharDev::Source>{"source", node}; }
            [[nodiscard]] Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct Smartcard : public QemuCharDev {
            enum class Mode {
                host,
                host_certificates,
                passthrough,
            };

            [[nodiscard]] Mode mode() const noexcept {
                std::string in{node->first_attribute("mode")->value()};
                std::replace(in.begin(), in.end(), '-', '_');
                return *magic_enum::enum_cast<Mode>(in);
            }
            [[nodiscard]] NamedSpan<StringNode> certificates() const noexcept { return NamedSpan<StringNode>{"certificate", node}; }
            [[nodiscard]] Optional<StringNode> database() const noexcept { return StringNode{node->first_node("database")}; }
        };
        struct Hub : public Node {
            enum class Type {
                usb,
            };

            [[nodiscard]] Type type() const noexcept { return enum_wrap_attr<Type>(node, "type"); }
            [[nodiscard]] Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct RedirDev : public QemuCharDev {
            enum class Bus {
                usb,
            };

            [[nodiscard]] Bus bus() const noexcept { return enum_wrap_attr<Bus>(node, "bus"); }
            [[nodiscard]] Optional<Boot> boot() const noexcept { return Boot{node->first_node("boot")}; }
        };

        /*
  <element name="devices">
    <interleave>
    <zeroOrMore>
      <choice>
        <ref name="redirfilter"/>
        <ref name="rng"/>
        <ref name="tpm"/>
        <ref name="shmem"/>
        <ref name="memorydev"/>
      </choice>
    </zeroOrMore>
    <optional>
      <ref name="watchdog"/>
    </optional>
    <optional>
      <ref name="memballoon"/>
    </optional>
    <optional>
      <ref name="nvram"/>
    </optional>
    <zeroOrMore>
      <ref name="panic"/>
    </zeroOrMore>
    <optional>
      <ref name="iommu"/>
    </optional>
    </interleave>
  </element>
 * */

        [[nodiscard]] Optional<Emulator> emulator() const noexcept { return Emulator{node->first_node("emulator")}; }
        [[nodiscard]] NamedSpan<Disk> disks() const noexcept { return NamedSpan<Disk>{"disk", node}; }
        [[nodiscard]] NamedSpan<Controller> controllers() const noexcept { return NamedSpan<Controller>{"controller", node}; }
        [[nodiscard]] NamedSpan<Lease> leases() const noexcept { return NamedSpan<Lease>{"lease", node}; }
        [[nodiscard]] NamedSpan<Filesystem> filesystems() const noexcept { return NamedSpan<Filesystem>{"filesystem", node}; }
        [[nodiscard]] NamedSpan<Interface> interfaces() const noexcept { return NamedSpan<Interface>{"interface", node}; }
        [[nodiscard]] NamedSpan<Input> inputs() const noexcept { return NamedSpan<Input>{"input", node}; }
        [[nodiscard]] NamedSpan<Sound> sounds() const noexcept { return NamedSpan<Sound>{"sound", node}; }
        [[nodiscard]] NamedSpan<HostDev> hostdevs() const noexcept { return NamedSpan<HostDev>{"hostdev", node}; }
        [[nodiscard]] NamedSpan<Graphics> graphics() const noexcept { return NamedSpan<Graphics>{"graphics", node}; }
        [[nodiscard]] NamedSpan<Video> videos() const noexcept { return NamedSpan<Video>{"video", node}; }
        [[nodiscard]] NamedSpan<Console> consoles() const noexcept { return NamedSpan<Console>{"console", node}; }
        [[nodiscard]] NamedSpan<Parallel> parallels() const noexcept { return NamedSpan<Parallel>{"parallel", node}; }
        [[nodiscard]] NamedSpan<Serial> serials() const noexcept { return NamedSpan<Serial>{"serial", node}; }
        [[nodiscard]] NamedSpan<Channel> channels() const noexcept { return NamedSpan<Channel>{"channel", node}; }
        [[nodiscard]] NamedSpan<Smartcard> smartcards() const noexcept { return NamedSpan<Smartcard>{"smartcard", node}; }
        [[nodiscard]] NamedSpan<Hub> hubs() const noexcept { return NamedSpan<Hub>{"hub", node}; }
        [[nodiscard]] NamedSpan<RedirDev> redir_devs() const noexcept { return NamedSpan<RedirDev>{"redirdev", node}; }
    };
    /*
     *<interleave>
        //<ref name="resources"/>
        //<ref name="features"/>

        <optional>
          //<ref name="pm"/>
        </optional>
        <optional>
          //<ref name="idmap"/>
        </optional>
        <optional>
          <ref name="devices"/>
        </optional>
        <zeroOrMore>
          <ref name="seclabel"/>
        </zeroOrMore>
        <optional>
          <ref name='qemucmdline'/>
        </optional>
        <optional>
          <ref name='lxcsharens'/>
        </optional>
        <optional>
          <ref name='keywrap'/>
        </optional>
      </interleave>
     * */
    ///

    ///
    [[nodiscard]] Type type() const noexcept { return *magic_enum::enum_cast<Type>(std::string_view{node->first_attribute("type")->value()}); }
    [[nodiscard]] Optional<Integral> id() const noexcept { return Integral{node->first_attribute("id")}; }
    [[nodiscard]] String name() const noexcept { return String{node->first_node("name")}; }
    [[nodiscard]] Optional<Uuid> uuid() const noexcept { return Uuid{node->first_node("uuid")}; }
    [[nodiscard]] Optional<String> title() const noexcept { return String{node->first_node("title")}; }
    [[nodiscard]] Optional<String> description() const noexcept { return String{node->first_node("description")}; }
    // metadata() // hidden because hard to interface and there is already an access in the Object API
    [[nodiscard]] Optional<Cpu> cpu() const noexcept { return Cpu{node->first_node("cpu")}; }
    [[nodiscard]] Optional<Sysinfo> sysinfo() const noexcept { return Sysinfo{node->first_node("sysinfo")}; }
    [[nodiscard]] Optional<Bootloader> bootloader() const noexcept { return Bootloader{node->first_node("bootloader")}; }
    [[nodiscard]] Optional<BootloaderArgs> bootloader_args() const noexcept { return BootloaderArgs{node->first_node("bootloader_args")}; }
    [[nodiscard]] Optional<Os> os() const noexcept { return Os{node->first_node("os")}; }
    [[nodiscard]] Optional<Clock> clock() const noexcept { return Clock{node->first_node("clock")}; }
    // resources() // left out because low priority since there is already an access in the Object API
    // features() // left out because low priority since there is already an access in the Object API
    [[nodiscard]] std::optional<OffOption> on_reboot() const noexcept {
        const auto track_attr = node->first_attribute("on_reboot");
        if (!track_attr)
            return std::nullopt;
        std::string in{track_attr->value()};
        std::replace(in.begin(), in.end(), '-', '_');
        return magic_enum::enum_cast<OffOption>(in);
    }
    [[nodiscard]] std::optional<OffOption> on_poweroff() const noexcept {
        const auto track_attr = node->first_attribute("on_poweroff");
        if (!track_attr)
            return std::nullopt;
        std::string in{track_attr->value()};
        std::replace(in.begin(), in.end(), '-', '_');
        return magic_enum::enum_cast<OffOption>(in);
    }
    [[nodiscard]] std::optional<CrashOption> on_crash() const noexcept {
        const auto track_attr = node->first_attribute("on_crash");
        if (!track_attr)
            return std::nullopt;
        std::string in{track_attr->value()};
        std::replace(in.begin(), in.end(), '-', '_');
        return magic_enum::enum_cast<CrashOption>(in);
    }
    [[nodiscard]] std::optional<LockFailureOption> track() const noexcept {
        const auto track_attr = node->first_attribute("on_lockfailure");
        return track_attr ? magic_enum::enum_cast<LockFailureOption>(track_attr->value()) : std::nullopt;
    }
    // pm() // left out because low priority
    [[nodiscard]] Optional<Perf> perf() const noexcept { return Perf{node->first_node("perf")}; }
    // idmap() // left out because low priority
    ///
}; // namespace

} // namespace
} // namespace virtxml