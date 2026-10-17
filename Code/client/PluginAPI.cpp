#include <cstdint>

namespace SkyrimTogetherAPI
{
enum class InterfaceVersion : uint8_t
{
    V1 = 1
};
} // namespace SkyrimTogetherAPI

// Looked up by SKSE plugins with GetProcAddress(GetModuleHandleW(nullptr), "RequestPluginAPI").
// ExeLoader::Load restores the export directory so the main module still exposes it.
extern "C" __declspec(dllexport) void* RequestPluginAPI(SkyrimTogetherAPI::InterfaceVersion aVersion)
{
    (void)aVersion;
    return nullptr;
}
