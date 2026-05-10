#include "./flagsdialog.h"

FlagsDialog::FlagsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Compiler Flags");
    resize(400, 200);

    asmEdit = new QLineEdit(this);
    cppEdit = new QLineEdit(this);
    cEdit   = new QLineEdit(this);
    qemuEdit = new QLineEdit(this);

    asmEdit->setPlaceholderText("Ex: -f elf64 -g");
    cppEdit->setPlaceholderText("Ex: -O2 -Wall -std=c++20");
    cEdit->setPlaceholderText("Ex: -O2 -Wall");
    qemuEdit->setPlaceholderText("");

    QFormLayout *formLayout = new QFormLayout;

    formLayout->addRow("ASM Flags:", asmEdit);
    formLayout->addRow("C++ Flags:", cppEdit);
    formLayout->addRow("C Flags:", cEdit);
    formLayout->addRow("Qemu Flags",qemuEdit);

    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);

    connect(okButton, &QPushButton::clicked,
            this, &QDialog::accept);

    connect(cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

QString FlagsDialog::getAsmFlags() const
{
    return asmEdit->text();
}

QString FlagsDialog::getCppFlags() const
{
    return cppEdit->text();
}

QString FlagsDialog::getCFlags() const
{
    return cEdit->text();
}

QString FlagsDialog::getQemuFlags() const{
    return qemuEdit->text();
}