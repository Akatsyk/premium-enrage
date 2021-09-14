#include <d3d9.h>
#include <wrl/client.h>
#include "../ImGui/imgui.h"
#include "BlurShaders/blur_x.h"
#include "BlurShaders/blur_y.h"
#include "PostProcessing.h"
using Microsoft::WRL::ComPtr;

static int BufferWidth = 0;
static int BufferHeight = 0;
static IDirect3DDevice9* device; // pihaem v super hook

static IDirect3DTexture9* CreateTexture(int width, int height) noexcept
{
    IDirect3DTexture9* texture;
    device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture, nullptr);
    return texture;
}

static void CopyBackgroundToTexture(IDirect3DTexture9* texture, D3DTEXTUREFILTERTYPE filtering) noexcept
{
    if (ComPtr<IDirect3DSurface9> backBuffer; device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, backBuffer.GetAddressOf()) == D3D_OK) 
    {
        if (ComPtr<IDirect3DSurface9> surface; texture->GetSurfaceLevel(0, surface.GetAddressOf()) == D3D_OK)
            device->StretchRect(backBuffer.Get(), nullptr, surface.Get(), nullptr, filtering);
    }
}

static void SetRenderTarget(IDirect3DTexture9* rtTexture) noexcept
{
    if (ComPtr<IDirect3DSurface9> surface; rtTexture->GetSurfaceLevel(0, surface.GetAddressOf()) == D3D_OK)
        device->SetRenderTarget(0, surface.Get());
}

class ShaderProgram 
{
public:

    void use(float uniform, int location) const noexcept
    {
        device->SetPixelShader(pixelShader.Get());
        const float params[4] = { uniform };
        device->SetPixelShaderConstantF(location, params, 1);
    }

    void init(const BYTE* pixelShaderSrc) noexcept
    {
        if (initialized)
            return;
        initialized = true;

        device->CreatePixelShader(reinterpret_cast<const DWORD*>(pixelShaderSrc), pixelShader.GetAddressOf());
    }

private:
    ComPtr<IDirect3DPixelShader9> pixelShader;
    bool initialized = false;
};

class BlurEffect 
{
public:
    static void draw(ImDrawList* drawList, ImVec4 color) noexcept
    {
        instance()._draw(drawList, color);
    }

    static void clearTextures() noexcept
    {
        if (instance().BluredTexture_One) {
            instance().BluredTexture_One->Release();
            instance().BluredTexture_One = nullptr;
        }
        if (instance().BluredTexture_Two) {
            instance().BluredTexture_Two->Release();
            instance().BluredTexture_Two = nullptr;
        }
    }

private:
    IDirect3DSurface9* Backup = nullptr;
    IDirect3DTexture9* BluredTexture_One = nullptr;
    IDirect3DTexture9* BluredTexture_Two = nullptr;

    ShaderProgram BlurShaderX;
    ShaderProgram BlurShaderY;
    static constexpr auto BlurScale = 6;

    BlurEffect() = default;
    BlurEffect(const BlurEffect&) = delete;

    ~BlurEffect()
    {
        if (Backup)
            Backup->Release();
        if (BluredTexture_One)
            BluredTexture_One->Release();
        if (BluredTexture_Two)
            BluredTexture_Two->Release();
    }

    static BlurEffect& instance() noexcept
    {
        static BlurEffect blurEffect;
        return blurEffect;
    }

    static void begin  (const ImDrawList*, const ImDrawCmd*) noexcept { instance()._begin(); }
    static void first  (const ImDrawList*, const ImDrawCmd*) noexcept { instance()._first(); }
    static void second (const ImDrawList*, const ImDrawCmd*) noexcept { instance()._second(); }
    static void end    (const ImDrawList*, const ImDrawCmd*) noexcept { instance()._end(); }
    //owner228 xD
    void OwningTextures() noexcept
    {
        if (!BluredTexture_One)
            BluredTexture_One = CreateTexture(BufferWidth / BlurScale, BufferHeight / BlurScale);
        if (!BluredTexture_Two)
            BluredTexture_Two = CreateTexture(BufferWidth / BlurScale, BufferHeight / BlurScale);
    }

    void OwningShaders() noexcept
    {
        BlurShaderX.init(blur_x);
        BlurShaderY.init(blur_y);
    }

    void _begin() noexcept
    {
        device->GetRenderTarget(0, &Backup);

        CopyBackgroundToTexture(BluredTexture_One, D3DTEXF_LINEAR);

        device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

        const D3DMATRIX projection
        {{{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            -1.0f / (BufferWidth / BlurScale), 1.0f / (BufferHeight / BlurScale), 0.0f, 1.0f
        }}};
        device->SetVertexShaderConstantF(0, &projection.m[0][0], 4);
    }

    void _first() noexcept
    {
        BlurShaderX.use(1.0f / (BufferWidth / BlurScale), 0);
        SetRenderTarget(BluredTexture_Two);
    }

    void _second() noexcept
    {
        BlurShaderY.use(1.0f / (BufferHeight / BlurScale), 0);
        SetRenderTarget(BluredTexture_One);
    }

    void _end() noexcept
    {
        device->SetRenderTarget(0, Backup);
        Backup->Release();

        device->SetPixelShader(nullptr);
        device->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
    }

    void _draw(ImDrawList* drawList, ImVec4 color) noexcept
    {
        OwningTextures();
        OwningShaders();

        if (!BluredTexture_One || !BluredTexture_Two)
            return;

        drawList->AddCallback(&begin, nullptr);
        {
            drawList->AddCallback(&first, nullptr);
            drawList->AddImage(reinterpret_cast<ImTextureID>(BluredTexture_One), { -1.0f, -1.0f }, { 1.0f, 1.0f });
            drawList->AddCallback(&second, nullptr);
            drawList->AddImage(reinterpret_cast<ImTextureID>(BluredTexture_Two), { -1.0f, -1.0f }, { 1.0f, 1.0f });
        }
        drawList->AddCallback(&end, nullptr);
        drawList->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
        //forma blura
        drawList->AddImage(reinterpret_cast<ImTextureID>(BluredTexture_One), { 0.0f, 0.0f }, { (float)BufferWidth, (float)BufferHeight }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, IM_COL32(color.x * 255, color.y * 255, color.z * 255, color.w * 255));
    }
};

void Shaders::SetDevice(IDirect3DDevice9* device) noexcept
{
    ::device = device;
}

void Shaders::ClearBlur() noexcept
{
    BlurEffect::clearTextures();
}

void Shaders::DeviceReset() noexcept
{
    BlurEffect::clearTextures();
}

void Shaders::NewFrame() noexcept
{
    if (const auto [width, height] = ImGui::GetIO().DisplaySize; BufferWidth != static_cast<int>(width) || BufferHeight != static_cast<int>(height)) {
        BlurEffect::clearTextures();
        BufferWidth = static_cast<int>(width);
        BufferHeight = static_cast<int>(height);
    }
}

void Shaders::GausenBlur(ImDrawList* drawList, ImColor color) noexcept
{
    BlurEffect::draw(drawList, color);
}
