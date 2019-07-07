//
// Created by _as on 2019-01-31.
//

#pragma once

#include <filesystem>
#include <stdexcept>
#include <vector>
#include <gsl/gsl>
#include "../cexpr_algs.hpp"
#include <libvirt/libvirt-domain.h>
#include "fwd.hpp"
#include "utility.hpp"

namespace tmp {
/*
using virConnectDomainEventAgentLifecycleCallback = void (*)(virConnectPtr conn, virDomainPtr dom, int state, int reason, void* opaque);

using virConnectDomainEventBalloonChangeCallback = void (*)(virConnectPtr conn, virDomainPtr dom, unsigned long long actual, void* opaque);

using virConnectDomainEventBlockJobCallback = void (*)(virConnectPtr conn, virDomainPtr dom, const char* disk, int type, int status, void* opaque);

using virConnectDomainEventBlockThresholdCallback = void (*)(virConnectPtr conn, virDomainPtr dom, const char* dev, const char* path,
                                                         unsigned long long threshold, unsigned long long excess, void* opaque);

using virConnectDomainEventCallback = int (*)(virConnectPtr conn, virDomainPtr dom, int event, int detail, void* opaque);

int virConnectDomainEventDeregister(virConnectPtr conn, virConnectDomainEventCallback cb);
int virConnectDomainEventDeregisterAny(virConnectPtr conn, int callbackID);
using virConnectDomainEventDeviceAddedCallback = void (*)(virConnectPtr conn, virDomainPtr dom, const char* devAlias, void* opaque);

using virConnectDomainEventDeviceRemovalFailedCallback = void (*)(virConnectPtr conn, virDomainPtr dom, const char* devAlias, void* opaque);

using virConnectDomainEventDeviceRemovedCallback = void (*)(virConnectPtr conn, virDomainPtr dom, const char* devAlias, void* opaque);

using virConnectDomainEventDiskChangeCallback = void (*)(virConnectPtr conn, virDomainPtr dom, const char* oldSrcPath, const char* newSrcPath,
                                                     const char* devAlias, int reason, void* opaque);

using virConnectDomainEventGenericCallback = void (*)(virConnectPtr conn, virDomainPtr dom, void* opaque);

using virConnectDomainEventGraphicsCallback = void (*)(virConnectPtr conn, virDomainPtr dom, int phase, const virDomainEventGraphicsAddress* local,
                                                   const virDomainEventGraphicsAddress* remote, const char* authScheme,
                                                   const virDomainEventGraphicsSubject* subject, void* opaque);

using virConnectDomainEventIOErrorCallback = void (*)(virConnectPtr conn, virDomainPtr dom, const char* srcPath, const char* devAlias, int action,
                                                  void* opaque);

using virConnectDomainEventIOErrorReasonCallback = void (*)(virConnectPtr conn, virDomainPtr dom, const char* srcPath, const char* devAlias,
                                                        int action, const char* reason, void* opaque);

using virConnectDomainEventJobCompletedCallback = void (*)(virConnectPtr conn, virDomainPtr dom, virTypedParameterPtr params, int nparams,
                                                       void* opaque);

using virConnectDomainEventMetadataChangeCallback = void (*)(virConnectPtr conn, virDomainPtr dom, int type, const char* nsuri, void* opaque);

using virConnectDomainEventMigrationIterationCallback = void (*)(virConnectPtr conn, virDomainPtr dom, int iteration, void* opaque);

using virConnectDomainEventPMSuspendCallback = void (*)(virConnectPtr conn, virDomainPtr dom, int reason, void* opaque);

using virConnectDomainEventPMSuspendDiskCallback = void (*)(virConnectPtr conn, virDomainPtr dom, int reason, void* opaque);

using virConnectDomainEventPMWakeupCallback = void (*)(virConnectPtr conn, virDomainPtr dom, int reason, void* opaque);

using virConnectDomainEventRTCChangeCallback = void (*)(virConnectPtr conn, virDomainPtr dom, long long utcoffset, void* opaque);

int virConnectDomainEventRegister(virConnectPtr conn, virConnectDomainEventCallback cb, void* opaque, virFreeCallback freecb);
int virConnectDomainEventRegisterAny(virConnectPtr conn, virDomainPtr dom, int eventID, virConnectDomainEventGenericCallback cb, void* opaque,
                                 virFreeCallback freecb);
using virConnectDomainEventTrayChangeCallback = void (*)(virConnectPtr conn, virDomainPtr dom, const char* devAlias, int reason, void* opaque);

using virConnectDomainEventTunableCallback = void (*)(virConnectPtr conn, virDomainPtr dom, virTypedParameterPtr params, int nparams, void* opaque);

using virConnectDomainEventWatchdogCallback = void (*)(virConnectPtr conn, virDomainPtr dom, int action, void* opaque);

char* virConnectDomainXMLFromNative(virConnectPtr conn, const char* nativeFormat, const char* nativeConfig, unsigned int flags);
char* virConnectDomainXMLToNative(virConnectPtr conn, const char* nativeFormat, const char* domainXml, unsigned int flags);
char* virConnectGetDomainCapabilities(virConnectPtr conn, const char* emulatorbin, const char* arch, const char* machine, const char* virttype,
                                  unsigned int flags);
                                  */

int virDomainBlockCommit(virDomainPtr dom, const char* disk, const char* base, const char* top, unsigned long bandwidth, unsigned int flags);
int virDomainBlockCopy(virDomainPtr dom, const char* disk, const char* destxml, virTypedParameterPtr params, int nparams, unsigned int flags);
int virDomainBlockJobAbort(virDomainPtr dom, const char* disk, unsigned int flags);
int virDomainBlockJobSetSpeed(virDomainPtr dom, const char* disk, unsigned long bandwidth, unsigned int flags);
int virDomainBlockPeek(virDomainPtr dom, const char* disk, unsigned long long offset, size_t size, void* buffer, unsigned int flags);
int virDomainBlockPull(virDomainPtr dom, const char* disk, unsigned long bandwidth, unsigned int flags);
int virDomainBlockRebase(virDomainPtr dom, const char* disk, const char* base, unsigned long bandwidth, unsigned int flags);
int virDomainBlockResize(virDomainPtr dom, const char* disk, unsigned long long size, unsigned int flags);
int virDomainBlockStats(virDomainPtr dom, const char* disk, virDomainBlockStatsPtr stats, size_t size);
int virDomainBlockStatsFlags(virDomainPtr dom, const char* disk, virTypedParameterPtr params, int* nparams, unsigned int flags);

int virDomainGetBlkioParameters(virDomainPtr domain, virTypedParameterPtr params, int* nparams, unsigned int flags);

int virDomainGetBlockInfo(virDomainPtr domain, const char* disk, virDomainBlockInfoPtr info, unsigned int flags);
int virDomainGetBlockIoTune(virDomainPtr dom, const char* disk, virTypedParameterPtr params, int* nparams, unsigned int flags);
int virDomainGetBlockJobInfo(virDomainPtr dom, const char* disk, virDomainBlockJobInfoPtr info, unsigned int flags);

int virDomainGetCPUStats(virDomainPtr domain, virTypedParameterPtr params, unsigned int nparams, int start_cpu, unsigned int ncpus,
                         unsigned int flags);

int virDomainGetGuestVcpus(virDomainPtr domain, virTypedParameterPtr* params, unsigned int* nparams, unsigned int flags);

int virDomainGetIOThreadInfo(virDomainPtr dom, virDomainIOThreadInfoPtr** info, unsigned int flags); //

int virDomainGetInterfaceParameters(virDomainPtr domain, const char* device, virTypedParameterPtr params, int* nparams, unsigned int flags);

int virDomainGetJobInfo(virDomainPtr domain, virDomainJobInfoPtr info);

int virDomainGetJobStats(virDomainPtr domain, int* type, virTypedParameterPtr* params, int* nparams, unsigned int flags);
int virDomainGetLaunchSecurityInfo(virDomainPtr domain, virTypedParameterPtr* params, int* nparams, unsigned int flags);

int virDomainGetMaxVcpus(virDomainPtr domain);

int virDomainGetMemoryParameters(virDomainPtr domain, virTypedParameterPtr params, int* nparams, unsigned int flags);

char* virDomainGetMetadata(virDomainPtr domain, int type, const char* uri, unsigned int flags);

int virDomainGetNumaParameters(virDomainPtr domain, virTypedParameterPtr params, int* nparams, unsigned int flags);
int virDomainGetPerfEvents(virDomainPtr domain, virTypedParameterPtr* params, int* nparams, unsigned int flags);
int virDomainGetSchedulerParameters(virDomainPtr domain, virTypedParameterPtr params, int* nparams);
int virDomainGetSchedulerParametersFlags(virDomainPtr domain, virTypedParameterPtr params, int* nparams, unsigned int flags);

char* virDomainGetSchedulerType(virDomainPtr domain, int* nparams);
int virDomainGetSecurityLabel(virDomainPtr domain, virSecurityLabelPtr seclabel);
int virDomainGetSecurityLabelList(virDomainPtr domain, virSecurityLabelPtr* seclabels);
int virDomainGetState(virDomainPtr domain, int* state, int* reason, unsigned int flags);
int virDomainGetTime(virDomainPtr dom, long long* seconds, unsigned int* nseconds, unsigned int flags);
int virDomainGetVcpuPinInfo(virDomainPtr domain, int ncpumaps, unsigned char* cpumaps, int maplen, unsigned int flags);
int virDomainGetVcpus(virDomainPtr domain, virVcpuInfoPtr info, int maxinfo, unsigned char* cpumaps, int maplen);
int virDomainGetVcpusFlags(virDomainPtr domain, unsigned int flags);
char* virDomainGetXMLDesc(virDomainPtr domain, unsigned int flags);
int virDomainHasManagedSaveImage(virDomainPtr dom, unsigned int flags);
void virDomainIOThreadInfoFree(virDomainIOThreadInfoPtr info);
int virDomainInjectNMI(virDomainPtr domain, unsigned int flags);
int virDomainInterfaceAddresses(virDomainPtr dom, virDomainInterfacePtr** ifaces, unsigned int source, unsigned int flags);
void virDomainInterfaceFree(virDomainInterfacePtr iface);
int virDomainInterfaceStats(virDomainPtr dom, const char* device, virDomainInterfaceStatsPtr stats, size_t size);
int virDomainIsPersistent(virDomainPtr dom);
int virDomainIsUpdated(virDomainPtr dom);
int virDomainListGetStats(virDomainPtr* doms, unsigned int stats, virDomainStatsRecordPtr** retStats, unsigned int flags);
virDomainPtr virDomainLookupByUUID(virConnectPtr conn, const unsigned char* uuid);
int virDomainManagedSave(virDomainPtr dom, unsigned int flags);
int virDomainManagedSaveDefineXML(virDomainPtr domain, const char* dxml, unsigned int flags);
char* virDomainManagedSaveGetXMLDesc(virDomainPtr domain, unsigned int flags);
int virDomainManagedSaveRemove(virDomainPtr dom, unsigned int flags);
int virDomainMemoryPeek(virDomainPtr dom, unsigned long long start, size_t size, void* buffer, unsigned int flags);
int virDomainMemoryStats(virDomainPtr dom, virDomainMemoryStatPtr stats, unsigned int nr_stats, unsigned int flags);
virDomainPtr virDomainMigrate(virDomainPtr domain, virConnectPtr dconn, unsigned long flags, const char* dname, const char* uri,
                              unsigned long bandwidth);
virDomainPtr virDomainMigrate2(virDomainPtr domain, virConnectPtr dconn, const char* dxml, unsigned long flags, const char* dname, const char* uri,
                               unsigned long bandwidth);

virDomainPtr virDomainMigrate3(virDomainPtr domain, virConnectPtr dconn, virTypedParameterPtr params, unsigned int nparams, unsigned int flags);
int virDomainMigrateGetCompressionCache(virDomainPtr domain, unsigned long long* cacheSize, unsigned int flags);
int virDomainMigrateGetMaxDowntime(virDomainPtr domain, unsigned long long* downtime, unsigned int flags);
int virDomainMigrateGetMaxSpeed(virDomainPtr domain, unsigned long* bandwidth, unsigned int flags);
int virDomainMigrateSetCompressionCache(virDomainPtr domain, unsigned long long cacheSize, unsigned int flags);
int virDomainMigrateSetMaxDowntime(virDomainPtr domain, unsigned long long downtime, unsigned int flags);
int virDomainMigrateSetMaxSpeed(virDomainPtr domain, unsigned long bandwidth, unsigned int flags);
int virDomainMigrateStartPostCopy(virDomainPtr domain, unsigned int flags);
int virDomainMigrateToURI(virDomainPtr domain, const char* duri, unsigned long flags, const char* dname, unsigned long bandwidth);
int virDomainMigrateToURI2(virDomainPtr domain, const char* dconnuri, const char* miguri, const char* dxml, unsigned long flags, const char* dname,
                           unsigned long bandwidth);
int virDomainMigrateToURI3(virDomainPtr domain, const char* dconnuri, virTypedParameterPtr params, unsigned int nparams, unsigned int flags);
int virDomainOpenChannel(virDomainPtr dom, const char* name, virStreamPtr st, unsigned int flags);
int virDomainOpenConsole(virDomainPtr dom, const char* dev_name, virStreamPtr st, unsigned int flags);
int virDomainOpenGraphics(virDomainPtr dom, unsigned int idx, int fd, unsigned int flags);
int virDomainOpenGraphicsFD(virDomainPtr dom, unsigned int idx, unsigned int flags);
int virDomainPMSuspendForDuration(virDomainPtr dom, unsigned int target, unsigned long long duration, unsigned int flags);
int virDomainPMWakeup(virDomainPtr dom, unsigned int flags);
int virDomainPinEmulator(virDomainPtr domain, unsigned char* cpumap, int maplen, unsigned int flags);
int virDomainPinIOThread(virDomainPtr domain, unsigned int iothread_id, unsigned char* cpumap, int maplen, unsigned int flags);
int virDomainPinVcpu(virDomainPtr domain, unsigned int vcpu, unsigned char* cpumap, int maplen);
int virDomainPinVcpuFlags(virDomainPtr domain, unsigned int vcpu, unsigned char* cpumap, int maplen, unsigned int flags);
int virDomainRestore(virConnectPtr conn, const char* from);
int virDomainRestoreFlags(virConnectPtr conn, const char* from, const char* dxml, unsigned int flags);
int virDomainSave(virDomainPtr domain, const char* to);
int virDomainSaveFlags(virDomainPtr domain, const char* to, const char* dxml, unsigned int flags);
int virDomainSaveImageDefineXML(virConnectPtr conn, const char* file, const char* dxml, unsigned int flags);
char* virDomainSaveImageGetXMLDesc(virConnectPtr conn, const char* file, unsigned int flags);
char* virDomainScreenshot(virDomainPtr domain, virStreamPtr stream, unsigned int screen, unsigned int flags);
int virDomainSendKey(virDomainPtr domain, unsigned int codeset, unsigned int holdtime, unsigned int* keycodes, int nkeycodes, unsigned int flags);
int virDomainSendProcessSignal(virDomainPtr domain, long long pid_value, unsigned int signum, unsigned int flags);
int virDomainSetBlkioParameters(virDomainPtr domain, virTypedParameterPtr params, int nparams, unsigned int flags);
int virDomainSetBlockIoTune(virDomainPtr dom, const char* disk, virTypedParameterPtr params, int nparams, unsigned int flags);
int virDomainSetBlockThreshold(virDomainPtr domain, const char* dev, unsigned long long threshold, unsigned int flags);
int virDomainSetGuestVcpus(virDomainPtr domain, const char* cpumap, int state, unsigned int flags);
int virDomainSetIOThreadParams(virDomainPtr domain, unsigned int iothread_id, virTypedParameterPtr params, int nparams, unsigned int flags);
int virDomainSetInterfaceParameters(virDomainPtr domain, const char* device, virTypedParameterPtr params, int nparams, unsigned int flags);
int virDomainSetLifecycleAction(virDomainPtr domain, unsigned int type, unsigned int action, unsigned int flags);
int virDomainSetMemoryFlags(virDomainPtr domain, unsigned long memory, unsigned int flags);
int virDomainSetMemoryParameters(virDomainPtr domain, virTypedParameterPtr params, int nparams, unsigned int flags);
int virDomainSetMemoryStatsPeriod(virDomainPtr domain, int period, unsigned int flags);
int virDomainSetMetadata(virDomainPtr domain, int type, const char* metadata, const char* key, const char* uri, unsigned int flags);
int virDomainSetNumaParameters(virDomainPtr domain, virTypedParameterPtr params, int nparams, unsigned int flags);
int virDomainSetPerfEvents(virDomainPtr domain, virTypedParameterPtr params, int nparams, unsigned int flags);
int virDomainSetSchedulerParameters(virDomainPtr domain, virTypedParameterPtr params, int nparams);
int virDomainSetSchedulerParametersFlags(virDomainPtr domain, virTypedParameterPtr params, int nparams, unsigned int flags);
int virDomainSetTime(virDomainPtr dom, long long seconds, unsigned int nseconds, unsigned int flags);
int virDomainSetUserPassword(virDomainPtr dom, const char* user, const char* password, unsigned int flags);
int virDomainSetVcpu(virDomainPtr domain, const char* vcpumap, int state, unsigned int flags);
int virDomainSetVcpus(virDomainPtr domain, unsigned int nvcpus);
int virDomainSetVcpusFlags(virDomainPtr domain, unsigned int nvcpus, unsigned int flags);
int virDomainUpdateDeviceFlags(virDomainPtr domain, const char* xml, unsigned int flags);
} // namespace tmp

