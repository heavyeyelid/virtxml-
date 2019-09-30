cmake_minimum_required(VERSION 3.12)

project(virtxml++)

cmake_policy(SET CMP0076 NEW)
set (CMAKE_CXX_STANDARD 17)
set(VirtXmlPP_VERSION 1.0.0)

add_library(VirtXmlPP INTERFACE)

target_sources(VirtXmlPP INTERFACE
        include/virtxml/basic.hpp
        include/virtxml/capabilities.hpp
        include/virtxml/cpu_types.hpp
        include/virtxml/device.hpp
        include/virtxml/domain.hpp
        include/virtxml/generic.hpp
        include/virtxml/network.hpp
        include/virtxml/storage.hpp
        include/virtxml/secret.hpp
        include/virtxml/virtxml.hpp
        include/virtxml/xmlspan.hpp
        include/virtxml/xmlval.hpp
        )


target_include_directories(VirtXmlPP INTERFACE thirdparty)
target_include_directories(VirtXmlPP INTERFACE thirdparty/rapidxml_ns)

add_subdirectory(thirdparty/gsl)

target_link_libraries(VirtXmlPP INTERFACE GSL)


add_library(virtxml++ ALIAS VirtXmlPP)
