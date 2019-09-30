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
    [[nodiscard]] inline std::optional<Format> format() const noexcept {
        if constexpr (std::is_void_v<Optional<void>>)
            return *magic_enum::enum_cast<Format>(this->get_node()->first_attribute("format")->value());
        const auto attr = this->get_node()->first_attribute("format");
        return attr ? magic_enum::enum_cast<Format>(attr->value()) : std::nullopt;
    }
};

struct Secret : public Node {
    [[nodiscard]] inline constexpr std::string_view type() const noexcept { return "passphrase"; }
    [[nodiscard]] inline Optional<Uuid> uuid() const noexcept { return Uuid{node->first_attribute("uuid")}; }
    [[nodiscard]] inline Optional<String> usage() const noexcept { return String{node->first_attribute("usage")}; }
};

struct Encryption : public Node {
    enum class Format {
        default_,
        qcow,
        luks,
    };

    struct Cipher : public Node {
        [[nodiscard]] inline String name() const noexcept { return String{node->first_attribute("name")}; }
        [[nodiscard]] inline Integral size() const noexcept { return Integral{node->first_attribute("size")}; }
        [[nodiscard]] inline Optional<String> mode() const noexcept { return String{node->first_attribute("mode")}; }
        [[nodiscard]] inline Optional<String> hash() const noexcept { return String{node->first_attribute("hash")}; }
    };

    struct IVGen : public Node {
        [[nodiscard]] inline String name() const noexcept { return String{node->first_attribute("name")}; }
        [[nodiscard]] inline Optional<String> hash() const noexcept { return String{node->first_attribute("hash")}; }
    };

    [[nodiscard]] inline Format format() const noexcept { return *magic_enum::enum_cast<Format>(node->first_attribute("format")->value()); }
    [[nodiscard]] inline NamedSpan<Secret> secrets() const noexcept { return NamedSpan<Secret>{"secret", node}; }
    [[nodiscard]] inline Optional<Cipher> cipher() const noexcept { return Cipher{node->first_node("cipher")}; }
    [[nodiscard]] inline Optional<IVGen> ivgen() const noexcept { return IVGen{node->first_node("ivgen")}; }
};
} // namespace virtxml::storage