namespace virt {
class Domain {
    friend Connection;

    virDomainPtr underlying = nullptr;

  public:
    using Info = virDomainInfo;
    struct Stats {
        enum class Types {
            STATE = VIR_DOMAIN_STATS_STATE,         // domain state
            CPU_TOTAL = VIR_DOMAIN_STATS_CPU_TOTAL, // domain CPU info
            BALLOON = VIR_DOMAIN_STATS_BALLOON,     // domain balloon info
            VCPU = VIR_DOMAIN_STATS_VCPU,           // domain virtual CPU info
            INTERFACE = VIR_DOMAIN_STATS_INTERFACE, // domain interfaces info
            BLOCK = VIR_DOMAIN_STATS_BLOCK,         // domain block info
            PERF = VIR_DOMAIN_STATS_PERF,           // domain perf event info
            // IOTHREAD	=	VIR_DOMAIN_STATS_IOTHREAD, // iothread poll info
        };

        class Record;
    };
    struct CoreDump {
        enum class Flags {
            CRASH = VIR_DUMP_CRASH,               /* crash after dump */
            LIVE = VIR_DUMP_LIVE,                 /* live dump */
            BYPASS_CACHE = VIR_DUMP_BYPASS_CACHE, /* avoid file system cache pollution */
            RESET = VIR_DUMP_RESET,               /* reset domain after dump finishes */
            MEMORY_ONLY = VIR_DUMP_MEMORY_ONLY,   /* use dump-guest-memory */
        };
        enum class Format {
            RAW = VIR_DOMAIN_CORE_DUMP_FORMAT_RAW,                   /* dump guest memory in raw format */
            KDUMP_ZLIB = VIR_DOMAIN_CORE_DUMP_FORMAT_KDUMP_ZLIB,     /* kdump-compressed format, with zlib compression */
            KDUMP_LZO = VIR_DOMAIN_CORE_DUMP_FORMAT_KDUMP_LZO,       /* kdump-compressed format, with lzo compression */
            KDUMP_SNAPPY = VIR_DOMAIN_CORE_DUMP_FORMAT_KDUMP_SNAPPY, /* kdump-compressed format, with snappy compression */
        };
    };
    enum class CreateFlags {
        NONE = VIR_DOMAIN_NONE,
        PAUSED = VIR_DOMAIN_START_PAUSED,
        AUTODESTROY = VIR_DOMAIN_START_AUTODESTROY,
        BYPASS_CACHE = VIR_DOMAIN_START_BYPASS_CACHE,
        FORCE_BOOT = VIR_DOMAIN_START_FORCE_BOOT,
        VALIDATE = VIR_DOMAIN_START_VALIDATE
    };
    enum class DestroyFlags {
        DEFAULT = VIR_DOMAIN_DESTROY_DEFAULT,   /* Default behavior - could lead to data loss!! */
        GRACEFUL = VIR_DOMAIN_DESTROY_GRACEFUL, /* only SIGTERM, no SIGKILL */
    };
    enum class DeviceModifyFlags {
        /* See ModificationImpactFlags for these flags.  */
        CURRENT = VIR_DOMAIN_DEVICE_MODIFY_CURRENT,
        LIVE = VIR_DOMAIN_DEVICE_MODIFY_LIVE,
        CONFIG = VIR_DOMAIN_DEVICE_MODIFY_CONFIG,

