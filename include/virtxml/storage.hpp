#pragma once

#include <magic_enum.hpp>
#include "generic.hpp"
#include "xmlspan.hpp"
#include "xmlval.hpp"

namespace virtxml::storage {
enum class Format {
    cow,
    qcow,
    qcow2,
    qed,
    vmdk,
    raw,
    dir,
    bochs,
    cloop,
    dmg,
    iso,
    vpc,
    vdi,
    fat,
    vhd,
    ploop,
    luks,
};

template <class CRTP, template <class> class Optional = std::void_t> struct HasFormat : public HasMore<CRTP, HasFormat<CRTP, Optional>> {
    template <std::enable_if_t<!std::is_void_v<Optional<void>>, int> = 0>[[nodiscard]] std::optional<Format> format() const noexcept {
        const auto attr = this->get_node()->next_attribute("format");
        if (!attr)
            return std::nullopt;
        return magic_enum::enum_cast<Format>(attr->value());
    }

    template <std::enable_if_t<std::is_void_v<Optional<void>>, int> = 0>[[nodiscard]] Format format() const noexcept {
        return magic_enum::enum_cast<Format>(this->get_node()->next_attribute("format")->value());
    }
};

struct Secret : public Node {
    [[nodiscard]] constexpr std::string_view type() const noexcept { return "passphrase"; }
    [[nodiscard]] Optional<Uuid> uuid() const noexcept { return Uuid{node->first_attribute("uuid")}; }
    [[nodiscard]] Optional<String> usage() const noexcept { return String{node->first_attribute("usage")}; }
};

struct Encryption : public Node {
    enum class Format {
        default_,
        qcow,
        luks,
    };

    struct Cipher : public Node {
        [[nodiscard]] String name() const noexcept { return String{node->first_attribute("name")}; }
        [[nodiscard]] Integral size() const noexcept { return Integral{node->first_attribute("size")}; }
        [[nodiscard]] Optional<String> mode() const noexcept { return String{node->first_attribute("mode")}; }
        [[nodiscard]] Optional<String> hash() const noexcept { return String{node->first_attribute("hash")}; }
    };

    struct IVGen : public Node {
        [[nodiscard]] String name() const noexcept { return String{node->first_attribute("name")}; }
        [[nodiscard]] Optional<String> hash() const noexcept { return String{node->first_attribute("hash")}; }
    };

    [[nodiscard]] Format format() const noexcept { return *magic_enum::enum_cast<Format>(node->first_attribute("format")->value()); }
    [[nodiscard]] NamedSpan<Secret> secrets() const noexcept { return NamedSpan<Secret>{"secret", node}; }
    [[nodiscard]] Optional<Cipher> cipher() const noexcept { return Cipher{node->first_node("cipher")}; }
    [[nodiscard]] Optional<IVGen> ivgen() const noexcept { return IVGen{node->first_node("ivgen")}; }
};
} // namespace virtxml::storage