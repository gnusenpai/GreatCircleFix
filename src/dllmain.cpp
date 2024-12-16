#include "stdafx.h"
#include "helper.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <safetyhook.hpp>

#define spdlog_confparse(var) spdlog::info("Config Parse: {}: {}", #var, var)

HMODULE exeModule = GetModuleHandle(NULL);
HMODULE thisModule;

// Fix details
std::string sFixName = "GreatCircleFix";
std::string sFixVersion = "0.0.3";
std::filesystem::path sFixPath;

// Logger
std::shared_ptr<spdlog::logger> logger;
std::string sLogFile = sFixName + ".log";
std::filesystem::path sExePath;
std::string sExeName;

// Aspect ratio / FOV / HUD
std::pair DesktopDimensions = { 0,0 };
const float fPi = 3.1415926535f;
const float fNativeAspect = 2.37f;
float fAspectRatio;
float fAspectMultiplier;
float fHUDWidth;
float fHUDWidthOffset;
float fHUDHeight;
float fHUDHeightOffset;

// Variables
int iCurrentResX;
int iCurrentResY;
int iOldResX;
int iOldResY;
uint8_t* idCmdSystemLocal = nullptr;

void Logging()
{
    // Get path to DLL
    WCHAR dllPath[_MAX_PATH] = { 0 };
    GetModuleFileNameW(thisModule, dllPath, MAX_PATH);
    sFixPath = dllPath;
    sFixPath = sFixPath.remove_filename();

    // Get game name and exe path
    WCHAR exePath[_MAX_PATH] = { 0 };
    GetModuleFileNameW(exeModule, exePath, MAX_PATH);
    sExePath = exePath;
    sExeName = sExePath.filename().string();
    sExePath = sExePath.remove_filename();

    // Spdlog initialisation
    try {
        logger = spdlog::basic_logger_st(sFixName.c_str(), sExePath.string() + sLogFile, true);
        spdlog::set_default_logger(logger);
        spdlog::flush_on(spdlog::level::debug);

        spdlog::info("----------");
        spdlog::info("{:s} v{:s} loaded.", sFixName.c_str(), sFixVersion.c_str());
        spdlog::info("----------");
        spdlog::info("Log file: {}", sFixPath.string() + sLogFile);
        spdlog::info("----------");
        spdlog::info("Module Name: {0:s}", sExeName.c_str());
        spdlog::info("Module Path: {0:s}", sExePath.string());
        spdlog::info("Module Address: 0x{0:x}", (uintptr_t)exeModule);
        spdlog::info("Module Timestamp: {0:d}", Memory::ModuleTimestamp(exeModule));
        spdlog::info("----------");
    }
    catch (const spdlog::spdlog_ex& ex) {
        AllocConsole();
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        std::cout << "Log initialisation failed: " << ex.what() << std::endl;
        FreeLibraryAndExitThread(thisModule, 1);
    }  
}

void CalculateAspectRatio(bool bLog)
{
    if (iCurrentResX <= 0 || iCurrentResY <= 0)
        return;

    if (iCurrentResX == 0 || iCurrentResY == 0) {
        spdlog::error("Current Resolution: Resolution invalid, using desktop resolution instead.");
        iCurrentResX = DesktopDimensions.first;
        iCurrentResY = DesktopDimensions.second;
    }

    // Calculate aspect ratio
    fAspectRatio = (float)iCurrentResX / (float)iCurrentResY;
    fAspectMultiplier = fAspectRatio / fNativeAspect;

    // HUD 
    fHUDWidth = (float)iCurrentResY * fNativeAspect;
    fHUDHeight = (float)iCurrentResY;
    fHUDWidthOffset = (float)(iCurrentResX - fHUDWidth) / 2.00f;
    fHUDHeightOffset = 0.00f;
    if (fAspectRatio < fNativeAspect) {
        fHUDWidth = (float)iCurrentResX;
        fHUDHeight = (float)iCurrentResX / fNativeAspect;
        fHUDWidthOffset = 0.00f;
        fHUDHeightOffset = (float)(iCurrentResY - fHUDHeight) / 2.00f;
    }

    // Log details about current resolution
    if (bLog) {
        spdlog::info("----------");
        spdlog::info("Current Resolution: Resolution: {:d}x{:d}", iCurrentResX, iCurrentResY);
        spdlog::info("Current Resolution: fAspectRatio: {}", fAspectRatio);
        spdlog::info("Current Resolution: fAspectMultiplier: {}", fAspectMultiplier);
        spdlog::info("Current Resolution: fHUDWidth: {}", fHUDWidth);
        spdlog::info("Current Resolution: fHUDHeight: {}", fHUDHeight);
        spdlog::info("Current Resolution: fHUDWidthOffset: {}", fHUDWidthOffset);
        spdlog::info("Current Resolution: fHUDHeightOffset: {}", fHUDHeightOffset);
        spdlog::info("----------");
    }
}

