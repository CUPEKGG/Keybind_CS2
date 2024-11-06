
#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

#define WM_TRAYICON (WM_USER + 1)

bool programEnabled = true;
NOTIFYICONDATA nid;  // Dados para o ícone da bandeja do sistema

// Mapeamento das teclas para os códigos virtuais do Windows
std::unordered_map<std::string, WORD> keyMap = {
    {"SPACE", VK_SPACE},
    {"A", 0x41},
    {"B", 0x42},
    {"C", 0x43},
    {"D", 0x44},
    {"E", 0x45},
    {"F", 0x46},
    {"G", 0x47},
    {"H", 0x48},
    {"I", 0x49},
    {"J", 0x4A},
    {"K", 0x4B},
    {"L", 0x4C},
    {"M", 0x4D},
    {"N", 0x4E},
    {"O", 0x4F},
    {"P", 0x50},
    {"Q", 0x51},
    {"R", 0x52},
    {"S", 0x53},
    {"T", 0x54},
    {"U", 0x55},
    {"V", 0x56},
    {"W", 0x57},
    {"X", 0x58},
    {"Y", 0x59},
    {"Z", 0x5A},
    {"CTRL", VK_CONTROL},
    {"ALT", VK_MENU},
    {"MOUSE1", VK_LBUTTON},
    {"MOUSE2", VK_RBUTTON},
    {"MOUSE4", VK_XBUTTON1},
    {"F1", VK_F1},
    {"F2", VK_F2},
    {"F3", VK_F3},
    {"F4", VK_F4},
    {"F5", VK_F5},
    {"F6", VK_F6},
    {"F7", VK_F7},
    {"F8", VK_F8},
    {"F9", VK_F9},
    {"F10", VK_F10},
    {"F11", VK_F11},
    {"F12", VK_F12}
};

// Função para mapear as teclas de string para os códigos virtuais do Windows (VK_*).
WORD MapKeyToVirtualKey(const std::string &key) {
    auto it = keyMap.find(key);
    if (it != keyMap.end()) {
        return it->second;  // Retorna o código da tecla correspondente.
    }
    return 0; // Retorna 0 se a tecla não for encontrada.
}

// Função para criar o arquivo de configuração com valores padrão.
void createDefaultConfig(const std::string &filename) {
    std::ofstream file(filename);
    file << "jump_key=SPACE\n";
    file << "crouch_key=CTRL\n";
    file << "throw_key=MOUSE1\n";
    file << "move_forward_key=W\n";
    file << "move_right_key=D\n";
    file << "/////////////////////////////////////////\n";
    file << "jumpthrow_macro_key=F1\n";
    file << "jumpthrow_w_macro_key=F2\n";
    file << "jumpthrow_d_macro_key=MOUSE4\n";
    file << "/////////////////////////////////////////\n";
    file << "jumpthrow_delay=20\n";
    file << "jumpthrow_with_w_delay=40\n";
    file << "jumpthrow_with_d_delay=40\n";
    file << "jump_and_crouch_delay=30\n";
    file << "jump_and_crouch_air_time=300\n";
    file << "toggle_key=F12\n";  // Adiciona o toggle_key no arquivo de configuração.
    file.close();
    std::cout << "Arquivo de configuração criado: " << filename << "\n";
}

// Função para carregar a configuração a partir do arquivo CFG.
std::unordered_map<std::string, std::string> loadConfig(const std::string &filename) {
    std::unordered_map<std::string, std::string> config;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        size_t delimiterPos = line.find("=");
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            config[key] = value;
        }
    }

    return config;
}

// Funções para simular teclas e cliques do mouse.
void PressKey(WORD key) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    SendInput(1, &input, sizeof(INPUT));
}

void ReleaseKey(WORD key) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = key;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

void PressMouse() {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));
}

void ReleaseMouse() {
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

// Função para alternar o estado do programa (ativado/desativado).
bool ToggleProgramState(bool currentState) {
    currentState = !currentState;
    std::cout << (currentState ? "Programa ativado\n" : "Programa desativado\n");

    // Atualiza o ícone na bandeja dependendo do estado
    if (currentState) {
        nid.uFlags = NIF_ICON | NIF_TIP;
        nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);  // Altere o ícone conforme desejado
        strcpy_s(nid.szTip, "Programa Ativo");
    } else {
        nid.uFlags = NIF_ICON | NIF_TIP;
        nid.hIcon = LoadIcon(NULL, IDI_ERROR);  // Altere o ícone conforme desejado
        strcpy_s(nid.szTip, "Programa Desativado");
    }
    Shell_NotifyIcon(NIM_MODIFY, &nid);  // Modifica o ícone na bandeja

    return currentState;
}

