#pragma once

#include <optional>
#include <type_traits>
#include <magic_enum.hpp>
#include "generic.hpp"
#include "xmlspan.hpp"
#include "xmlval.hpp"

namespace virtxml::cpu {

enum class Arch {
    aarch64,
    alpha,
    armv7l,
    cris,
    i686,
    ia64,
    lm32,
    m68k,
    microblaze,
    microblazeel,
    mips,
    mipsel,
    mips64,
    mips64el,
    openrisc,
    parisc,
    parisc64,
    ppc,
    ppc64,
    ppc64le,
    ppcemb,
    s390,
    s390x,
    sh4,
    sh4eb,
    sparc,
    sparc64,
    unicore32,
    x86_64,
    xtensa,
    xtensaeb,
};

enum class Mode {
    custom,
    host_model,
    host_passthrough,
};

template <class CRTP, template <class> class Optional = std::void_t> struct HasMode : public HasMore<CRTP, HasMode<CRTP, Optional>> {
    [[nodiscard]] inline auto mode() const noexcept {
        if constexpr (std::is_void_v<Optional<void>>)
            return *magic_enum::enum_cast<Mode>(this->get_node()->first_attribute("mode")->value());
        const auto mode_attr = this->get_node()->first_attribute("mode");
        return mode_attr ? magic_enum::enum_cast<Mode>(mode_attr->value()) : std::optional<Mode>{std::nullopt};
    }
};

enum class Match {
    minimum,
    exact,
    strict,
};

template <class CRTP, template <class> class Optional = std::void_t> struct HasMatch : public HasMore<CRTP, HasMatch<CRTP, Optional>> {
    [[nodiscard]] inline auto match() const noexcept {
        if constexpr (std::is_void_v<Optional<void>>)
            return *magic_enum::enum_cast<Match>(this->get_node()->first_attribute("mode")->value());
        const auto match_attr = this->get_node()->first_attribute("mode");
        return match_attr ? magic_enum::enum_cast<Match>(match_attr->value()) : std::optional<Match>{std::nullopt};
    }
};

enum class Check {
    none,
    partial,
    full,
};

template <class CRTP, template <class> class Optional = std::void_t> struct HasCheck : public HasMore<CRTP, HasCheck<CRTP, Optional>> {
    [[nodiscard]] inline auto check() const noexcept {
        if constexpr (std::is_void_v<Optional<void>>)
            return *magic_enum::enum_cast<Check>(this->get_node()->first_attribute("mode")->value());
        const auto check_attr = this->get_node()->first_attribute("mode");
        return check_attr ? magic_enum::enum_cast<Check>(check_attr->value()) : std::optional<Check>{std::nullopt};
    }
};

struct Model : public Node {
    enum class Fallback {
        allow,
        forbid,
    };

    [[nodiscard]] inline std::optional<Fallback> fallback() const noexcept {
        const auto fb_node = node->first_attribute("fallback");
        return fb_node ? magic_enum::enum_cast<Fallback>(fb_node->value()) : std::nullopt;
    }
    [[nodiscard]] inline Optional<String> vendor_id() const noexcept { return String{node->first_attribute("vendor_id")}; }
    [[nodiscard]] inline explicit operator std::string_view() const noexcept { return {node->value(), node->value_size()}; }
};

struct Topology : public Node {
    [[nodiscard]] inline Integral sockets() const noexcept { return Integral{node->first_attribute("sockets")}; }
    [[nodiscard]] inline Integral cores() const noexcept { return Integral{node->first_attribute("cores")}; }
    [[nodiscard]] inline Integral threads() const noexcept { return Integral{node->first_attribute("threads")}; }
};

struct Feature : public Node {
    enum class Policy {
        force,
        require,
        optional,
        disable,
        forbid,
    };

    [[nodiscard]] inline Policy policy() const noexcept { return *magic_enum::enum_cast<Policy>(node->first_attribute("policy")->value()); }
    [[nodiscard]] inline String name() const noexcept { return String{node->first_attribute("name")}; }
};

struct Cache : public Node {
    enum class Mode {
        emulate,
        passthrough,
        disable,
    };

    [[nodiscard]] inline Optional<String> level() const noexcept { return String{node->first_attribute("level")}; }
    [[nodiscard]] inline Mode mode() const noexcept { return *magic_enum::enum_cast<Mode>(node->first_attribute("mode")->value()); }
};

struct NumaDistance : public Node {
    [[nodiscard]] inline Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
    [[nodiscard]] inline Integral value() const noexcept { return Integral{node->first_attribute("value")}; }
};

struct NumaCell : public Node {
    enum class MemAccess {
        shared,
        private_,
    };

    [[nodiscard]] inline Optional<Integral> id() const noexcept { return Integral{node->first_attribute("id")}; }
    [[nodiscard]] inline String cpuset() const noexcept { return String{node->first_attribute("cpuset")}; }
    [[nodiscard]] inline Integral memory() const noexcept { return Integral{node->first_attribute("memory")}; }
    [[nodiscard]] inline Optional<String> unit() const noexcept { return String{node->first_attribute("unit")}; }
    [[nodiscard]] inline std::optional<MemAccess> mem_access() const noexcept {
        const auto ma_node = node->first_attribute("memAccess");
        return ma_node ? magic_enum::enum_cast<MemAccess>(ma_node->value()) : std::nullopt;
    }
    [[nodiscard]] inline Optional<NamedSpan<NumaDistance>> distances() const noexcept {
        return NamedSpan<NumaDistance>{"sibling", node->first_node("distances")};
    }
};
} // namespace virtxml::cpu