void Miscellaneous()
{
    // com_skipIntroVideo
    std::uint8_t* SkipIntroVideoScanResult = Memory::PatternScan(exeModule, "0F 95 ?? ?? ?? FF 15 ?? ?? ?? ?? 48 8B ?? ?? 48 8D ?? ?? ?? ?? ??");
    if (SkipIntroVideoScanResult) {
        spdlog::info("Skip Intro Video: Address is {:s}+{:x}", sExeName.c_str(), SkipIntroVideoScanResult - (std::uint8_t*)exeModule);
        static SafetyHookMid SkipIntroVideoMidHook{};
        SkipIntroVideoMidHook = safetyhook::create_mid(SkipIntroVideoScanResult,
            [](SafetyHookContext& ctx) {
                // Clear ZF
                ctx.rflags &= ~(1 << 6);

                spdlog::info("Skip Intro Video: Skipped intro videos.");
            });
    }
    else {
        spdlog::error("Skip Intro Video: Pattern scan failed.");
    }

    // Remove cvar restrictions
    std::uint8_t* CVarRestrictionsScanResult = Memory::PatternScan(exeModule, "BA 01 00 00 00 49 ?? ?? 44 ?? ?? 41 FF ?? ?? 66 0F ?? ?? ?? ?? ?? ??");
    if (CVarRestrictionsScanResult) {
        spdlog::info("CVar Restrictions: Address is {:s}+{:x}", sExeName.c_str(), CVarRestrictionsScanResult - (std::uint8_t*)exeModule);
        Memory::Write(CVarRestrictionsScanResult + 0x1, (int)0);
        spdlog::info("CVar Restrictions: Disabled cvar restrictions.");
    }
    else {
        spdlog::error("CVar Restrictions: Pattern scan failed.");
    }

    // Remove read-only flag check for cvars
    std::uint8_t* ReadOnlyCvarScanResult = Memory::PatternScan(exeModule, "0F ?? ?? 0E 73 ?? 48 8B ?? ?? 48 8D ?? ?? ?? ?? ?? E8 ?? ?? ?? ??");
    if (ReadOnlyCvarScanResult) {
        spdlog::info("Read-Only Cvars: Address is {:s}+{:x}", sExeName.c_str(), ReadOnlyCvarScanResult - (std::uint8_t*)exeModule);
        static SafetyHookMid ReadOnlyCvarMidHook{};
        ReadOnlyCvarMidHook = safetyhook::create_mid(ReadOnlyCvarScanResult,
            [](SafetyHookContext& ctx) {
                // Clear read-only flag (bit 15)
                ctx.rax &= ~(1 << 15);
            });
    }
    else {
        spdlog::error("Read-Only Cvars: Pattern scan failed.");
    }
}

