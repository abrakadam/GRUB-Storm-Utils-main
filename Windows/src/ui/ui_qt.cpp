#include "ui_qt.h"
#include <QHeaderView>
#include <QProcess>
#include <QTimer>
#include <QInputDialog>

MainWindow::MainWindow(GRUBManager& mgr, QWidget* parent) 
    : QMainWindow(parent), manager(mgr) {
    sys_info = SystemInfoCollector::collect();
    setupUI();
    refreshAll();
}

MainWindow::~MainWindow() {
    // Qt will handle child widget cleanup automatically
}

void MainWindow::setupUI() {
    setWindowTitle("GRUB Storm Utils - Advanced GRUB Manager");
    setMinimumSize(900, 700);
    
    // Create central widget and main layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Header with mode toggle
    QHBoxLayout* headerLayout = new QHBoxLayout();
    modeLabel = new QLabel("Mode: SAFE");
    modeLabel->setStyleSheet("QLabel { font-weight: bold; color: green; padding: 5px; }");
    QPushButton* toggleModeBtn = new QPushButton("Toggle Mode");
    connect(toggleModeBtn, &QPushButton::clicked, this, &MainWindow::onToggleMode);
    
    headerLayout->addWidget(modeLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(toggleModeBtn);
    
    // Create tab widget
    tabWidget = new QTabWidget(this);
    
    setupSystemInfoTab();
    setupGrubInfoTab();
    setupEntriesTab();
    setupTerminalTab();
    setupOperationsTab();
    
    tabWidget->addTab(systemInfoTab, "System Info");
    tabWidget->addTab(grubInfoTab, "GRUB Info");
    tabWidget->addTab(entriesTab, "Entries");
    tabWidget->addTab(terminalTab, "Terminal");
    tabWidget->addTab(operationsTab, "Operations");
    
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(tabWidget);
    
    setCentralWidget(centralWidget);
}

void MainWindow::setupSystemInfoTab() {
    systemInfoTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(systemInfoTab);
    
    QGroupBox* osGroup = new QGroupBox("Operating System");
    QFormLayout* osLayout = new QFormLayout();
    osLabel = new QLabel();
    kernelLabel = new QLabel();
    archLabel = new QLabel();
    distroLabel = new QLabel();
    osLayout->addRow("OS:", osLabel);
    osLayout->addRow("Kernel:", kernelLabel);
    osLayout->addRow("Architecture:", archLabel);
    osLayout->addRow("Distribution:", distroLabel);
    osGroup->setLayout(osLayout);
    
    QGroupBox* hwGroup = new QGroupBox("Hardware");
    QFormLayout* hwLayout = new QFormLayout();
    cpuLabel = new QLabel();
    memoryLabel = new QLabel();
    hwLayout->addRow("CPU:", cpuLabel);
    hwLayout->addRow("Memory:", memoryLabel);
    hwGroup->setLayout(hwLayout);
    
    QGroupBox* userGroup = new QGroupBox("User Info");
    QFormLayout* userLayout = new QFormLayout();
    userLabel = new QLabel();
    userLayout->addRow("User:", userLabel);
    userGroup->setLayout(userLayout);
    
    layout->addWidget(osGroup);
    layout->addWidget(hwGroup);
    layout->addWidget(userGroup);
    layout->addStretch();
}

void MainWindow::setupGrubInfoTab() {
    grubInfoTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(grubInfoTab);
    
    QGroupBox* infoGroup = new QGroupBox("GRUB Information");
    QFormLayout* infoLayout = new QFormLayout();
    grubVersionLabel = new QLabel();
    grubInstalledLabel = new QLabel();
    defaultEntryLabel = new QLabel();
    entriesCountLabel = new QLabel();
    infoLayout->addRow("Version:", grubVersionLabel);
    infoLayout->addRow("Installed:", grubInstalledLabel);
    infoLayout->addRow("Default Entry:", defaultEntryLabel);
    infoLayout->addRow("Total Entries:", entriesCountLabel);
    infoGroup->setLayout(infoLayout);
    
    QGroupBox* configGroup = new QGroupBox("GRUB Configuration");
    QVBoxLayout* configLayout = new QVBoxLayout();
    grubConfigText = new QTextEdit();
    grubConfigText->setReadOnly(true);
    grubConfigText->setFont(QFont("Courier", 9));
    configLayout->addWidget(grubConfigText);
    configGroup->setLayout(configLayout);
    
    layout->addWidget(infoGroup);
    layout->addWidget(configGroup);
}

void MainWindow::setupEntriesTab() {
    entriesTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(entriesTab);
    
    entriesTable = new QTableWidget();
    entriesTable->setColumnCount(4);
    entriesTable->setHorizontalHeaderLabels({"Title", "UUID", "Kernel", "Default"});
    entriesTable->horizontalHeader()->setStretchLastSection(true);
    entriesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    entriesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    refreshEntriesBtn = new QPushButton("Refresh");
    viewDetailsBtn = new QPushButton("View Details");
    setDefaultBtn = new QPushButton("Set Default");
    addEntryBtn = new QPushButton("Add Entry");
    removeEntryBtn = new QPushButton("Remove Entry");
    
    connect(refreshEntriesBtn, &QPushButton::clicked, this, &MainWindow::onRefreshEntries);
    connect(viewDetailsBtn, &QPushButton::clicked, this, &MainWindow::onViewDetails);
    connect(setDefaultBtn, &QPushButton::clicked, this, &MainWindow::onSetDefault);
    connect(addEntryBtn, &QPushButton::clicked, this, &MainWindow::onAddEntry);
    connect(removeEntryBtn, &QPushButton::clicked, this, &MainWindow::onRemoveEntry);
    
    buttonLayout->addWidget(refreshEntriesBtn);
    buttonLayout->addWidget(viewDetailsBtn);
    buttonLayout->addWidget(setDefaultBtn);
    buttonLayout->addWidget(addEntryBtn);
    buttonLayout->addWidget(removeEntryBtn);
    
    layout->addWidget(entriesTable);
    layout->addLayout(buttonLayout);
}

void MainWindow::setupTerminalTab() {
    terminalTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(terminalTab);
    
    terminalOutput = new QTextEdit();
    terminalOutput->setReadOnly(true);
    terminalOutput->setFont(QFont("Courier", 9));
    terminalOutput->setStyleSheet("QTextEdit { background-color: #1e1e1e; color: #d4d4d4; }");
    
    QHBoxLayout* inputLayout = new QHBoxLayout();
    terminalInput = new QLineEdit();
    terminalInput->setPlaceholderText("Enter command...");
    executeBtn = new QPushButton("Execute");
    clearBtn = new QPushButton("Clear");
    
    connect(executeBtn, &QPushButton::clicked, this, &MainWindow::onExecuteCommand);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClearTerminal);
    connect(terminalInput, &QLineEdit::returnPressed, this, &MainWindow::onExecuteCommand);
    
    inputLayout->addWidget(terminalInput);
    inputLayout->addWidget(executeBtn);
    inputLayout->addWidget(clearBtn);
    
    layout->addWidget(terminalOutput);
    layout->addLayout(inputLayout);
    
    // Welcome message
    terminalOutput->append("=== GRUB Storm Utils Terminal ===");
    terminalOutput->append("Available commands: help, clear, sysinfo, grub, exit");
    terminalOutput->append("Or any shell command\n");
}

