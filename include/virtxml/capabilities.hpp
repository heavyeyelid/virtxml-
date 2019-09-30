#pragma once

#include <string_view>
#include <gsl/gsl>
#include <rapidxml_ns.hpp>
#include "generic.hpp"
#include "xmlspan.hpp"
#include "xmlval.hpp"

namespace virtxml {
using namespace rapidxml_ns;
inline namespace {

struct DriverCapabilities {
  public:
    struct Host : public Node {
        struct Cpu : public Node {
            struct Model : public Node {
#if 1 /* With functions */
                constexpr explicit operator String() const noexcept { return String{node}; }
                [[nodiscard]] inline Optional<String> fallback_str() const noexcept { return String{node->first_attribute("fallback")}; }
#else /* With paramexpr */
                using operator String(this s) = String{s.node};
                using fallback_str() = String{node->first_attribute("fallback")};
#endif
            };

            struct Feature;
            struct FeatureList : public NamedSpan<Feature> {
                constexpr explicit FeatureList(xml_node<>* node) : NamedSpan<Feature>("feature", node) {}
            };

            struct Feature : public Node {
#if 1 /* With functions */
                [[nodiscard]] inline String name() const noexcept { return String{node->first_attribute("name")}; }
#else /* With paramexpr */
                using name(this s) = String{s.node->first_attribute("name")};
#endif
            };

            struct Features; // NOT IMPLEMENTED YET // requires autogen from https://github.com/wiedi/libvirt/blob/master/src/cpu/cpu_map.xml

            struct MigrationFeatures : public Node {
                struct UriTransport : public String {};
                struct UriTransports : public NamedSpan<UriTransport> {
                    constexpr explicit UriTransports(xml_node<>* node) : NamedSpan<UriTransport>("uri_transport", node) {}
                };

#if 1 /* With functions */
                [[nodiscard]] inline bool live() const noexcept { return node->first_node("live") != nullptr; }
                [[nodiscard]] inline Optional<UriTransports> uri_transports() const noexcept {
                    return UriTransports{node->first_node("uri_transports")};
                }
#else /* With paramexpr */
                using live(this s) = s.node->first_node("live") != nullptr;
                using uri_transports(this s) = UriTransports{s.node->first_node("uri_transports")};
#endif
            };

            struct Topology : public Node {
                struct Cell : public Node {
                    struct Cpu : public Node {
#if 1 /* With functions */
                        [[nodiscard]] inline Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
#else /* With paramexpr */
                        using id(this s) = Integral{node->first_attribute("id")};
#endif
                    };
                    struct Cpus : public NamedSpan<Cpu> {
#if 1 /* With functions */
                        Cpus() : NamedSpan("cpu", node) {}
                        [[nodiscard]] inline Integral num() const noexcept { return Integral{node->first_attribute("num")}; }
#else /* With paramexpr */
                        using num(this s) = Integral{node->first_attribute("num")};
#endif
                    };
#if 1 /* With functions */
                    [[nodiscard]] inline Integral id() const noexcept { return Integral{node->first_attribute("id")}; }
#else /* With paramexpr */
                    using id(this s) = Integral{node->first_attribute("id")};
#endif
                };
                struct Cells : public NamedSpan<Cell> {
                    using NamedSpan::NamedSpan;
#if 1 /* With functions */
                    [[nodiscard]] inline Integral num() const noexcept { return Integral{node->first_attribute("num")}; }
#else /* With paramexpr */
                    using num(this s) = Integral{node->first_attribute("num")};
#endif
                };
#if 1 /* With functions */
                [[nodiscard]] inline Optional<Integral> sockets() const noexcept { return Integral{node->first_attribute("sockets")}; }
                [[nodiscard]] inline Optional<Integral> cores() const noexcept { return Integral{node->first_attribute("cores")}; }
                [[nodiscard]] inline Optional<Integral> threads() const noexcept { return Integral{node->first_attribute("threads")}; }
                [[nodiscard]] inline Cells cells() const noexcept { return Cells{"cell", node->first_node("cells")}; }
#else /* With paramexpr */
                using sockets(this s) = Integral{node->first_attribute("sockets")};
                using cores(this s) = Integral{node->first_attribute("cores")};
                using threads(this s) = Integral{node->first_attribute("threads")};
                using cells(this s) = Cells{"cell", s.node->first_node("cells")};
#endif
            };

            struct SecModel : public Node {
#if 1 /* With functions */
                [[nodiscard]] inline String model() const noexcept { return String{node->first_node("model")}; }
                [[nodiscard]] inline Integral doi() const noexcept { return Integral{node->first_node("doi")}; }
#else /* With paramexpr */
                using model(this s) = String{s.node->first_node("model")};
                using doi(this s) = Integral{s.node->first_node("doi")};
#endif
            };

#if 1 /* With functions */
            [[nodiscard]] inline String arch() const noexcept { return String{node->first_node("arch")}; }
            [[nodiscard]] inline Optional<String> vendor() const noexcept { return String{node->first_node("vendor")}; }
            // Optional<String> model() const noexcept;
            [[nodiscard]] inline FeatureList feature_list() const noexcept { return FeatureList{node}; }
            [[nodiscard]] inline Optional<MigrationFeatures> migration_features() const noexcept {
                return MigrationFeatures{node->first_node("migration_features")};
            }
            [[nodiscard]] inline Optional<SecModel> secmodel() const noexcept { return SecModel{node->first_node("secmodel")}; }
            [[nodiscard]] inline Topology topology() const noexcept { return Topology{node->first_node("topology")}; }
#else /* With paramexpr */
            using arch(this s) = String{s.node->first_node("arch")};
            using arch(this s) = String{s.node->first_node("vendor")};
            using model(this s) = Node{s.node->first_node("model")};
            using feature_list(this s) = FeatureList{s.node};
            using migration_features(this s) = MigrationFeatures{s.node->first_node("migration_features")};
            using secmodel(this s) = SecModel{s.node->first_node("secmodel")};
            using topology(this s) = Topology{s.node->first_node("topology")};
#endif
        };

        /*
        void uuid(std::string_view sv) {
            auto pool_val = node->document()->allocate_string(sv.data(), sv.size());
            if(auto uuid_node = node->first_node("uuid"); uuid_node != nullptr){
                uuid_node->name(pool_val, sv.size());
                return;
            }
            node->insert_node(nullptr, node->document()->allocate_node(node_element, "uuid", pool_val, 0, sv.size()));
        }
         */

#if 1 /* With functions */
        [[nodiscard]] inline String uuid() const noexcept { return String{node->first_node("uuid")}; }
        // String uuid() noexcept { return {node->first_node("uuid")}; }
#else /* With paramexpr */
        using uuid(this s) = String{s.node->first_node("uuid")};
        using cpu() = Cpu{s.node->first_node("cpu")};
#endif
    };
    struct Guest;
    struct GuestList : public NamedSpan<Guest> {
        constexpr explicit GuestList(xml_node<>* node) : NamedSpan<Guest>("guest", node) {}
    };

    struct Guest : public Node {
#if 1 /* With functions */
#else /* With paramexpr */
#endif
    };

    explicit DriverCapabilities(gsl::zstring<> xml) { doc.parse<0>(xml); }

#if 1 /* With functions */
  private:
    // inline auto cap() { return doc.first_node("capabilities"); }
    [[nodiscard]] inline inline auto cap() const { return doc.first_node("capabilities"); }

  public:
    // inline auto host() { return Host{cap()->first_node("host")}; }
    [[nodiscard]] inline inline Host host() const { return {cap()->first_node("host")}; }
    [[nodiscard]] inline inline GuestList guest_list() { return GuestList{cap()}; }
#else /* With paramexpr */
  private
    using cap(this s) = s.doc.first_node("capabilities");
  public
    using host(this s) = Host{s.cap()->first_node("host")};
    using guest_list(this s) = GuestList{s.cap()->};
#endif
  private:
    xml_document<> doc{};
};
} // namespace
} // namespace virtxml

#if 1 /* With functions */
#else /* With paramexpr */
#endif