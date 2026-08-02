#pragma once
#include <QString>
#include <QMap>

class I18n {
public:
    static void setLanguage(const QString &lang) { s_lang = lang; }
    static QString language() { return s_lang; }

    static QString tr(const QString &key) {
        if (s_lang == "tr" && s_trMap.contains(key)) {
            return s_trMap[key];
        }
        if (s_enMap.contains(key)) {
            return s_enMap[key];
        }
        return key;
    }

private:
    static inline QString s_lang = "en";
    static inline QMap<QString, QString> s_enMap = {
        {"welcome_title", "Welcome to GötünOS"},
        {"welcome_subtitle", "A gaming-first Arch Linux distribution. Let's get you set up."},
        {"lang_label", "Installer Language:"},
        {"tos_header", "Terms of Service"},
        {"tos_agree", "✓  By clicking Continue you accept the Terms of Service above."},
        {"network_title", "Network Connection"},
        {"network_subtitle", "Connect to the internet, then choose a DNS provider"},
        {"scan_networks", "⟳  Scan Networks"},
        {"wifi_password", "Wi-Fi Password:"},
        {"connect", "Connect"},
        {"dns_provider", "Encrypted DNS (DoT):"},
        {"locale_title", "Locale & Regional Settings"},
        {"locale_subtitle", "Set system language, timezone and keyboard layout"},
        {"system_locale", "System Locale:"},
        {"timezone", "Timezone:"},
        {"keyboard_layout", "Keyboard Layout:"},
        {"test_keyboard", "Test Keyboard:"},
        {"users_title", "User Accounts"},
        {"users_subtitle", "Create your user account and set the root password"},
        {"users_section", "USERS"},
        {"root_section", "ROOT ACCOUNT"},
        {"add_user", "+ Add User"},
        {"root_pass", "Root Password:"},
        {"same_root_pass", "Use the same password for the root account"},
        {"nopass_sudo", "Passwordless sudo for wheel group (gaming convenience)"},
        {"hardware_title", "Hardware & Drivers"},
        {"hardware_subtitle", "Select GPU driver, CPU, and optional Razer support"},
        {"gpu_driver", "GPU Driver:"},
        {"cpu_ucode", "CPU / Microcode:"},
        {"razer_support", "Enable Razer peripheral support  (OpenRazer + Polychromatic)"},
        {"disk_title", "Disk Partitioning"},
        {"disk_subtitle", "Choose target disk, partition mode, filesystem and bootloader"},
        {"disks_header", "AVAILABLE DISKS"},
        {"mode_header", "PARTITION MODE"},
        {"filesystem", "Filesystem:"},
        {"bootloader", "Bootloader:"},
        {"sec_drives_title", "Secondary Drives"},
        {"sec_drives_subtitle", "Mount additional drives and optionally create symlinks (e.g. ~/Games → /mnt/games).\nAll mounted drives will have Steam-compatible permissions."},
        {"opt_title", "System Options"},
        {"opt_subtitle", "Hostname, ZRAM and optional swap"},
        {"hostname", "PC Hostname:"},
        {"domain", "Domain:"},
        {"summary_title", "Installation Summary"},
        {"summary_subtitle", "Review every setting before installation begins. Click Install Now to proceed."},
        {"install_now", "Install Now"},
        {"continue", "Continue →"},
        {"back", "← Back"},
        {"reboot_now", "⟳  Reboot Now"},
        {"exit_shell", "→  Exit to Shell"},
        {"finish_title", "GötünOS is Installed!"},
        {"finish_sub", "Your system is ready. Remove the installation media and reboot to start GötünOS.\nOn first boot, Decky Loader will be installed automatically via the firstboot service."}
    };