        /* Additionally, these flags may be bitwise-OR'd in.  */
        FORCE = VIR_DOMAIN_DEVICE_MODIFY_FORCE, /* Forcibly modify device (ex. force eject a cdrom) */
    };
    enum class ShutdownFlags {
        DEFAULT = VIR_DOMAIN_REBOOT_DEFAULT,               /* hypervisor choice */
        ACPI_POWER_BTN = VIR_DOMAIN_REBOOT_ACPI_POWER_BTN, /* Send ACPI event */
        GUEST_AGENT = VIR_DOMAIN_REBOOT_GUEST_AGENT,       /* Use guest agent */
        INITCTL = VIR_DOMAIN_REBOOT_INITCTL,               /* Use initctl */
        SIGNAL = VIR_DOMAIN_REBOOT_SIGNAL,                 /* Send a signal */
        PARAVIRT = VIR_DOMAIN_REBOOT_PARAVIRT,             /* Use paravirt guest control */
    };
    enum class ModificationImpactFlags {
        CURRENT = VIR_DOMAIN_AFFECT_CURRENT, /* Affect current domain state.  */
        LIVE = VIR_DOMAIN_AFFECT_LIVE,       /* Affect running domain state.  */
        CONFIG = VIR_DOMAIN_AFFECT_CONFIG,   /* Affect persistent domain state.  */
    };
    enum class State {
        NOSTATE = VIR_DOMAIN_NOSTATE,         /* no state */
        RUNNING = VIR_DOMAIN_RUNNING,         /* the domain is running */
        BLOCKED = VIR_DOMAIN_BLOCKED,         /* the domain is blocked on resource */
        PAUSED = VIR_DOMAIN_PAUSED,           /* the domain is paused by user */
        SHUTDOWN = VIR_DOMAIN_SHUTDOWN,       /* the domain is being shut down */
        SHUTOFF = VIR_DOMAIN_SHUTOFF,         /* the domain is shut off */
        CRASHED = VIR_DOMAIN_CRASHED,         /* the domain is crashed */
        PMSUSPENDED = VIR_DOMAIN_PMSUSPENDED, /* the domain is suspended by guest power management */
        ENUM_END
    };
    struct alignas(alignof(virDomainDiskError)) DiskError;
    struct FSInfo;
    class States {
        constexpr static std::array states = {"No State", "Running", "Blocked", "Paused",
                                              "Shutdown", "Shutoff", "Crashed", "Power Management Suspended"};

