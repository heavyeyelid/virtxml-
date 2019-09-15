#pragma once
#include <type_traits>
#include <magic_enum.hpp>
#include "generic.hpp"

namespace virtxml {
inline namespace {

enum class MacTableManager {
    kernel,
    libvirt,
};

template <class CRTP, template <class> class Optional = std::void_t>
struct HasMacTableManager : public HasMore<CRTP, HasMacTableManager<CRTP, Optional>> {
    [[nodiscard]] std::optional<MacTableManager> mode() const noexcept {
        if constexpr (std::is_void_v<Optional<void>>)
            *magic_enum::enum_cast<MacTableManager>(this->get_node()->first_attribute("macTableManager")->value());
        const auto attr = this->get_node()->first_attribute("macTableManager");
        return attr ? magic_enum::enum_cast<MacTableManager>(attr->value()) : std::nullopt;
    }
};

} // namespace
}