void CutsceneFOV()
{
    // Grab desktop resolution/aspect just in case
    DesktopDimensions = Util::GetPhysicalDesktopDimensions();
    iCurrentResX = DesktopDimensions.first;
    iCurrentResY = DesktopDimensions.second;
    CalculateAspectRatio(true);

    // Cutscene FOV
    std::uint8_t* CutsceneFOVScanResult = Memory::PatternScan(exeModule, "83 ?? ?? ?? 02 0F 28 ?? 48 8B ?? ?? ?? 0F 57 ?? F3 0F ?? ?? F3 0F ?? ?? ?? ?? ?? ??");
    if (CutsceneFOVScanResult) {
        spdlog::info("Cutscene FOV: Address is {:s}+{:x}", sExeName.c_str(), CutsceneFOVScanResult - (std::uint8_t*)exeModule);
        static SafetyHookMid CutsceneFOVMidHook{};
        CutsceneFOVMidHook = safetyhook::create_mid(CutsceneFOVScanResult + 0x8,
            [](SafetyHookContext& ctx) {
                // Check for "Fullscreen" picture framing option
                if (ctx.rax == 0) {
                    // Get current resolution
                    int iResX = (int)ctx.xmm1.f32[0];
                    int iResY = (int)ctx.xmm2.f32[0];

                    if (iResX != iCurrentResX || iResY != iCurrentResY) {
                        iCurrentResX = iResX;
                        iCurrentResY = iResY;
                        CalculateAspectRatio(true);
                    }

                    if (fAspectRatio > fNativeAspect) {
                        // Set ZF to jump over 16:9 FOV modification
                        ctx.rflags |= (1ULL << 6);

                        // Fix vert- FOV
                        ctx.xmm3.f32[0] = atanf(tanf(ctx.xmm3.f32[0] * (fPi / 360)) / fNativeAspect * fAspectRatio) * (360 / fPi);
                    }
                }
            });
    }
    else {
        spdlog::error("Cutscene FOV: Pattern scan failed.");
    }    
}

struct CVar {
    int type;
    const char* name;
    const char* value;
};

using SetCVar_t = char(*)(void*, void*);
SetCVar_t SetCVar_fn = nullptr;

void SetCVar(const std::string& cvarString)
{
    if (!SetCVar_fn || !idCmdSystemLocal)
        return;

    auto separator = cvarString.find(' ');
    if (separator == std::string::npos)
        return;

    std::string name = cvarString.substr(0, separator);
    std::string value = cvarString.substr(separator + 1);

    CVar cvar{ 2, name.c_str(), value.c_str() };

    SetCVar_fn(idCmdSystemLocal, &cvar);
    spdlog::info("Set CVar: {} = {}", name, value);
}

void CVars()
{
    // Get idCmdSystemLocal
    std::uint8_t* idCmdSystemScanResult = Memory::PatternScan(exeModule, "48 8B ?? ?? ?? ?? ?? 4C 8B ?? 48 8B ?? FF ?? ?? B9 ?? ?? ?? ?? 41 ?? ?? ?? ?? ??");
    if (idCmdSystemScanResult) {
        spdlog::info("idCmdSystemLocal: Address is {:s}+{:x}", sExeName.c_str(), idCmdSystemScanResult - (std::uint8_t*)exeModule);
        idCmdSystemLocal = *reinterpret_cast<uint8_t**>(Memory::GetAbsolute(idCmdSystemScanResult + 0x3));
        spdlog::info("idCmdSystemLocal: idCmdSystemLocal address is {:x}", (uintptr_t)idCmdSystemLocal);
    }
    else {
        spdlog::error("idCmdSystemLocal: Pattern scan failed.");
    }

    // Get SetCVar function
    std::uint8_t* SetCVarScanResult = Memory::PatternScan(exeModule, "40 ?? 53 41 ?? 48 8D ?? ?? ?? 48 81 ?? ?? ?? ?? ?? 48 8B ?? ?? ?? ?? ?? 48 33 ?? 48 89 ?? ?? 8B ?? 4C 8B ??");
    if (SetCVarScanResult) {
        spdlog::info("Set CVar Function: Address is {:s}+{:x}", sExeName.c_str(), SetCVarScanResult - (std::uint8_t*)exeModule);
        SetCVar_fn = reinterpret_cast<SetCVar_t>(SetCVarScanResult);
    }
    else {
        spdlog::error("Set CVar Function: Pattern scan failed.");
    }

    // Set CVars
    SetCVar("r_SkipGPUTriangleCulling 1"); // Fixes culling artifacts at wider aspect ratios.
}

DWORD __stdcall Main(void*)
{
    Logging();
    Miscellaneous();
    CutsceneFOV();
    CVars();
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        thisModule = hModule;
        HANDLE mainHandle = CreateThread(NULL, 0, Main, 0, NULL, 0);
        if (mainHandle) {
            SetThreadPriority(mainHandle, THREAD_PRIORITY_HIGHEST);
            CloseHandle(mainHandle);
        }
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}