    static inline QMap<QString, QString> s_trMap = {
        {"welcome_title", "GötünOS'a Hoş Geldiniz"},
        {"welcome_subtitle", "Oyun odaklı Arch Linux dağıtımı. Kuruluma başlayalım."},
        {"lang_label", "Kurulum Dili:"},
        {"tos_header", "Kullanım Şartları"},
        {"tos_agree", "✓  Devam Et'e tıklayarak yukarıdaki Kullanım Şartlarını kabul etmiş olursunuz."},
        {"network_title", "Ağ Bağlantısı"},
        {"network_subtitle", "İnternete bağlanın ve şifreli bir DNS sağlayıcısı seçin"},
        {"scan_networks", "⟳  Ağları Tara"},
        {"wifi_password", "Wi-Fi Şifresi:"},
        {"connect", "Bağlan"},
        {"dns_provider", "Şifreli DNS (DoT):"},
        {"locale_title", "Yerel ve Bölgesel Ayarlar"},
        {"locale_subtitle", "Sistem dilini, zaman dilimini ve klavye düzenini belirleyin"},
        {"system_locale", "Sistem Dili:"},
        {"timezone", "Zaman Dilimi:"},
        {"keyboard_layout", "Klavye Düzeni:"},
        {"test_keyboard", "Klavyeyi Test Et:"},
        {"users_title", "Kullanıcı Hesapları"},
        {"users_subtitle", "Kullanıcı hesabınızı oluşturun ve root şifresini belirleyin"},
        {"users_section", "KULLANICILAR"},
        {"root_section", "ROOT HESABI"},
        {"add_user", "+ Kullanıcı Ekle"},
        {"root_pass", "Root Şifresi:"},
        {"same_root_pass", "Root hesabı için aynı şifreyi kullan"},
        {"nopass_sudo", "wheel grubu için şifresiz sudo (oyun kolaylığı için)"},
        {"hardware_title", "Donanım ve Sürücüler"},
        {"hardware_subtitle", "GPU sürücüsünü, CPU'yu ve isteğe bağlı Razer desteğini seçin"},
        {"gpu_driver", "GPU Sürücüsü:"},
        {"cpu_ucode", "CPU / Mikrokod:"},
        {"razer_support", "Razer çevre birimi desteğini etkinleştir (OpenRazer + Polychromatic)"},
        {"disk_title", "Disk Bölümleme"},
        {"disk_subtitle", "Hedef diski, bölümleme modunu, dosya sistemini ve ön yükleyiciyi seçin"},
        {"disks_header", "MEVCUT DİSKLER"},
        {"mode_header", "BÖLÜMLEME MODU"},
        {"filesystem", "Dosya Sistemi:"},
        {"bootloader", "Ön Yükleyici:"},
        {"sec_drives_title", "İkincil Sürücüler"},
        {"sec_drives_subtitle", "Ek sürücüleri bağlayın ve isteğe bağlı sembolik bağlantılar oluşturun (ör. ~/Games → /mnt/games).\nTüm sürücüler Steam uyumlu izinlerle bağlanır."},
        {"opt_title", "Sistem Seçenekleri"},
        {"opt_subtitle", "Bilgisayar adı, ZRAM ve isteğe bağlı takas alanı"},
        {"hostname", "Bilgisayar Adı:"},
        {"domain", "Alan Adı:"},
        {"summary_title", "Kurulum Özeti"},
        {"summary_subtitle", "Kurulum başlamadan önce tüm ayarları gözden geçirin. Devam etmek için Şimdi Kur'a tıklayın."},
        {"install_now", "Şimdi Kur"},
        {"continue", "Devam Et →"},
        {"back", "← Geri"},
        {"reboot_now", "⟳  Yeniden Başlat"},
        {"exit_shell", "→  Kabuğa Çık"},
        {"finish_title", "GötünOS Kuruldu!"},
        {"finish_sub", "Sisteminiz hazır. Kurulum medyasını çıkarın ve GötünOS'u başlatmak için yeniden başlatın.\nİlk açılışta Decky Loader otomatik olarak kurulacaktır."}
    };
};
