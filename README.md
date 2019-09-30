# virtxml++

This repository contains the (header-only) C++ binding for the XML produced and consumed by the libvirt API.
Its main use is in the [virthttp](https://github.com/heavyeyelid/virthttp) project.

The intended way to use this header-only library is to add it to your git submodules and use a simple `add_subdirectory` on the submodule's folder.
Note: This library uses submodules itself; if someone clones your upstream project, they must recursively init and update the submodules.
Note: until further notice, this behavior is not setup.
