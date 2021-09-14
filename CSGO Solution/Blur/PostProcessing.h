#pragma once

struct ImDrawList;
struct IDirect3DDevice9;
class ImColor;

namespace Shaders
{
    void SetDevice(IDirect3DDevice9* device) noexcept;
    void ClearBlur() noexcept;
    void DeviceReset() noexcept;
    void NewFrame() noexcept;
    void GausenBlur(ImDrawList* drawList, ImColor color) noexcept;
}
