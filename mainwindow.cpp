#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./flagsdialog.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Model do sistema de arquivos
    model = new QFileSystemModel(this);
    //model->setRootPath("");
    ui->treeView->setModel(model);
    // esconder colunas extras (tamanho, tipo, data)
    for (int i = 1; i < model->columnCount(); ++i)
        ui->treeView->hideColumn(i);
    // 🔗 Conexões (melhor que on_pushButton...)
    connect(ui->pushButton,   &QPushButton::clicked, this, &MainWindow::runProject);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::createImage);
    connect(ui->toolButton,   &QToolButton::clicked, this, &MainWindow::generateCMake);
    //
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::openProject);
    connect(ui->actionAdd_folder, &QAction::triggered, this, &MainWindow::openProject);
    //
    connect(ui->pushButton_5, &QPushButton::clicked,this,&MainWindow::closeProject);
    connect(ui->actionClose_project, &QAction::triggered,this,&MainWindow::closeProject);
    //
    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::CleanLogs);
    connect(ui->actionRAM,&QAction::triggered,this,&MainWindow::N_RAM);
    connect(ui->actionDisco_de_img,&QAction::triggered,this,&MainWindow::N_armazenamento);
    connect(ui->actionNumero_de_Threads,&QAction::triggered,this,&MainWindow::N_threads);
    connect(ui->actionFlags_unicas,&QAction::triggered,this,&MainWindow::New_flags);
    //
    connect(ui->actionx86,&QAction::triggered,this,&MainWindow::newx86);
    connect(ui->actionArm,&QAction::triggered,this,&MainWindow::newarm);
    connect(ui->actionRiscV,&QAction::triggered,this,&MainWindow::newriscv);
    //
    QActionGroup *archGroup = new QActionGroup(this);
    archGroup->setExclusive(true);
    ui->actionArm_2->setCheckable(true);
    ui->actionRiscV_2->setCheckable(true);
    ui->actionx86_2->setCheckable(true);
    archGroup->addAction(ui->actionArm_2);
    archGroup->addAction(ui->actionRiscV_2);
    archGroup->addAction(ui->actionx86_2);

    connect(ui->actionArm_2,&QAction::triggered,this,&MainWindow::arquitetura);
    connect(ui->actionRiscV_2,&QAction::triggered,this,&MainWindow::arquitetura);
    connect(ui->actionx86_2,&QAction::triggered,this,&MainWindow::arquitetura);
    //
    connect(ui->actionAudio, &QAction::triggered, this, &MainWindow::Options);
    connect(ui->actionC, &QAction::triggered, this, &MainWindow::Options);
    connect(ui->actionC_2, &QAction::triggered, this, &MainWindow::Options);
    connect(ui->actionVirtualiza_ao_em_rede, &QAction::triggered, this,&MainWindow::Options);
    connect(ui->actionIPV4, &QAction::triggered, this, &MainWindow::Options);
    connect(ui->actionIPV6, &QAction::triggered, this, &MainWindow::Options);
    connect(ui->actionBluethoot, &QAction::triggered, this, &MainWindow::Options);
    //
    connect(ui->actionDefine_path,&QAction::triggered,this,&MainWindow::definePATH);
}

MainWindow::~MainWindow(){
    delete ui; // model é deletado automaticamente (parent = this)
}

void MainWindow::runProject()
{
    if (currentProjectPath.isEmpty())
    {
        ui->textEdit->append("❌ Nenhum projeto aberto");
        return;
    }

    ui->textEdit->append("▶ Iniciando build...");

    QString buildDir = currentProjectPath + "/build";

    QDir().mkpath(buildDir);

    QString qemu;

    switch (arq)
    {
    case X86:
        qemu = "qemu-system-x86_64";
        break;

    case ARM:
        qemu = "qemu-system-arm";
        break;

    case RISCV:
        qemu = "qemu-system-riscv64";
        break;
    }

    QString command;

    command += "cd \"" + buildDir + "\" && ";

    command += "cmake .. && ";

    command += "make -j" + QString::number(Threads) + " && ";

    command += qemu;

    command += " -m " + QString::number(RAM);

    if (options[VirRede])
        command += " -net nic";

    if (options[Audio])
        command += " -audiodev pa,id=snd0";

    command += " " + flags_qemu;

    process = new QProcess(this);

    connect(process, &QProcess::readyReadStandardOutput,
            this, [this]()
            {
                ui->textEdit->append(process->readAllStandardOutput());
            });

    connect(process, &QProcess::readyReadStandardError,
            this, [this]()
            {
                ui->textEdit->append(process->readAllStandardError());
            });

    process->start("bash", QStringList() << "-c" << command);

    ui->textEdit->append("🚀 Executando:");
    ui->textEdit->append(command);
}

