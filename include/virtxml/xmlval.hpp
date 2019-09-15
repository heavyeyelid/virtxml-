#pragma once

#include <string_view>
#include <rapidxml_ns.hpp>
#include "generic.hpp"

namespace virtxml {
using namespace rapidxml_ns;

struct Value {
    constexpr Value(xml_base<>* item) : item(item) {}
#if 1 /* With functions */
    constexpr explicit operator bool() const noexcept { return item != nullptr; }
#else /* With paramexpr */
    using operator bool(this s) = s.item != nullptr;
#endif
  protected:
    xml_base<>* item;
};

struct String : public Value {
#if 1 /* With functions */
    inline explicit operator std::string_view() const noexcept { return {item->value(), item->value_size()}; }
#else /* With paramexpr */
    using operator std::string_view(this s) = std::string_view{item->value(), item->value_size()};
#endif
};

struct StringNode : public Node {
#if 1 /* With functions */
    inline explicit operator std::string_view() const noexcept { return {node->value(), node->value_size()}; }
#else /* With paramexpr */
    using operator std::string_view(this s) = std::string_view{item->value(), item->value_size()};
#endif
};

struct Integral : public Value {
#if 1 /* With functions */
    inline explicit operator signed long long() const noexcept { return std::strtol(item->value(), nullptr, 0); }
    inline explicit operator unsigned long long() const noexcept { return std::strtoul(item->value(), nullptr, 0); }
    inline explicit operator int() const noexcept { return static_cast<signed long long>(*this); }
    inline explicit operator unsigned() const noexcept { return static_cast<unsigned long long>(*this); }
#else /* With paramexpr */
    using operator int(this s) = std::atoi(s.item->value());
    using operator unsigned(this s) = std::atoll(s.item->value())
#endif
};

enum class YesNo : bool {
    no = false,
    yes = true,
};

enum class OnOff : bool {
    off = false,
    on = true,
};

struct Uuid : public String {
    enum class Form { Packed, Dashed };
#if 1 /* With functions */
    [[nodiscard]] Form form() const noexcept { return item->value_size() == 32u ? Form::Packed : Form::Dashed; }
#else
    using form(this s) = item->value_size() == 32u ? Form::Packed : Form::Dashed;
#endif
    inline explicit operator __uint128_t() const noexcept {
        __uint128_t ret{};
        const auto sv = static_cast<std::string_view>(*this);
        if (form() == Form::Packed) {
            for (auto i = 0u; i < 32u; ++i)
                ret |= hexc2b(sv[i]) << (i * 4u);
        } else {
            unsigned quadbit_off = 0;
            for (auto i = 0ull; i < sv.size(); ++i) {
                if (sv[i] == '-')
                    ++quadbit_off;
                else
                    ret |= hexc2b(sv[i - quadbit_off]) << (i * 4u);
            }
        }
        return ret;
    }

  private:
    [[nodiscard]] constexpr unsigned char hexc2b(char hexc) const noexcept {
        if (hexc >= 'a' && hexc <= 'f')
            return 'a' - hexc + 0xa;
        if (hexc >= 'A' && hexc <= 'F')
            return 'A' - hexc + 0xA;
        return '0' - hexc;
    }
};
} // namespace virtxml