const auto _wapiShowCursor   = ShowCursor;
const auto _wapiGetCursorPos = GetCursorPos;

class RenderDevice : public RenderDeviceBase
{
public:
    struct WindowInfo {
        union {
            struct {
                UINT width;
                UINT height;
                UINT refresh_rate;
            };
            D3DDISPLAYMODE internal;
        } * displays;
        D3DVIEWPORT9 viewport;
    };
    static WindowInfo displayInfo;

    static bool Init();
    static void CopyFrameBuffer();
    static void FlipScreen();
    static void Release(bool32 isRefresh);

    static void RefreshWindow();
    static void GetWindowSize(int32 *width, int32 *height);

    static void SetupImageTexture(int32 width, int32 height, uint8 *imagePixels);
    static void SetupVideoTexture_YUV420(int32 width, int32 height, uint8 *yPlane, uint8 *uPlane, uint8 *vPlane, int32 strideY, int32 strideU,
                                         int32 strideV);
    static void SetupVideoTexture_YUV422(int32 width, int32 height, uint8 *yPlane, uint8 *uPlane, uint8 *vPlane, int32 strideY, int32 strideU,
                                         int32 strideV);
    static void SetupVideoTexture_YUV444(int32 width, int32 height, uint8 *yPlane, uint8 *uPlane, uint8 *vPlane, int32 strideY, int32 strideU,
                                         int32 strideV);

    static bool ProcessEvents();

    static void InitFPSCap();
    static bool CheckFPSCap();
    static void UpdateFPSCap();

    static bool InitShaders();
    static void LoadShader(const char *fileName, bool32 linear);

    inline static void ShowCursor(bool32 shown)
    {
        if (shown) {
            while (_wapiShowCursor(true) < 0)
                ;
        }
        else {
            while (_wapiShowCursor(false) >= 0)
                ;
        }
    }

    inline static bool GetCursorPos(Vector2 *pos)
    {
        POINT cursorPos{};
        _wapiGetCursorPos(&cursorPos);
        ScreenToClient(windowHandle, &cursorPos);
        pos->x = cursorPos.x;
        pos->y = cursorPos.y;
        return true;
    }

    static inline void SetWindowTitle()
    {
#if _UNICODE
        // shoddy workaround to get the title into wide chars in UNICODE mode
        std::string str   = RSDK::gameVerInfo.gameTitle;
        std::wstring temp = std::wstring(str.begin(), str.end());
        LPCWSTR gameTitle = temp.c_str();
#else
        std::string str  = RSDK::gameVerInfo.gameTitle;
        LPCSTR gameTitle = str.c_str();
#endif

        SetWindowText(windowHandle, gameTitle);
    }

    static HWND windowHandle;
    static IDirect3DTexture9 *imageTexture;

    static IDirect3D9 *dx9Context;
    static IDirect3DDevice9 *dx9Device;

    static UINT dxAdapter;
    static int32 adapterCount;

    // WinMain args
    static HINSTANCE hInstance;
    static HINSTANCE hPrevInstance;
    static INT nShowCmd;

private:
    static bool SetupRendering();
    static void InitVertexBuffer();
    static bool InitGraphicsAPI();

    static void GetDisplays();

    static void ProcessEvent(MSG msg);
    static LRESULT CALLBACK WindowEventCallback(HWND hRecipient, UINT message, WPARAM wParam, LPARAM lParam);

    static bool useFrequency;

    static LARGE_INTEGER performanceCount, frequency, initialFrequency, curFrequency;

    static HDEVNOTIFY deviceNotif;
    static PAINTSTRUCT Paint;

    static IDirect3DVertexDeclaration9 *dx9VertexDeclare;
    static IDirect3DVertexBuffer9 *dx9VertexBuffer;
    static IDirect3DTexture9 *screenTextures[SCREEN_COUNT];
    static D3DVIEWPORT9 dx9ViewPort;

    static RECT monitorDisplayRect;
    static GUID deviceIdentifier;
};

struct ShaderEntry : public ShaderEntryBase {
    IDirect3DVertexShader9 *vertexShaderObject;
    IDirect3DPixelShader9 *pixelShaderObject;
};