      public:
        [[nodiscard]] constexpr const char* operator[](State val) const noexcept { return states[to_integral(val)]; }
        [[nodiscard]] constexpr const char* operator[](unsigned char val) const noexcept { return states[+val]; }
        [[nodiscard]] constexpr State operator[](std::string_view sv) const noexcept {
            const auto res = cexpr::find(states.cbegin(), states.cend(), sv);
            return State(std::distance(states.cbegin(), res));
        }
    } constexpr static States{};
    enum class UndefineFlags {
        MANAGED_SAVE = VIR_DOMAIN_UNDEFINE_MANAGED_SAVE,             /* Also remove any managed save */
        SNAPSHOTS_METADATA = VIR_DOMAIN_UNDEFINE_SNAPSHOTS_METADATA, /* If last use of domain, then also remove any snapshot metadata */
        NVRAM = VIR_DOMAIN_UNDEFINE_NVRAM,                           /* Also remove any nvram file */
        KEEP_NVRAM = VIR_DOMAIN_UNDEFINE_KEEP_NVRAM,                 /* Keep nvram file */
    };

    constexpr inline explicit Domain(virDomainPtr ptr = nullptr) noexcept;

    Domain(const Domain&) = delete;

    constexpr inline Domain(Domain&&) noexcept;

    Domain& operator=(const Domain&) = delete;

