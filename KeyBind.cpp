
#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <cstdio>
#include <string.h>

using namespace std;


// Função para exibir informações do criador
void ShowCreatorInfo()
{
    std::cout << "**************************************************\n";
    std::cout << "*       Criado por:  CUPEKGG                     *\n";
    std::cout << "*       Contato:     https://github.com/CUPEKGG  *\n";
    std::cout << "*       Versão:      1.0.4                       *\n";
    std::cout << "*       Data:        25/08/2024                  *\n";
    std::cout << "**************************************************\n";
}

// Função para criar o arquivo de configuração com valores padrão
void createDefaultConfig(const std::string &filename)
{
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
    file << "jump_and_crouch_macro_key=C\n";
    file << "/////////////////////////////////////////\n";
    file << "jumpthrow_delay=20\n";
    file << "jumpthrow_with_w_delay=40\n";
    file << "jumpthrow_with_d_delay=40\n";
    file << "jump_and_crouch_delay=30\n";
    file << "jump_and_crouch_air_time=300\n";
    file.close();
    std::cout << "Arquivo de configuração criado: " << filename << "\n";
}

// Função para carregar a configuração a partir do arquivo CFG
std::unordered_map<std::string, std::string> loadConfig(const std::string &filename)
{
    std::unordered_map<std::string, std::string> config;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
        size_t delimiterPos = line.find("=");
        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);
        config[key] = value;
    }

    return config;
}

// Função para verificar se o arquivo existe
bool fileExists(const std::string &filename)
{
    FILE *file = fopen(filename.c_str(), "r");
    if (file)
    {
        fclose(file);
        return true;
    }
    return false;
}

// Função para simular uma tecla pressionada
void PressKey(WORD key)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    input.ki.wVk = key;
    input.ki.dwFlags = 0;
    SendInput(1, &input, sizeof(INPUT));
}

// Função para simular uma tecla liberada
void ReleaseKey(WORD key)
{
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;

    input.ki.wVk = key;
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

// Função para simular um botão do mouse pressionado
void PressMouse()
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = 0;
    input.mi.dy = 0;
    input.mi.mouseData = 0;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;
    SendInput(1, &input, sizeof(INPUT));
}

// Função para simular um botão do mouse liberado
void ReleaseMouse()
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = 0;
    input.mi.dy = 0;
    input.mi.mouseData = 0;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;
    SendInput(1, &input, sizeof(INPUT));
}

// Estrutura para armazenar os tempos das macros
struct MacroTimings {
    int jumpthrowDelay;
    int jumpthrowWithWDelay;
    int jumpthrowWithDDelay;
    int jumpAndCrouchDelay;
    int jumpAndCrouchAirTime;
};

// Função para carregar os tempos das macros a partir da configuração
MacroTimings LoadMacroTimingsFromConfig(const std::unordered_map<std::string, std::string>& config) {
    MacroTimings timings;
    try {
        timings.jumpthrowDelay = std::stoi(config.at("jumpthrow_delay"));
        timings.jumpthrowWithWDelay = std::stoi(config.at("jumpthrow_with_w_delay"));
        timings.jumpthrowWithDDelay = std::stoi(config.at("jumpthrow_with_d_delay"));
        timings.jumpAndCrouchDelay = std::stoi(config.at("jump_and_crouch_delay"));
        timings.jumpAndCrouchAirTime = std::stoi(config.at("jump_and_crouch_air_time"));
    } catch (const std::out_of_range& e) {
        std::cerr << "Erro: Falta uma ou mais chaves no arquivo de configuração.\n";
        std::cerr << "Certifique-se de que todas as chaves necessárias estão presentes.\n";
        throw; // Relança a exceção para que o programa possa ser interrompido
    } catch (const std::invalid_argument& e) {
        std::cerr << "Erro: Valor inválido no arquivo de configuração.\n";
        std::cerr << "Certifique-se de que todos os valores são números inteiros válidos.\n";
        throw; // Relança a exceção para que o programa possa ser interrompido
    }
    return timings;
}

// Função para realizar o Jumpthrow
void JumpThrow(WORD jumpKey, WORD throwKey, int delay) {
    PressKey(jumpKey);
    Sleep(delay);

    if (throwKey == VK_LBUTTON) {
        PressMouse();
    } else {
        PressKey(throwKey);
    }
    Sleep(delay);

    ReleaseKey(jumpKey);

    if (throwKey == VK_LBUTTON) {
        ReleaseMouse();
    } else {
        ReleaseKey(throwKey);
    }
}

