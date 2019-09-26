# List of error codes and their descriptions
## Handler

| Error code | Description |
| ---------- | ----------- |
| 0          | Syntax error |
| 1          | Bad X-Auth-Key |
| 2          | Bad URL |
| 3          | Bad request |
| 122        | Bad action |
| 123        | Unknown action |

## libvirt daemon communication
### Connection 

| Error code | Description |
| ---------- | ----------- |
| 10         | Failed to open connection to the libvirt daemon |

### Generic Objects

| Error code | Description |
| ---------- | ----------- |
| 100        | Bad object identifier |
| 101        | Invalid search key |
| 102        | Search flag doesn't exist |
| 298        | PATCH data has to be an array of actions |
| 301        | Invalid flag |

### Domains

| Error code | Description |
| ---------- | ----------- |
| 105        | Failed to create domain |
| 200        | Could not shut down the domain |
| 201        | Domain is not running |
| 202        | Could not start the domain |
| 203        | Domain is already active |
| 204        | No status actions specified |
| 205        | Renaming failed |
| 206        | Setting available memory failed |
| 207        | Setting maximum available memory failed |
| 208        | Setting autostart policy failed |
| 209        | Could not destroy the domain |
| 210        | Domain is not active |
| 211        | Domain is not suspended |
| 212        | Cannot resume the domain |
| 213        | Cannot reboot the domain |
| 214        | Cannot reset the domain |
| 215        | Cannot suspend the domain |
| 300        | Invalid power management value |

### Networks

| Error code | Description |
| ---------- | ----------- |
| 500        | Error occurred while getting network status |
| 503        | Error occurred while getting network autostart |