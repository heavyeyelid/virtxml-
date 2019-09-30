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
    [[nodiscard]] inline Integral order() const noexcept { return Integral{node->first_attribute("order")}; }
    [[nodiscard]] inline Optional<String> loadparm() const noexcept { return String{node->first_attribute("loadparm")}; }
};

struct Rom : public Node {
    [[nodiscard]] inline std::optional<bool> bar() const noexcept {
        const auto attr = node->first_attribute("bar");
        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
    }
    [[nodiscard]] inline Optional<String> file() const noexcept { return String{node->first_attribute("file")}; }
};
struct Reconnect : public Node {
    [[nodiscard]] inline bool enabled() const noexcept { return static_cast<bool>(*magic_enum::enum_cast<YesNo>(node->first_attribute("enabled"))); }
    [[nodiscard]] inline Optional<Integral> timeout() const noexcept { return Integral{node->first_attribute("timeout")}; }
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

    [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
    [[nodiscard]] inline Optional<Integral> domain() const noexcept { return Integral{node->first_attribute("domain")}; }
    [[nodiscard]] inline Optional<Integral> bus() const noexcept { return Integral{node->first_attribute("bus")}; }
    [[nodiscard]] inline Optional<String> function() const noexcept { return String{node->first_attribute("domain")}; }
    [[nodiscard]] inline std::optional<bool> multifunction() const noexcept {
        const auto mf_attr = node->first_attribute("multifunction");
        return mf_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(mf_attr->value()))} : std::nullopt;
    }
    [[nodiscard]] inline Optional<Integral> controller() const noexcept { return Integral{node->first_attribute("controller")}; }
    [[nodiscard]] inline Optional<Integral> target() const noexcept { return Integral{node->first_attribute("target")}; }
    [[nodiscard]] inline Optional<Integral> unit() const noexcept { return Integral{node->first_attribute("unit")}; }
    [[nodiscard]] inline Optional<Integral> drive_controller() const noexcept { return Integral{node->first_attribute("controller")}; }
    [[nodiscard]] inline Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
    [[nodiscard]] inline Optional<Integral> reg() const noexcept { return Integral{node->first_attribute("reg")}; }
    [[nodiscard]] inline Optional<Integral> cssid() const noexcept { return Integral{node->first_attribute("cssid")}; }
    [[nodiscard]] inline Optional<Integral> ssid() const noexcept { return Integral{node->first_attribute("ssid")}; }
    [[nodiscard]] inline Optional<Integral> devno() const noexcept { return Integral{node->first_attribute("devno")}; }
    [[nodiscard]] inline Optional<Integral> iobase() const noexcept { return Integral{node->first_attribute("iobase")}; }
    [[nodiscard]] inline Optional<Integral> irq() const noexcept { return Integral{node->first_attribute("irq")}; }
    [[nodiscard]] inline Optional<Integral> slot() const noexcept { return Integral{node->first_attribute("slot")}; }
    [[nodiscard]] inline Optional<Integral> base() const noexcept { return Integral{node->first_attribute("base")}; }
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
        [[nodiscard]] inline Optional<cpu::Model> model() const noexcept { return cpu::Model{node->first_node("model")}; }
        [[nodiscard]] inline Optional<String> vendor() const noexcept { return String{node->first_node("vendor")}; }
        [[nodiscard]] inline Optional<cpu::Topology> topology() const noexcept { return cpu::Topology{node->first_node("topology")}; }
        [[nodiscard]] inline NamedSpan<cpu::Feature> features() const noexcept { return NamedSpan<cpu::Feature>{"feature", node}; }
        [[nodiscard]] inline Optional<NamedSpan<cpu::NumaCell>> numa() const noexcept {
            return NamedSpan<cpu::NumaCell>{"cell", node->first_node("numa")};
        }
        [[nodiscard]] inline Optional<cpu::Cache> cache() const noexcept { return cpu::Cache{node->first_node("cache")}; }
    };
    struct Sysinfo : public Node {
        struct BiosEntry : public StringNode {
            enum class Name {
                vendor,
                version,
                date,
                release,
            };

            [[nodiscard]] inline Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
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

            [[nodiscard]] inline Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
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

            [[nodiscard]] inline Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
        };

        [[nodiscard]] inline std::string_view type() const noexcept { return "smbios"; } // Attribute always has this value
        [[nodiscard]] inline Optional<NamedSpan<BiosEntry>> bios() const noexcept { return NamedSpan<BiosEntry>{"entry", node->first_node("bios")}; }
        [[nodiscard]] inline Optional<NamedSpan<SystemEntry>> system() const noexcept {
            return NamedSpan<SystemEntry>{"entry", node->first_node("system")};
        }
        [[nodiscard]] inline NamedSpan<BaseboardEntry> baseboard() const noexcept {
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
            [[nodiscard]] inline std::optional<cpu::Arch> arch() const noexcept {
                const auto arch_attr = node->first_attribute("arch");
                return arch_attr ? std::optional{*magic_enum::enum_cast<cpu::Arch>(arch_attr->value())} : std::nullopt;
            }
            [[nodiscard]] inline Optional<String> machine() const noexcept { return String{node->first_attribute("machine")}; }
            explicit operator TypeValue() const noexcept { return *magic_enum::enum_cast<TypeValue>(node->value()); }
        };
        struct HvmLoader : public StringNode {
            enum class Type {
                rom,
                pflash,
            };

            [[nodiscard]] inline std::optional<bool> readonly() const noexcept {
                const auto ro_attr = node->first_attribute("readonly");
                return ro_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(ro_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline std::optional<bool> secure() const noexcept {
                const auto ro_attr = node->first_attribute("secure");
                return ro_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(ro_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline std::optional<Type> type() const noexcept {
                const auto ro_attr = node->first_attribute("type");
                return ro_attr ? std::optional{*magic_enum::enum_cast<Type>(ro_attr->value())} : std::nullopt;
            }
        };
        struct HvmNvram : public StringNode {
            [[nodiscard]] inline Optional<String> template_() const noexcept { return String{node->first_attribute("template")}; }
        };
        struct Boot : public Node {
            enum class Device { hd, fd, cdrom, network };

            [[nodiscard]] inline Device dev() const noexcept { return *magic_enum::enum_cast<Device>(node->first_attribute("dev")->value()); }
        };
        struct HvmBootMenu : public Node {
            [[nodiscard]] inline bool enable() const noexcept {
                return static_cast<bool>(*magic_enum::enum_cast<YesNo>(node->first_attribute("enable")));
            }

            [[nodiscard]] inline Optional<Integral> timeout() const noexcept { return Integral{node->first_attribute("timeout")}; }
        };
        struct HvmSMBIOS : public Node {
            enum class Mode {
                emulate,
                host,
                sysinfo,
            };

            [[nodiscard]] inline Mode mode() const noexcept { return *magic_enum::enum_cast<Mode>(node->first_attribute("mode")->value()); }
        };
        struct HvmBIOS : public Node {
            [[nodiscard]] inline std::optional<bool> use_serial() const noexcept {
                const auto us_attr = node->first_attribute("useserial");
                return us_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(us_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline Optional<Integral> reboot_timeout() const noexcept { return Integral{node->first_attribute("rebootTimeout")}; }
        };
        struct HvmAcpiTable : public StringNode {
            [[nodiscard]] inline constexpr std::string_view type() const noexcept { return "slic"; }
        };
        struct ExeInit : public StringNode {};
        struct ExeInitArg : public StringNode {};
        struct ExeInitEnv : public StringNode {
            [[nodiscard]] inline String name() const noexcept { return String{node->first_attribute("name")}; }
        };
        struct ExeInitDir : public StringNode {};
        struct ExeInitUser : public StringNode {};
        struct ExeInitGroup : public StringNode {};

        [[nodiscard]] inline Type type() const noexcept { return Type{node->first_node("type")}; }
        [[nodiscard]] inline Optional<HvmLoader> hvm_loader() const noexcept { return HvmLoader{node->first_node("loader")}; }
        [[nodiscard]] inline Optional<HvmNvram> hvm_nvram() const noexcept { return HvmNvram{node->first_node("nvram")}; }
        [[nodiscard]] inline Optional<HvmBootMenu> hvm_boot_menu() const noexcept { return HvmBootMenu{node->first_node("bootmenu")}; }
        [[nodiscard]] inline Optional<HvmSMBIOS> hvm_smbios() const noexcept { return HvmSMBIOS{node->first_node("smbios")}; }
        [[nodiscard]] inline Optional<HvmBIOS> hvm_bios() const noexcept { return HvmBIOS{node->first_node("bios")}; }
        [[nodiscard]] inline Optional<NamedSpan<HvmAcpiTable>> hvm_acpi() const noexcept {
            return NamedSpan<HvmAcpiTable>{"table", node->first_node("acpi")};
        }
        [[nodiscard]] inline Optional<String> kernel() const noexcept { return String{node->first_node("kernel")}; }
        [[nodiscard]] inline Optional<String> initrd() const noexcept { return String{node->first_node("initrd")}; }
        [[nodiscard]] inline Optional<String> root() const noexcept { return String{node->first_node("root")}; }
        [[nodiscard]] inline Optional<String> cmdline() const noexcept { return String{node->first_node("cmdline")}; }
        [[nodiscard]] inline Optional<String> dtb() const noexcept { return String{node->first_node("dtb")}; }
        [[nodiscard]] inline NamedSpan<Boot> boots() const noexcept { return NamedSpan<Boot>{"boot", node}; }
        [[nodiscard]] inline Optional<ExeInit> exe_init() const noexcept { return ExeInit{node->first_node("init")}; }
        [[nodiscard]] inline NamedSpan<ExeInitArg> exe_init_args() const noexcept { return NamedSpan<ExeInitArg>{"initarg", node}; }
        [[nodiscard]] inline NamedSpan<ExeInitEnv> exe_init_env() const noexcept { return NamedSpan<ExeInitEnv>{"initenv", node}; }
        [[nodiscard]] inline Optional<ExeInitDir> exe_init_dir() const noexcept { return ExeInitDir{node->first_node("initdir")}; }
        [[nodiscard]] inline Optional<ExeInitUser> exe_init_user() const noexcept { return ExeInitUser{node->first_node("inituser")}; }
        [[nodiscard]] inline Optional<ExeInitGroup> exe_init_group() const noexcept { return ExeInitGroup{node->first_node("initgroup")}; }
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
                [[nodiscard]] inline Optional<Integral> threshold() const noexcept { return Integral{node->first_attribute("threshold")}; }
                [[nodiscard]] inline Optional<Integral> slew() const noexcept { return Integral{node->first_attribute("slew")}; }
                [[nodiscard]] inline Optional<Integral> limit() const noexcept { return Integral{node->first_attribute("limit")}; }
            };

            [[nodiscard]] inline Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
            [[nodiscard]] inline std::optional<Track> track() const noexcept {
                const auto track_attr = node->first_attribute("track");
                return track_attr ? magic_enum::enum_cast<Track>(track_attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline std::optional<TickPolicy> tick_policy() const noexcept {
                const auto track_attr = node->first_attribute("tickpolicy");
                return track_attr ? magic_enum::enum_cast<TickPolicy>(track_attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline Optional<Catchup> catchup() const noexcept { return Catchup{node->first_node("catchup")}; }
            [[nodiscard]] inline Optional<Integral> frequency() const noexcept { return Integral{node->first_attribute("frequency")}; }
            [[nodiscard]] inline std::optional<Mode> mode() const noexcept {
                const auto mode_attr = node->first_attribute("mode");
                return mode_attr ? magic_enum::enum_cast<Mode>(mode_attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline std::optional<bool> present() const noexcept {
                const auto pr_attr = node->first_attribute("present");
                return pr_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(pr_attr->value()))} : std::nullopt;
            }
        };

        [[nodiscard]] inline Offset offset() const noexcept { return *magic_enum::enum_cast<Offset>(node->first_attribute("offset")->value()); }
        [[nodiscard]] inline Optional<String> adjustment() const noexcept { return String{node->first_attribute("adjustment")}; }
        [[nodiscard]] inline Optional<String> timezone() const noexcept { return String{node->first_attribute("timezone")}; }
        [[nodiscard]] inline std::optional<Basis> basis() const noexcept {
            const auto basis_attr = node->first_attribute("basis");
            return basis_attr ? magic_enum::enum_cast<Basis>(basis_attr->value()) : std::nullopt;
        }

        [[nodiscard]] inline NamedSpan<Timer> timers() const noexcept { return NamedSpan<Timer>{"timer", node}; }
    };
    struct Pm : public Node {
        struct SuspendTo : public Node {
            [[nodiscard]] inline std::optional<bool> enabled() const noexcept { return bool_wrap_attr<YesNo, Optional>(node, "enabled"); }
        };

        [[nodiscard]] inline Optional<SuspendTo> suspend_to_mem() const noexcept { return SuspendTo{node->first_node("suspend-to-mem")}; }
        [[nodiscard]] inline Optional<SuspendTo> suspend_to_disk() const noexcept { return SuspendTo{node->first_node("suspend-to-disk")}; }
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

            [[nodiscard]] inline Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
            [[nodiscard]] inline bool enabled() const noexcept {
                return static_cast<bool>(*magic_enum::enum_cast<YesNo>(node->first_attribute("enabled")->value()));
            }
        };

        [[nodiscard]] inline NamedSpan<Event> events() const noexcept { return NamedSpan<Event>{"event", node}; }
    };
    struct IdMap : public Node {
        struct Id : public Node {
            [[nodiscard]] inline Integral start() const noexcept { return Integral{node->first_attribute("start")}; }
            [[nodiscard]] inline Integral target() const noexcept { return Integral{node->first_attribute("target")}; }
            [[nodiscard]] inline Integral count() const noexcept { return Integral{node->first_attribute("count")}; }
        };

        [[nodiscard]] inline NamedSpan<Id> uids() const noexcept { return NamedSpan<Id>{"uid", node}; }
        [[nodiscard]] inline NamedSpan<Id> gids() const noexcept { return NamedSpan<Id>{"gid", node}; }
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
            [[nodiscard]] inline Optional<String> model() const noexcept { return String{node->first_attribute("model")}; }
            [[nodiscard]] inline std::optional<bool> relabel() const noexcept {
                const auto rlb_attr = node->first_attribute("relabel");
                return rlb_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(rlb_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline std::optional<bool> labelskip() const noexcept {
                const auto lbs_attr = node->first_attribute("labelskip");
                return lbs_attr ? std::optional{static_cast<bool>(magic_enum::enum_cast<YesNo>(lbs_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline NamedSpan<StringNode> labels() const noexcept { return NamedSpan<StringNode>{"label", node}; }
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

                    [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                    [[nodiscard]] inline Optional<Uuid> uuid() const noexcept { return Uuid{node->first_attribute("uuid")}; }
                    [[nodiscard]] inline String usage() const noexcept { return String{node->first_attribute("usage")}; }
                };

                [[nodiscard]] inline String username() const noexcept { return String{node->first_attribute("username")}; }
                [[nodiscard]] inline Secret secret() const noexcept { return Secret{node->first_node("secret")}; }
            };
            struct Snapshot : public Node {
                [[nodiscard]] inline String name() const noexcept { return String{node->first_attribute("name")}; }
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

                    [[nodiscard]] inline std::optional<Transport> transport() const noexcept {
                        const auto transport_attr = node->first_attribute("transport");
                        return transport_attr ? magic_enum::enum_cast<Transport>(transport_attr->value()) : std::nullopt;
                    }
                    [[nodiscard]] inline Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
                    [[nodiscard]] inline Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
                    [[nodiscard]] inline Optional<String> socket() const noexcept { return String{node->first_attribute("socket")}; }
                };
                struct Config : public Node {
                    [[nodiscard]] inline String file() const noexcept { return String{node->first_attribute("file")}; }
                };

                [[nodiscard]] inline Optional<String> file() const noexcept { return String{node->first_attribute("file")}; }
                [[nodiscard]] inline Optional<String> dev() const noexcept { return String{node->first_attribute("dev")}; }
                [[nodiscard]] inline Optional<String> dir() const noexcept { return String{node->first_attribute("dir")}; }
                [[nodiscard]] inline Optional<String> pool() const noexcept { return String{node->first_attribute("pool")}; }
                [[nodiscard]] inline Optional<String> volume() const noexcept { return String{node->first_attribute("volume")}; }
                [[nodiscard]] inline std::optional<Mode> mode() const noexcept {
                    const auto mode_attr = node->first_attribute("mode");
                    return mode_attr ? magic_enum::enum_cast<Mode>(mode_attr->value()) : std::nullopt;
                }
                [[nodiscard]] inline std::optional<Protocol> protocol() const noexcept {
                    const auto proto_attr = node->first_attribute("protocol");
                    return proto_attr ? magic_enum::enum_cast<Protocol>(proto_attr->value()) : std::nullopt;
                }
                [[nodiscard]] inline Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
                [[nodiscard]] inline std::optional<bool> tls() const noexcept {
                    const auto tls_attr = node->first_attribute("tls");
                    return tls_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(tls_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline NamedSpan<Host> hosts() const noexcept { return NamedSpan<Host>{"host", node}; }
                [[nodiscard]] inline Optional<Snapshot> snapshot() const noexcept { return Snapshot{node->first_node("snapshot")}; }
                [[nodiscard]] inline Optional<Config> config() const noexcept { return Config{node->first_node("config")}; }
                [[nodiscard]] inline Optional<Auth> auth() const noexcept { return Auth{node->first_node("auth")}; }
                [[nodiscard]] inline Optional<storage::Encryption> encryption() const noexcept {
                    return storage::Encryption{node->first_node("encryption")};
                }
                [[nodiscard]] inline NamedSpan<SecLabel> seclabels() const noexcept { return NamedSpan<SecLabel>{"seclabel", node}; }
            };
            struct BackingStore : public Node {
                struct Format : public Node {
                    [[nodiscard]] inline std::optional<storage::Format> type() const noexcept {
                        const auto type_attr = node->first_attribute("type");
                        return type_attr ? magic_enum::enum_cast<storage::Format>(type_attr->value()) : std::nullopt;
                    }
                };
                [[nodiscard]] inline Optional<Integral> index() const noexcept { return Integral{node->first_attribute("index")}; }
                [[nodiscard]] inline Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
                [[nodiscard]] inline Optional<BackingStore> next() const noexcept { return BackingStore{node->first_node("backingStore")}; }
                [[nodiscard]] inline Optional<Format> format() const noexcept { return Format{node->first_node("format")}; }
            };

            [[nodiscard]] inline std::optional<Device> device() const noexcept {
                const auto dev_attr = node->first_attribute("device");
                return dev_attr ? magic_enum::enum_cast<Device>(dev_attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline std::optional<Type> type() const noexcept {
                const auto type_attr = node->first_attribute("type");
                return type_attr ? magic_enum::enum_cast<Type>(type_attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
            [[nodiscard]] inline Optional<BackingStore> backing_store() const noexcept { return BackingStore{node->first_node("backingStore")}; }
            [[nodiscard]] inline std::optional<bool> raw_io() const noexcept {
                const auto rio_attr = node->first_attribute("raw_io");
                return rio_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(rio_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline std::optional<SgIO> sg_io() const noexcept {
                const auto sgio_attr = node->first_attribute("sgio");
                return sgio_attr ? magic_enum::enum_cast<SgIO>(sgio_attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline Optional<Snapshot> snapshot() const noexcept { return Snapshot{node->first_node("snapshot")}; }
        };
        struct Ip : public Node {
            [[nodiscard]] inline String address() const noexcept { return String{node->first_attribute("address")}; }
            [[nodiscard]] inline Optional<String> family() const noexcept { return String{node->first_attribute("family")}; }
            [[nodiscard]] inline Optional<Integral> prefix() const noexcept { return Integral{node->first_attribute("prefix")}; }
            [[nodiscard]] inline Optional<String> peer() const noexcept { return String{node->first_attribute("peer")}; }
        };
        struct Route : public Node {
            [[nodiscard]] inline Optional<String> family() const noexcept { return String{node->first_attribute("family")}; }
            [[nodiscard]] inline String address() const noexcept { return String{node->first_attribute("address")}; }
            [[nodiscard]] inline Optional<String> netmask() const noexcept { return String{node->first_attribute("netmask")}; }
            [[nodiscard]] inline Optional<Integral> prefix() const noexcept { return Integral{node->first_attribute("prefix")}; }
            [[nodiscard]] inline String gateway() const noexcept { return String{node->first_attribute("gateway")}; }
            [[nodiscard]] inline Optional<Integral> metric() const noexcept { return Integral{node->first_attribute("metric")}; }
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
                [[nodiscard]] inline Optional<Integral> start_port() const noexcept { return Integral{node->first_attribute("startport")}; }
            };
            struct PciTarget : public Node {
                [[nodiscard]] inline Optional<Integral> chassis_nr() const noexcept { return Integral{Node::node->first_attribute("chassisNr")}; }
                [[nodiscard]] inline Optional<Integral> chassis() const noexcept { return Integral{Node::node->first_attribute("chassis")}; }
                [[nodiscard]] inline Optional<Integral> port() const noexcept { return Integral{Node::node->first_attribute("port")}; }
                [[nodiscard]] inline Optional<Integral> bus_nr() const noexcept { return Integral{Node::node->first_attribute("busNr")}; }
                [[nodiscard]] inline Optional<Integral> index() const noexcept { return Integral{Node::node->first_attribute("index")}; }
                [[nodiscard]] inline Optional<Integral> node() const noexcept { return Integral{Node::node->first_node("node")}; }
            };
            struct PciHole64 : public Node {
                [[nodiscard]] inline Optional<String> unit() const noexcept { return String{node->first_attribute("unit")}; }
                [[nodiscard]] inline Integral value() const noexcept { return Integral{node}; }
            };
            struct Driver : public Node {
                [[nodiscard]] inline Optional<Integral> queues() const noexcept { return Integral{node->first_attribute("queues")}; }
                [[nodiscard]] inline Optional<Integral> cmd_per_lun() const noexcept { return Integral{node->first_attribute("cmd_per_lun")}; }
                [[nodiscard]] inline Optional<Integral> max_sectors() const noexcept { return Integral{node->first_attribute("max_sectors")}; }
                [[nodiscard]] inline std::optional<bool> ioeventfd() const noexcept {
                    const auto ioefd_attr = node->first_attribute("ioeventfd");
                    return ioefd_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ioefd_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline Optional<Integral> iothread() const noexcept { return Integral{node->first_attribute("iothread")}; }
                [[nodiscard]] inline std::optional<bool> iommu() const noexcept {
                    const auto iommu_attr = node->first_attribute("iommu");
                    return iommu_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(iommu_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline std::optional<bool> ats() const noexcept {
                    const auto ats_attr = node->first_attribute("ats");
                    return ats_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ats_attr->value()))} : std::nullopt;
                }
            };

            [[nodiscard]] inline Optional<Integral> index() const noexcept { return Integral{node->first_attribute("index")}; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
            [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] inline std::optional<Model> model() const noexcept {
                const auto model_attr = node->first_attribute("model");
                if (!model_attr)
                    return std::nullopt;
                std::string in{model_attr->value()};
                std::replace(in.begin(), in.end(), '-', '_');
                return magic_enum::enum_cast<Model>(in);
            }
            [[nodiscard]] inline Optional<UsbMaster> usb_master() const noexcept { return UsbMaster{node->first_node("address")}; }
            [[nodiscard]] inline Optional<Integral> ports() const noexcept { return Integral{node->first_attribute("ports")}; }
            [[nodiscard]] inline Optional<PciTarget> pci_target() const noexcept { return PciTarget{node->first_node("target")}; }
            [[nodiscard]] inline Optional<PciHole64> pci_hole64() const noexcept { return PciHole64{node->first_node("pcihole64")}; }
            [[nodiscard]] inline Optional<Integral> vio_vectors() const noexcept { return Integral{node->first_attribute("vectors")}; }
            [[nodiscard]] inline Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
        };
        struct Lease : public Node {
            struct Target : public Node {
                [[nodiscard]] inline String path() const noexcept { return String{node->first_attribute("path")}; }
                [[nodiscard]] inline Optional<Integral> offset() const noexcept { return Integral{node->first_attribute("offset")}; }
            };

            [[nodiscard]] inline String lockspace() const noexcept { return String{node->first_node("lockspace")}; }
            [[nodiscard]] inline String key() const noexcept { return String{node->first_node("key")}; }
            [[nodiscard]] inline Target target() const noexcept { return Target{node->first_node("target")}; }
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

                [[nodiscard]] inline std::optional<Type> type() const noexcept {
                    const auto type_attr = node->first_attribute("type");
                    return type_attr ? magic_enum::enum_cast<Type>(type_attr->value()) : std::nullopt;
                }
                [[nodiscard]] inline String wrpolicy() const noexcept { return String{node->first_attribute("wrpolicy")}; }
                [[nodiscard]] inline std::optional<bool> iommu() const noexcept {
                    const auto iommu_attr = node->first_attribute("iommu");
                    return iommu_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(iommu_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline std::optional<bool> ats() const noexcept {
                    const auto ats_attr = node->first_attribute("ats");
                    return ats_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ats_attr->value()))} : std::nullopt;
                }
            };
            struct Source : public Node {
                [[nodiscard]] inline Optional<String> file() const noexcept { return String{node->first_attribute("file")}; }
                [[nodiscard]] inline Optional<String> dev() const noexcept { return String{node->first_attribute("dev")}; }
                [[nodiscard]] inline Optional<String> dir() const noexcept { return String{node->first_attribute("dir")}; }
                [[nodiscard]] inline Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
                [[nodiscard]] inline Optional<Integral> usage() const noexcept { return Integral{node->first_attribute("usage")}; }
                [[nodiscard]] inline Optional<String> units() const noexcept { return String{node->first_attribute("units")}; }
            };
            struct Target : public Node {
                [[nodiscard]] inline String dir() const noexcept { return String{node->first_attribute("dir")}; }
            };
            struct SpaceLimit : public Node {
                [[nodiscard]] inline Optional<String> unit() const noexcept { return String{node->first_attribute("unit")}; }
                [[nodiscard]] inline Integral value() const noexcept { return Integral{node}; }
            };

            [[nodiscard]] inline std::optional<Type> type() const noexcept {
                const auto type_attr = node->first_attribute("type");
                return type_attr ? magic_enum::enum_cast<Type>(type_attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
            [[nodiscard]] inline Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
            [[nodiscard]] inline Target target() const noexcept { return Target{node->first_node("target")}; }
            [[nodiscard]] inline std::optional<AccessMode> access_mode() const noexcept {
                const auto am_attr = node->first_attribute("accessmode");
                return am_attr ? magic_enum::enum_cast<AccessMode>(am_attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline bool readonly() const noexcept { return node->first_node("readonly") != nullptr; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
            [[nodiscard]] inline Optional<SpaceLimit> hard_space_limit() const noexcept { return SpaceLimit{node->first_node("hard_space_limit")}; }
            [[nodiscard]] inline Optional<SpaceLimit> soft_space_limit() const noexcept { return SpaceLimit{node->first_node("soft_space_limit")}; }
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
                    [[nodiscard]] inline Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
                    [[nodiscard]] inline Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
                };
                struct Vendor : public Node {
                    [[nodiscard]] inline Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                };
                struct Product : public Node {
                    [[nodiscard]] inline Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                };
                struct HostdevAddress : public Node {
                    enum class Type {
                        pci,
                        usb,
                    };

                    [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                    [[nodiscard]] inline Optional<Integral> usb_bus() const noexcept { return Integral{node->first_attribute("bus")}; }
                    [[nodiscard]] inline Optional<Integral> usb_device() const noexcept { return Integral{node->first_attribute("device")}; }
                    [[nodiscard]] inline Optional<Integral> pci_domain() const noexcept { return Integral{node->first_attribute("domain")}; }
                    [[nodiscard]] inline Optional<Integral> pci_bus() const noexcept { return Integral{node->first_attribute("bus")}; }
                    [[nodiscard]] inline Optional<String> pci_function() const noexcept { return String{node->first_attribute("domain")}; }
                    [[nodiscard]] inline std::optional<bool> pci_multifunction() const noexcept {
                        const auto mf_attr = node->first_attribute("multifunction");
                        return mf_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(mf_attr->value()))} : std::nullopt;
                    }
                };

                [[nodiscard]] inline Optional<String> bridge() const noexcept { return String{node->first_attribute("bridge")}; }
                [[nodiscard]] inline Optional<String> network() const noexcept { return String{node->first_attribute("network")}; }
                [[nodiscard]] inline Optional<String> port_group() const noexcept { return String{node->first_attribute("portgroup")}; }
                [[nodiscard]] inline NamedSpan<Ip> ethernet_ips() const noexcept { return NamedSpan<Ip>{"ip", node}; }
                [[nodiscard]] inline NamedSpan<Route> ethernet_routes() const noexcept { return NamedSpan<Route>{"route", node}; }
                [[nodiscard]] inline std::optional<VhostUserType> vhostuser_type() const noexcept {
                    const auto type_attr = node->first_attribute("type");
                    return type_attr ? magic_enum::enum_cast<VhostUserType>(type_attr->value()) : std::nullopt;
                }
                [[nodiscard]] inline Optional<String> vhostuser_path() const noexcept { return String{node->first_attribute("path")}; }
                [[nodiscard]] inline std::optional<VHostUserMode> vhostuser_mode() const noexcept {
                    const auto mode_attr = node->first_attribute("mode");
                    return mode_attr ? magic_enum::enum_cast<VHostUserMode>(mode_attr->value()) : std::nullopt;
                }
                [[nodiscard]] inline Optional<Reconnect> vhostuser_reconnect() const noexcept { return Reconnect{node->first_node("reconnect")}; }
                [[nodiscard]] inline Optional<String> direct_dev() const noexcept { return String{node->first_attribute("dev")}; }
                [[nodiscard]] inline std::optional<BridgeMode> direct_mode() const noexcept {
                    const auto mode_attr = node->first_attribute("mode");
                    return mode_attr ? magic_enum::enum_cast<BridgeMode>(mode_attr->value()) : std::nullopt;
                }
                [[nodiscard]] inline Optional<String> internal_name() const noexcept { return String{node->first_attribute("name")}; }
                [[nodiscard]] inline Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
                [[nodiscard]] inline Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
                [[nodiscard]] inline Optional<UdpLocal> udp_local() const noexcept { return UdpLocal{node->first_node("local")}; }
                [[nodiscard]] inline std::optional<bool> hostdev_missing() const noexcept {
                    const auto attr = node->first_attribute("missing");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline Optional<Vendor> hostdev_vendor() const noexcept { return Vendor{node->first_node("vendor")}; }
                [[nodiscard]] inline Optional<Product> hostdev_product() const noexcept { return Product{node->first_node("product")}; }
                [[nodiscard]] inline Optional<HostdevAddress> hostdev_address() const noexcept { return HostdevAddress{node->first_node("address")}; }
            };
            struct Link : public Node {
                enum class State : bool {
                    down = false,
                    up = true,
                };
                [[nodiscard]] inline State state() const noexcept { return *magic_enum::enum_cast<State>(node->first_attribute("state")->value()); }
            };
            struct Mtu : public Node {
                [[nodiscard]] inline Integral size() const noexcept { return Integral{node->first_attribute("size")}; }
            };
            struct Coalesce : public Node {
                struct Rx : public Node {
                    struct Frames : public Node {
                        [[nodiscard]] inline Optional<Integral> max() const noexcept { return Integral{node->first_attribute("max")}; }
                    };
                    [[nodiscard]] inline Optional<Frames> frames() const noexcept { return Frames{node->first_node("frames")}; }
                };
                [[nodiscard]] inline Optional<Rx> rx() const noexcept { return Rx{node->first_node("rx")}; }
            };
            struct Target : public Node {
                [[nodiscard]] inline String dev() const noexcept { return String{node->first_attribute("dev")}; }
            };
            struct Guest : public Node {
                [[nodiscard]] inline Optional<String> dev() const noexcept { return String{node->first_attribute("dev")}; }
                [[nodiscard]] inline Optional<String> actual() const noexcept { return String{node->first_attribute("actual")}; }
            };
            struct Mac : public Node {
                [[nodiscard]] inline String address() const noexcept { return String{node->first_attribute("address")}; }
            };
            struct Script : public Node {
                [[nodiscard]] inline String path() const noexcept { return String{node->first_attribute("path")}; }
            };
            struct BackendDomain : public Node {
                [[nodiscard]] inline String name() const noexcept { return String{node->first_attribute("name")}; }
            };
            struct Model : public Node {
                [[nodiscard]] inline String type() const noexcept { return String{node->first_attribute("type")}; }
            };
            struct Backend : public Node {
                [[nodiscard]] inline Optional<String> tap() const noexcept { return String{node->first_attribute("tap")}; }
                [[nodiscard]] inline Optional<String> vhost() const noexcept { return String{node->first_attribute("vhost")}; }
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
                    [[nodiscard]] inline std::optional<bool> csum() const noexcept {
                        const auto attr = node->first_attribute("csum");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                    [[nodiscard]] inline std::optional<bool> tso4() const noexcept {
                        const auto attr = node->first_attribute("tso4");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                    [[nodiscard]] inline std::optional<bool> tso6() const noexcept {
                        const auto attr = node->first_attribute("tso6");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                    [[nodiscard]] inline std::optional<bool> ecn() const noexcept {
                        const auto attr = node->first_attribute("ecn");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                    [[nodiscard]] inline std::optional<bool> ufo() const noexcept {
                        const auto attr = node->first_attribute("ufo");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                };
                struct Host : public Guest {
                    [[nodiscard]] inline std::optional<bool> gso() const noexcept {
                        const auto attr = node->first_attribute("gso");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                    [[nodiscard]] inline std::optional<bool> mrg_rxbuf() const noexcept {
                        const auto attr = node->first_attribute("mrg_rxbuf");
                        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                    }
                };

                [[nodiscard]] inline std::optional<Name> name() const noexcept {
                    const auto track_attr = node->first_attribute("name");
                    return track_attr ? magic_enum::enum_cast<Name>(track_attr->value()) : std::nullopt;
                }
                [[nodiscard]] inline Optional<Integral> queues() const noexcept { return Integral{node->first_attribute("queues")}; }
                [[nodiscard]] inline Optional<Integral> rx_queue_size() const noexcept { return Integral{node->first_attribute("rx_queue_size")}; }
                [[nodiscard]] inline Optional<Integral> tx_queue_size() const noexcept { return Integral{node->first_attribute("tx_queue_size")}; }
                [[nodiscard]] inline std::optional<TxMode> tx_mode() const noexcept {
                    const auto track_attr = node->first_attribute("txmode");
                    return track_attr ? magic_enum::enum_cast<TxMode>(track_attr->value()) : std::nullopt;
                }
                [[nodiscard]] inline std::optional<bool> ioeventfd() const noexcept {
                    const auto attr = node->first_attribute("ioeventfd");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline std::optional<bool> event_idx() const noexcept {
                    const auto attr = node->first_attribute("event_idx");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline std::optional<bool> iommu() const noexcept {
                    const auto iommu_attr = node->first_attribute("iommu");
                    return iommu_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(iommu_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline std::optional<bool> ats() const noexcept {
                    const auto ats_attr = node->first_attribute("ats");
                    return ats_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ats_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline Optional<Host> host() const noexcept { return Host{node->first_node("host")}; }
                [[nodiscard]] inline Optional<Guest> guest() const noexcept { return Guest{node->first_node("guest")}; }
            };
            struct Tune : public Node {
                [[nodiscard]] inline Optional<Integral> sndbuf() const noexcept { return Integral{node->first_node("sndbuf")}; }
            };
            struct FilterRef : public Node {
                struct Parameter : public Node {
                    [[nodiscard]] inline String name() const noexcept { return String{node->first_attribute("name")}; }
                    [[nodiscard]] inline String value() const noexcept { return String{node->first_attribute("value")}; }
                };

                [[nodiscard]] inline String nc_name() const noexcept { return String{node->first_attribute("NCName")}; }
                [[nodiscard]] inline NamedSpan<Parameter> parameters() const noexcept { return NamedSpan<Parameter>{"parameter", node}; }
            };

            [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] inline Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
            [[nodiscard]] inline std::optional<bool> hostdev_managed() const noexcept {
                const auto attr = node->first_attribute("managed");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline Optional<VirtualPort> virtual_port() const noexcept { return VirtualPort{node->first_node("virtualport")}; }
            [[nodiscard]] inline Optional<Link> link() const noexcept { return Link{node->first_node("link")}; }
            [[nodiscard]] inline Optional<Mtu> mtu() const noexcept { return Mtu{node->first_node("mtu")}; }
            [[nodiscard]] inline Optional<Coalesce> coalesce() const noexcept { return Coalesce{node->first_node("coalesce")}; }
            [[nodiscard]] inline Optional<Target> target() const noexcept { return Target{node->first_node("target")}; }
            [[nodiscard]] inline Optional<Guest> guest() const noexcept { return Guest{node->first_node("guest")}; }
            [[nodiscard]] inline Optional<Mac> mac() const noexcept { return Mac{node->first_node("mac")}; }
            [[nodiscard]] inline NamedSpan<Ip> ips() const noexcept { return NamedSpan<Ip>{"ip", node}; }
            [[nodiscard]] inline Optional<Script> script() const noexcept { return Script{node->first_node("script")}; }
            [[nodiscard]] inline Optional<BackendDomain> backend_domain() const noexcept { return BackendDomain{node->first_node("backenddomain")}; }
            [[nodiscard]] inline Optional<Model> model() const noexcept { return Model{node->first_node("model")}; }
            [[nodiscard]] inline Optional<Backend> backend() const noexcept { return Backend{node->first_node("backend")}; }
            [[nodiscard]] inline Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
            [[nodiscard]] inline Optional<FilterRef> filter_ref() const noexcept { return FilterRef{node->first_node("filterref")}; }
            [[nodiscard]] inline Optional<Tune> tune() const noexcept { return Tune{node->first_node("tune")}; }
            [[nodiscard]] inline Optional<Boot> boot() const noexcept { return Boot{node->first_node("boot")}; }
            [[nodiscard]] inline Optional<Rom> rom() const noexcept { return Rom{node->first_node("rom")}; }
            [[nodiscard]] inline Optional<Bandwidth> bandwidth() const noexcept { return Bandwidth{node->first_node("bandwidth")}; }
            [[nodiscard]] inline Optional<Vlan> vlan() const noexcept { return Vlan{node->first_node("vlan")}; }
            [[nodiscard]] inline std::optional<bool> trust_guest_rx_filters() const noexcept {
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
                [[nodiscard]] inline std::optional<bool> iommu() const noexcept {
                    const auto iommu_attr = node->first_attribute("iommu");
                    return iommu_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(iommu_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline std::optional<bool> ats() const noexcept {
                    const auto ats_attr = node->first_attribute("ats");
                    return ats_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ats_attr->value()))} : std::nullopt;
                }
            };
            struct Source : public Node {
                [[nodiscard]] inline String evdev() const noexcept { return String{node->first_attribute("evdev")}; }
            };

            [[nodiscard]] inline Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
            [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] inline Bus bus() const noexcept { return *magic_enum::enum_cast<Bus>(node->first_attribute("bus")->value()); }
            [[nodiscard]] inline Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
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

                [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            };

            [[nodiscard]] inline Model model() const noexcept { return *magic_enum::enum_cast<Model>(node->first_attribute("model")->value()); }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
            [[nodiscard]] inline NamedSpan<Codec> codecs() const noexcept { return NamedSpan<Codec>{"codec", node}; }
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

                [[nodiscard]] inline Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
            };
            struct Source : public Node {
                enum class Protocol {
                    adapter,
                    iscsi,
                };

                struct PciAddress : public Node {
                    [[nodiscard]] inline Optional<Integral> domain() const noexcept { return Integral{node->first_attribute("domain")}; }
                    [[nodiscard]] inline Optional<Integral> bus() const noexcept { return Integral{node->first_attribute("bus")}; }
                    [[nodiscard]] inline Optional<String> function() const noexcept { return String{node->first_attribute("domain")}; }
                    [[nodiscard]] inline std::optional<bool> multifunction() const noexcept {
                        const auto mf_attr = node->first_attribute("multifunction");
                        return mf_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(mf_attr->value()))} : std::nullopt;
                    }
                };
                struct Vendor : public Node {
                    [[nodiscard]] inline Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                };
                struct Product : public Node {
                    [[nodiscard]] inline Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
                };
                struct UsbScsiAddress : public Node {
                    enum class Type {
                        pci,
                        usb,
                    };

                    [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                    [[nodiscard]] inline Optional<Integral> bus() const noexcept { return Integral{node->first_attribute("bus")}; }
                    [[nodiscard]] inline Optional<Integral> usb_device() const noexcept { return Integral{node->first_attribute("device")}; }
                    [[nodiscard]] inline Optional<Integral> scsi_target() const noexcept { return Integral{node->first_attribute("target")}; }
                    [[nodiscard]] inline Optional<String> scsi_unit() const noexcept { return String{node->first_attribute("uint")}; }
                };
                struct IscsiHost : public Node {
                    [[nodiscard]] inline String name() const noexcept { return String{node->first_attribute("name")}; }
                    [[nodiscard]] inline Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
                };
                struct MdevAddress : public Node {
                    [[nodiscard]] inline Optional<Uuid> uuid() const noexcept { return Uuid{node->first_node("uuid")}; }
                };

                [[nodiscard]] inline std::optional<StartupPolicy> startup_policy() const noexcept {
                    const auto attr = node->first_attribute("startupPolicy");
                    return attr ? magic_enum::enum_cast<StartupPolicy>(attr->value()) : std::nullopt;
                }
                [[nodiscard]] inline Optional<PciAddress> pci_address() const noexcept { return PciAddress{node->first_node("address")}; }
                [[nodiscard]] inline Optional<Vendor> vendor() const noexcept { return Vendor{node->first_node("vendor")}; }
                [[nodiscard]] inline Optional<Product> product() const noexcept { return Product{node->first_node("product")}; }
                [[nodiscard]] inline Optional<UsbScsiAddress> usb_address() const noexcept { return UsbScsiAddress{node->first_node("address")}; }
                [[nodiscard]] inline Optional<UsbScsiAddress> scsi_address() const noexcept { return UsbScsiAddress{node->first_node("address")}; }
                [[nodiscard]] inline NamedSpan<IscsiHost> iscsi_hosts() const noexcept { return NamedSpan<IscsiHost>{"host", node}; }
                [[nodiscard]] inline Optional<Disk::Auth> iscsi_auth() const noexcept { return Disk::Auth{node->first_node("auth")}; }
                [[nodiscard]] inline Optional<String> vhost_wwpn() const noexcept { return String{node->first_attribute("wwpn")}; }
                [[nodiscard]] inline Optional<MdevAddress> mdev_address() const noexcept { return MdevAddress{node->first_node("address")}; }
                [[nodiscard]] inline Optional<String> storage_block() const noexcept { return String{node->first_attribute("block")}; }
                [[nodiscard]] inline Optional<String> misc_char() const noexcept { return String{node->first_attribute("char")}; }
                [[nodiscard]] inline Optional<String> net_interface() const noexcept { return String{node->first_attribute("interface")}; }
            };

            [[nodiscard]] inline std::optional<Mode> mode() const noexcept {
                const auto attr = node->first_attribute("mode");
                return attr ? magic_enum::enum_cast<Mode>(attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline std::optional<bool> managed() const noexcept {
                const auto attr = node->first_attribute("managed");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] inline Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
            [[nodiscard]] inline Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
            [[nodiscard]] inline std::optional<SgIO> sg_io() const noexcept {
                const auto sgio_attr = node->first_attribute("sgio");
                return sgio_attr ? magic_enum::enum_cast<SgIO>(sgio_attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline std::optional<bool> raw_io() const noexcept {
                const auto rio_attr = node->first_attribute("raw_io");
                return rio_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(rio_attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline Optional<Adapter> adapter() const noexcept { return Adapter{node->first_node("adapter")}; }
            [[nodiscard]] inline Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
            [[nodiscard]] inline std::optional<Model> model() const noexcept {
                const auto track_attr = node->first_attribute("model");
                if (!track_attr)
                    return std::nullopt;
                std::string in{track_attr->value()};
                std::replace(in.begin(), in.end(), '-', '_');
                return magic_enum::enum_cast<Model>(in);
            }
            [[nodiscard]] inline NamedSpan<Ip> net_ips() const noexcept { return NamedSpan<Ip>{"ip", node}; }
            [[nodiscard]] inline NamedSpan<Route> net_routes() const noexcept { return NamedSpan<Route>{"route", node}; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Boot> boot() const noexcept { return Boot{node->first_node("boot")}; }
            [[nodiscard]] inline Optional<Rom> rom() const noexcept { return Rom{node->first_node("rom")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
            [[nodiscard]] inline bool readonly() const noexcept { return node->first_node("readonly") != nullptr; }
            [[nodiscard]] inline bool shareable() const noexcept { return node->first_node("shareable") != nullptr; }
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

                [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                [[nodiscard]] inline Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
                [[nodiscard]] inline Optional<String> socket() const noexcept { return String{node->first_attribute("socket")}; }
                [[nodiscard]] inline Optional<String> network() const noexcept { return String{node->first_attribute("network")}; }
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

                [[nodiscard]] inline Name name() const noexcept { return *magic_enum::enum_cast<Name>(node->first_attribute("name")->value()); }
                [[nodiscard]] inline Mode mode() const noexcept { return *magic_enum::enum_cast<Mode>(node->first_attribute("mode")->value()); }
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

                [[nodiscard]] inline Compression compression() const noexcept {
                    return *magic_enum::enum_cast<Compression>(node->first_attribute("compression")->value());
                }
            };
            struct JpegZlib : public Node {
                enum class Compression {
                    auto_,
                    never,
                    always,
                };

                [[nodiscard]] inline Compression compression() const noexcept {
                    return *magic_enum::enum_cast<Compression>(node->first_attribute("compression")->value());
                }
            };
            struct Playback : public Node {
                [[nodiscard]] inline bool compression() const noexcept {
                    return static_cast<bool>(*magic_enum::enum_cast<OnOff>(node->first_attribute("compression")->value()));
                }
            };
            struct Streaming : public Node {
                enum class Mode {
                    filter,
                    all,
                    off,
                };

                [[nodiscard]] inline Mode mode() const noexcept { return *magic_enum::enum_cast<Mode>(node->first_attribute("mode")->value()); }
            };
            struct Clipboard : public Node {
                [[nodiscard]] inline bool copypaste() const noexcept {
                    return static_cast<bool>(*magic_enum::enum_cast<OnOff>(node->first_attribute("copypaste")->value()));
                }
            };
            struct Mouse : public Node {
                enum class Mode {
                    server,
                    client,
                };

                [[nodiscard]] inline Mode mode() const noexcept { return *magic_enum::enum_cast<Mode>(node->first_attribute("mode")->value()); }
            };
            struct EnableC : public Node {
                [[nodiscard]] inline bool enable() const noexcept {
                    return static_cast<bool>(*magic_enum::enum_cast<YesNo>(node->first_attribute("copypaste")->value()));
                }
            };
            struct Gl : public EnableC {
                [[nodiscard]] inline Optional<String> render_node() const noexcept { return String{node->first_attribute("rendernode")}; }
            };

            [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
            [[nodiscard]] inline Optional<String> display() const noexcept { return String{node->first_attribute("display")}; }
            [[nodiscard]] inline Optional<String> xauth() const noexcept { return String{node->first_attribute("xauth")}; }
            [[nodiscard]] inline std::optional<bool> fullscreen() const noexcept {
                const auto attr = node->first_attribute("fullscreen");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline Optional<Integral> port() const noexcept { return Integral{node->first_attribute("port")}; }
            [[nodiscard]] inline std::optional<bool> autoport() const noexcept {
                const auto attr = node->first_attribute("autoport");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline Optional<Integral> websocket() const noexcept { return Integral{node->first_attribute("websocket")}; }
            [[nodiscard]] inline Optional<String> listen() const noexcept { return String{node->first_attribute("listen")}; }
            [[nodiscard]] inline std::optional<SharePolicy> share_policy() const noexcept {
                const auto track_attr = node->first_attribute("sharePolicy");
                if (!track_attr)
                    return std::nullopt;
                std::string in{track_attr->value()};
                std::replace(in.begin(), in.end(), '-', '_');
                return magic_enum::enum_cast<SharePolicy>(in);
            }
            [[nodiscard]] inline Optional<String> socket() const noexcept { return String{node->first_attribute("socket")}; }
            [[nodiscard]] inline Optional<String> passwd() const noexcept { return String{node->first_attribute("passwd")}; }
            [[nodiscard]] inline Optional<String> keymap() const noexcept { return String{node->first_attribute("keymap")}; }
            [[nodiscard]] inline Optional<String> passwd_valid_to() const noexcept { return String{node->first_attribute("passwdValidTo")}; }
            [[nodiscard]] inline Optional<Integral> tls_port() const noexcept { return Integral{node->first_attribute("tlsPort")}; }
            [[nodiscard]] inline std::optional<Connected> connected() const noexcept {
                const auto attr = node->first_attribute("connected");
                return attr ? magic_enum::enum_cast<Connected>(attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline std::optional<Mode> default_mode() const noexcept {
                const auto attr = node->first_attribute("defaultMode");
                return attr ? magic_enum::enum_cast<Mode>(attr->value()) : std::nullopt;
            }
            [[nodiscard]] inline NamedSpan<Listen> listens() const noexcept { return NamedSpan<Listen>{"listen", node}; }
            [[nodiscard]] inline NamedSpan<Channel> channels() const noexcept { return NamedSpan<Channel>{"channel", node}; }
            [[nodiscard]] inline Optional<Image> image() const noexcept { return Image{node->first_node("image")}; }
            [[nodiscard]] inline Optional<JpegZlib> jpeg() const noexcept { return JpegZlib{node->first_node("jpeg")}; }
            [[nodiscard]] inline Optional<JpegZlib> zlib() const noexcept { return JpegZlib{node->first_node("zlib")}; }
            [[nodiscard]] inline Optional<Playback> playback() const noexcept { return Playback{node->first_node("playback")}; }
            [[nodiscard]] inline Optional<Streaming> streaming() const noexcept { return Streaming{node->first_node("streaming")}; }
            [[nodiscard]] inline Optional<Clipboard> clipboard() const noexcept { return Clipboard{node->first_node("clipboard")}; }
            [[nodiscard]] inline Optional<Mouse> mouse() const noexcept { return Mouse{node->first_node("mouse")}; }
            [[nodiscard]] inline Optional<EnableC> filetransfer() const noexcept { return EnableC{node->first_node("filetransfer")}; }
            [[nodiscard]] inline Optional<Gl> gl() const noexcept { return Gl{node->first_node("gl")}; }
            [[nodiscard]] inline std::optional<bool> replace_user() const noexcept {
                const auto attr = node->first_attribute("replaceUser");
                return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
            }
            [[nodiscard]] inline std::optional<bool> multi_user() const noexcept {
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

                [[nodiscard]] inline std::optional<bool> iommu() const noexcept {
                    const auto iommu_attr = node->first_attribute("iommu");
                    return iommu_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(iommu_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline std::optional<bool> ats() const noexcept {
                    const auto ats_attr = node->first_attribute("ats");
                    return ats_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ats_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline std::optional<VgaConf> vga_conf() const noexcept {
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
                    [[nodiscard]] inline std::optional<bool> accel3d() const noexcept { return bool_wrap_attr<YesNo, Optional>(node, "accel3d"); }
                    [[nodiscard]] inline std::optional<bool> accel2d() const noexcept { return bool_wrap_attr<YesNo, Optional>(node, "accel2d"); }
                };

                [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); }
                [[nodiscard]] inline Optional<Integral> qxl_ram() const noexcept { return Integral{node->first_attribute("ram")}; }
                [[nodiscard]] inline Optional<Integral> qxl_vgamem() const noexcept { return Integral{node->first_attribute("vgamem")}; }
                [[nodiscard]] inline Optional<Integral> qxl_vram64() const noexcept { return Integral{node->first_attribute("vram64")}; }
                [[nodiscard]] inline Optional<Integral> vram() const noexcept { return Integral{node->first_attribute("vram")}; }
                [[nodiscard]] inline Optional<Integral> heads() const noexcept { return Integral{node->first_attribute("heads")}; }
                [[nodiscard]] inline std::optional<bool> primary() const noexcept {
                    const auto attr = node->first_attribute("primary");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline Optional<Acceleration> acceleration() const noexcept { return Acceleration{node->first_node("acceleration")}; }
            };

            [[nodiscard]] inline Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
            [[nodiscard]] inline Optional<Model> model() const noexcept { return Model{node->first_node("model")}; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct QemuCharDev : public Node {
            struct Source : public Node {
                [[nodiscard]] inline Optional<String> mode() const noexcept { return String{node->first_attribute("mode")}; }
                [[nodiscard]] inline Optional<String> path() const noexcept { return String{node->first_attribute("path")}; }
                [[nodiscard]] inline Optional<String> host() const noexcept { return String{node->first_attribute("host")}; }
                [[nodiscard]] inline Optional<String> service() const noexcept { return String{node->first_attribute("service")}; }
                [[nodiscard]] inline Optional<String> channel() const noexcept { return String{node->first_attribute("channel")}; }
                [[nodiscard]] inline Optional<String> master() const noexcept { return String{node->first_attribute("master")}; }
                [[nodiscard]] inline Optional<String> slave() const noexcept { return String{node->first_attribute("slave")}; }
                [[nodiscard]] inline std::optional<bool> append() const noexcept {
                    const auto attr = node->first_attribute("append");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline std::optional<bool> tls() const noexcept {
                    const auto attr = node->first_attribute("tls");
                    return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline Optional<Reconnect> reconnect() const noexcept { return Reconnect{node->first_node("reconnect")}; }
                [[nodiscard]] inline NamedSpan<SecLabel> seclabels() const noexcept { return NamedSpan<SecLabel>{"seclabel", node}; }
            };
            struct Protocol : public Node {
                enum class Type {
                    raw,
                    telnet,
                    telnets,
                    tls,
                };
                [[nodiscard]] inline Type type() const noexcept { return enum_wrap_attr<Type>(node, "type"); }
            };
            struct Log : public Node {
                [[nodiscard]] inline String file() const noexcept { return String{node->first_attribute("file")}; }
                [[nodiscard]] inline std::optional<bool> append() const noexcept { return bool_wrap_attr<OnOff, Optional>(node, "append"); }
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
                    [[nodiscard]] inline Name name() const noexcept {
                        std::string in{node->first_attribute("name")->value()};
                        std::replace(in.begin(), in.end(), '-', '_');
                        return *magic_enum::enum_cast<Name>(in);
                    }
                };

                [[nodiscard]] inline std::optional<Type> type() const noexcept {
                    const auto attr = node->first_attribute("type");
                    if (!attr)
                        return std::nullopt;
                    std::string in{attr->value()};
                    std::replace(in.begin(), in.end(), '-', '_');
                    return magic_enum::enum_cast<Type>(in);
                }
                [[nodiscard]] inline Optional<String> port() const noexcept { return String{node->first_attribute("port")}; }
                [[nodiscard]] inline Optional<Model> model() const noexcept { return Model{node->first_node("model")}; }
            };
            [[nodiscard]] inline QemuCharDevType type() const noexcept {
                return *magic_enum::enum_cast<QemuCharDevType>(node->first_attribute("type")->value());
            }
            [[nodiscard]] inline Optional<String> tty() const noexcept { return String{node->first_attribute("tty")}; }
            [[nodiscard]] inline NamedSpan<Source> sources() const noexcept { return NamedSpan<Source>{"source", node}; }
            [[nodiscard]] inline Optional<Protocol> protocol() const noexcept { return Protocol{node->first_node("protocol")}; }
            [[nodiscard]] inline Optional<Log> log() const noexcept { return Log{node->first_node("log")}; }
            [[nodiscard]] inline Optional<Target> target() const noexcept { return Target{node->first_node("target")}; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
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

                [[nodiscard]] inline Type type() const noexcept { return enum_wrap_attr<Type>(node, "type"); }
                [[nodiscard]] inline Optional<String> address() const noexcept { return String{node->first_attribute("address")}; }
                [[nodiscard]] inline Optional<String> port() const noexcept { return String{node->first_attribute("port")}; }
                [[nodiscard]] inline Optional<String> name() const noexcept { return String{node->first_attribute("name")}; }
                [[nodiscard]] inline std::optional<State> state() const noexcept { return enum_wrap_attr<State, Optional>(node, "state"); }
            };

            [[nodiscard]] inline QemuCharDevType type() const noexcept {
                return *magic_enum::enum_cast<QemuCharDevType>(node->first_attribute("type")->value());
            }
            [[nodiscard]] inline NamedSpan<QemuCharDev::Source> sources() const noexcept { return NamedSpan<QemuCharDev::Source>{"source", node}; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct Smartcard : public QemuCharDev {
            enum class Mode {
                host,
                host_certificates,
                passthrough,
            };

            [[nodiscard]] inline Mode mode() const noexcept {
                std::string in{node->first_attribute("mode")->value()};
                std::replace(in.begin(), in.end(), '-', '_');
                return *magic_enum::enum_cast<Mode>(in);
            }
            [[nodiscard]] inline NamedSpan<StringNode> certificates() const noexcept { return NamedSpan<StringNode>{"certificate", node}; }
            [[nodiscard]] inline Optional<StringNode> database() const noexcept { return StringNode{node->first_node("database")}; }
        };
        struct Hub : public Node {
            enum class Type {
                usb,
            };

            [[nodiscard]] inline Type type() const noexcept { return enum_wrap_attr<Type>(node, "type"); }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct RedirDev : public QemuCharDev {
            enum class Bus {
                usb,
            };

            [[nodiscard]] inline Bus bus() const noexcept { return enum_wrap_attr<Bus>(node, "bus"); }
            [[nodiscard]] inline Optional<Boot> boot() const noexcept { return Boot{node->first_node("boot")}; }
        };
        struct RedirFilter : public Node {
            struct UsbDev : public Node {
                [[nodiscard]] inline bool allow() const noexcept { return bool_wrap_attr<YesNo>(node, "allow"); }
                [[nodiscard]] inline Optional<Integral> class_() const noexcept { return Integral{node->first_attribute("class")}; }
                [[nodiscard]] inline Optional<Integral> vendor() const noexcept { return Integral{node->first_attribute("vendor")}; }
                [[nodiscard]] inline Optional<Integral> version() const noexcept { return Integral{node->first_attribute("version")}; }
            };

            [[nodiscard]] inline NamedSpan<UsbDev> usb_devs() const noexcept { return NamedSpan<UsbDev>{"usbdev", node}; }
        };
        struct Rng : public Node {
            enum class Model {
                virtio,
            };

            struct Backend : public QemuCharDev {
                enum class Model {
                    random,
                    egd,
                };
                [[nodiscard]] inline Model model() const noexcept { return enum_wrap_attr<Model>(node, "model"); }
                operator String() const noexcept { return String{node}; }
                auto target() const noexcept = delete;
                auto alias() const noexcept = delete;
                auto address() const noexcept = delete;
            };
            struct Driver : public Node {
                [[nodiscard]] inline std::optional<bool> iommu() const noexcept {
                    const auto iommu_attr = node->first_attribute("iommu");
                    return iommu_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(iommu_attr->value()))} : std::nullopt;
                }
                [[nodiscard]] inline std::optional<bool> ats() const noexcept {
                    const auto ats_attr = node->first_attribute("ats");
                    return ats_attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<OnOff>(ats_attr->value()))} : std::nullopt;
                }
            };
            struct Rate : public Node {
                [[nodiscard]] inline Integral bytes() const noexcept { return Integral{node->first_attribute("bytes")}; }
                [[nodiscard]] inline Optional<Integral> period() const noexcept { return Integral{node->first_attribute("period")}; }
            };

            [[nodiscard]] inline Model model() const noexcept { return enum_wrap_attr<Model>(node, "model"); }
            [[nodiscard]] inline Backend backend() const noexcept { return Backend{node->first_node("backend")}; }
            [[nodiscard]] inline Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
            [[nodiscard]] inline Optional<Rate> rate() const noexcept { return Rate{node->first_node("rate")}; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct Tpm : public Node {
            enum class Model { tpm_tis };

            struct Backend : public Node {
                enum class Type {
                    passthrough,
                };

                [[nodiscard]] inline Type type() const noexcept { return enum_wrap_attr<Type>(node, "model"); }
                [[nodiscard]] inline Optional<String> path() const noexcept { return String{node->first_attribute("path")}; }
            };

            [[nodiscard]] inline std::optional<Model> model() const noexcept { return enum_wrap_attr<Model, Optional>(node, "model", true); }
            [[nodiscard]] inline Backend backend() const noexcept { return Backend{node->first_node("backend")}; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
        };
        struct ShMem : public Node {
            struct Model : public Node {
                enum class Type {
                    ivshmem,
                    ivshmem_plain,
                    ivshmem_doorbell,
                };

                [[nodiscard]] inline Type type() const noexcept { return enum_wrap_attr<Type>(node, "type", true); }
            };
            struct Server : public Node {
                [[nodiscard]] inline Optional<String> path() const noexcept { return String{node->first_attribute("path")}; }
            };
            struct Msi : public Node {
                [[nodiscard]] inline std::optional<bool> ioeventfd() const noexcept { return bool_wrap_attr<OnOff, Optional>(node, "ioeventfd"); }
                [[nodiscard]] inline Optional<Integral> vectors() const noexcept { return Integral{node->first_attribute("vectors")}; }
            };

            [[nodiscard]] inline String name() const noexcept { return String{node->first_attribute("name")}; }
            [[nodiscard]] inline Optional<Model> model() const noexcept { return Model{node->first_node("model")}; }
            [[nodiscard]] inline Optional<Integral> size() const noexcept { return Integral{node->first_node("size")}; }
            [[nodiscard]] inline Optional<Server> server() const noexcept { return Server{node->first_node("server")}; }
            [[nodiscard]] inline Optional<Msi> msi() const noexcept { return Msi{node->first_node("msi")}; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct MemoryDev : public Node {
            enum class Model {
                dimm,
                nvdimm,
            };
            enum class Access {
                shared,
                private_,
            };

            struct Source : public Node {
                [[nodiscard]] inline Optional<Integral> page_size() const noexcept { return Integral{node->first_attribute("pagesize")}; }
                [[nodiscard]] inline Optional<String> cpuset() const noexcept { return String{node->first_attribute("cpuset")}; }
                [[nodiscard]] inline Optional<String> path() const noexcept { return String{node->first_attribute("path")}; }
            };
            struct Target : public Node {
                struct Label : public Node {
                    [[nodiscard]] inline Integral size() const noexcept { return Integral{node->first_node("size")}; }
                };

                [[nodiscard]] inline Integral size() const noexcept { return Integral{Node::node->first_node("size")}; }
                [[nodiscard]] inline Optional<Integral> node() const noexcept { return Integral{Node::node->first_node("node")}; }
                [[nodiscard]] inline Optional<Label> label() const noexcept { return Label{Node::node->first_node("label")}; }
            };

            [[nodiscard]] inline Model model() const noexcept { return enum_wrap_attr<Model>(node, "model"); }
            [[nodiscard]] inline std::optional<Access> access() const noexcept { return enum_wrap_attr<Access, Optional>(node, "access"); }
            [[nodiscard]] inline Optional<Source> source() const noexcept { return Source{node->first_node("source")}; }
            [[nodiscard]] inline Target target() const noexcept { return Target{node->first_node("target")}; }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct Watchdog : public Node {
            enum class Model {
                i6300esb,
                ib700,
                diag288,
            };
            enum class Action {
                reset,
                shutdown,
                poweroff,
                pause,
                none,
                dump,
                inject_nmi,
            };

            [[nodiscard]] inline Model model() const noexcept { return enum_wrap_attr<Model>(node, "model"); }
            [[nodiscard]] inline std::optional<Action> action() const noexcept { return enum_wrap_attr<Action, Optional>(node, "action", true); }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct MemBalloon : public Node {
            enum class Model {
                virtio,
                xen,
                none,
            };

            struct Stats : public Node {
                [[nodiscard]] inline Integral period() const noexcept { return Integral{node->first_attribute("period")}; }
            };
            struct Driver : public Node {
                [[nodiscard]] inline std::optional<bool> iommu() const noexcept { return bool_wrap_attr<OnOff, Optional>(node, "iommu"); }
                [[nodiscard]] inline std::optional<bool> ats() const noexcept { return bool_wrap_attr<OnOff, Optional>(node, "ats"); }
            };

            [[nodiscard]] inline Model model() const noexcept { return enum_wrap_attr<Model>(node, "model"); }
            [[nodiscard]] inline std::optional<bool> autodeflate() const noexcept { return bool_wrap_attr<OnOff, Optional>(node, "autodeflate"); }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
            [[nodiscard]] inline Optional<Stats> stats() const noexcept { return Stats{node->first_node("stats")}; }
            [[nodiscard]] inline Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
        };
        struct NvRam : public Node {
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct Panic : public Node {
            enum class Model {
                isa,
                pseries,
                hyperv,
                s390,
            };

            [[nodiscard]] inline Model model() const noexcept { return enum_wrap_attr<Model>(node, "model"); }
            [[nodiscard]] inline Optional<Alias> alias() const noexcept { return Alias{node->first_node("alias")}; }
            [[nodiscard]] inline Optional<Address> address() const noexcept { return Address{node->first_node("address")}; }
        };
        struct Iommu : public Node {
            enum class Model {
                intel,
            };

            struct Driver : public Node {
                [[nodiscard]] inline std::optional<bool> intremap() const noexcept { return bool_wrap_attr<OnOff, Optional>(node, "intremap"); }
                [[nodiscard]] inline std::optional<bool> caching_mode() const noexcept {
                    return bool_wrap_attr<OnOff, Optional>(node, "caching_mode");
                }
                [[nodiscard]] inline std::optional<bool> eim() const noexcept { return bool_wrap_attr<OnOff, Optional>(node, "eim"); }
                [[nodiscard]] inline std::optional<bool> iotlb() const noexcept { return bool_wrap_attr<OnOff, Optional>(node, "iotlb"); }
            };

            [[nodiscard]] inline Model model() const noexcept { return enum_wrap_attr<Model>(node, "model"); }
            [[nodiscard]] inline Optional<Driver> driver() const noexcept { return Driver{node->first_node("driver")}; }
        };

        [[nodiscard]] inline Optional<Emulator> emulator() const noexcept { return Emulator{node->first_node("emulator")}; }
        [[nodiscard]] inline NamedSpan<Disk> disks() const noexcept { return NamedSpan<Disk>{"disk", node}; }
        [[nodiscard]] inline NamedSpan<Controller> controllers() const noexcept { return NamedSpan<Controller>{"controller", node}; }
        [[nodiscard]] inline NamedSpan<Lease> leases() const noexcept { return NamedSpan<Lease>{"lease", node}; }
        [[nodiscard]] inline NamedSpan<Filesystem> filesystems() const noexcept { return NamedSpan<Filesystem>{"filesystem", node}; }
        [[nodiscard]] inline NamedSpan<Interface> interfaces() const noexcept { return NamedSpan<Interface>{"interface", node}; }
        [[nodiscard]] inline NamedSpan<Input> inputs() const noexcept { return NamedSpan<Input>{"input", node}; }
        [[nodiscard]] inline NamedSpan<Sound> sounds() const noexcept { return NamedSpan<Sound>{"sound", node}; }
        [[nodiscard]] inline NamedSpan<HostDev> hostdevs() const noexcept { return NamedSpan<HostDev>{"hostdev", node}; }
        [[nodiscard]] inline NamedSpan<Graphics> graphics() const noexcept { return NamedSpan<Graphics>{"graphics", node}; }
        [[nodiscard]] inline NamedSpan<Video> videos() const noexcept { return NamedSpan<Video>{"video", node}; }
        [[nodiscard]] inline NamedSpan<Console> consoles() const noexcept { return NamedSpan<Console>{"console", node}; }
        [[nodiscard]] inline NamedSpan<Parallel> parallels() const noexcept { return NamedSpan<Parallel>{"parallel", node}; }
        [[nodiscard]] inline NamedSpan<Serial> serials() const noexcept { return NamedSpan<Serial>{"serial", node}; }
        [[nodiscard]] inline NamedSpan<Channel> channels() const noexcept { return NamedSpan<Channel>{"channel", node}; }
        [[nodiscard]] inline NamedSpan<Smartcard> smartcards() const noexcept { return NamedSpan<Smartcard>{"smartcard", node}; }
        [[nodiscard]] inline NamedSpan<Hub> hubs() const noexcept { return NamedSpan<Hub>{"hub", node}; }
        [[nodiscard]] inline NamedSpan<RedirDev> redir_devs() const noexcept { return NamedSpan<RedirDev>{"redirdev", node}; }
        [[nodiscard]] inline NamedSpan<RedirFilter> redir_filters() const noexcept { return NamedSpan<RedirFilter>{"redirfilter", node}; }
        [[nodiscard]] inline NamedSpan<Rng> rngs() const noexcept { return NamedSpan<Rng>{"rng", node}; }
        [[nodiscard]] inline NamedSpan<Tpm> tpms() const noexcept { return NamedSpan<Tpm>{"tpm", node}; }
        [[nodiscard]] inline NamedSpan<ShMem> sh_mems() const noexcept { return NamedSpan<ShMem>{"shmem", node}; }
        [[nodiscard]] inline NamedSpan<MemoryDev> memory_devs() const noexcept { return NamedSpan<MemoryDev>{"memorydev", node}; }
        [[nodiscard]] inline Optional<Watchdog> watchdog() const noexcept { return Watchdog{node->first_node("watchdog")}; }
        [[nodiscard]] inline Optional<MemBalloon> memballoon() const noexcept { return MemBalloon{node->first_node("memballoon")}; }
        [[nodiscard]] inline Optional<NvRam> nvram() const noexcept { return NvRam{node->first_node("nvram")}; }
        [[nodiscard]] inline NamedSpan<Panic> panics() const noexcept { return NamedSpan<Panic>{"panic", node}; }
        [[nodiscard]] inline Optional<Iommu> iommu() const noexcept { return Iommu{node->first_node("iommu")}; }
    };
    struct SecLabel : public Node {
        enum class Type {
            dynamic,
            static_,
            none,
        };

        [[nodiscard]] inline Optional<String> model() const noexcept { return String{node->first_attribute("model")}; }
        [[nodiscard]] inline std::optional<Type> type() const noexcept { return enum_wrap_attr<Type, Optional>(node, "type"); }
        [[nodiscard]] inline std::optional<bool> relabel() const noexcept { return bool_wrap_attr<YesNo, Optional>(node, "relabel"); }
        [[nodiscard]] inline Optional<String> label() const noexcept { return String{node->first_node("label")}; }
        [[nodiscard]] inline Optional<String> image_label() const noexcept { return String{node->first_node("imagelabel")}; }
        [[nodiscard]] inline Optional<String> base_label() const noexcept { return String{node->first_node("baselabel")}; }
    };
    struct Keywrap : public Node {
        struct Cipher : public Node {
            enum class Name {
                aes,
                dsa,
            };

            [[nodiscard]] inline Name name() const noexcept { return enum_wrap_attr<Name>(node, "name"); }
            [[nodiscard]] inline bool state() const noexcept { return bool_wrap_attr<OnOff>(node, "state"); }
        };

        [[nodiscard]] inline NamedSpan<Cipher> ciphers() const noexcept { return NamedSpan<Cipher>{"cipher", node}; }
    };

    [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(std::string_view{node->first_attribute("type")->value()}); }
    [[nodiscard]] inline Optional<Integral> id() const noexcept { return Integral{node->first_attribute("id")}; }
    [[nodiscard]] inline String name() const noexcept { return String{node->first_node("name")}; }
    [[nodiscard]] inline Optional<Uuid> uuid() const noexcept { return Uuid{node->first_node("uuid")}; }
    [[nodiscard]] inline Optional<String> title() const noexcept { return String{node->first_node("title")}; }
    [[nodiscard]] inline Optional<String> description() const noexcept { return String{node->first_node("description")}; }
    // metadata() // hidden because hard to interface and there is already an access in the Object API
    [[nodiscard]] inline Optional<Cpu> cpu() const noexcept { return Cpu{node->first_node("cpu")}; }
    [[nodiscard]] inline Optional<Sysinfo> sysinfo() const noexcept { return Sysinfo{node->first_node("sysinfo")}; }
    [[nodiscard]] inline Optional<Bootloader> bootloader() const noexcept { return Bootloader{node->first_node("bootloader")}; }
    [[nodiscard]] inline Optional<BootloaderArgs> bootloader_args() const noexcept { return BootloaderArgs{node->first_node("bootloader_args")}; }
    [[nodiscard]] inline Optional<Os> os() const noexcept { return Os{node->first_node("os")}; }
    [[nodiscard]] inline std::optional<OffOption> on_reboot() const noexcept { return enum_wrap_attr<OffOption, Optional>(node, "on_reboot", true); }
    [[nodiscard]] inline std::optional<OffOption> on_poweroff() const noexcept {
        return enum_wrap_attr<OffOption, Optional>(node, "on_poweroff", true);
    }
    [[nodiscard]] inline std::optional<CrashOption> on_crash() const noexcept {
        return enum_wrap_attr<CrashOption, Optional>(node, "on_crash", true);
    }
    [[nodiscard]] inline std::optional<LockFailureOption> on_lockfailure() const noexcept {
        return enum_wrap_attr<LockFailureOption, Optional>(node, "on_lockfailure");
    }
    [[nodiscard]] inline Optional<Clock> clock() const noexcept { return Clock{node->first_node("clock")}; }
    [[nodiscard]] inline Optional<Pm> pm() const noexcept { return Pm{node->first_node("pm")}; }
    [[nodiscard]] inline Optional<Perf> perf() const noexcept { return Perf{node->first_node("perf")}; }
    [[nodiscard]] inline Optional<IdMap> idmap() const noexcept { return IdMap{node->first_node("idmap")}; }
    // resources() // left out because low priority since there is already an access in the Object API
    // features() // left out because low priority since there is already an access in the Object API
    [[nodiscard]] inline Optional<Devices> devices() const noexcept { return Devices{node->first_node("devices")}; }
    [[nodiscard]] inline NamedSpan<SecLabel> seclabels() const noexcept { return NamedSpan<SecLabel>{"seclabel", node}; }
    // qemucmdline() // left out because hard to interface, and driver specific anyway
    // lxcsharens() // left out because hard to interface, and driver specific anyway
    [[nodiscard]] inline Optional<Keywrap> keywrap() const noexcept { return Keywrap{node->first_node("keywrap")}; }
    ///
}; // namespace

} // namespace
} // namespace virtxml