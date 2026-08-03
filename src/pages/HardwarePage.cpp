#include "HardwarePage.h"
#include "../backend/I18n.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QGroupBox>
#include <QRegularExpression>

// nvidia-open only supports Turing and newer. Anything older needs the
// proprietary DKMS driver, so guess from the marketing name lspci reports.
static bool looksLikeTuringOrNewer(const QString &pciLower) {
    static const QRegularExpression modern(
        R"((rtx\s*[2-9]\d{3})|(gtx\s*16\d{2})|(tu\d{3})|(ga\d{3})|(ad\d{3})|(gb\d{3}))");
    return modern.match(pciLower).hasMatch();
}

HardwarePage::HardwarePage(InstallConfig *cfg, QWidget *parent) : BasePage(cfg, parent) {
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(52, 40, 52, 20);
    lay->setSpacing(16);

    auto *tt = new QLabel(I18n::tr("hardware_title")); tt->setObjectName("pageTitle");
    auto *ts = new QLabel(I18n::tr("hardware_subtitle"));
    ts->setObjectName("pageSubtitle");
    lay->addWidget(tt); lay->addWidget(ts);
    lay->addSpacing(4);

    m_detected = new QLabel(I18n::language() == "tr" ? "Donanım algılanıyor..." : "Autodetecting hardware...");
    m_detected->setObjectName("card");
    m_detected->setStyleSheet("padding: 10px 14px; font-size: 12px; color: #1d1d1f;");
    m_detected->setWordWrap(true);
    lay->addWidget(m_detected);

    // ── GPU ───────────────────────────────────────────────────────────────────
    auto *gr = new QHBoxLayout(); gr->setSpacing(14);
    auto *gl = new QLabel(I18n::tr("gpu_driver"));
    gl->setFixedWidth(160); gl->setObjectName("fieldLabel");
    m_gpuBox = new QComboBox();
    m_gpuBox->addItem("AMD  (AMDGPU — RDNA & GCN)",             "amd");
    m_gpuBox->addItem("NVIDIA  Turing+ 16xx+  (open-source)",   "nvidia-open");
    m_gpuBox->addItem("NVIDIA  Legacy  (proprietary DKMS)",     "nvidia-legacy");
    m_gpuBox->addItem("Intel  (mesa + vulkan-intel)",           "intel");
    m_gpuBox->addItem("None / Virtual Machine",                 "none");
    gr->addWidget(gl); gr->addWidget(m_gpuBox, 1);
    lay->addLayout(gr);

    // ── CPU ───────────────────────────────────────────────────────────────────
    auto *cr = new QHBoxLayout(); cr->setSpacing(14);
    auto *cl = new QLabel(I18n::tr("cpu_ucode"));
    cl->setFixedWidth(160); cl->setObjectName("fieldLabel");
    m_cpuBox = new QComboBox();
    m_cpuBox->addItem("AMD  (amd-ucode)",   "amd");
    m_cpuBox->addItem("Intel  (intel-ucode)", "intel");
    m_cpuBox->addItem("Virtual / No ucode", "none");
    cr->addWidget(cl); cr->addWidget(m_cpuBox, 1);
    lay->addLayout(cr);

    // ── Razer ─────────────────────────────────────────────────────────────────
    m_razerBox = new QCheckBox(I18n::tr("razer_support"));
    lay->addWidget(m_razerBox);

    // Info card
    auto *info = new QLabel(
        "ℹ  NVIDIA: Turing+ (RTX 20 / GTX 1600+) → choose open-source. Older cards → Legacy.\n"
        "   AMD: AMDGPU covers all GCN/RDNA cards. iGPUs included.\n"
        "   Intel: mesa + vulkan-intel for all modern Intel graphics."
    );
    info->setWordWrap(true);
    info->setObjectName("hint");
    info->setStyleSheet("padding: 10px 14px; margin-top: 6px;");
    lay->addWidget(info);
    lay->addStretch();
}

void HardwarePage::onShow() {
    // Detect once: coming back to this page should not undo a manual choice.
    if (m_detectDone) return;
    m_detectDone = true;
    autodetect();
}

void HardwarePage::autodetect() {
    QProcess p;
    p.start("lspci", {"-nn"});
    p.waitForFinished(4000);
    const QString pci = QString::fromUtf8(p.readAllStandardOutput()).toLower();

    // GPU Detection
    if (pci.contains("nvidia")) {
        m_gpuBox->setCurrentIndex(looksLikeTuringOrNewer(pci) ? 1 : 2);
    } else if (pci.contains("amd") || pci.contains("radeon")) {
        m_gpuBox->setCurrentIndex(0);
    } else if (pci.contains("intel")) {
        m_gpuBox->setCurrentIndex(3);
    } else {
        m_gpuBox->setCurrentIndex(4); // None / VM
    }

    // CPU Detection
    QProcess c;
    c.start("grep", {"-m1", "-i", "model name", "/proc/cpuinfo"});
    c.waitForFinished(2000);
    const QString cpu = QString::fromUtf8(c.readAllStandardOutput()).toLower();
    if (cpu.contains("intel")) {
        m_cpuBox->setCurrentIndex(1);
    } else if (cpu.contains("amd")) {
        m_cpuBox->setCurrentIndex(0);
    } else {
        m_cpuBox->setCurrentIndex(2); // Virtual / No ucode
    }

    // Razer Peripheral Detection (USB Vendor ID 1532)
    QProcess r;
    r.start("lsusb", {});
    r.waitForFinished(2000);
    const QString usb = QString::fromUtf8(r.readAllStandardOutput()).toLower();
    bool hasRazer = usb.contains("1532:") || usb.contains("razer");
    m_razerBox->setChecked(hasRazer);

    QString gpuStr = pci.contains("nvidia") ? "NVIDIA" : pci.contains("amd") || pci.contains("radeon") ? "AMD / Radeon" : pci.contains("intel") ? "Intel" : "Generic/VM";
    QString cpuStr = cpu.contains("intel") ? "Intel" : cpu.contains("amd") ? "AMD" : "Generic/VM";
    QString razerStr = hasRazer ? (I18n::language() == "tr" ? "Evet (Tespit Edildi)" : "Yes (Detected)") : (I18n::language() == "tr" ? "Bulunamadı" : "Not Found");

    m_detected->setText(
        QString(I18n::language() == "tr" ? "Tespit Edilen GPU: %1\nTespit Edilen CPU: %2\nRazer Çevre Birimi: %3"
                                         : "Detected GPU: %1\nDetected CPU: %2\nRazer Peripherals: %3")
            .arg(gpuStr, cpuStr, razerStr));
}

void HardwarePage::commitToConfig() {
    m_cfg->gpuDriver = m_gpuBox->currentData().toString();
    m_cfg->cpuType   = m_cpuBox->currentData().toString();
    m_cfg->razer     = m_razerBox->isChecked();
}
