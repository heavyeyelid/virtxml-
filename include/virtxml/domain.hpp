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
        struct BootMenu : public Node {
            [[nodiscard]] bool enable() const noexcept { return static_cast<bool>(*magic_enum::enum_cast<YesNo>(node->first_attribute("enable"))); }
            [[nodiscard]] Optional<Integral> timeout() const noexcept { return Integral{node->first_attribute("timeout")}; }
        };
        /* // hvm only
         * <interleave>
        <optional>
          <element name="bootmenu">
            <optional>
              <attribute name="timeout">
                <ref name="unsignedShort"/>
              </attribute>
            </optional>
          </element>
        </optional>
        <optional>
          <ref name="smbios"/>
        </optional>
        <optional>
          <ref name="bios"/>
        </optional>
        <optional>
          <ref name="acpiTable"/>
        </optional>
      </interleave>
         * */
        /* // exe only
      <interleave>
        <optional>
          <element name="init">
            <ref name="absFilePath"/>
          </element>
        </optional>
        <zeroOrMore>
          <element name="initarg">
            <text/>
          </element>
        </zeroOrMore>
        <zeroOrMore>
          <element name="initenv">
            <attribute name="name">
              <data type='string'>
                <param name='pattern'>[a-zA-Z_]+[a-zA-Z0-9_]*</param>
              </data>
            </attribute>
            <text/>
          </element>
        </zeroOrMore>
        <optional>
          <element name="initdir">
            <ref name="absFilePath"/>
          </element>
        </optional>
        <optional>
          <element name="inituser">
            <choice>
              <ref name="unsignedInt"/>
              <ref name="genericName"/>
            </choice>
          </element>
          <element name="initgroup">
            <choice>
              <ref name="unsignedInt"/>
              <ref name="genericName"/>
            </choice>
          </element>
        </optional>
      </interleave>
         * */
        [[nodiscard]] Type type() const noexcept { return Type{node->first_node("type")}; }
        [[nodiscard]] Optional<HvmLoader> hvm_loader() const noexcept { return HvmLoader{node->first_node("loader")}; }
        [[nodiscard]] Optional<HvmNvram> hvm_nvram() const noexcept { return HvmNvram{node->first_node("nvram")}; }
        [[nodiscard]] Optional<String> kernel() const noexcept { return String{node->first_node("kernel")}; }
        [[nodiscard]] Optional<String> initrd() const noexcept { return String{node->first_node("initrd")}; }
        [[nodiscard]] Optional<String> root() const noexcept { return String{node->first_node("root")}; }
        [[nodiscard]] Optional<String> cmdline() const noexcept { return String{node->first_node("cmdline")}; }
        [[nodiscard]] Optional<String> dtb() const noexcept { return String{node->first_node("dtb")}; }
        [[nodiscard]] NamedSpan<Boot> boots() const noexcept { return NamedSpan<Boot>{"boot", node}; }
        [[nodiscard]] Optional<BootMenu> boot_menu() const noexcept { return BootMenu{node->first_node("bootmenu")}; }
    };
    struct Bootloader : public StringNode {};
    struct BootloaderArgs : public StringNode {};
    /*
     *<interleave>
        <ref name="os"/>
        <ref name="clock"/>
        <ref name="resources"/>
        <ref name="features"/>
        <ref name="events"/>
        <optional>
          <ref name="pm"/>
        </optional>
        <optional>
          <ref name="perf"/>
        </optional>
        <optional>
          <ref name="idmap"/>
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

    ///
};

} // namespace
}