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
    template <std::enable_if_t<!std::is_void_v<Optional<void>>, int> = 0>[[nodiscard]] std::optional<MacTableManager> mode() const noexcept {
        const auto attr = this->get_node()->next_attribute("macTableManager");
        if (!attr)
            return std::nullopt;
        return magic_enum::enum_cast<MacTableManager>(attr->value());
    }

    template <std::enable_if_t<std::is_void_v<Optional<void>>, int> = 0>[[nodiscard]] MacTableManager mode() const noexcept {
        return *magic_enum::enum_cast<MacTableManager>(this->get_node()->next_attribute("macTableManager")->value());
    }
};

} // namespace
}