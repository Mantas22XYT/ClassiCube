#include "../../src/Core.h"

#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Devices::Input;
using namespace Windows::Graphics::Display;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;

#include "../../src/_WindowBase.h"
#include "../../src/String.h"
#include "../../src/Funcs.h"
#include "../../src/Bitmap.h"
#include "../../src/Options.h"
#include "../../src/Errors.h"
#define UWP_STRING(str) ((wchar_t*)(str)->uni)


/*########################################################################################################################*
*--------------------------------------------------Public implementation--------------------------------------------------*
*#########################################################################################################################*/
void Window_PreInit(void) { 
	DisplayInfo.CursorVisible = true;
}

void Window_Init(void) {
	CoreWindow& window = CoreWindow::GetForCurrentThread();
	Input.Sources = INPUT_SOURCE_NORMAL;

	DisplayInfo.Width  = 640;
	DisplayInfo.Height = 480;
	DisplayInfo.Depth  = 32;
	DisplayInfo.ScaleX = 1.0f;
	DisplayInfo.ScaleY = 1.0f;

	Rect bounds = window.Bounds();

	WindowInfo.UIScaleX = DEFAULT_UI_SCALE_X;
	WindowInfo.UIScaleY = DEFAULT_UI_SCALE_Y;
	WindowInfo.Width    = bounds.Width;
	WindowInfo.Height   = bounds.Height;
}

void Window_Free(void) { }

void Window_Create2D(int width, int height) { }
void Window_Create3D(int width, int height) { }

void Window_Destroy(void) {
}

void Window_SetTitle(const cc_string* title) {
}

void Clipboard_GetText(cc_string* value) {
	DataPackageView content = Clipboard::GetContent();
	hstring str = content.GetTextAsync().get();
}

void Clipboard_SetText(const cc_string* value) {
	cc_winstring raw;
	Platform_EncodeString(&raw, value);
	auto str = hstring(UWP_STRING(&raw));

	DataPackage package = DataPackage();
	package.SetText(str);
	Clipboard::SetContent(package);
}

int Window_GetWindowState(void) {
	return WINDOW_STATE_NORMAL;
}

cc_result Window_EnterFullscreen(void) {
	return ERR_NOT_SUPPORTED;
}

cc_result Window_ExitFullscreen(void) {
	return ERR_NOT_SUPPORTED;
}

int Window_IsObscured(void) { return 0; }

void Window_Show(void) {
}

void Window_SetSize(int width, int height) {
}

void Window_RequestClose(void) {
	Event_RaiseVoid(&WindowEvents.Closing);
}

void Window_ProcessEvents(float delta) {
	CoreWindow& window = CoreWindow::GetForCurrentThread();

	CoreDispatcher& dispatcher = window.Dispatcher();
	dispatcher.ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
}

void Gamepads_Init(void) {

}

void Gamepads_Process(float delta) { }

static void Cursor_GetRawPos(int* x, int* y) {
	CoreWindow& window = CoreWindow::GetForCurrentThread();

	Point point = window.PointerPosition();
	*x = point.X;
	*y = point.Y;
}

void Cursor_SetPosition(int x, int y) {
	CoreWindow& window = CoreWindow::GetForCurrentThread();

	Point point = Point(x, y);
	window.PointerPosition(point);
}

static void Cursor_DoSetVisible(cc_bool visible) {
}

static void ShowDialogCore(const char* title, const char* msg) {
}

static cc_result OpenSaveFileDialog(const cc_string* filters, FileDialogCallback callback, cc_bool load,
									const char* const* fileExts, const cc_string* defaultName) {
	return ERR_NOT_SUPPORTED;
}

