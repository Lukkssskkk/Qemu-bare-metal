#ifndef FLAGSDIALOG_H
#define FLAGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>

class FlagsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FlagsDialog(QWidget *parent = nullptr);

    QString getAsmFlags() const;
    QString getCppFlags() const;
    QString getCFlags() const;
    QString getQemuFlags() const;
private:
    QLineEdit *qemuEdit;
    QLineEdit *asmEdit;
    QLineEdit *cppEdit;
    QLineEdit *cEdit;

    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // FLAGSDIALOG_H