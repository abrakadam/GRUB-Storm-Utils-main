#ifndef UI_QT_H
#define UI_QT_H

// заголовочные файлы qt-виджетов
#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QProgressBar>
#include <QInputDialog>
#include "grub_manager.h"
#include "system_info.h"

// главное окно приложения (qt-версия для windows)
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    GRUBManager& manager;  // ссылка на менеджер grub
    SystemInfo sys_info;   // кэшированная информация о системе

    // основной виджет вкладок
    QTabWidget* tabWidget;

    // вкладка «system info» — системная информация
    QWidget* systemInfoTab;
    QLabel* osLabel;
    QLabel* kernelLabel;
    QLabel* archLabel;
    QLabel* distroLabel;
    QLabel* cpuLabel;
    QLabel* memoryLabel;
    QLabel* userLabel;
    QLabel* modeLabel;  // метка текущего режима безопасности

    // вкладка «grub info» — информация о grub
    QWidget* grubInfoTab;
    QLabel* grubVersionLabel;
    QLabel* grubInstalledLabel;
    QLabel* defaultEntryLabel;
    QLabel* entriesCountLabel;
    QTextEdit* grubConfigText;  // содержимое grub.cfg

    // вкладка «entries» — таблица записей загрузки
    QWidget* entriesTab;
    QTableWidget* entriesTable;
    QPushButton* refreshEntriesBtn;
    QPushButton* viewDetailsBtn;
    QPushButton* setDefaultBtn;
    QPushButton* addEntryBtn;
    QPushButton* removeEntryBtn;

    // вкладка «terminal» — встроенный терминал
    QWidget* terminalTab;
    QTextEdit* terminalOutput;
    QLineEdit* terminalInput;
    QPushButton* executeBtn;
    QPushButton* clearBtn;

    // вкладка «operations» — операции с grub
    QWidget* operationsTab;
    QComboBox* defaultEntryCombo;
    QPushButton* setDefaultBtn2;
    QLineEdit* newEntryTitle;
    QLineEdit* newEntryKernel;
    QLineEdit* newEntryInitrd;
    QPushButton* addNewEntryBtn;
    QSpinBox* timeoutSpinBox;
    QPushButton* setTimeoutBtn;
    QPushButton* updateGrubBtn;

    // методы построения интерфейса
    void setupUI();
    void setupSystemInfoTab();
    void setupGrubInfoTab();
    void setupEntriesTab();
    void setupTerminalTab();
    void setupOperationsTab();

    // методы обновления данных в виджетах
    void updateSystemInfo();
    void updateGrubInfo();
    void updateEntriesTable();
    void refreshAll();  // обновить все вкладки сразу

private slots:
    // обработчики событий (qt-слоты)
    void onRefreshEntries();
    void onViewDetails();
    void onSetDefault();
    void onAddEntry();
    void onRemoveEntry();
    void onExecuteCommand();
    void onClearTerminal();
    void onSetDefault2();
    void onAddNewEntry();
    void onSetTimeout();
    void onUpdateGrub();
    void onToggleMode();

public:
    MainWindow(GRUBManager& mgr, QWidget* parent = nullptr);
    ~MainWindow();
};

#endif // UI_QT_H
