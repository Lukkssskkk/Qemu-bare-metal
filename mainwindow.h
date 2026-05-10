#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QDir>
#include <QInputDialog>
#include <QProcess>
#include <QTextStream>
#include<QAction>
#include <QActionGroup>

#define VirRede 0
#define IPV4 1
#define IPV6 2
#define Bluetooth 3
#define C 4
#define Cpp 5
#define Audio 6

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void runProject();        // Run
    void createImage();       // Create img
    void generateCMake();     // Generate CMake
    void openProject();       // Open folder
    void closeProject();      //Close the folder of project
    void CleanLogs();         // Limpar logs
    //void Audio_Cmake();
    void arquitetura();
    void Options();
    //
    void New_flags();
    void N_armazenamento();
    void N_RAM();
    void N_threads();
    //
    void newx86();
    void newarm();
    void newriscv();
    //
    void definePATH();
private:
    bool options[7];
    enum { X86, ARM, RISCV } arq = X86;
    int RAM = 512;
    int DISK = 512;
    int Threads = 1;
    QString flags_asm;
    QString flags_cpp;
    QString flags_c;
    QString flags_qemu;
    QString caminho;
    QString currentProjectPath;
    QProcess *process = nullptr;
    Ui::MainWindow *ui;
    QFileSystemModel *model = nullptr;
};

#endif // MAINWINDOW_H