#include "Input.h"

bool Input::operator[](int index) {
    if (index < 0 || 0xff < index)
        throw std::out_of_range("Index for input out of range");
    return keyState[index];
}

bool Input::operator[](char index) {
    TCHAR tmp = toupper(index);
    VkKeyScanEx(tmp, locale);
    return this->operator[](tmp & 0xff);
}
/*
Input::Input() {
memset(keyState, 0, 0xff);
}*/

int Input::OnInput(HWND hwnd, WPARAM wParam, LPARAM lParam) {
    UINT dwSize;

    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    LPBYTE lpb = new BYTE[dwSize];
    if (lpb == NULL)
    {
        return 0;
    }

    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize);
       // OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

    RAWINPUT* raw = (RAWINPUT*)lpb;

    if (raw->header.dwType == RIM_TYPEKEYBOARD)
    {
        keyState[raw->data.keyboard.VKey] = !(raw->data.keyboard.Flags & RI_KEY_BREAK);
        if (func) {
            char param = '\0';
            param = MapVirtualKeyEx(raw->data.keyboard.VKey, MAPVK_VK_TO_CHAR, locale);
            func(param);
            func = nullptr;
        }
    }

    delete[] lpb;
    return 0;
}

void Input::OnCreate(HWND hwnd, WPARAM wParam, LPARAM lParam){
    func = nullptr;
    locale = ::GetKeyboardLayout(0);
    memset(keyState, 0, 0xff);

    rid.usUsagePage = 0x01;
    rid.usUsage = 0x06;
    rid.dwFlags = RIDEV_NOLEGACY;
    rid.hwndTarget = hwnd;

    if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
        throw std::runtime_error("Could not initialize rawInput device");
}

void Input::blockForInput(const std::function<void (char)> &f) {
    func = f;
}