void MainWindow::setupOperationsTab() {
    operationsTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(operationsTab);
    
    QGroupBox* defaultGroup = new QGroupBox("Set Default Entry");
    QHBoxLayout* defaultLayout = new QHBoxLayout();
    defaultEntryCombo = new QComboBox();
    setDefaultBtn2 = new QPushButton("Set Default");
    connect(setDefaultBtn2, &QPushButton::clicked, this, &MainWindow::onSetDefault2);
    defaultLayout->addWidget(defaultEntryCombo);
    defaultLayout->addWidget(setDefaultBtn2);
    defaultGroup->setLayout(defaultLayout);
    
    QGroupBox* addGroup = new QGroupBox("Add New Entry");
    QFormLayout* addLayout = new QFormLayout();
    newEntryTitle = new QLineEdit();
    newEntryKernel = new QLineEdit();
    newEntryInitrd = new QLineEdit();
    addNewEntryBtn = new QPushButton("Add Entry");
    connect(addNewEntryBtn, &QPushButton::clicked, this, &MainWindow::onAddNewEntry);
    addLayout->addRow("Title:", newEntryTitle);
    addLayout->addRow("Kernel:", newEntryKernel);
    addLayout->addRow("Initrd:", newEntryInitrd);
    addLayout->addWidget(addNewEntryBtn);
    addGroup->setLayout(addLayout);
    
    QGroupBox* timeoutGroup = new QGroupBox("Boot Timeout");
    QHBoxLayout* timeoutLayout = new QHBoxLayout();
    timeoutSpinBox = new QSpinBox();
    timeoutSpinBox->setRange(0, 300);
    timeoutSpinBox->setValue(5);
    timeoutSpinBox->setSuffix(" seconds");
    setTimeoutBtn = new QPushButton("Set Timeout");
    connect(setTimeoutBtn, &QPushButton::clicked, this, &MainWindow::onSetTimeout);
    timeoutLayout->addWidget(timeoutSpinBox);
    timeoutLayout->addWidget(setTimeoutBtn);
    timeoutGroup->setLayout(timeoutLayout);
    
    updateGrubBtn = new QPushButton("Update GRUB (update-grub)");
    updateGrubBtn->setStyleSheet("QPushButton { background-color: #ff6b6b; color: white; padding: 10px; font-weight: bold; }");
    connect(updateGrubBtn, &QPushButton::clicked, this, &MainWindow::onUpdateGrub);
    
    layout->addWidget(defaultGroup);
    layout->addWidget(addGroup);
    layout->addWidget(timeoutGroup);
    layout->addWidget(updateGrubBtn);
    layout->addStretch();
    
    // Disable operations in safe mode
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        defaultGroup->setEnabled(false);
        addGroup->setEnabled(false);
        timeoutGroup->setEnabled(false);
        updateGrubBtn->setEnabled(false);
    }
}

