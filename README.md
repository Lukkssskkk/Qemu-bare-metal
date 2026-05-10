# Qemu Bare Metal Port

Aplicação em **C++/Qt** para facilitar a configuração e execução de código **bare-metal** em diferentes arquiteturas utilizando o **QEMU**.

## ✨ Funcionalidades
- Interface gráfica em Qt para configurar flags e parâmetros.
- Suporte a múltiplas arquiteturas (x86, ARM, etc.).
- Compilação e execução de código bare-metal diretamente no QEMU.
- Arquivos de tradução (`.ts`) para suporte multilíngue.

## 📂 Estrutura do Projeto
- `main.cpp` → Ponto de entrada da aplicação.
- `mainwindow.*` → Janela principal da interface.
- `flagsdialog.*` → Diálogo para configuração de flags.
- `CMakeLists.txt` → Configuração de build com CMake.
- `Qemu_bare_metal_port_pt_BR.ts` → Tradução para português.

## 🚀 Como compilar
1. Instale:
   - **Qt 5/6**
   - **CMake**
   - **QEMU**
   - Compilador C++ (g++, clang, etc.)

2. Clone o repositório:
   ```bash
   git clone https://github.com/Lukkssskkk/Qemu-bare-metal.git
   cd Qemu-bare-metal