    inline Domain& operator=(Domain&&) noexcept;

    inline ~Domain() noexcept;

    constexpr inline explicit operator bool() const noexcept;

    bool abortJob() noexcept;

    bool addIOThread(unsigned int iothread_id, ModificationImpactFlags flags = ModificationImpactFlags::CURRENT) noexcept;

    bool attachDevice(gsl::czstring<> xml) noexcept;

    bool attachDevice(gsl::czstring<> xml, DeviceModifyFlags flags) noexcept;

    bool create() noexcept;

    bool create(CreateFlags flags) noexcept;

    // createWithFiles() // Left out

    bool coreDump(std::filesystem::path to, CoreDump::Flags flags) const noexcept;

    bool coreDump(std::filesystem::path to, CoreDump::Format format, CoreDump::Flags flags) const noexcept;

    bool delIOThread(unsigned int iothread_id, ModificationImpactFlags flags) noexcept;

    bool destroy() noexcept;

    bool destroy(DestroyFlags flags) noexcept;

    bool detachDevice(gsl::czstring<> xml) noexcept;

    bool detachDevice(gsl::czstring<> xml, DeviceModifyFlags flags = DeviceModifyFlags::LIVE) noexcept;

    bool detachDeviceAlias(gsl::czstring<> alias, DeviceModifyFlags flags = DeviceModifyFlags::LIVE) noexcept;