void MainWindow::updateSystemInfo() {
    osLabel->setText(QString::fromStdString(sys_info.os_name + " " + sys_info.os_version));
    kernelLabel->setText(QString::fromStdString(sys_info.kernel_version));
    archLabel->setText(QString::fromStdString(sys_info.architecture + (sys_info.is_64bit ? " (64-bit)" : " (32-bit)")));
    distroLabel->setText(QString::fromStdString(sys_info.distribution));
    cpuLabel->setText(QString::fromStdString(sys_info.cpu_model));
    memoryLabel->setText(QString::fromStdString(SystemInfoCollector::format_bytes(sys_info.total_memory) + " total, " + 
                       SystemInfoCollector::format_bytes(sys_info.available_memory) + " available"));
    userLabel->setText(QString::fromStdString(sys_info.username + "@" + sys_info.hostname));
}

void MainWindow::updateGrubInfo() {
    grubVersionLabel->setText(QString::fromStdString(manager.get_grub_version()));
    grubInstalledLabel->setText(manager.check_grub_installed() ? "Yes" : "No");
    defaultEntryLabel->setText(QString::fromStdString(manager.get_default_entry()));
    
    auto entries = manager.get_entries();
    entriesCountLabel->setText(QString::number(entries.size()));
    
    grubConfigText->setText(QString::fromStdString(manager.get_config_content()));
}

void MainWindow::updateEntriesTable() {
    auto entries = manager.get_entries();
    entriesTable->setRowCount(entries.size());
    
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        entriesTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(entry.title)));
        entriesTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(entry.uuid)));
        entriesTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(entry.kernel)));
        entriesTable->setItem(i, 3, new QTableWidgetItem(entry.is_default ? "Yes" : "No"));
    }
    
    entriesTable->resizeColumnsToContents();
    
    // Update combo box
    defaultEntryCombo->clear();
    for (const auto& entry : entries) {
        defaultEntryCombo->addItem(QString::fromStdString(entry.title));
    }
}

