#include "util_log.h"
#include "util_env.h"
#include "util_string.h"

using PFN_wineDbgOutput = int(__cdecl*)(const char*);

static PFN_wineDbgOutput wineDbgOutput = nullptr;

namespace dxvk::log {
    void print(const std::string& message) {
        auto line = message + '\n';
        if (wineDbgOutput)
            wineDbgOutput(line.c_str());
        else
            std::cerr << line;
    }

    void initialize(std::ofstream& filestream, bool& skipAllLogging) {
#ifdef _WIN32
        auto ntdllModule = ::GetModuleHandleA("ntdll.dll");
        if (ntdllModule != nullptr)
            wineDbgOutput = reinterpret_cast<PFN_wineDbgOutput>(reinterpret_cast<void*>(GetProcAddress(ntdllModule, "__wine_dbg_output")));
#endif

        constexpr auto logLevelEnvName = "DXVK_NVAPI_LOG_LEVEL";
        constexpr auto logPathEnvName = "DXVK_NVAPI_LOG_PATH";
        constexpr auto logFileName = "dxvk-nvapi.log";

        auto logLevel = env::getEnvVariable(logLevelEnvName);
        if (logLevel != "info") {
            skipAllLogging = true;
            return;
        }

        auto logPath = env::getEnvVariable(logPathEnvName);
        if (logPath.empty())
            return;

        if ((*logPath.rbegin()) != '/')
            logPath += '/';

        auto fullPath = logPath + logFileName;
        filestream = std::ofstream(fullPath, std::ios::app);
        filestream << "---------- " << env::getCurrentDateTime() << " ----------" << std::endl;
        print(str::format(logPathEnvName, " is set to '", logPath, "', appending log statements to ", fullPath));
    }

    void write(const std::string& message) {
        static bool alreadyInitialized = false;
        static bool skipAllLogging = false;
        static std::ofstream filestream;
        if (!std::exchange(alreadyInitialized, true))
            initialize(filestream, skipAllLogging);

        if (skipAllLogging)
            return;

        print(message);
        if (filestream)
            filestream << message << std::endl;
    }
}