// Funções de macros.
void JumpThrow(WORD jumpKey, WORD throwKey, int delay) {
    PressKey(jumpKey);
    Sleep(delay);
    throwKey == VK_LBUTTON ? PressMouse() : PressKey(throwKey);
    Sleep(delay);
    ReleaseKey(jumpKey);
    throwKey == VK_LBUTTON ? ReleaseMouse() : ReleaseKey(throwKey);
}

void JumpThrowWithW(WORD jumpKey, WORD throwKey, WORD moveForwardKey, int delay) {
    PressKey(moveForwardKey);
    Sleep(delay);
    PressKey(jumpKey);
    Sleep(delay);
    throwKey == VK_LBUTTON ? PressMouse() : PressKey(throwKey);
    Sleep(delay);
    ReleaseKey(jumpKey);
    throwKey == VK_LBUTTON ? ReleaseMouse() : ReleaseKey(throwKey);
    ReleaseKey(moveForwardKey);
}

void JumpThrowWithD(WORD jumpKey, WORD throwKey, WORD moveRightKey, int delay) {
    PressKey(moveRightKey);
    Sleep(delay);
    PressKey(jumpKey);
    Sleep(delay);
    throwKey == VK_LBUTTON ? PressMouse() : PressKey(throwKey);
    Sleep(delay);
    ReleaseKey(jumpKey);
    throwKey == VK_LBUTTON ? ReleaseMouse() : ReleaseKey(throwKey);
    ReleaseKey(moveRightKey);
}

void JumpAndCrouch(int jumpAndCrouchDelay, int jumpAndCrouchAirTime) {
    PressKey(VK_SPACE);
    PressKey(VK_CONTROL);
    Sleep(jumpAndCrouchDelay);
    ReleaseKey(VK_CONTROL);
    Sleep(jumpAndCrouchAirTime);
    ReleaseKey(VK_SPACE);
}

// Função para tratar os eventos da bandeja.
LRESULT CALLBACK TrayWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_TRAYICON:
            if (lp == WM_RBUTTONUP) {
                // Cria um menu de contexto
                HMENU hMenu = CreatePopupMenu();
                AppendMenu(hMenu, MF_STRING, 1, "Fechar");

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                PostMessage(hwnd, WM_NULL, 0, 0);
            }
            break;
        case WM_COMMAND:
            if (wp == 1) {
                // Fechar o programa
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
            }
            break;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

// Função para criar o ícone na bandeja do sistema.
void CreateTrayIcon(HWND hwnd) {
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);
    strcpy_s(nid.szTip, "Programa Ativo");
    nid.uCallbackMessage = WM_TRAYICON;
    Shell_NotifyIcon(NIM_ADD, &nid);
}

// Função para ocultar o console.
void HideConsole() {
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);
}

int main() {
    // Esconde o console (cmd).
    HideConsole();

    std::string configFile = "config.cfg";
    std::unordered_map<std::string, std::string> config;

    // Verifica se o arquivo de configuração existe.
    std::ifstream file(configFile);
    if (!file) {
        createDefaultConfig(configFile);
    }

    config = loadConfig(configFile);

    // Inicializa o ícone na bandeja
    HWND hwnd = GetConsoleWindow();
    CreateTrayIcon(hwnd);

    // Loop principal de macros
    while (true) {
        if (GetAsyncKeyState(MapKeyToVirtualKey(config.at("toggle_key"))) & 0x8000) {
            programEnabled = ToggleProgramState(programEnabled);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));  // Evita múltiplos toques rápidos
        }

        // Fecha o programa ao pressionar F11
        if (GetAsyncKeyState(VK_F11) & 0x8000) {
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;
        }

        if (programEnabled) {
            // Implementação das macros de ações com base nas configurações
            if (GetAsyncKeyState(MapKeyToVirtualKey(config.at("jumpthrow_macro_key"))) & 0x8000) {
                JumpThrow(MapKeyToVirtualKey(config.at("jump_key")),
                          MapKeyToVirtualKey(config.at("throw_key")),
                          std::stoi(config.at("jumpthrow_delay")));
            }

            if (GetAsyncKeyState(MapKeyToVirtualKey(config.at("jumpthrow_w_macro_key"))) & 0x8000) {
                JumpThrowWithW(MapKeyToVirtualKey(config.at("jump_key")),
                               MapKeyToVirtualKey(config.at("throw_key")),
                               MapKeyToVirtualKey(config.at("move_forward_key")),
                               std::stoi(config.at("jumpthrow_with_w_delay")));
            }

            if (GetAsyncKeyState(MapKeyToVirtualKey(config.at("jumpthrow_d_macro_key"))) & 0x8000) {
                JumpThrowWithD(MapKeyToVirtualKey(config.at("jump_key")),
                               MapKeyToVirtualKey(config.at("throw_key")),
                               MapKeyToVirtualKey(config.at("move_right_key")),
                               std::stoi(config.at("jumpthrow_with_d_delay")));
            }

        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}