void MainWindow::refreshAll() {
    updateSystemInfo();
    updateGrubInfo();
    updateEntriesTable();
    
    // Update mode label
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        modeLabel->setText("Mode: SAFE");
        modeLabel->setStyleSheet("QLabel { font-weight: bold; color: green; padding: 5px; }");
        operationsTab->setEnabled(false);
    } else {
        modeLabel->setText("Mode: DANGEROUS");
        modeLabel->setStyleSheet("QLabel { font-weight: bold; color: red; padding: 5px; }");
        operationsTab->setEnabled(true);
    }
}

void MainWindow::onRefreshEntries() {
    updateEntriesTable();
}

void MainWindow::onViewDetails() {
    int row = entriesTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Warning", "Please select an entry first.");
        return;
    }
    
    auto entries = manager.get_entries();
    if (row >= (int)entries.size()) return;
    
    const auto& entry = entries[row];
    
    QString details = QString(
        "=== Entry Details ===\n\n"
        "Title: %1\n"
        "UUID: %2\n"
        "Kernel: %3\n"
        "Initrd: %4\n"
        "Is Default: %5\n\n"
        "=== Analysis ===\n"
    ).arg(QString::fromStdString(entry.title))
     .arg(QString::fromStdString(entry.uuid.empty() ? "N/A" : entry.uuid))
     .arg(QString::fromStdString(entry.kernel.empty() ? "N/A" : entry.kernel))
     .arg(QString::fromStdString(entry.initrd.empty() ? "N/A" : entry.initrd))
     .arg(entry.is_default ? "Yes" : "No");
    
    if (!entry.kernel.empty()) {
        details += "Kernel specified: YES\n";
        if (entry.kernel.find("vmlinuz") != std::string::npos) {
            details += "Kernel type: Linux kernel\n";
        }
    } else {
        details += "Kernel specified: NO\n";
    }
    
    if (!entry.initrd.empty()) {
        details += "Initrd specified: YES\n";
    } else {
        details += "Initrd specified: NO\n";
    }
    
    QMessageBox::information(this, "Entry Details", details);
}

void MainWindow::onSetDefault() {
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        QMessageBox::warning(this, "Warning", "Switch to DANGEROUS mode first.");
        return;
    }
    
    int row = entriesTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Warning", "Please select an entry first.");
        return;
    }
    
    auto entries = manager.get_entries();
    if (row >= (int)entries.size()) return;
    
    if (manager.set_default_entry(entries[row].title)) {
        QMessageBox::information(this, "Success", "Default entry set successfully!");
        refreshAll();
    } else {
        QMessageBox::critical(this, "Error", "Failed to set default entry.");
    }
}

void MainWindow::onAddEntry() {
    QMessageBox::information(this, "Info", "Use the Operations tab to add a new entry.");
    tabWidget->setCurrentWidget(operationsTab);
}

void MainWindow::onRemoveEntry() {
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        QMessageBox::warning(this, "Warning", "Switch to DANGEROUS mode first.");
        return;
    }
    
    int row = entriesTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Warning", "Please select an entry first.");
        return;
    }
    
    auto entries = manager.get_entries();
    if (row >= (int)entries.size()) return;
    
    auto reply = QMessageBox::question(this, "Confirm", 
        "Are you sure you want to remove this entry?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (manager.remove_entry(entries[row].title)) {
            QMessageBox::information(this, "Success", "Entry removed successfully!");
            refreshAll();
        } else {
            QMessageBox::critical(this, "Error", "Failed to remove entry.");
        }
    }
}

