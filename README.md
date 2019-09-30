# virtxml++

This repository contains the C++ binding for the XML produced and consumed by the libvirt API.
Its main use is in the [virthttp](https://github.com/heavyeyelid/virthttp) project.

The intended way to use this header-only library is to add it to your git submodules and use a simple `add_subdirectory` on the submodule's folder.
Note: until further notice, this behavior is not setup