void MainWindow::createImage()
{
    if (currentProjectPath.isEmpty())
    {
        ui->textEdit->append("❌ Nenhum projeto aberto");
        return;
    }

    QString imgPath = currentProjectPath + "/disk.img";

    QString command =
        "dd if=/dev/zero of=\"" + imgPath +
        "\" bs=1M count=" + QString::number(DISK);

    process = new QProcess(this);

    connect(process, &QProcess::readyReadStandardOutput,
            this, [this]()
            {
                ui->textEdit->append(process->readAllStandardOutput());
            });

    process->start("bash", QStringList() << "-c" << command);

    ui->textEdit->append("💾 Criando imagem de disco...");
}

void MainWindow::generateCMake()
{
    if (currentProjectPath.isEmpty())
    {
        ui->textEdit->append("❌ Nenhum projeto aberto");
        return;
    }

    QString cmakePath = currentProjectPath + "/CMakeLists.txt";

    QFile file(cmakePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        ui->textEdit->append("❌ Erro ao criar CMakeLists.txt");
        return;
    }

    QTextStream out(&file);

    // =====================================================
    // HEADER
    // =====================================================

    out << "cmake_minimum_required(VERSION 3.16)\n\n";

    // =====================================================
    // ARQUITETURA
    // =====================================================

    switch (arq)
    {
    case X86:
    {
        out << "project(BareMetalProject C CXX ASM_NASM)\n\n";

        out << "enable_language(ASM_NASM)\n";

        out << "set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)\n\n";

        break;
    }

    case ARM:
    {
        out << "project(BareMetalProject C CXX ASM)\n\n";

        out << "enable_language(ASM)\n\n";

        break;
    }

    case RISCV:
    {
        out << "project(BareMetalProject C CXX ASM)\n\n";

        out << "enable_language(ASM)\n\n";

        break;
    }
    }

    // =====================================================
    // COMPILATION DATABASE
    // =====================================================

    out << "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n\n";

    // =====================================================
    // FLAGS C/C++
    // =====================================================

    if (options[C])
    {
        out << "set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} ";

        out << "-ffreestanding ";

        out << flags_c;

        out << "\")\n";
    }

    if (options[Cpp])
    {
        out << "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} ";

        out << "-ffreestanding -fno-exceptions -fno-rtti ";

        out << flags_cpp;

        out << "\")\n";
    }

    // =====================================================
    // ASM FLAGS
    // =====================================================

    if (!flags_asm.isEmpty())
    {
        out << "set(CMAKE_ASM_FLAGS \"${CMAKE_ASM_FLAGS} ";

        out << flags_asm;

        out << "\")\n";
    }

    out << "\n";

    // =====================================================
    // SOURCE FILES
    // =====================================================

    out << "file(GLOB_RECURSE SOURCES\n";

    if (options[Cpp])
        out << "    \"src/*.cpp\"\n";

    if (options[C])
        out << "    \"src/*.c\"\n";

    if (arq == X86)
        out << "    \"src/*.nasm\"\n";
    else
        out << "    \"src/*.S\"\n";

    out << ")\n\n";

    // =====================================================
    // EXECUTABLE
    // =====================================================

    out << "add_executable(kernel ${SOURCES})\n\n";

    // =====================================================
    // LINKER
    // =====================================================

    out << "set_target_properties(kernel PROPERTIES\n";

    if (options[C])
        out << "    LINKER_LANGUAGE C\n";
    else
        out << "    LINKER_LANGUAGE CXX\n";

    out << ")\n\n";

    // =====================================================
    // LINK OPTIONS
    // =====================================================

    out << "target_link_options(kernel PRIVATE\n";

    out << "    -T ${CMAKE_SOURCE_DIR}/linker.ld\n";

    out << "    -nostdlib\n";

    out << "    -ffreestanding\n";

    switch (arq)
    {
    case X86:
        out << "    -no-pie\n";
        break;

    case ARM:
        out << "    -Wl,--build-id=none\n";
        break;

    case RISCV:
        out << "    -Wl,--build-id=none\n";
        break;
    }

    out << ")\n\n";

    // =====================================================
    // QEMU CUSTOM TARGET
    // =====================================================

    QString qemu;

    switch (arq)
    {
    case X86:
        qemu = "qemu-system-x86_64";
        break;

    case ARM:
        qemu = "qemu-system-arm";
        break;

    case RISCV:
        qemu = "qemu-system-riscv64";
        break;
    }

    out << "add_custom_target(run\n";

    out << "    COMMAND " << qemu;

    out << " -kernel kernel";

    out << " -m " << RAM;

    if (options[VirRede])
        out << " -net nic";

    if (options[Audio])
        out << " -audiodev pa,id=snd0";

    out << " " << flags_qemu;

    out << "\n";

    out << "    DEPENDS kernel\n";

    out << "    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}\n";

    out << ")\n";

    // =====================================================
    // END
    // =====================================================

    file.close();

    ui->textEdit->append("⚙️ CMakeLists.txt gerado com sucesso");
}