void MainWindow::onExecuteCommand() {
    QString cmd = terminalInput->text();
    if (cmd.isEmpty()) return;
    
    terminalOutput->append("$ " + cmd);
    
    if (cmd == "help") {
        terminalOutput->append("Available commands:");
        terminalOutput->append("  help - Show this help");
        terminalOutput->append("  clear - Clear terminal");
        terminalOutput->append("  sysinfo - Show system info");
        terminalOutput->append("  grub - Show GRUB info");
        terminalOutput->append("  exit - Exit program");
    } else if (cmd == "clear") {
        terminalOutput->clear();
    } else if (cmd == "sysinfo") {
        tabWidget->setCurrentWidget(systemInfoTab);
    } else if (cmd == "grub") {
        tabWidget->setCurrentWidget(grubInfoTab);
    } else if (cmd == "exit") {
        close();
    } else {
        QProcess process;
        process.start("bash", QStringList() << "-c" << cmd);
        process.waitForFinished();
        QString output = process.readAllStandardOutput();
        QString error = process.readAllStandardError();
        
        if (!output.isEmpty()) {
            terminalOutput->append(output);
        }
        if (!error.isEmpty()) {
            terminalOutput->append("Error: " + error);
        }
    }
    
    terminalInput->clear();
}

void MainWindow::onClearTerminal() {
    terminalOutput->clear();
}

void MainWindow::onSetDefault2() {
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        QMessageBox::warning(this, "Warning", "Switch to DANGEROUS mode first.");
        return;
    }
    
    QString entry = defaultEntryCombo->currentText();
    if (manager.set_default_entry(entry.toStdString())) {
        QMessageBox::information(this, "Success", "Default entry set successfully!");
        refreshAll();
    } else {
        QMessageBox::critical(this, "Error", "Failed to set default entry.");
    }
}

void MainWindow::onAddNewEntry() {
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        QMessageBox::warning(this, "Warning", "Switch to DANGEROUS mode first.");
        return;
    }
    
    GRUBEntry entry;
    entry.title = newEntryTitle->text().toStdString();
    entry.kernel = newEntryKernel->text().toStdString();
    entry.initrd = newEntryInitrd->text().toStdString();
    
    if (entry.title.empty()) {
        QMessageBox::warning(this, "Warning", "Please enter a title.");
        return;
    }
    
    if (manager.add_entry(entry)) {
        QMessageBox::information(this, "Success", "Entry added successfully!");
        newEntryTitle->clear();
        newEntryKernel->clear();
        newEntryInitrd->clear();
        refreshAll();
    } else {
        QMessageBox::critical(this, "Error", "Failed to add entry.");
    }
}

void MainWindow::onSetTimeout() {
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        QMessageBox::warning(this, "Warning", "Switch to DANGEROUS mode first.");
        return;
    }
    
    int timeout = timeoutSpinBox->value();
    if (manager.update_timeout(timeout)) {
        QMessageBox::information(this, "Success", "Timeout set successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to set timeout.");
    }
}

void MainWindow::onUpdateGrub() {
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        QMessageBox::warning(this, "Warning", "Switch to DANGEROUS mode first.");
        return;
    }
    
    auto reply = QMessageBox::question(this, "Confirm", 
        "This will run update-grub. Are you sure?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        QMessageBox::information(this, "Info", "Running update-grub...\nThis may take a moment.");
        
        if (manager.update_grub()) {
            QMessageBox::information(this, "Success", "GRUB updated successfully!");
            refreshAll();
        } else {
            QMessageBox::critical(this, "Error", "Failed to update GRUB.");
        }
    }
}

void MainWindow::onToggleMode() {
    if (manager.get_safety_mode() == SafetyMode::SAFE) {
        auto reply = QMessageBox::question(this, "Confirm",
            "You are about to switch to DANGEROUS mode.\n"
            "This allows modifications to GRUB configuration.\n"
            "Incorrect changes may make your system unbootable!\n\n"
            "Type 'DANGEROUS' to confirm.",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            bool ok;
            QString text = QInputDialog::getText(this, "Confirm",
                "Type 'DANGEROUS' to confirm:", QLineEdit::Normal, "", &ok);
            
            if (ok && text == "DANGEROUS") {
                manager.set_safety_mode(SafetyMode::DANGEROUS);
                QMessageBox::information(this, "Success", "Switched to DANGEROUS mode.");
                refreshAll();
            }
        }
    } else {
        manager.set_safety_mode(SafetyMode::SAFE);
        QMessageBox::information(this, "Success", "Switched to SAFE mode.");
        refreshAll();
    }
}