// Função para realizar o Jumpthrow com W
void JumpThrowWithW(WORD jumpKey, WORD throwKey, WORD moveForwardKey, int delay) {
    PressKey(moveForwardKey);
    Sleep(delay);

    PressKey(jumpKey);
    Sleep(delay);

    if (throwKey == VK_LBUTTON) {
        PressMouse();
    } else {
        PressKey(throwKey);
    }
    Sleep(delay);

    ReleaseKey(jumpKey);

    if (throwKey == VK_LBUTTON) {
        ReleaseMouse();
    } else {
        ReleaseKey(throwKey);
    }

    ReleaseKey(moveForwardKey);
}

// Função para realizar o Jumpthrow com D
void JumpThrowWithD(WORD jumpKey, WORD throwKey, WORD moveRightKey, int delay) {
    PressKey(moveRightKey);
    Sleep(delay);

    PressKey(jumpKey);
    Sleep(delay);

    if (throwKey == VK_LBUTTON) {
        PressMouse();
    } else {
        PressKey(throwKey);
    }
    Sleep(delay);

    ReleaseKey(jumpKey);

    if (throwKey == VK_LBUTTON) {
        ReleaseMouse();
    } else {
        ReleaseKey(throwKey);
    }

    ReleaseKey(moveRightKey);
}

// Função para pular + agachar
void JumpAndCrouch(int jumpAndCrouchDelay, int jumpAndCrouchAirTime) {
    PressKey(VK_SPACE); // Pressiona a tecla de pular
    Sleep(30);

    ReleaseKey(VK_SPACE); // Solta a tecla de pular

    Sleep(2); // Pequena pausa entre as ações

    PressKey(VK_CONTROL); // Pressiona a tecla de agachar
    Sleep(jumpAndCrouchAirTime);

    ReleaseKey(VK_CONTROL); // Solta a tecla de agachar
}

int main()
{
    const std::string configFile = "config.cfg";

    // Exibe informações do criador
    ShowCreatorInfo();

    // Verifica se o arquivo de configuração existe
    if (!fileExists(configFile))
    {
        // Se não existir, cria com valores padrão
        createDefaultConfig(configFile);
    }

    // Carrega a configuração do arquivo
    auto config = loadConfig(configFile);

    // Carrega os tempos das macros da configuração
    MacroTimings timings = LoadMacroTimingsFromConfig(config);

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
        {"F1", VK_F1},
        {"F2", VK_F2},
        {"F3", VK_F3},
        {"F4", VK_F4},
        {"MOUSE1", VK_LBUTTON},
        {"MOUSE2", VK_RBUTTON},
        {"MOUSE3", VK_MBUTTON},
        {"MOUSE4", VK_XBUTTON1},
        {"MOUSE5", VK_XBUTTON2},

        // Adicione outras teclas conforme necessário
    };

    WORD jumpKey = keyMap[config["jump_key"]];
    WORD throwKey = keyMap[config["throw_key"]];
    WORD moveForwardKey = keyMap[config["move_forward_key"]];
    WORD moveRightKey = keyMap[config["move_right_key"]];
    WORD jumpthrowMacroKey = keyMap[config["jumpthrow_macro_key"]];
    WORD jumpthrowWMacroKey = keyMap[config["jumpthrow_w_macro_key"]];
    WORD jumpthrowDMacroKey = keyMap[config["jumpthrow_d_macro_key"]];
    WORD jumpAndCrouchMacroKey = keyMap[config["jump_and_crouch_macro_key"]];

    std::cout << "Pressione " << config["jumpthrow_macro_key"] << " para realizar o Jumpthrow...\n";
    std::cout << "Pressione " << config["jumpthrow_w_macro_key"] << " para realizar o Jumpthrow com W...\n";
    std::cout << "Pressione " << config["jumpthrow_d_macro_key"] << " para realizar o Jumpthrow com D...\n";
    std::cout << "Pressione " << config["jump_and_crouch_macro_key"] << " para realizar o Pulo + Agachamento...\n";

    while (true) {
        if (GetAsyncKeyState(jumpthrowMacroKey) & 0x8000) {
            JumpThrow(jumpKey, throwKey, timings.jumpthrowDelay);
            Sleep(1000);
        } else if (GetAsyncKeyState(jumpthrowWMacroKey) & 0x8000) {
            JumpThrowWithW(jumpKey, throwKey, moveForwardKey, timings.jumpthrowWithWDelay);
            Sleep(1000);
        } else if (GetAsyncKeyState(jumpthrowDMacroKey) & 0x8000) {
            JumpThrowWithD(jumpKey, throwKey, moveRightKey, timings.jumpthrowWithDDelay);
            Sleep(1000);
        } else if (GetAsyncKeyState(jumpAndCrouchMacroKey) & 0x8000) {
            JumpAndCrouch(timings.jumpAndCrouchDelay, timings.jumpAndCrouchAirTime);
            Sleep(70);
        } else {
            Sleep(30);
        }
    }

    return 0;
}