void MainWindow::openProject()
{
    QString pasta = QFileDialog::getExistingDirectory(
        this,
        "Selecionar pasta do projeto",
        QDir::homePath()
        );

    if (pasta.isEmpty())
        return;

    currentProjectPath = pasta;

    model->setRootPath(pasta);

    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(model->index(pasta));

    ui->textEdit->append("📁 Projeto aberto: " + pasta);
}

void MainWindow::CleanLogs(){
    ui->textEdit->clear();
}

void MainWindow::N_RAM(){
    bool ok;

    int ram = QInputDialog::getInt(
        this,
        "RAM do QEMU",
        "Quantidade de RAM (MB):",
        512,
        1,
        65536,
        128,
        &ok
        );
    if (!ok)return;
    RAM=ram;
}

void MainWindow::N_armazenamento(){
    bool ok;

    int disk = QInputDialog::getInt(
        this,
        "Tamanho do sistema",
        "Disco (MB):",
        512,
        10,
        65536,
        128,
        &ok
        );
    if (!ok)return;
    DISK=disk;
}

void MainWindow::N_threads(){
    bool ok;
    int th = QInputDialog::getInt(
        this,
        "Nucleos CPUs virtuais",
        "Numero de Threads:",
        12,
        1,
        65536,
        128,
        &ok
        );
    if (!ok)return;
    Threads=th;
}

void MainWindow::New_flags(){
    FlagsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted){
        flags_asm = dialog.getAsmFlags();
        flags_cpp = dialog.getCppFlags();
        flags_c  = dialog.getCFlags();
        flags_qemu = dialog.getQemuFlags();
    }
}

void MainWindow::closeProject(){
    // Remove a pasta atual
    ui->treeView->setRootIndex(QModelIndex());
    // Reseta o model para raiz do sistema
    model->setRootPath(QDir::rootPath());
    ui->treeView->setModel(nullptr);
    // Limpa logs
    ui->textEdit->append("❌ Projeto fechado");
}

void MainWindow::arquitetura()
{
    if (ui->actionARM->isChecked())
    {
        arq = ARM;
        ui->textEdit->append("Arquitetura: ARM");
    }
    else if (ui->actionRiscV_2->isChecked())
    {
        arq = RISCV;
        ui->textEdit->append("Arquitetura: RISC-V");
    }
    else if (ui->actionx86_2->isChecked())
    {
        arq = X86;
        ui->textEdit->append("Arquitetura: x86");
    }
}

void MainWindow::newx86()
{
    if (currentProjectPath.isEmpty())
    {
        ui->textEdit->append("❌ Nenhum projeto aberto");
        return;
    }

    QDir dir(currentProjectPath);

    dir.mkpath("src");

    // =========================
    // boot.asm
    // =========================

    QFile bootFile(currentProjectPath + "/src/boot.nasm");

    if (bootFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&bootFile);

        out <<
            R"(bits 32

global _start

section .text

_start:

    mov eax, 0xb8000

    mov byte [eax], 'H'
    mov byte [eax + 1], 0x0f

.loop:
    hlt
    jmp .loop
)";
    }

    // =========================
    // linker.ld
    // =========================

    QFile linker(currentProjectPath + "/linker.ld");

    if (linker.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&linker);

        out <<
            R"(ENTRY(_start)

