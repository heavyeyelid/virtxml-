# Domains
## Power management flags

| Action	| Flags	|
| ------	| -----	|
| destroy	| _graceful_	|
| reboot	| _see shutdown flags_	|
| reset	| _N/A_	|
| resume	| _N/A_	|
| shutdown	| _acpi\_power\_btn, guest_agent, initctl, signal, paravirt_	|
| start	| _paused, autodestroy, bypass\_cache, force_boot, validate_	|
| suspend	| _N/A_	|

These flags can be a _string_, or an _array of strings_ to combine flags.

N.B. : If wrong flag specified, API will return an "Invalid flag" error.

## Status flags

| Flag | Desctription | Selector string |
| ---- | ------------ | --------------- |
| No State | no state | _N/A_ |
| Running | the domain is running | running |
| Blocked | the domain is blocked on resource | _N/A_ |
| Paused | the domain is paused by user | paused |
| Shutting down | the domain is being shut down | _N/A_ |
| Shutoff | the domain is shut off | shutoff |
| Crashed | the domain is crashed | _N/A_ |
| Power Management Suspended | the domain is suspended by guest power management | _N/A_ |

N.B. : Libvirt does not provide a selector flag for each possible status, but does provide a catch-all _other_ for the selector flags not provided. HE wraps this directly and so will only accept the same selectors as libvirt, being _running_, _shutoff_, _paused_, and _other_.

## Delete flags

| Flag | Desctription |
| ---- | ------------ |
| managed_save | Also remove any managed save |
| snapshots_metadata | If last use of domain, then also remove any snapshot metadata |
| nvram | Also remove any NVRAM file |
| keep_nvram | Keep NVRAM file |