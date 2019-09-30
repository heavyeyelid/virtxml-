#pragma once

#include <magic_enum.hpp>
#include "generic.hpp"
#include "xmlval.hpp"

namespace virtxml {
inline namespace {
struct Secret : public Node {
    struct Usage : public Node {
        enum class Type {
            volume,
            ceph,
            iscsi,
            tls,
        };

        [[nodiscard]] inline Type type() const noexcept { return *magic_enum::enum_cast<Type>(node->first_attribute("type")->value()); };
        [[nodiscard]] inline Optional<String> volume() const noexcept { return String{node->first_node("volume")}; }
        [[nodiscard]] inline Optional<String> name() const noexcept { return String{node->first_node("name")}; }
        [[nodiscard]] inline Optional<String> target() const noexcept { return String{node->first_node("target")}; }
    };

    [[nodiscard]] inline std::optional<bool> ephemeral() const noexcept {
        const auto attr = node->first_attribute("ephemeral");
        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
    }
    [[nodiscard]] inline std::optional<bool> private_() const noexcept {
        const auto attr = node->first_attribute("private");
        return attr ? std::optional{static_cast<bool>(*magic_enum::enum_cast<YesNo>(attr->value()))} : std::nullopt;
    }
    [[nodiscard]] inline Optional<Uuid> uuid() const noexcept { return Uuid{node->first_node("uuid")}; }
    [[nodiscard]] inline Optional<String> description() const noexcept { return String{node->first_node("description")}; }
};
} // namespace
}