    int fsFreeze(gsl::span<gsl::czstring<>> mountpoints) noexcept;

    int fsThaw(gsl::span<gsl::czstring<>> mountpoints) noexcept;

    bool fsTrim(gsl::czstring<> mountpoint, unsigned long long minimum) noexcept;

    [[nodiscard]] bool getAutostart() const noexcept;

    [[nodiscard]] Connection getConnect() const noexcept;

    [[nodiscard]] std::optional<virDomainControlInfo> getControlInfo() const noexcept;

    [[nodiscard]] auto getDiskErrors() const noexcept;

    [[nodiscard]] std::vector<DiskError> extractDiskErrors() const;

    // [[nodiscard]] int getEmulatorPinInfo(unsigned char* cpumap, int maplen, unsigned int flags); // TODO figure out the intended way of using this

    [[nodiscard]] auto getFSInfo() const noexcept;

    [[nodiscard]] std::vector<FSInfo> extractFSInfo() const;

    [[nodiscard]] UniqueZstring getHostname() const noexcept;

    [[nodiscard]] std::string extractHostname() const noexcept;

    [[nodiscard]] Info getInfo() const noexcept;

    [[nodiscard]] gsl::czstring<> getName() const noexcept;

    [[nodiscard]] unsigned getID() const noexcept;

