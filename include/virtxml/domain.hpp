#pragma once

#include <string_view>
#include <gsl/gsl>
#include <magic_enum.hpp>
#include <rapidxml_ns.hpp>
#include "cpu_types.hpp"
#include "generic.hpp"
#include "xmlspan.hpp"
#include "xmlval.hpp"

namespace virtxml {
inline namespace {

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
};

} // namespace
}