SECTIONS
{
    . = 1M;

    .text :
    {
        *(.text)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}
)";
    }

    // =========================
    // CMakeLists.txt
    // =========================

    QFile cmake(currentProjectPath + "/CMakeLists.txt");

    if (cmake.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&cmake);

        out <<
            R"(cmake_minimum_required(VERSION 3.16)

project(x86_baremetal ASM)

enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

add_executable(kernel
    src/boot.asm
)

target_link_options(kernel PRIVATE
    -T ${CMAKE_SOURCE_DIR}/linker.ld
    -nostdlib
)
)";
    }

    ui->textEdit->append("✅ Projeto x86 criado");
}

void MainWindow::newarm()
{
    if (currentProjectPath.isEmpty())
    {
        ui->textEdit->append("❌ Nenhum projeto aberto");
        return;
    }

    QDir dir(currentProjectPath);

    dir.mkpath("src");

    // =========================
    // boot.asm
    // =========================

    QFile bootFile(currentProjectPath + "/src/boot.nasm");

    if (bootFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&bootFile);

        out <<
            R"(.global _start

.section .text

_start:
    ldr r0, =message

loop:
    b loop

message:
    .asciz "Hello ARM"
)";
    }

    // =========================
    // linker.ld
    // =========================

    QFile linker(currentProjectPath + "/linker.ld");

    if (linker.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&linker);

        out <<
            R"(ENTRY(_start)

SECTIONS
{
    . = 0x8000;

    .text :
    {
        *(.text)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}
)";
    }

    // =========================
    // CMakeLists.txt
    // =========================

    QFile cmake(currentProjectPath + "/CMakeLists.txt");

    if (cmake.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&cmake);

        out <<
            R"(cmake_minimum_required(VERSION 3.16)

project(arm_baremetal ASM)

enable_language(ASM)

add_executable(kernel
    src/boot.asm
)

target_link_options(kernel PRIVATE
    -T ${CMAKE_SOURCE_DIR}/linker.ld
    -nostdlib
)
)";
    }

    ui->textEdit->append("✅ Projeto ARM criado");
}

void MainWindow::newriscv()
{
    if (currentProjectPath.isEmpty())
    {
        ui->textEdit->append("❌ Nenhum projeto aberto");
        return;
    }

    QDir dir(currentProjectPath);

    dir.mkpath("src");

    // =========================
    // boot.asm
    // =========================

    QFile bootFile(currentProjectPath + "/src/boot.nasm");

    if (bootFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&bootFile);

        out <<
            R"(.section .text
.global _start

_start:
    li a0, 0

loop:
    j loop
)";
    }

    // =========================
    // linker.ld
    // =========================

    QFile linker(currentProjectPath + "/linker.ld");

    if (linker.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&linker);

        out <<
            R"(ENTRY(_start)

SECTIONS
{
    . = 0x80000000;

    .text :
    {
        *(.text)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}
)";
    }

    // =========================
    // CMakeLists.txt
    // =========================

    QFile cmake(currentProjectPath + "/CMakeLists.txt");

    if (cmake.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&cmake);

        out <<
            R"(cmake_minimum_required(VERSION 3.16)

project(riscv_baremetal ASM)

enable_language(ASM)

add_executable(kernel
    src/boot.asm
)

target_link_options(kernel PRIVATE
    -T ${CMAKE_SOURCE_DIR}/linker.ld
    -nostdlib
)
)";
    }

    ui->textEdit->append("✅ Projeto RISC-V criado");
}

void MainWindow::Options(){
    options[Audio]    = ui->actionAudio->isChecked();
    options[C]        = ui->actionC->isChecked();
    options[Cpp]      = ui->actionC_2->isChecked();
    options[VirRede]  = ui->actionVirtualiza_ao_em_rede->isChecked();
    options[IPV4]     = ui->actionIPV4->isChecked();
    options[IPV6]     = ui->actionIPV6->isChecked();
    options[Bluetooth]= ui->actionBluethoot->isChecked();
}

void MainWindow::definePATH(){
    QString pasta = QFileDialog::getExistingDirectory(
        this,
        "Selecionar pasta do projeto",
        QDir::homePath()
        );

    if (pasta.isEmpty())
        return;
    currentProjectPath = pasta;
}