cc_result Window_OpenFileDialog(const struct OpenFileDialogArgs* args) {
	const char* const* fileExts = args->filters;
	cc_string filters; char buffer[NATIVE_STR_LEN];
	int i;

	/* Filter tokens are \0 separated - e.g. "Maps (*.cw;*.dat)\0*.cw;*.dat\0 */
	String_InitArray(filters, buffer);
	String_Format1(&filters, "%c (", args->description);
	for (i = 0; fileExts[i]; i++)
	{
		if (i) String_Append(&filters, ';');
		String_Format1(&filters, "*%c", fileExts[i]);
	}
	String_Append(&filters, ')');
	String_Append(&filters, '\0');

	for (i = 0; fileExts[i]; i++)
	{
		if (i) String_Append(&filters, ';');
		String_Format1(&filters, "*%c", fileExts[i]);
	}
	String_Append(&filters, '\0');

	return OpenSaveFileDialog(&filters, args->Callback, true, fileExts, &String_Empty);
}

cc_result Window_SaveFileDialog(const struct SaveFileDialogArgs* args) {
	const char* const* titles   = args->titles;
	const char* const* fileExts = args->filters;
	cc_string filters; char buffer[NATIVE_STR_LEN];
	int i;

	/* Filter tokens are \0 separated - e.g. "Map (*.cw)\0*.cw\0 */
	String_InitArray(filters, buffer);
	for (i = 0; fileExts[i]; i++)
	{
		String_Format2(&filters, "%c (*%c)", titles[i], fileExts[i]);
		String_Append(&filters,  '\0');
		String_Format1(&filters, "*%c", fileExts[i]);
		String_Append(&filters,  '\0');
	}
	return OpenSaveFileDialog(&filters, args->Callback, false, fileExts, &args->defaultName);
}

void Window_AllocFramebuffer(struct Bitmap* bmp, int width, int height) {
	bmp->scan0  = (BitmapCol*)Mem_Alloc(width * height, BITMAPCOLOR_SIZE, "bitmap");
	bmp->width  = width;
	bmp->height = height;
}

void Window_DrawFramebuffer(Rect2D r, struct Bitmap* bmp) {
}

void Window_FreeFramebuffer(struct Bitmap* bmp) {
	Mem_Free(bmp->scan0);
}

static cc_bool rawMouseInited, rawMouseSupported;
static void InitRawMouse(void) {
	
}

void OnscreenKeyboard_Open(struct OpenKeyboardArgs* args) { }
void OnscreenKeyboard_SetText(const cc_string* text) { }
void OnscreenKeyboard_Close(void) { }

void Window_EnableRawMouse(void) {
	DefaultEnableRawMouse();
	if (!rawMouseInited) InitRawMouse();

	rawMouseInited = true;
}

void Window_UpdateRawMouse(void) {
	if (rawMouseSupported) {
		/* handled in WM_INPUT messages */
		CentreMousePosition();
	} else {
		DefaultUpdateRawMouse();
	}
}

void Window_DisableRawMouse(void) { 
	DefaultDisableRawMouse();
}


void OpenFileDialog(void) {
    auto picker = Windows::Storage::Pickers::FileOpenPicker();
    //picker.FileTypeFilter().Append(hstring(L".jpg"));

    //Windows::Storage::StorageFile file = picker->PickSingleFileAsync();
}

struct CCApp : implements<CCApp, IFrameworkViewSource, IFrameworkView>
{

	// IFrameworkViewSource interface
	IFrameworkView CreateView()
	{
		return *this;
	}

	// IFrameworkView interface
	void Initialize(CoreApplicationView const& view)
    {
    }

    void Load(hstring const& entryPoint)
    {
    }

    void Uninitialize()
    {
    }

    void Run()
    {
        CoreWindow& window = CoreWindow::GetForCurrentThread();
		window.Activate();
		Window_Main.Handle.ptr = get_abi(window);

		extern int main(int argc, char** argv);
		main(0, NULL);
    }

    void SetWindow(CoreWindow const& win)
    {
    }
};

int __stdcall wWinMain(void*, void*, wchar_t** argv, int argc)
{
	auto app = make<CCApp>();
    CoreApplication::Run(app);
}