    [[nodiscard]] auto getUUID() const;

    [[nodiscard]] bool isActive() const noexcept;

    [[nodiscard]] auto getUUIDString() const noexcept -> std::optional<std::array<char, VIR_UUID_STRING_BUFLEN>>;

    [[nodiscard]] auto extractUUIDString() const -> std::string;

    [[nodiscard]] auto getOSType() const;

    [[nodiscard]] unsigned long getMaxMemory() const noexcept;

    bool setMaxMemory(unsigned long);

    bool setMemory(unsigned long);

    void reboot(ShutdownFlags flags = ShutdownFlags::DEFAULT);

    void reset();

    bool rename(gsl::czstring<>);

    bool resume() noexcept;

    bool setAutoStart(bool);
    bool shutdown() noexcept;

    bool shutdown(ShutdownFlags flags) noexcept;

    void suspend();

    bool undefine() noexcept;

    bool undefine(UndefineFlags) noexcept;

    [[nodiscard]] static Domain createXML(Connection&, gsl::czstring<> xml, CreateFlags flags = CreateFlags::NONE);

    // [[nodiscard]] static Domain defineXML();
};

class Domain::Stats::Record {
    friend Connection;

    Domain dom;
    std::vector<TypedParameter> params{};

    explicit Record(const virDomainStatsRecord&) noexcept;

  public:
};

struct alignas(alignof(virDomainDiskError)) Domain::DiskError {
    enum class Code : decltype(virDomainDiskError::error) {
        NONE = VIR_DOMAIN_DISK_ERROR_NONE,         /* no error */
        UNSPEC = VIR_DOMAIN_DISK_ERROR_UNSPEC,     /* unspecified I/O error */
        NO_SPACE = VIR_DOMAIN_DISK_ERROR_NO_SPACE, /* no space left on the device */
    };

    UniqueZstring disk;
    Code error;
};

struct Domain::FSInfo {
    std::string mountpoint;             /* path to mount point */
    std::string name;                   /* device name in the guest (e.g. "sda1") */
    std::string fstype;                 /* filesystem type */
    std::vector<std::string> dev_alias; /* vector of disk device aliases */

    FSInfo(virDomainFSInfo* from) noexcept
        : mountpoint(from->mountpoint), name(from->name), fstype(from->fstype), dev_alias(from->devAlias, from->devAlias + from->ndevAlias) {
        virDomainFSInfoFree(from);
    }
};

[[nodiscard]] constexpr inline Domain::CoreDump::Flags operator|(Domain::CoreDump::Flags lhs, Domain::CoreDump::Flags rhs) noexcept;
[[nodiscard]] constexpr inline Domain::ModificationImpactFlags operator|(Domain::ModificationImpactFlags lhs,
                                                                         Domain::ModificationImpactFlags rhs) noexcept;
[[nodiscard]] constexpr inline Domain::Stats::Types operator|(Domain::Stats::Types lhs, Domain::Stats::Types rhs) noexcept;
} // namespace virt
