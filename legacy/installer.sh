#!/usr/bin/env bash



if [ "$EUID" -ne 0 ]; then
    echo "Please run as root."
    exit 1
fi

MSG_WELCOME_en="Welcome to GötünOS! Please select your language."
MSG_WELCOME_tr="GötünOS'a Hoş Geldiniz! Lütfen dilinizi seçin."
MSG_TOS_TITLE_en="GötünOS Covenant (Terms of Service)"
MSG_TOS_TITLE_tr="GötünOS Sözleşmesi (Kullanım Şartları)"
MSG_TOS_TEXT_en="GötünOS is provided as-is without any warranties of any kind, express or implied. By proceeding with the installation of this operating system, you acknowledge and agree that:\n1. The installation process may format, erase, or modify partitions on your storage devices. You are solely responsible for backing up any critical data before proceeding.\n2. The developers and contributors of GötünOS shall not be held liable for any data loss, hardware damage, system instability, or other issues resulting from the installation or usage of this software.\n3. You are responsible for ensuring your hardware meets compatibility requirements and that you comply with any third-party licenses for software installed during this process.\n\nPlease select Agree to accept these terms and continue, or Disagree to cancel the installation."
MSG_TOS_TEXT_tr="GötünOS, açık veya zımni hiçbir garanti verilmeksizin 'olduğu gibi' sunulmaktadır. Bu işletim sisteminin kurulumuna devam ederek aşağıdaki şartları kabul etmiş olursunuz:\n1. Kurulum işlemi depolama cihazlarınızdaki bölümleri biçimlendirebilir, silebilir veya değiştirebilir. Devam etmeden önce kritik verilerinizi yedeklemek tamamen sizin sorumluluğunuzdadır.\n2. GötünOS geliştiricileri ve katkıda bulunanları, bu yazılımın kurulumundan veya kullanılmasından kaynaklanan veri kaybı, donanım hasarı, sistem kararsızlığı veya diğer sorunlardan sorumlu tutulamaz.\n3. Donanımınızın uyumluluk gereksinimlerini karşılamasını sağlamaktan ve bu işlem sırasında yüklenen yazılımlar için üçüncü taraf lisanslarına uymaktan siz sorumlusunuz.\n\nDevam etmek için Kabul Ediyorum'u, kurulumu iptal etmek için Kabul Etmiyorum'u seçin."
MSG_TOS_AGREE_en="Agree"
MSG_TOS_AGREE_tr="Kabul Ediyorum"
MSG_TOS_DISAGREE_en="Disagree"
MSG_TOS_DISAGREE_tr="Kabul Etmiyorum"
MSG_NET_TITLE_en="Network Connection Setup"
MSG_NET_TITLE_tr="Ağ Bağlantısı Kurulumu"
MSG_NET_HEADER_en="Choose a network connection mode:"
MSG_NET_HEADER_tr="Bir ağ bağlantısı modu seçin:"
MSG_NET_ETHERNET_en="Wired Connection (Ethernet)"
MSG_NET_ETHERNET_tr="Kablolu Bağlantı (Ethernet)"
MSG_NET_WIFI_en="Wireless Connection (Wi-Fi)"
MSG_NET_WIFI_tr="Kablosuz Bağlantı (Wi-Fi)"
MSG_NET_SKIP_en="Skip Network Setup"
MSG_NET_SKIP_tr="Ağ Kurulumunu Atla"
MSG_SELECT_WIFI_en="Select Wi-Fi Network"
MSG_SELECT_WIFI_tr="Wi-Fi Ağı Seçin"
MSG_ENTER_WIFI_PASS_en="Enter Wi-Fi Password: "
MSG_ENTER_WIFI_PASS_tr="Wi-Fi Şifresini Girin: "
MSG_CONNECTING_WIFI_en="Connecting to Wi-Fi..."
MSG_CONNECTING_WIFI_tr="Wi-Fi'ye Bağlanılıyor..."
MSG_VERIFYING_NET_en="Verifying network connection..."
MSG_VERIFYING_NET_tr="Ağ bağlantısı doğrulanıyor..."
MSG_NET_SUCCESS_en="Network connection verified successfully."
MSG_NET_SUCCESS_tr="Ağ bağlantısı başarıyla doğrulandı."
MSG_NET_FAIL_en="Network verification failed. Do you want to continue anyway?"
MSG_NET_FAIL_tr="Ağ doğrulaması başarısız oldu. Yine de devam etmek istiyor musunuz?"
MSG_RUNNING_REFLECTOR_en="Running reflector to get best mirrors, this may take a while..."
MSG_RUNNING_REFLECTOR_tr="En iyi yansımaları almak için reflector çalıştırılıyor, bu biraz zaman alabilir..."
MSG_DNS_SELECT_HEADER_en="Select Domain Name System (DNS) Provider:"
MSG_DNS_SELECT_HEADER_tr="Domain Name System (DNS) Sağlayıcısı Seçin:"
MSG_YES_en="Yes"
MSG_YES_tr="Evet"
MSG_NO_en="No"
MSG_NO_tr="Hayır"
MSG_BACK_en="Back"
MSG_BACK_tr="Geri"
MSG_SELECT_LOCALE_en="Select System Locale"
MSG_SELECT_LOCALE_tr="Sistem Bölgesel Ayarını Seçin"
MSG_SELECT_CONTINENT_en="Select Timezone Continent"
MSG_SELECT_CONTINENT_tr="Saat Dilimi Kıtasını Seçin"
MSG_SELECT_CITY_en="Select Timezone City"
MSG_SELECT_CITY_tr="Saat Dilimi Şehrini Seçin"
MSG_ENTER_USERNAME_en="Enter Username (required): "
MSG_ENTER_USERNAME_tr="Kullanıcı Adı Girin (gerekli): "
MSG_ADD_ANOTHER_USER_en="Would you like to add another user?"
MSG_ADD_ANOTHER_USER_tr="Başka bir kullanıcı eklemek ister misiniz?"
MSG_PASS_MODE_en="Select Password Allocation Mode:"
MSG_PASS_MODE_tr="Şifre Belirleme Modunu Seçin:"
MSG_PASS_MODE_1_en="Use same password for root and all users"
MSG_PASS_MODE_1_tr="Root ve tüm kullanıcılar için aynı şifreyi kullan"
MSG_PASS_MODE_2_en="Use different password for root, but same password for all users"
MSG_PASS_MODE_2_tr="Root için farklı şifre, tüm kullanıcılar için aynı şifreyi kullan"
MSG_PASS_MODE_3_en="Use different password for all users and root"
MSG_PASS_MODE_3_tr="Tüm kullanıcılar ve root için farklı şifre kullan"
MSG_ENTER_PASS_FOR_en="Enter password for "
MSG_ENTER_PASS_FOR_tr="Şifre girin - "
MSG_CONFIRM_PASS_FOR_en="Confirm password for "
MSG_CONFIRM_PASS_FOR_tr="Şifreyi onaylayın - "
MSG_PASS_MISMATCH_en="Passwords do not match. Please try again."
MSG_PASS_MISMATCH_tr="Şifreler eşleşmiyor. Lütfen tekrar deneyin."
MSG_SUDO_HEADER_en="Would you like to activate passwordless sudo? If enabled, you will not be asked for a password when running sudo commands."
MSG_SUDO_HEADER_tr="Şifresiz sudo özelliğini aktif etmek ister misiniz? Etkinleştirilirse, sudo komutlarını çalıştırırken şifre sorulmayacaktır."
MSG_GPU_HEADER_en="Select your GPU Driver:"
MSG_GPU_HEADER_tr="GPU Sürücünüzü Seçin:"
MSG_CPU_HEADER_en="Select your CPU Type:"
MSG_CPU_HEADER_tr="CPU Tipinizi Seçin:"
MSG_RAZER_HEADER_en="Do you have Razer peripherals?"
MSG_RAZER_HEADER_tr="Razer donanımınız var mı?"
MSG_SELECT_DISK_en="Select Target Disk:"
MSG_SELECT_DISK_tr="Hedef Diski Seçin:"
MSG_PART_MODE_en="Select Partitioning Method:"
MSG_PART_MODE_tr="Bölümleme Yöntemini Seçin:"
MSG_PART_MANUAL_en="Partition the disk manually (Drops to cfdisk)"
MSG_PART_MANUAL_tr="Diski manuel bölümle (cfdisk komutuna geçer)"
MSG_PART_WIPE_en="Wipe Disk (Clean installation)"
MSG_PART_WIPE_tr="Diski Temizle (Temiz kurulum)"
MSG_PART_REPLACE_en="Replace Partition (Keep EFI, format root)"
MSG_PART_REPLACE_tr="Bölümü Değiştir (EFI'yi koru, root'u biçimlendir)"
MSG_PART_GROW_en="Grow EFI Partition (Safe backup and resize)"
MSG_PART_GROW_tr="EFI Bölümünü Büyüt (Güvenli yedekleme ve yeniden boyutlandırma)"
MSG_PART_SHELL_en="Open Shell (zsh)"
MSG_PART_SHELL_tr="Kabuk Aç (zsh)"
MSG_SELECT_FS_en="Select Root Filesystem Type:"
MSG_SELECT_FS_tr="Root Dosya Sistemi Tipini Seçin:"
MSG_CONFIRM_WIPE_en="Are you absolutely sure you want to wipe the target disk? All data will be lost!"
MSG_CONFIRM_WIPE_tr="Hedef diski temizlemek istediğinize tamamen emin misiniz? Tüm veriler kaybolacak!"
MSG_SELECT_EFI_en="Select EFI Partition:"
MSG_SELECT_EFI_tr="EFI Bölümünü Seçin:"
MSG_SELECT_ROOT_en="Select Root Partition:"
MSG_SELECT_ROOT_tr="Root Bölümünü Seçin:"
MSG_CONFIRM_REPLACE_en="Are you sure you want to replace and format root partition?"
MSG_CONFIRM_REPLACE_tr="Root bölümünü değiştirmek ve biçimlendirmek istediğinize emin misiniz?"
MSG_SELECT_SECONDARY_DRIVES_en="Select additional drives to mount (Optional):"
MSG_SELECT_SECONDARY_DRIVES_tr="Bağlanacak ek diskleri seçin (İsteğe bağlı):"
MSG_SYMLINK_PROMPT_en="Would you like to symlink this drive to user homes?"
MSG_SYMLINK_PROMPT_tr="Bu diski kullanıcı ev dizinlerine sembolik bağlamak ister misiniz?"
MSG_ENTER_SYMLINK_PATH_en="Enter path name inside /home/username (e.g. Games): "
MSG_ENTER_SYMLINK_PATH_tr="/home/kullaniciadi altındaki dizin ismini girin (örn. Games): "
MSG_SWAP_ZRAM_HEADER_en="Enable Swap / Zram options:"
MSG_SWAP_ZRAM_HEADER_tr="Swap / Zram seçeneklerini aktif et:"
MSG_SWAP_SIZE_PROMPT_en="Enter Swap size in GB (e.g. 4): "
MSG_SWAP_SIZE_PROMPT_tr="GB cinsinden Swap boyutu girin (örn. 4): "
MSG_ZRAM_SIZE_PROMPT_en="Enter Zram size in GB (e.g. 4): "
MSG_ZRAM_SIZE_PROMPT_tr="GB cinsinden Zram boyutu girin (örn. 4): "
MSG_SELECT_BOOTLOADER_en="Select Bootloader:"
MSG_SELECT_BOOTLOADER_tr="Önyükleyici Seçin:"
MSG_ENTER_HOSTNAME_en="Enter PC Hostname (Computer's network identifier): "
MSG_ENTER_HOSTNAME_tr="PC Bilgisayar Adını Girin (Bilgisayarın ağ tanımlayıcısı): "
MSG_ENTER_DOMAIN_en="Enter PC Domain Name (e.g. localdomain): "
MSG_ENTER_DOMAIN_tr="PC Alan Adını Girin (örn. localdomain): "
MSG_SELECT_KEYBOARD_en="Select Keyboard Layout:"
MSG_SELECT_KEYBOARD_tr="Klavye Düzeni Seçin:"
MSG_MOUNT_SECONDARY_en="Would you like to mount additional secondary drives?"
MSG_MOUNT_SECONDARY_tr="Ek ikincil sürücüleri bağlamak ister misiniz?"
MSG_NONE_en="None"
MSG_NONE_tr="Hiçbiri"
MSG_START_INSTALL_en="Would you like to start the installation now?"
MSG_START_INSTALL_tr="Kurulumu şimdi başlatmak ister misiniz?"
MSG_INSTALLING_en="Installing GötünOS..."
MSG_INSTALLING_tr="GötünOS Kuruluyor..."
MSG_SUCCESS_TITLE_en="Installation Complete!"
MSG_SUCCESS_TITLE_tr="Kurulum Tamamlandı!"
MSG_SUCCESS_TEXT_en="GötünOS has been successfully installed. Have fun!"
MSG_SUCCESS_TEXT_tr="GötünOS başarıyla kuruldu. İyi eğlenceler!"
MSG_REBOOT_PROMPT_en="Would you like to reboot the system now?"
MSG_REBOOT_PROMPT_tr="Sistemi şimdi yeniden başlatmak ister misiniz?"

cleanup() {
    swapoff /mnt/swapfile 2>/dev/null || true
    umount -R /mnt 2>/dev/null || true
}
trap cleanup EXIT

get_text() {
    local key="$1"
    local var_name="MSG_${key}_${ILANG}"
    echo "${!var_name:-$key}"
}

configure_dns() {
    DNS_CHOICE=$(gum choose --header "$(get_text DNS_SELECT_HEADER)" \
        "Cloudflare DNS (1.1.1.1)" \
        "AdGuard DNS (94.140.14.14)" \
        "Quad9 DNS (9.9.9.9)" \
        "Custom DNS" \
        "None")
    
    DNS_IP1=""
    DNS_IP2=""
    DNS_HOST=""
    
    case "$DNS_CHOICE" in
        "Cloudflare DNS (1.1.1.1)")
            DNS_IP1="1.1.1.1"
            DNS_IP2="1.0.0.1"
            DNS_HOST="cloudflare-dns.com"
            ;;
        "AdGuard DNS (94.140.14.14)")
            DNS_IP1="94.140.14.14"
            DNS_IP2="94.140.15.15"
            DNS_HOST="dns.adguard-dns.com"
            ;;
        "Quad9 DNS (9.9.9.9)")
            DNS_IP1="9.9.9.9"
            DNS_IP2="149.112.112.112"
            DNS_HOST="dns.quad9.net"
            ;;
        "Custom DNS")
            DNS_IP1=$(gum input --placeholder "Enter Primary DNS IP (e.g. 8.8.8.8)")
            DNS_IP2=$(gum input --placeholder "Enter Secondary DNS IP (e.g. 8.8.4.4)")
            DNS_HOST=$(gum input --placeholder "Enter TLS Hostname (e.g. dns.google)")
            ;;
        "None"|*)
            return
            ;;
    esac

    if [ -n "$DNS_IP1" ]; then
        mkdir -p /etc/NetworkManager/conf.d
        echo -e "[main]\ndns=none" > /etc/NetworkManager/conf.d/dns.conf
        systemctl reload NetworkManager 2>/dev/null || true
        echo -e "nameserver $DNS_IP1\nnameserver $DNS_IP2" > /etc/resolv.conf
    fi
}

read_password_tab() {
    local prompt="$1"
    local char
    local pass=""
    local show=false
    echo -ne "$prompt" >&2
    while true; do
        IFS= read -r -s -n 1 char
        if [[ "$char" == $'\0' || "$char" == $'\n' ]]; then
            echo "" >&2
            break
        elif [[ "$char" == $'\t' ]]; then
            if [ "$show" = true ]; then
                show=false
            else
                show=true
            fi
            echo -ne "\r\033[K$prompt" >&2
            if [ "$show" = true ]; then
                echo -n "$pass" >&2
            else
                echo -n "${pass//?/*}" >&2
            fi
        elif [[ "$char" == $'\177' || "$char" == $'\010' ]]; then
            if [ ${#pass} -gt 0 ]; then
                pass="${pass%?}"
                echo -ne "\r\033[K$prompt" >&2
                if [ "$show" = true ]; then
                    echo -n "$pass" >&2
                else
                    echo -n "${pass//?/*}" >&2
                fi
            fi
        else
            pass+="$char"
            if [ "$show" = true ]; then
                echo -n "$char" >&2
            else
                echo -n "*" >&2
            fi
        fi
    done
    echo "$pass"
}
grow_efi_logic() {
    clear
    gum style --foreground 4 --border double --align center --width 60 "Grow EFI Partition Mode"
    
    USB_RAW=$(lsblk -dno NAME,RM,SIZE,MODEL | grep -v 'loop' | awk '$2 == "1" {print $1 " (" $3 ") - " $4}')
    if [ -z "$USB_RAW" ]; then
        gum style --foreground 1 "No removable USB devices detected for backup. Please insert a USB drive and try again."
        sleep 3
        return
    fi
    
    USB_PICK=$(echo -e "$USB_RAW\nBack" | gum choose --header "Select USB Drive for EFI Backup:")
    if [ "$USB_PICK" = "Back" ] || [ -z "$USB_PICK" ]; then
        return
    fi
    
    USB_DEV="/dev/$(echo "$USB_PICK" | awk '{print $1}')"
    
    USB_PARTS=$(lsblk -rno NAME,TYPE "$USB_DEV" | grep -w "part" | awk '{print "/dev/"$1}')
    if [ -z "$USB_PARTS" ]; then
        USB_PARTS="$USB_DEV"
    fi
    
    USB_PART_PICK=$(echo "$USB_PARTS" | gum choose --header "Select USB Partition to write backup:")
    if [ -z "$USB_PART_PICK" ]; then
        return
    fi
    
    NEW_SIZE=$(gum input --placeholder "Enter new EFI size in MB (e.g. 1024):" --value "1024")
    if [ -z "$NEW_SIZE" ]; then
        return
    fi
    
    PARTS_RAW=$(lsblk -rno NAME,SIZE,FSTYPE "$TARGET_DISK" | grep -v "^$(basename "$TARGET_DISK")$")
    PART_CHOICES=""
    while IFS= read -r line; do
        pname=$(echo "$line" | awk '{print $1}')
        psize=$(echo "$line" | awk '{print $2}')
        pfs=$(echo "$line" | awk '{print $3}')
        PART_CHOICES+="$pname ($psize) $pfs\n"
    done <<< "$PARTS_RAW"
    
    EFI_PICK=$(echo -e "$PART_CHOICES" | gum choose --header "Select current EFI partition to grow:")
    if [ -z "$EFI_PICK" ]; then
        return
    fi
    CURRENT_EFI="/dev/$(echo "$EFI_PICK" | awk '{print $1}')"
    
    SHRINK_PICK=$(echo -e "$PART_CHOICES" | gum choose --header "Select adjacent partition to shrink (safely from end):")
    if [ -z "$SHRINK_PICK" ]; then
        return
    fi
    SHRINK_PART="/dev/$(echo "$SHRINK_PICK" | awk '{print $1}')"
    
    mkdir -p /tmp/mnt_usb /tmp/mnt_efi
    mount "$USB_PART_PICK" /tmp/mnt_usb && MOUNTED_USB=true || MOUNTED_USB=false
    mount "$CURRENT_EFI" /tmp/mnt_efi && MOUNTED_EFI=true || MOUNTED_EFI=false
    
    if [ "$MOUNTED_USB" = true ] && [ "$MOUNTED_EFI" = true ]; then
        gum spin --spinner dot --title "Backing up EFI to USB..." -- cp -a /tmp/mnt_efi/* /tmp/mnt_usb/
        gum style --foreground 2 "Backup successfully created on $USB_PART_PICK."
    else
        gum style --foreground 1 "Error: Failed to mount partitions for backup."
    fi
    
    umount /tmp/mnt_usb 2>/dev/null || true
    umount /tmp/mnt_efi 2>/dev/null || true
    
    gum style --foreground 3 "Attempting to shrink $SHRINK_PART and grow $CURRENT_EFI..."

    # Resolve the actual partition numbers from device paths (not hardcoded)
    EFI_PART_NUM=$(cat /sys/class/block/$(basename "$CURRENT_EFI")/partition 2>/dev/null)
    SHRINK_PART_NUM=$(cat /sys/class/block/$(basename "$SHRINK_PART")/partition 2>/dev/null)
    if [ -z "$EFI_PART_NUM" ] || [ -z "$SHRINK_PART_NUM" ]; then
        gum style --foreground 1 "Error: Could not resolve partition numbers for $CURRENT_EFI or $SHRINK_PART."
        sleep 3
        return
    fi

    # Determine the current end sector of SHRINK_PART and compute its new (smaller) end
    SHRINK_CURRENT_END=$(parted -sm "$TARGET_DISK" unit MiB print 2>/dev/null | awk -F: -v pn="$SHRINK_PART_NUM" '$1==pn{gsub("MiB","",$3); print $3}')
    if [ -z "$SHRINK_CURRENT_END" ]; then
        gum style --foreground 1 "Error: Could not determine size of partition $SHRINK_PART_NUM."
        sleep 3
        return
    fi
    SPACE_NEEDED=$((NEW_SIZE))
    EFI_CURRENT_SIZE=$(parted -sm "$TARGET_DISK" unit MiB print 2>/dev/null | awk -F: -v pn="$EFI_PART_NUM" '$1==pn{gsub("MiB","",$4); print $4}')
    EXTRA_NEEDED=$(( SPACE_NEEDED - ${EFI_CURRENT_SIZE:-0} ))
    SHRINK_NEW_END=$(( SHRINK_CURRENT_END - EXTRA_NEEDED ))

    SHRINK_FS=$(blkid -o value -s TYPE "$SHRINK_PART")
    if [ "$SHRINK_FS" = "ext4" ]; then
        e2fsck -f -y "$SHRINK_PART" &>/dev/null || true
        # Shrink filesystem to new size first, then shrink partition table entry
        resize2fs "$SHRINK_PART" "${SHRINK_NEW_END}M" &>/dev/null || true
        parted -s "$TARGET_DISK" resizepart "$SHRINK_PART_NUM" "${SHRINK_NEW_END}MiB" &>/dev/null || true
    else
        gum style --foreground 1 "Warning: Cannot safely shrink $SHRINK_FS filesystem — only ext4 shrinking is supported. Aborting resize."
        sleep 4
        return
    fi

    # Now grow the EFI partition to the new size using its actual partition number
    parted -s "$TARGET_DISK" resizepart "$EFI_PART_NUM" "${NEW_SIZE}MiB" &>/dev/null || true

    gum style --foreground 2 "EFI resizing completed (Safe backup and resize executed)."
    sleep 3
}

ILANG=""
STEP=1

while true; do
    clear
    case "$STEP" in
        1)
            LANG_CHOICE=$(gum choose --header "Select Installer Language / Kurulum Dili Seçin" "English" "Türkçe")
            if [ "$LANG_CHOICE" = "Türkçe" ]; then
                ILANG="tr"
            else
                ILANG="en"
            fi
            STEP=2
            ;;
        2)
            gum style --foreground 4 --border double --align center --width 60 "$(get_text TOS_TITLE)"
            gum style "$(get_text TOS_TEXT)"
            TOS_AGREE=$(gum choose "$(get_text TOS_AGREE)" "$(get_text TOS_DISAGREE)" "$(get_text BACK)")
            if [ "$TOS_AGREE" = "$(get_text TOS_AGREE)" ]; then
                STEP=3
            elif [ "$TOS_AGREE" = "$(get_text TOS_DISAGREE)" ]; then
                exit 1
            else
                STEP=1
            fi
            ;;
        3)
            # Ensure NetworkManager is running in the live environment before any nmcli calls
            if ! systemctl is-active --quiet NetworkManager; then
                gum spin --spinner dot --title "Starting NetworkManager..." -- \
                    systemctl start NetworkManager
                sleep 2
            fi

            # Full wificonnector-style loop: scan, connect, re-scan, disconnect
            NET_DONE=false
            while [ "$NET_DONE" = false ]; do
                active_map=$(nmcli -t -f NAME,DEVICE connection show --active 2>/dev/null)
                active_conns=$(echo "$active_map" | cut -d: -f1)
                ethernet_active=$(nmcli -t -f DEVICE,TYPE,STATE device status 2>/dev/null | grep "ethernet:connected")

                mapfile -t wifi_list < <(nmcli -t -f SSID device wifi list 2>/dev/null | grep -v '^--' | grep -v '^$' | sort -u)

                net_choices=()

                # Add ethernet option if adapter present
                if nmcli device status 2>/dev/null | grep -q "ethernet"; then
                    if [ -n "$ethernet_active" ]; then
                        net_choices+=("Wired Connection [connected]")
                    else
                        net_choices+=("Wired Connection")
                    fi
                fi

                # Add Wi-Fi SSIDs, mark connected ones
                for net in "${wifi_list[@]}"; do
                    [ -z "$net" ] && continue
                    if echo "$active_conns" | grep -Fqx "$net"; then
                        net_choices+=("WiFi: $net [connected]")
                    else
                        net_choices+=("WiFi: $net")
                    fi
                done

                net_choices+=("Re-scan Networks")
                net_choices+=("$(get_text NET_SKIP)")
                net_choices+=("$(get_text BACK)")

                chosen_raw=$(printf '%s\n' "${net_choices[@]}" | gum choose --header "$(get_text NET_HEADER)")

                if [ -z "$chosen_raw" ] || [ "$chosen_raw" = "$(get_text BACK)" ]; then
                    STEP=2
                    NET_DONE=true

                elif [ "$chosen_raw" = "Re-scan Networks" ]; then
                    gum spin --spinner dot --title "Scanning for networks..." -- \
                        nmcli device wifi rescan 2>/dev/null; sleep 2
                    continue

                elif [ "$chosen_raw" = "$(get_text NET_SKIP)" ]; then
                    STEP=4
                    NET_DONE=true

                elif [[ "$chosen_raw" == *"Wired Connection"* ]]; then
                    if [[ "$chosen_raw" == *"[connected]"* ]]; then
                        action=$(gum choose "Disconnect" "Continue to next step" "Go back")
                        if [ "$action" = "Disconnect" ]; then
                            dev=$(echo "$ethernet_active" | cut -d: -f1 | head -n1)
                            gum spin --spinner dot --title "Disconnecting $dev..." -- \
                                nmcli device disconnect "$dev" 2>/dev/null || true
                        elif [ "$action" = "Continue to next step" ]; then
                            gum spin --spinner dot --title "$(get_text RUNNING_REFLECTOR)" -- \
                                reflector --latest 20 --protocol https --sort rate --save /etc/pacman.d/mirrorlist
                            configure_dns
                            STEP=4
                            NET_DONE=true
                        fi
                    else
                        eth_dev=$(nmcli -t -f DEVICE,TYPE device status 2>/dev/null | grep "ethernet" | head -n1 | cut -d: -f1)
                        eth_profile=$(nmcli -g NAME connection show 2>/dev/null | grep -iE 'wired|ethernet' | head -n1)
                        gum spin --spinner dot --title "Connecting to Wired Network..." -- \
                            bash -c "nmcli connection up '${eth_profile}' 2>/dev/null || nmcli device connect '${eth_dev}' 2>/dev/null"
                        if ping -c 1 -W 3 1.1.1.1 &>/dev/null; then
                            gum style --foreground 2 "Wired connected successfully!"
                            gum spin --spinner dot --title "$(get_text RUNNING_REFLECTOR)" -- \
                                reflector --latest 20 --protocol https --sort rate --save /etc/pacman.d/mirrorlist
                            configure_dns
                            STEP=4
                            NET_DONE=true
                        else
                            gum style --foreground 1 "Wired connection failed. Try again."
                            sleep 2
                        fi
                    fi

                else
                    # Wi-Fi network selected
                    chosen_net=$(echo "$chosen_raw" | sed 's/^WiFi: //' | sed 's/ \[connected\]//')

                    if [[ "$chosen_raw" == *"[connected]"* ]]; then
                        action=$(gum choose "Disconnect" "Continue to next step" "Go back")
                        if [ "$action" = "Disconnect" ]; then
                            dev=$(echo "$active_map" | grep "^${chosen_net}:" | cut -d: -f2 | head -n1)
                            if [ -n "$dev" ]; then
                                gum spin --spinner dot --title "Disconnecting $dev..." -- \
                                    nmcli device disconnect "$dev" 2>/dev/null || true
                            else
                                gum spin --spinner dot --title "Disconnecting $chosen_net..." -- \
                                    nmcli connection down id "$chosen_net" 2>/dev/null || true
                            fi
                        elif [ "$action" = "Continue to next step" ]; then
                            gum spin --spinner dot --title "$(get_text RUNNING_REFLECTOR)" -- \
                                reflector --latest 20 --protocol https --sort rate --save /etc/pacman.d/mirrorlist
                            configure_dns
                            STEP=4
                            NET_DONE=true
                        fi
                    else
                        is_secure=$(nmcli -t -f SSID,SECURITY device wifi list 2>/dev/null | grep "^${chosen_net}:" | grep -E "WPA|WEP")
                        WIFI_OK=false
                        WIFI_ERR=""
                        if [ -n "$is_secure" ]; then
                            WIFI_PASS=$(gum input --password --placeholder "Enter password for $chosen_net")
                            if [ -z "$WIFI_PASS" ]; then
                                gum style --foreground 1 "Password cannot be empty."
                                sleep 2
                                continue
                            fi
                            WIFI_ERR=$(nmcli device wifi connect "$chosen_net" password "$WIFI_PASS" 2>&1)
                            echo "$WIFI_ERR" | grep -qi "successfully" && WIFI_OK=true || WIFI_OK=false
                        else
                            WIFI_ERR=$(nmcli device wifi connect "$chosen_net" 2>&1)
                            echo "$WIFI_ERR" | grep -qi "successfully" && WIFI_OK=true || WIFI_OK=false
                        fi

                        if [ "$WIFI_OK" = true ]; then
                            gum style --foreground 2 "Connected to $chosen_net!"
                            gum spin --spinner dot --title "$(get_text RUNNING_REFLECTOR)" -- \
                                reflector --latest 20 --protocol https --sort rate --save /etc/pacman.d/mirrorlist
                            configure_dns
                            STEP=4
                            NET_DONE=true
                        else
                            gum style --foreground 1 "Failed to connect to $chosen_net:"
                            gum style --foreground 1 "$WIFI_ERR"
                            sleep 4
                        fi
                    fi
                fi
            done
            ;;
        4)
            # Only show UTF-8 locales — prevents ISO-8859-1/other encodings that break Qt6, KDE Plasma 6, DBus, and Python
            LOCALES=$(grep -E '^#?[a-zA-Z]' /etc/locale.gen | sed 's/^#//' | grep 'UTF-8' | awk '{print $1}' | sort -u)
            SELECTED_LOCALE=$(printf '%s\n' $LOCALES "$(get_text BACK)" | gum filter --placeholder "$(get_text SELECT_LOCALE)")
            if [ "$SELECTED_LOCALE" = "$(get_text BACK)" ] || [ -z "$SELECTED_LOCALE" ]; then
                STEP=3
            else
                CONTINENTS="Africa\nAmerica\nAntarctica\nArctic\nAsia\nAtlantic\nAustralia\nEurope\nIndian\nPacific"
                SELECTED_CONTINENT=$(echo -e "$CONTINENTS\n$(get_text BACK)" | gum choose --header "$(get_text SELECT_CONTINENT)")
                if [ "$SELECTED_CONTINENT" = "$(get_text BACK)" ]; then
                    STEP=4
                else
                    CITIES=$(find /usr/share/zoneinfo/"$SELECTED_CONTINENT" -type f -o -type l | sed "s|/usr/share/zoneinfo/$SELECTED_CONTINENT/||" | sort)
                    SELECTED_CITY=$(printf '%s\n' $CITIES "$(get_text BACK)" | gum filter --placeholder "$(get_text SELECT_CITY)")
                    if [ "$SELECTED_CITY" = "$(get_text BACK)" ] || [ -z "$SELECTED_CITY" ]; then
                        STEP=4
                    else
                        TZ="$SELECTED_CONTINENT/$SELECTED_CITY"
                        STEP=4.5
                    fi
                fi
            fi
            ;;
        4.5)
            KEYMAPS=$(localectl list-keymaps 2>/dev/null | sort)
            SELECTED_KEYMAP=$(printf '%s\n' $KEYMAPS "$(get_text BACK)" | gum filter --placeholder "$(get_text SELECT_KEYBOARD)")
            if [ "$SELECTED_KEYMAP" = "$(get_text BACK)" ] || [ -z "$SELECTED_KEYMAP" ]; then
                STEP=4
            else
                loadkeys "$SELECTED_KEYMAP" 2>/dev/null || true
                STEP=5
            fi
            ;;
        5)
            USERS=()
            declare -A USER_PASSWORDS
            ROOT_PASS=""
            
            U1=$(gum input --placeholder "$(get_text ENTER_USERNAME)")
            if [ -z "$U1" ] || [[ ! "$U1" =~ ^[a-z_][a-z0-9_-]*$ ]]; then
                STEP=5
            else
                USERS+=("$U1")
                while true; do
                    ADD_MORE=$(gum choose --header "$(get_text ADD_ANOTHER_USER)" "$(get_text YES)" "$(get_text NO)")
                    if [ "$ADD_MORE" = "$(get_text YES)" ]; then
                        UN=$(gum input --placeholder "$(get_text ENTER_USERNAME)")
                        if [ -n "$UN" ] && [[ "$UN" =~ ^[a-z_][a-z0-9_-]*$ ]]; then
                            USERS+=("$UN")
                        fi
                    else
                        break
                    fi
                done
                
                PASS_MODE=$(gum choose --header "$(get_text PASS_MODE)" "$(get_text PASS_MODE_1)" "$(get_text PASS_MODE_2)" "$(get_text PASS_MODE_3)" "$(get_text BACK)")
                if [ "$PASS_MODE" = "$(get_text BACK)" ]; then
                    STEP=4
                else
                    if [ "$PASS_MODE" = "$(get_text PASS_MODE_1)" ]; then
                        while true; do
                            P1=$(read_password_tab "$(get_text ENTER_PASS_FOR)all users & root: ")
                            P2=$(read_password_tab "$(get_text CONFIRM_PASS_FOR)all users & root: ")
                            if [ "$P1" = "$P2" ] && [ -n "$P1" ]; then
                                ROOT_PASS="$P1"
                                for u in "${USERS[@]}"; do
                                    USER_PASSWORDS["$u"]="$P1"
                                done
                                break
                            else
                                gum style --foreground 1 "$(get_text PASS_MISMATCH)"
                            fi
                        done
                    elif [ "$PASS_MODE" = "$(get_text PASS_MODE_2)" ]; then
                        while true; do
                            P1=$(read_password_tab "$(get_text ENTER_PASS_FOR)root: ")
                            P2=$(read_password_tab "$(get_text CONFIRM_PASS_FOR)root: ")
                            if [ "$P1" = "$P2" ] && [ -n "$P1" ]; then
                                ROOT_PASS="$P1"
                                break
                            else
                                gum style --foreground 1 "$(get_text PASS_MISMATCH)"
                            fi
                        done
                        while true; do
                            P1=$(read_password_tab "$(get_text ENTER_PASS_FOR)all users: ")
                            P2=$(read_password_tab "$(get_text CONFIRM_PASS_FOR)all users: ")
                            if [ "$P1" = "$P2" ] && [ -n "$P1" ]; then
                                for u in "${USERS[@]}"; do
                                    USER_PASSWORDS["$u"]="$P1"
                                done
                                break
                            else
                                gum style --foreground 1 "$(get_text PASS_MISMATCH)"
                            fi
                        done
                    else
                        for u in "${USERS[@]}"; do
                            while true; do
                                P1=$(read_password_tab "$(get_text ENTER_PASS_FOR)$u: ")
                                P2=$(read_password_tab "$(get_text CONFIRM_PASS_FOR)$u: ")
                                if [ "$P1" = "$P2" ] && [ -n "$P1" ]; then
                                    USER_PASSWORDS["$u"]="$P1"
                                    break
                                else
                                    gum style --foreground 1 "$(get_text PASS_MISMATCH)"
                                fi
                            done
                        done
                        while true; do
                            P1=$(read_password_tab "$(get_text ENTER_PASS_FOR)root: ")
                            P2=$(read_password_tab "$(get_text CONFIRM_PASS_FOR)root: ")
                            if [ "$P1" = "$P2" ] && [ -n "$P1" ]; then
                                ROOT_PASS="$P1"
                                break
                            else
                                gum style --foreground 1 "$(get_text PASS_MISMATCH)"
                            fi
                        done
                    fi
                    
                    SUDO_CONF=$(gum choose --header "$(get_text SUDO_HEADER)" "$(get_text YES)" "$(get_text NO)")
                    if [ "$SUDO_CONF" = "$(get_text YES)" ]; then
                        PASSWORDLESS_SUDO="true"
                    else
                        PASSWORDLESS_SUDO="false"
                    fi
                    STEP=6
                fi
            fi
            ;;
        6)
            GPU_DRIVER=$(gum choose --header "$(get_text GPU_HEADER)" "AMD" "NVIDIA Turing+ (16xx+) (open-source DKMS)" "Legacy NVIDIA (proprietary DKMS)" "Intel" "Intel iGPU" "AMD iGPU" "Do not install drivers" "$(get_text BACK)")
            if [ "$GPU_DRIVER" = "$(get_text BACK)" ]; then
                STEP=5
            else
                CPU_TYPE=$(gum choose --header "$(get_text CPU_HEADER)" "AMD" "Intel" "$(get_text BACK)")
                if [ "$CPU_TYPE" = "$(get_text BACK)" ]; then
                    STEP=6
                else
                    RAZER_CONF=$(gum choose --header "$(get_text RAZER_HEADER)" "$(get_text YES)" "$(get_text NO)" "$(get_text BACK)")
                    if [ "$RAZER_CONF" = "$(get_text BACK)" ]; then
                        STEP=6
                    else
                        if [ "$RAZER_CONF" = "$(get_text YES)" ]; then
                            RAZER_SUPPORT="true"
                        else
                            RAZER_SUPPORT="false"
                        fi
                        STEP=7
                    fi
                fi
            fi
            ;;
        7)
            DISKS_RAW=$(lsblk -dno NAME,SIZE,MODEL | grep -v 'loop')
            DISK_CHOICES=""
            while IFS= read -r line; do
                dev=$(echo "$line" | awk '{print $1}')
                sz=$(echo "$line" | awk '{print $2}')
                mdl=$(echo "$line" | cut -d' ' -f3-)
                DISK_CHOICES+="$dev ($sz) - $mdl\n"
            done <<< "$DISKS_RAW"
            
            DISK_PICK=$(echo -e "$DISK_CHOICES$(get_text BACK)" | gum choose --header "$(get_text SELECT_DISK)")
            if [ "$DISK_PICK" = "$(get_text BACK)" ] || [ -z "$DISK_PICK" ]; then
                STEP=6
            else
                TARGET_DISK="/dev/$(echo "$DISK_PICK" | awk '{print $1}')"
                PART_MODE=$(gum choose --header "$(get_text PART_MODE)" "$(get_text PART_WIPE)" "$(get_text PART_MANUAL)" "$(get_text PART_REPLACE)" "$(get_text PART_GROW)" "$(get_text PART_SHELL)" "$(get_text BACK)")
                if [ "$PART_MODE" = "$(get_text BACK)" ]; then
                    STEP=7
                elif [ "$PART_MODE" = "$(get_text PART_SHELL)" ]; then
                    gum style --foreground 3 "Dropping to zsh shell for manual partitioning. Use fdisk, parted, or cfdisk."
                    gum style --foreground 3 "Type 'exit' or Ctrl+D to return to the installer when done."
                    zsh
                    partprobe "$TARGET_DISK" 2>/dev/null || true
                    sync
                    # Select EFI and root after manual shell
                    PARTS_RAW=$(lsblk -rno NAME,SIZE,FSTYPE "$TARGET_DISK" | grep -v "^$(basename "$TARGET_DISK")$")
                    PART_CHOICES=""
                    while IFS= read -r line; do
                        pname=$(echo "$line" | awk '{print $1}')
                        psize=$(echo "$line" | awk '{print $2}')
                        pfs=$(echo "$line" | awk '{print $3}')
                        PART_CHOICES+="$pname ($psize) $pfs\n"
                    done <<< "$PARTS_RAW"
                    EFI_PICK=$(echo -e "$PART_CHOICES$(get_text BACK)" | gum choose --header "$(get_text SELECT_EFI)")
                    if [ "$EFI_PICK" != "$(get_text BACK)" ] && [ -n "$EFI_PICK" ]; then
                        EFI_PART="/dev/$(echo "$EFI_PICK" | awk '{print $1}')"
                        ROOT_PICK=$(echo -e "$PART_CHOICES$(get_text BACK)" | gum choose --header "$(get_text SELECT_ROOT)")
                        if [ "$ROOT_PICK" != "$(get_text BACK)" ] && [ -n "$ROOT_PICK" ]; then
                            ROOT_PART="/dev/$(echo "$ROOT_PICK" | awk '{print $1}')"
                            PARTITION_MODE="manual"
                            TARGET_FS=$(gum choose --header "$(get_text SELECT_FS)" "ext4" "xfs" "btrfs")
                            STEP=7.1
                        fi
                    fi
                elif [ "$PART_MODE" = "$(get_text PART_GROW)" ]; then
                    grow_efi_logic
                elif [ "$PART_MODE" = "$(get_text PART_MANUAL)" ]; then
                    gum style --foreground 3 "Dropping to cfdisk for manual partitioning. Partition your drive, write changes, and exit cfdisk when done."
                    cfdisk "$TARGET_DISK"
                    partprobe "$TARGET_DISK"
                    sync
                    PARTS_RAW=$(lsblk -rno NAME,SIZE,FSTYPE "$TARGET_DISK" | grep -v "^$(basename "$TARGET_DISK")$")
                    PART_CHOICES=""
                    while IFS= read -r line; do
                        pname=$(echo "$line" | awk '{print $1}')
                        psize=$(echo "$line" | awk '{print $2}')
                        pfs=$(echo "$line" | awk '{print $3}')
                        PART_CHOICES+="$pname ($psize) $pfs\n"
                    done <<< "$PARTS_RAW"
                    EFI_PICK=$(echo -e "$PART_CHOICES$(get_text BACK)" | gum choose --header "$(get_text SELECT_EFI)")
                    if [ "$EFI_PICK" != "$(get_text BACK)" ] && [ -n "$EFI_PICK" ]; then
                        EFI_PART="/dev/$(echo "$EFI_PICK" | awk '{print $1}')"
                        ROOT_PICK=$(echo -e "$PART_CHOICES$(get_text BACK)" | gum choose --header "$(get_text SELECT_ROOT)")
                        if [ "$ROOT_PICK" != "$(get_text BACK)" ] && [ -n "$ROOT_PICK" ]; then
                            ROOT_PART="/dev/$(echo "$ROOT_PICK" | awk '{print $1}')"
                            PARTITION_MODE="manual"
                            TARGET_FS=$(gum choose --header "$(get_text SELECT_FS)" "ext4" "xfs" "btrfs")
                            STEP=7.1
                        fi
                    fi
                elif [ "$PART_MODE" = "$(get_text PART_WIPE)" ]; then
                    CONFIRM=$(gum choose --header "$(get_text CONFIRM_WIPE)" "$(get_text NO)" "$(get_text YES)")
                    if [ "$CONFIRM" = "$(get_text YES)" ]; then
                        PARTITION_MODE="wipe"
                        TARGET_FS=$(gum choose --header "$(get_text SELECT_FS)" "ext4" "xfs" "btrfs")
                        STEP=7.1
                    fi
                else
                    PARTS_RAW=$(lsblk -rno NAME,SIZE,FSTYPE "$TARGET_DISK" | grep -v "^$(basename "$TARGET_DISK")$")
                    PART_CHOICES=""
                    while IFS= read -r line; do
                        pname=$(echo "$line" | awk '{print $1}')
                        psize=$(echo "$line" | awk '{print $2}')
                        pfs=$(echo "$line" | awk '{print $3}')
                        PART_CHOICES+="$pname ($psize) $pfs\n"
                    done <<< "$PARTS_RAW"
                    
                    EFI_PICK=$(echo -e "$PART_CHOICES$(get_text BACK)" | gum choose --header "$(get_text SELECT_EFI)")
                    if [ "$EFI_PICK" != "$(get_text BACK)" ] && [ -n "$EFI_PICK" ]; then
                        EFI_PART="/dev/$(echo "$EFI_PICK" | awk '{print $1}')"
                        ROOT_PICK=$(echo -e "$PART_CHOICES$(get_text BACK)" | gum choose --header "$(get_text SELECT_ROOT)")
                        if [ "$ROOT_PICK" != "$(get_text BACK)" ] && [ -n "$ROOT_PICK" ]; then
                            ROOT_PART="/dev/$(echo "$ROOT_PICK" | awk '{print $1}')"
                            CONFIRM=$(gum choose --header "$(get_text CONFIRM_REPLACE) $ROOT_PART?" "$(get_text NO)" "$(get_text YES)")
                            if [ "$CONFIRM" = "$(get_text YES)" ]; then
                                PARTITION_MODE="replace"
                                TARGET_FS=$(gum choose --header "$(get_text SELECT_FS)" "ext4" "xfs" "btrfs")
                                STEP=7.1
                            fi
                        fi
                    fi
                fi
            fi
            ;;
        7.1)
            SECONDARY_DRIVES=()
            SYMLINK_DRIVES=()
            declare -A SYMLINK_PATHS
            declare -A MOUNT_POINTS
            
            OTHER_DISKS_RAW=$(lsblk -dno NAME,SIZE,MODEL | grep -v 'loop' | grep -v "$(basename "$TARGET_DISK")")
            if [ -n "$OTHER_DISKS_RAW" ]; then
                MOUNT_SEC=$(gum choose --header "$(get_text MOUNT_SECONDARY)" "$(get_text YES)" "$(get_text NO)")
                if [ "$MOUNT_SEC" = "$(get_text YES)" ]; then
                OTHER_CHOICES=""
                while IFS= read -r line; do
                    odev=$(echo "$line" | awk '{print $1}')
                    osz=$(echo "$line" | awk '{print $2}')
                    omdl=$(echo "$line" | cut -d' ' -f3-)
                    OTHER_CHOICES+="$odev ($osz) - $omdl\n"
                done <<< "$OTHER_DISKS_RAW"
                
                SELECTED_SEC=$(echo -e "$OTHER_CHOICES" | gum choose --no-limit --header "$(get_text SELECT_SECONDARY_DRIVES)")
                if [ -n "$SELECTED_SEC" ]; then
                    # Use mapfile+for instead of while/read/herestring to avoid
                    # hijacking stdin while gum interactive prompts run inside the loop
                    mapfile -t sec_lines <<< "$SELECTED_SEC"
                    for sline in "${sec_lines[@]}"; do
                        if [ -n "$sline" ]; then
                            sdev="/dev/$(echo "$sline" | awk '{print $1}')"
                            parts=""
                            parts=$(lsblk -rno NAME,TYPE "$sdev" | grep -w "part" | awk '{print "/dev/"$1}')
                            if [ -z "$parts" ]; then
                                parts="$sdev"
                            fi
                            for part in $parts; do
                                fstype=""
                                fstype=$(blkid -o value -s TYPE "$part" 2>/dev/null || true)
                                if [ -n "$fstype" ]; then
                                    SECONDARY_DRIVES+=("$part")
                                    # Ask user WHERE to mount this drive
                                    part_label=$(blkid -o value -s LABEL "$part" 2>/dev/null || true)
                                    default_mnt="/mnt/data"
                                    [ -n "$part_label" ] && default_mnt="/mnt/$part_label"
                                    MPATH=$(gum input \
                                        --placeholder "Where to mount $part ($fstype)? e.g. /mnt/data" \
                                        --value "$default_mnt")
                                    [ -z "$MPATH" ] && MPATH="$default_mnt"
                                    MOUNT_POINTS["$part"]="$MPATH"
                                    gum style --foreground 4 "  $part ($fstype) → will be mounted at $MPATH"

                                    LINK_CONF=$(gum choose --header "$(get_text SYMLINK_PROMPT) ($part → $MPATH)" "$(get_text NO)" "$(get_text YES)")
                                    if [ "$LINK_CONF" = "$(get_text YES)" ]; then
                                        SYMLINK_DRIVES+=("$part")
                                        default_hint="Games"
                                        if [ "${#USERS[@]}" -eq 1 ]; then
                                            default_hint="Games"
                                        fi
                                        LPATH=$(gum input \
                                            --placeholder "Folder name inside /home/username (e.g. Games)" \
                                            --value "$default_hint")
                                        [ -z "$LPATH" ] && LPATH="$default_hint"
                                        SYMLINK_PATHS["$part"]="$LPATH"
                                    fi
                                else
                                    gum style --foreground 3 "  $part has no filesystem — skipping (format it first if you want to mount it)"
                                fi
                            done
                        fi
                    done
                fi
            fi
        fi
            STEP=7.2
            ;;
        7.2)
            SWAP_ZRAM_CHOICE=$(gum choose --no-limit --header "$(get_text SWAP_ZRAM_HEADER)" "Swap" "Zram" "$(get_text NONE)")
            SWAP_ENABLED="false"
            ZRAM_ENABLED="false"
            
            if echo "$SWAP_ZRAM_CHOICE" | grep -q "Swap"; then
                SWAP_ENABLED="true"
                SWAP_SIZE=$(gum input --placeholder "$(get_text SWAP_SIZE_PROMPT)" --value "4")
            fi
            if echo "$SWAP_ZRAM_CHOICE" | grep -q "Zram"; then
                ZRAM_ENABLED="true"
                ZRAM_SIZE=$(gum input --placeholder "$(get_text ZRAM_SIZE_PROMPT)" --value "4")
            fi
            STEP=7.3
            ;;
        7.3)
            BOOTLOADER=$(gum choose --header "$(get_text SELECT_BOOTLOADER)" "systemd-boot (Recommended)" "GRUB" "$(get_text BACK)")
            if [ "$BOOTLOADER" = "$(get_text BACK)" ]; then
                STEP=7.2
            else
                if [ "$BOOTLOADER" = "systemd-boot (Recommended)" ]; then
                    BOOTLOADER="systemd-boot"
                fi
                STEP=7.4
            fi
            ;;
        7.4)
            gum spin --spinner dot --title "Running partprobe and syncing..." -- sh -c "partprobe && sync"
            STEP=8
            ;;
        8)
            gum style --foreground 4 --border normal --padding "1 2" \
                "PC Name (hostname): This is how your computer identifies itself on the network." \
                "Example: 'mygamingpc', 'johns-laptop', 'desktop'" \
                "" \
                "Domain: Usually 'local' or 'localdomain'. Only change if you know what this means." \
                "Example: 'local', 'localdomain', 'home'"
            HOSTNAME=$(gum input --placeholder "e.g. mygamingpc" --value "gotunos")
            if [ -z "$HOSTNAME" ]; then HOSTNAME="gotunos"; fi
            DOMAIN=$(gum input --placeholder "e.g. gotunos-x86-64" --value "gotunos-x86-64")
            if [ -z "$DOMAIN" ]; then DOMAIN="gotunos-x86-64"; fi
            STEP=9
            ;;
        9)
            SUMMARY="Language: $LANG_CHOICE\n"
            SUMMARY+="Locale: $SELECTED_LOCALE\n"
            SUMMARY+="Timezone: $TZ\n"
            SUMMARY+="Hostname: $HOSTNAME.$DOMAIN\n"
            SUMMARY+="Target Disk: $TARGET_DISK\n"
            SUMMARY+="Partition Mode: $PARTITION_MODE\n"
            SUMMARY+="Filesystem: $TARGET_FS\n"
            SUMMARY+="Bootloader: $BOOTLOADER\n"
            SUMMARY+="GPU Driver: $GPU_DRIVER\n"
            SUMMARY+="CPU Type: $CPU_TYPE\n"
            SUMMARY+="Razer Support: $RAZER_SUPPORT\n"
            if [ "$SWAP_ENABLED" = "true" ]; then
                SUMMARY+="Swap: $SWAP_SIZE GB\n"
            else
                SUMMARY+="Swap: Disabled\n"
            fi
            if [ "$ZRAM_ENABLED" = "true" ]; then
                SUMMARY+="Zram: $ZRAM_SIZE GB\n"
            else
                SUMMARY+="Zram: Disabled\n"
            fi
            SUMMARY+="Users: ${USERS[*]}\n"
            if [ ${#SECONDARY_DRIVES[@]} -gt 0 ]; then
                SUMMARY+="Secondary Drives:\n"
                for drive in "${SECONDARY_DRIVES[@]}"; do
                    mpt="${MOUNT_POINTS[$drive]:-/mnt/$(basename $drive)}"
                    lnk="${SYMLINK_PATHS[$drive]:-}"
                    if [ -n "$lnk" ]; then
                        SUMMARY+="  $drive → $mpt (symlinked as ~/$lnk)\n"
                    else
                        SUMMARY+="  $drive → $mpt\n"
                    fi
                done
            fi
            
            echo -e "$SUMMARY" | gum style --foreground 2 --border normal --padding "1 2" --margin 1
            
            CONFIRM_INSTALL=$(gum choose --header "$(get_text START_INSTALL)" "$(get_text YES)" "$(get_text NO)")
            if [ "$CONFIRM_INSTALL" = "$(get_text YES)" ]; then
                STEP=10
            else
                STEP=8
            fi
            ;;
        10)
            LOG=/tmp/gotunos_install.log
            rm -f "$LOG"

            # Print each step clearly so errors are visible
            step() { echo; gum style --foreground 6 "━━ $* ━━"; }
            die()  { echo; gum style --foreground 1 "FATAL: $1"; echo "Full log: $LOG"; exit 1; }
            run()  { echo "  » $*"; "$@" 2>&1 | tee -a "$LOG"; return ${PIPESTATUS[0]}; }

            # ── 1. Partition (wipe mode only) ──
            if [ "$PARTITION_MODE" = "wipe" ]; then
                step "Partitioning $TARGET_DISK (EFI=2GiB)"
                run parted -s "$TARGET_DISK" mklabel gpt || die "mklabel failed"
                run parted -s "$TARGET_DISK" mkpart ESP fat32 1MiB 2049MiB || die "EFI mkpart failed"
                run parted -s "$TARGET_DISK" set 1 esp on || die "esp flag failed"
                run parted -s "$TARGET_DISK" mkpart root "$TARGET_FS" 2049MiB 100% || die "root mkpart failed"
                run partprobe "$TARGET_DISK"
                sleep 3
                if [[ "$TARGET_DISK" =~ nvme|mmcblk ]]; then
                    EFI_PART="${TARGET_DISK}p1"
                    ROOT_PART="${TARGET_DISK}p2"
                else
                    EFI_PART="${TARGET_DISK}1"
                    ROOT_PART="${TARGET_DISK}2"
                fi
            fi

            [ -z "$EFI_PART" ]  && die "EFI partition not set."
            [ -z "$ROOT_PART" ] && die "Root partition not set."
            echo "EFI=$EFI_PART  ROOT=$ROOT_PART"

            # ── 2. Format ──
            step "Formatting partitions"
            # Only wipe EFI in full wipe mode — replace mode keeps the existing EFI untouched
            if [ "$PARTITION_MODE" = "wipe" ]; then
                run mkfs.vfat -F32 "$EFI_PART" || die "Failed to format EFI $EFI_PART"
            else
                echo "  » Skipping EFI format (replace mode — existing EFI preserved)"
            fi
            case "$TARGET_FS" in
                ext4)  run mkfs.ext4  -F  "$ROOT_PART" || die "Failed to format root" ;;
                xfs)   run mkfs.xfs   -f  "$ROOT_PART" || die "Failed to format root" ;;
                btrfs) run mkfs.btrfs -f  "$ROOT_PART" || die "Failed to format root" ;;
            esac

            # ── 3. Mount ──
            step "Mounting partitions"
            umount -R /mnt 2>/dev/null || true
            run mount "$ROOT_PART" /mnt || die "Failed to mount root"
            mkdir -p /mnt/boot
            run mount "$EFI_PART" /mnt/boot || die "Failed to mount EFI to /boot"

            for drive in "${SECONDARY_DRIVES[@]}"; do
                dname=$(basename "$drive")
                mpoint="${MOUNT_POINTS["$drive"]}"
                [ -z "$mpoint" ] && mpoint="/mnt/$dname"
                # Strip leading / so we mount under /mnt properly
                mpoint_rel=$(echo "$mpoint" | sed 's|^/||')
                target_mpoint="/mnt/$mpoint_rel"
                mkdir -p "$target_mpoint"

                fstype=$(blkid -o value -s TYPE "$drive" 2>/dev/null || echo "ext4")
                if [[ "$fstype" == "ntfs" || "$fstype" == "vfat" || "$fstype" == "exfat" ]]; then
                    run mount -o uid=1000,gid=1000,rw,user,exec,umask=0000,dmask=0000,fmask=0000 "$drive" "$target_mpoint" || true
                else
                    run mount "$drive" "$target_mpoint" || true
                    chmod 777 "$target_mpoint" 2>/dev/null || true
                    chown -R 1000:1000 "$target_mpoint" 2>/dev/null || true
                fi

            done

            # ── 5. fstab ──
            step "Generating fstab"
            genfstab -U /mnt > /mnt/etc/fstab && cat /mnt/etc/fstab | tee -a "$LOG" || die "genfstab failed"

            # Post-process fstab to ensure all secondary drives have correct permissions and nofail flags for Steam Proton
            for drive in "${SECONDARY_DRIVES[@]}"; do
                mpoint="${MOUNT_POINTS["$drive"]}"
                [ -z "$mpoint" ] && mpoint="/$(basename "$drive")"
                drive_uuid=$(blkid -o value -s UUID "$drive" 2>/dev/null || echo "")
                fstype=$(blkid -o value -s TYPE "$drive" 2>/dev/null || echo "ext4")
                
                # Make sure the mount point directory inside /mnt has full ownership & permissions
                target_mpoint="/mnt$(echo "$mpoint" | sed 's|^/||')"
                mkdir -p "$target_mpoint"
                if [[ "$fstype" == "ntfs" || "$fstype" == "vfat" || "$fstype" == "exfat" ]]; then
                    chmod 777 "$target_mpoint" 2>/dev/null || true
                else
                    chmod 777 "$target_mpoint" 2>/dev/null || true
                    chown -R 1000:1000 "$target_mpoint" 2>/dev/null || true
                fi

                if [ -n "$drive_uuid" ]; then
                    # Remove any weak default genfstab entry for this UUID if present
                    sed -i "/UUID=$drive_uuid/d" /mnt/etc/fstab
                    if [[ "$fstype" == "ntfs" || "$fstype" == "vfat" || "$fstype" == "exfat" ]]; then
                        echo "UUID=$drive_uuid $mpoint $fstype uid=1000,gid=1000,rw,user,exec,umask=0000,dmask=0000,fmask=0000,nofail,x-systemd.device-timeout=5s 0 0" >> /mnt/etc/fstab
                    else
                        echo "UUID=$drive_uuid $mpoint $fstype defaults,exec,rw,nofail,x-systemd.device-timeout=5s 0 2" >> /mnt/etc/fstab
                    fi
                fi
            done
            echo "--- Final /etc/fstab ---"
            cat /mnt/etc/fstab | tee -a "$LOG"

            # ── 6. Timezone & locale ──
            step "Timezone and locale"
            run ln -sf "/usr/share/zoneinfo/$TZ" /mnt/etc/localtime
            run arch-chroot /mnt hwclock --systohc
            sed -i "s|#\?${SELECTED_LOCALE}|${SELECTED_LOCALE}|" /mnt/etc/locale.gen
            # Always ensure en_US.UTF-8 is enabled as fallback
            sed -i 's|#\?en_US.UTF-8 UTF-8|en_US.UTF-8 UTF-8|' /mnt/etc/locale.gen
            run arch-chroot /mnt locale-gen
            LOCALE_NAME=$(echo "$SELECTED_LOCALE" | awk '{print $1}')
            # Write complete locale.conf — LANG + LC_ALL ensures UTF-8 everywhere
            printf 'LANG=%s\nLC_ALL=%s\nLC_CTYPE=%s\n' \
                "$LOCALE_NAME" "$LOCALE_NAME" "$LOCALE_NAME" > /mnt/etc/locale.conf
            printf 'export LANG=%s\nexport LC_ALL=%s\n' \
                "$LOCALE_NAME" "$LOCALE_NAME" > /mnt/etc/profile.d/locale.sh
            chmod +x /mnt/etc/profile.d/locale.sh
            # Write /etc/environment — required for Qt6/KDE apps to pick up correct theme and encoding
            cat > /mnt/etc/environment << 'ENVEOF'
LANG=en_US.UTF-8
LC_ALL=en_US.UTF-8
QT_QPA_PLATFORMTHEME=kde
XDG_CURRENT_DESKTOP=KDE
XDG_MENU_PREFIX=kde-
ENVEOF
            echo "Locale set to: $LOCALE_NAME"

            # ── 7. Hostname ──
            step "Hostname"
            echo "$HOSTNAME" > /mnt/etc/hostname
            printf '127.0.0.1\tlocalhost\n::1\t\tlocalhost\n127.0.1.1\t%s.%s %s\n' \
                "$HOSTNAME" "$DOMAIN" "$HOSTNAME" > /mnt/etc/hosts
            [ -n "$SELECTED_KEYMAP" ] && echo "KEYMAP=${SELECTED_KEYMAP}" > /mnt/etc/vconsole.conf

            # ── 8. Pacman multilib + users + passwords ──
            step "Enabling multilib"
            sed -i '/\[multilib\]/,/Include/s/^#//' /mnt/etc/pacman.conf
            run arch-chroot /mnt pacman -Sy --noconfirm

            step "Setting passwords and users"
            echo "root:${ROOT_PASS}" | arch-chroot /mnt chpasswd
            sed -i 's/# %wheel ALL=(ALL:ALL) ALL/%wheel ALL=(ALL:ALL) ALL/' /mnt/etc/sudoers

            for u in "${USERS[@]}"; do
                pass="${USER_PASSWORDS[$u]}"
                run arch-chroot /mnt useradd -m -G wheel -s /bin/zsh "$u" || true
                echo "${u}:${pass}" | arch-chroot /mnt chpasswd
            done

            if [ "$PASSWORDLESS_SUDO" = "true" ]; then
                mkdir -p /mnt/etc/sudoers.d
                echo "%wheel ALL=(ALL:ALL) NOPASSWD: ALL" > /mnt/etc/sudoers.d/gotunos
                chmod 440 /mnt/etc/sudoers.d/gotunos
            fi

            # ── 9. Bootloader — installed AFTER packages so microcode is present ──
            # (moving bootloader here ensures intel-ucode/amd-ucode are in /boot when grub-mkconfig runs)

            # ── 10. KDE Plasma + drivers ──
            PKGS="plasma-meta sddm plasma-nm plasma-pa bluez bluez-utils bluedevil pipewire \
wireplumber pipewire-audio pipewire-alsa pipewire-pulse pipewire-jack \
power-profiles-daemon dolphin konsole"
            [ "$CPU_TYPE" = "Intel" ] && PKGS="$PKGS intel-ucode"
            [ "$CPU_TYPE" = "AMD" ]   && PKGS="$PKGS amd-ucode"
            case "$GPU_DRIVER" in
                "AMD"|"AMD iGPU")                              PKGS="$PKGS xf86-video-amdgpu mesa vulkan-radeon lib32-vulkan-radeon" ;;
                # egl-wayland is required for NVIDIA + Wayland (Plasma 6) to work without black screens
                "NVIDIA Turing+ (16xx+) (open-source DKMS)")  PKGS="$PKGS nvidia-open-dkms nvidia-utils lib32-nvidia-utils nvidia-settings nvidia-prime egl-wayland" ;;
                "Legacy NVIDIA (proprietary DKMS)")            PKGS="$PKGS nvidia-dkms nvidia-utils lib32-nvidia-utils nvidia-settings nvidia-prime egl-wayland" ;;
                "Intel"|"Intel iGPU")                         PKGS="$PKGS xf86-video-intel mesa vulkan-intel lib32-vulkan-intel" ;;
            esac
            [ "$TARGET_FS" = "btrfs" ] && PKGS="$PKGS btrfs-progs"
            # btrfs-assistant (AUR) installed later in AUR step — add it here so it's flagged
            [ "$TARGET_FS" = "btrfs" ] && AUR_BTRFS=true || AUR_BTRFS=false

            step "Installing KDE Plasma and drivers"
            run arch-chroot /mnt pacman -S --noconfirm --overwrite '*' $PKGS \
                || die "Desktop install failed"

            # Remove unwanted packages
            for rpkg in plasma-welcome malcontent discover micro winetricks stacer scrcpy pavucontrol; do
                arch-chroot /mnt pacman -Rdd --noconfirm "$rpkg" >> "$LOG" 2>&1 || true
            done

            step "Enabling services"
            run arch-chroot /mnt systemctl enable sddm NetworkManager bluetooth power-profiles-daemon

            cp -a /etc/NetworkManager/system-connections/. /mnt/etc/NetworkManager/system-connections/ 2>/dev/null || true

            if [ -n "$DNS_IP1" ]; then
                mkdir -p /mnt/etc/systemd
                printf '[Resolve]\nDNS=%s#%s %s#%s\nDNSOverTLS=yes\n' \
                    "$DNS_IP1" "$DNS_HOST" "$DNS_IP2" "$DNS_HOST" > /mnt/etc/systemd/resolved.conf
                arch-chroot /mnt systemctl enable systemd-resolved >> "$LOG" 2>&1
                ln -sf ../run/systemd/resolve/stub-resolv.conf /mnt/etc/resolv.conf
            fi

            # ── NVIDIA early KMS — prevents black screen on Wayland/SDDM ──
            if [[ "$GPU_DRIVER" == *"NVIDIA"* ]]; then
                step "Configuring NVIDIA early KMS modules"
                # Directly replace MODULES line to avoid BRE backreference issues with empty parens
                sed -i 's/^MODULES=(.*/MODULES=(nvidia nvidia_modeset nvidia_uvm nvidia_drm)/' /mnt/etc/mkinitcpio.conf
            fi

            # ── Plymouth Splash Configuration ──
            if grep -q "autodetect" /mnt/etc/mkinitcpio.conf; then
                sed -i 's/autodetect/autodetect plymouth/' /mnt/etc/mkinitcpio.conf
            fi

            step "Building initramfs with Plymouth splash support"
            run arch-chroot /mnt plymouth-set-default-theme -R spinner 2>/dev/null || true
            run arch-chroot /mnt mkinitcpio -P || die "mkinitcpio failed"
            arch-chroot /mnt chsh -s /bin/zsh root >> "$LOG" 2>&1 || true

            # ── Now install bootloader — microcode is in /boot, grub-mkconfig will detect it ──
            step "Installing bootloader"
            if [ "$BOOTLOADER" = "systemd-boot" ]; then
                # No --no-variables so bootctl writes to NVRAM and machine boots seamlessly
                run arch-chroot /mnt bootctl install --esp-path=/boot || die "bootctl install failed"
                ROOT_UUID=$(blkid -o value -s UUID "$ROOT_PART")
                echo "Root UUID: $ROOT_UUID"
                mkdir -p /mnt/boot/loader/entries
                printf 'default linux-zen.conf\ntimeout 4\nconsole-mode max\neditor no\n' \
                    > /mnt/boot/loader/loader.conf
                printf 'title   GötünOS\nlinux   /vmlinuz-linux-zen\ninitrd  /initramfs-linux-zen.img\noptions root=UUID=%s rw quiet splash plymouth.ignore-serial-consoles\n' \
                    "$ROOT_UUID" > /mnt/boot/loader/entries/linux-zen.conf
                printf 'title   GötünOS (LTS)\nlinux   /vmlinuz-linux-lts\ninitrd  /initramfs-linux-lts.img\noptions root=UUID=%s rw quiet splash plymouth.ignore-serial-consoles\n' \
                    "$ROOT_UUID" > /mnt/boot/loader/entries/linux-lts.conf
                if [ "$CPU_TYPE" = "Intel" ]; then
                    sed -i '/vmlinuz-linux-zen/a initrd  /intel-ucode.img' /mnt/boot/loader/entries/linux-zen.conf
                    sed -i '/vmlinuz-linux-lts/a initrd  /intel-ucode.img' /mnt/boot/loader/entries/linux-lts.conf
                elif [ "$CPU_TYPE" = "AMD" ]; then
                    sed -i '/vmlinuz-linux-zen/a initrd  /amd-ucode.img' /mnt/boot/loader/entries/linux-zen.conf
                    sed -i '/vmlinuz-linux-lts/a initrd  /amd-ucode.img' /mnt/boot/loader/entries/linux-lts.conf
                fi
                if [ "$GPU_DRIVER" = "AMD" ] || [ "$GPU_DRIVER" = "AMD iGPU" ]; then
                    sed -i 's/options root=.*/& amdgpu.ppfeaturemask=0xffffffff/' /mnt/boot/loader/entries/linux-zen.conf
                    sed -i 's/options root=.*/& amdgpu.ppfeaturemask=0xffffffff/' /mnt/boot/loader/entries/linux-lts.conf
                fi
                if [[ "$GPU_DRIVER" == *"NVIDIA"* ]]; then
                    # nvidia_drm.modeset=1 enables DRM kernel mode setting (required for Wayland)
                    # nvidia_drm.fbdev=1   fixes black screen on Plasma 6 Wayland (the critical missing param)
                    sed -i 's/options \(.*\)/options \1 nvidia_drm.modeset=1 nvidia_drm.fbdev=1/' /mnt/boot/loader/entries/linux-zen.conf
                    sed -i 's/options \(.*\)/options \1 nvidia_drm.modeset=1 nvidia_drm.fbdev=1/' /mnt/boot/loader/entries/linux-lts.conf
                fi
                echo "Boot entries:"
                ls -la /mnt/boot/loader/entries/
            else
                run arch-chroot /mnt pacman -S --noconfirm --overwrite '*' grub efibootmgr os-prober
                run arch-chroot /mnt grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=GötünOS
                sed -i 's/#GRUB_DISABLE_OS_PROBER=false/GRUB_DISABLE_OS_PROBER=false/' /mnt/etc/default/grub
                if [[ "$GPU_DRIVER" == *"NVIDIA"* ]]; then
                    # nvidia_drm.modeset=1 + nvidia_drm.fbdev=1 required for Plasma 6 Wayland
                    sed -i 's/GRUB_CMDLINE_LINUX_DEFAULT="\(.*\)"/GRUB_CMDLINE_LINUX_DEFAULT="\1 nvidia_drm.modeset=1 nvidia_drm.fbdev=1"/' /mnt/etc/default/grub
                fi
                run arch-chroot /mnt grub-mkconfig -o /boot/grub/grub.cfg || die "grub-mkconfig failed"
            fi

            mkdir -p /mnt/etc/sddm.conf.d
            printf '[Theme]\nCurrent=breeze\n' > /mnt/etc/sddm.conf.d/kde_settings.conf
            [ -f /mnt/usr/share/xsessions/plasma.desktop ] && \
                mv /mnt/usr/share/xsessions/plasma.desktop /mnt/usr/share/xsessions/plasma.desktop.bak
            ln -sf /dev/null /mnt/etc/systemd/user/plasma-krunner.service
            sed -i 's/#\?DefaultTimeoutStopSec=.*/DefaultTimeoutStopSec=20s/' /mnt/etc/systemd/system.conf

            mkdir -p /mnt/etc/pacman.d/hooks
            cat > /mnt/etc/pacman.d/hooks/clean_cache.hook <<'EOF'
[Trigger]
Operation = Upgrade
Operation = Install
Operation = Remove
Type = Package
Target = *
[Action]
Description = Cleaning pacman cache...
When = PostTransaction
Exec = /usr/bin/paccache -rk0
EOF

            # ── 11. yay + AUR ──
            USER1="${USERS[0]}"
            mkdir -p /mnt/etc/sudoers.d
            echo "$USER1 ALL=(ALL) NOPASSWD: ALL" > /mnt/etc/sudoers.d/temp_install
            chmod 440 /mnt/etc/sudoers.d/temp_install

            step "Building yay AUR helper (compiling...)"
            # Run without gum spin so compilation output is visible
            run arch-chroot /mnt /bin/bash -c \
                "cd /tmp && rm -rf yay-bin && git clone https://aur.archlinux.org/yay-bin.git && \
                 chown -R ${USER1}:${USER1} yay-bin && \
                 cd yay-bin && sudo -u ${USER1} makepkg -si --noconfirm" || die "yay install failed"

            AUR_APPS=(
                ark atlauncher-bin bazaar shelly-bin rs-pug-git obsidian-bin vesktop-bin
                zen-browser-bin coolercontrol-bin filelight krita goverlay protonplus
                protontricks faugus-launcher kitty kate kdeconnect localsend-bin gparted
                popsicle-bin hardinfo2 okular openrgb proton-vpn-gtk-app fastfetch
                heroic-games-launcher-bin steam onlyoffice-bin winboat-bin easyeffects
                gamemode zapzap waydroid kcalc thunderbird flatseal sunshine-bin
                supertuxkart bleachbit stacer-bin fuse2 hydra-launcher-bin gnome-boxes
                woeusb input-remapper arch-update lutris
                plasma6-wallpapers-smart-video-wallpaper-reborn
                wallpaper-engine-kde-plugin-git optiscaler-client fresh-editor-bin furmark2-bin
            )
            [ "$GPU_DRIVER" = "AMD" ] || [ "$GPU_DRIVER" = "AMD iGPU" ] && AUR_APPS+=(lact)
            [[ "$GPU_DRIVER" == *"NVIDIA"* ]] && AUR_APPS+=(greenwithenvy)
            [ "$RAZER_SUPPORT" = "true" ] && AUR_APPS+=(openrazer-driver-dkms polychromatic)
            # btrfs-assistant for snapshot GUI (one of the main reasons to pick btrfs)
            [ "$AUR_BTRFS" = "true" ] && AUR_APPS+=(btrfs-assistant timeshift)

            step "Installing AUR packages (takes a while, displaying progress...)"
            AUR_FAILED=()
            for app in "${AUR_APPS[@]}"; do
                echo "  » Compiling/Installing AUR package: $app"
                if ! arch-chroot /mnt sudo -u "${USER1}" yay -S --noconfirm --overwrite '*' "$app" >> "$LOG" 2>&1; then
                    AUR_FAILED+=("$app")
                    gum style --foreground 3 "  ⚠ AUR: $app failed — continuing with remaining packages"
                fi
            done
            if [ ${#AUR_FAILED[@]} -gt 0 ]; then
                gum style --foreground 1 "AUR build failures (${#AUR_FAILED[@]} package(s)):"
                for f in "${AUR_FAILED[@]}"; do
                    gum style --foreground 1 "  ✗ $f"
                done
                echo "AUR_FAILED: ${AUR_FAILED[*]}" >> "$LOG"
            else
                gum style --foreground 2 "All AUR packages installed successfully."
            fi

            rm -f /mnt/etc/sudoers.d/temp_install

            [ "$GPU_DRIVER" = "AMD" ] || [ "$GPU_DRIVER" = "AMD iGPU" ] && \
                arch-chroot /mnt systemctl enable lactd >> "$LOG" 2>&1 || true
            if [ "$RAZER_SUPPORT" = "true" ]; then
                for u in "${USERS[@]}"; do
                    arch-chroot /mnt gpasswd -a "$u" openrazer >> "$LOG" 2>&1 || true
                done
            fi

            # ── 12. Flatpaks ──
            step "Installing Flatpak apps from Flathub"
            run arch-chroot /mnt flatpak remote-add --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo
            for fp in rocks.shy.VacuumTube com.dec05eba.gpu_screen_recorder me.proton.Pass \
                      com.rafaelmardojai.Blanket com.spotify.Client \
                      net.retrodeck.retrodeck net.shadps4.shadPS4 \
                      com.stremio.Stremio; do
                echo "  » Installing Flatpak: $fp"
                run arch-chroot /mnt flatpak install -y flathub "$fp"
            done

            # ── 13. Per-user setup ──
            # Copy shortcuts template to target
            mkdir -p /mnt/usr/share/gotunos-installer
            if [ -f "/usr/share/gotunos-installer/gotunshortcuts.kksrc" ]; then
                cp "/usr/share/gotunos-installer/gotunshortcuts.kksrc" /mnt/usr/share/gotunos-installer/gotunshortcuts.kksrc
            fi

            # Write python script to parse shortcuts
            cat > /mnt/tmp/import_shortcuts.py <<'EOF'
import sys
import os

src = '/usr/share/gotunos-installer/gotunshortcuts.kksrc'
if not os.path.exists(src):
    sys.exit(0)

target_globals = sys.argv[1]
target_shortcuts = sys.argv[2]

globals_sec = []
shortcuts_sec = []
current_target = None

with open(src, 'r') as f:
    for line in f:
        line_stripped = line.strip()
        if line_stripped.startswith('[') and line_stripped.endswith(']'):
            if line_stripped == '[StandardShortcuts]':
                current_target = 'globals'
                globals_sec.append(line)
            elif '][Global Shortcuts]' in line_stripped:
                current_target = 'shortcuts'
                clean_header = line_stripped.replace('][Global Shortcuts]', '')
                shortcuts_sec.append(clean_header + '\n')
            else:
                current_target = 'shortcuts'
                shortcuts_sec.append(line)
        else:
            if current_target == 'globals':
                globals_sec.append(line)
            elif current_target == 'shortcuts':
                shortcuts_sec.append(line)

os.makedirs(os.path.dirname(target_globals), exist_ok=True)
os.makedirs(os.path.dirname(target_shortcuts), exist_ok=True)

with open(target_globals, 'a') as f:
    f.write(''.join(globals_sec))

with open(target_shortcuts, 'a') as f:
    f.write(''.join(shortcuts_sec))
EOF

            for u in "${USERS[@]}"; do
                mkdir -p "/mnt/home/$u/.config/autostart"
                cat > "/mnt/home/$u/.config/autostart/steam-gamepadui.desktop" <<'EOF'
[Desktop Entry]
Name=Steam Gamepad UI
Exec=steam -gamepadui
Icon=steam
Terminal=false
Type=Application
Categories=Game;
EOF
                # Write KDE Dark Mode config with complete Breeze Dark color tokens
                # Applications need the [Colors:*] sections to actually render dark — missing them causes white windows
                mkdir -p "/mnt/home/$u/.config"
                cat > "/mnt/home/$u/.config/kdeglobals" <<'EOF'
[ColorEffects:Disabled]
ChangeSelectionColor=true
Color=56,56,56
ColorAmount=0
ColorEffect=0
ContrastAmount=0.65
ContrastEffect=1
IntensityAmount=0.1
IntensityEffect=2

[ColorEffects:Inactive]
ChangeSelectionColor=true
Color=112,111,110
ColorAmount=0.025
ColorEffect=2
ContrastAmount=0.1
ContrastEffect=2
Enable=false
IntensityAmount=0
IntensityEffect=0

[Colors:Button]
BackgroundAlternate=30,87,116
BackgroundNormal=49,54,59
DecorationFocus=61,174,233
DecorationHover=61,174,233
ForegroundActive=61,174,233
ForegroundInactive=161,169,177
ForegroundLink=29,153,243
ForegroundNegative=218,68,83
ForegroundNeutral=246,116,0
ForegroundNormal=252,252,252
ForegroundPositive=39,174,96
ForegroundVisited=155,89,182

[Colors:Complementary]
BackgroundAlternate=22,24,35
BackgroundNormal=30,33,48
DecorationFocus=61,174,233
DecorationHover=61,174,233
ForegroundActive=61,174,233
ForegroundInactive=161,169,177
ForegroundLink=29,153,243
ForegroundNegative=218,68,83
ForegroundNeutral=246,116,0
ForegroundNormal=252,252,252
ForegroundPositive=39,174,96
ForegroundVisited=155,89,182

[Colors:Header]
BackgroundAlternate=42,46,50
BackgroundNormal=49,54,59
DecorationFocus=61,174,233
DecorationHover=61,174,233
ForegroundActive=61,174,233
ForegroundInactive=161,169,177
ForegroundLink=29,153,243
ForegroundNegative=218,68,83
ForegroundNeutral=246,116,0
ForegroundNormal=252,252,252
ForegroundPositive=39,174,96
ForegroundVisited=155,89,182

[Colors:Selection]
BackgroundAlternate=30,87,116
BackgroundNormal=61,174,233
DecorationFocus=61,174,233
DecorationHover=61,174,233
ForegroundActive=252,252,252
ForegroundInactive=161,169,177
ForegroundLink=253,188,75
ForegroundNegative=218,68,83
ForegroundNeutral=246,116,0
ForegroundNormal=252,252,252
ForegroundPositive=39,174,96
ForegroundVisited=155,89,182

[Colors:Tooltip]
BackgroundAlternate=42,46,50
BackgroundNormal=49,54,59
DecorationFocus=61,174,233
DecorationHover=61,174,233
ForegroundActive=61,174,233
ForegroundInactive=161,169,177
ForegroundLink=29,153,243
ForegroundNegative=218,68,83
ForegroundNeutral=246,116,0
ForegroundNormal=252,252,252
ForegroundPositive=39,174,96
ForegroundVisited=155,89,182

[Colors:View]
BackgroundAlternate=27,30,32
BackgroundNormal=35,38,41
DecorationFocus=61,174,233
DecorationHover=61,174,233
ForegroundActive=61,174,233
ForegroundInactive=161,169,177
ForegroundLink=29,153,243
ForegroundNegative=218,68,83
ForegroundNeutral=246,116,0
ForegroundNormal=252,252,252
ForegroundPositive=39,174,96
ForegroundVisited=155,89,182

[Colors:Window]
BackgroundAlternate=42,46,50
BackgroundNormal=49,54,59
DecorationFocus=61,174,233
DecorationHover=61,174,233
ForegroundActive=61,174,233
ForegroundInactive=161,169,177
ForegroundLink=29,153,243
ForegroundNegative=218,68,83
ForegroundNeutral=246,116,0
ForegroundNormal=252,252,252
ForegroundPositive=39,174,96
ForegroundVisited=155,89,182

[General]
ColorScheme=BreezeDark
Name=Breeze Dark
ThemeVersion=1
font=JetBrainsMono Nerd Font,10,-1,5,50,0,0,0,0,0
fixedWidthFont=JetBrainsMono Nerd Font,10,-1,5,50,0,0,0,0,0
toolBarFont=JetBrainsMono Nerd Font,10,-1,5,50,0,0,0,0,0
menuFont=JetBrainsMono Nerd Font,10,-1,5,50,0,0,0,0,0
smallestReadableFont=JetBrainsMono Nerd Font,8,-1,5,50,0,0,0,0,0
activeFont=JetBrainsMono Nerd Font,10,-1,5,50,0,0,0,0,0

[KDE]
LookAndFeelPackage=org.kde.breezedark.desktop
widgetStyle=Breeze
EOF
                # Apply it to the skeleton directory so new users also get dark theme
                mkdir -p /mnt/etc/skel/.config
                cp "/mnt/home/$u/.config/kdeglobals" /mnt/etc/skel/.config/kdeglobals

                # env HOME fixes oh-my-zsh installing to wrong directory
                arch-chroot /mnt sudo -u "$u" env HOME="/home/$u" \
                    sh -c "$(wget -qO- https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" \
                    "" --unattended >> "$LOG" 2>&1 || true
                echo "fastfetch" >> "/mnt/home/$u/.zshrc"
                # Must run chown inside chroot so $u resolves against the target /etc/passwd, not the live ISO
                arch-chroot /mnt chown -R "$u:$u" "/home/$u"
            done
            arch-chroot /mnt env HOME="/root" sh -c \
                "$(wget -qO- https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" \
                "" --unattended >> "$LOG" 2>&1 || true
            echo "fastfetch" >> /mnt/root/.zshrc

            if [ "$ZRAM_ENABLED" = "true" ]; then
                printf '[zram0]\nzram-size = %s\ncompression-algorithm = zstd\n' \
                    "$((ZRAM_SIZE * 1024))" > /mnt/etc/systemd/zram-generator.conf
            fi
            for drive in "${SYMLINK_DRIVES[@]}"; do
                linkname="${SYMLINK_PATHS["$drive"]}"
                mpoint="${MOUNT_POINTS["$drive"]}"
                [ -z "$mpoint" ] && mpoint="/mnt/$(basename "$drive")"
                for u in "${USERS[@]}"; do
                    tlink="/mnt/home/$u/$linkname"
                    mkdir -p "$(dirname "$tlink")"
                    ln -sf "$mpoint" "$tlink"
                    chown -h "$u:$u" "$tlink"
                done
            done

            # ── 15. First-boot Decky service ──
            FBOOT_USER="${USER1}"
            mkdir -p /mnt/usr/local/bin
            {
                printf '#!/usr/bin/env bash\n'
                printf 'GAMING_USER="%s"\n' "$FBOOT_USER"
                printf 'STATE=/var/lib/gotunos-firstboot.state\n'
                printf 'notify() {\n'
                printf '    uid=$(id -u "$GAMING_USER" 2>/dev/null) || return\n'
                printf '    [ -S /run/user/${uid}/bus ] &&\\\n'
                printf '        sudo -u "$GAMING_USER" DBUS_SESSION_BUS_ADDRESS="unix:path=/run/user/${uid}/bus"\\\n'
                printf '        notify-send "GotunOS Setup" "$1" 2>/dev/null || true\n'
                printf '}\n'
                printf 'powerprofilesctl set performance 2>/dev/null || true\n'
                printf 'if [ ! -f "$STATE" ]; then\n'
                printf '    echo 1 > "$STATE"\n'
                printf '    (\n'
                printf '      for i in $(seq 1 30); do\n'
                printf '          uid=$(id -u "$GAMING_USER" 2>/dev/null)\n'
                printf '          if [ -n "$uid" ] && [ -S "/run/user/${uid}/bus" ]; then break; fi\n'
                printf '          sleep 2\n'
                printf '      done\n'
                printf '      uid=$(id -u "$GAMING_USER" 2>/dev/null)\n'
                printf '      if [ -n "$uid" ] && [ -S "/run/user/${uid}/bus" ]; then\n'
                printf '          sudo -u "$GAMING_USER" DBUS_SESSION_BUS_ADDRESS="unix:path=/run/user/${uid}/bus"\\\n'
                printf '              lookandfeeltool -a org.kde.breezedark.desktop 2>/dev/null || true\n'
                printf '      fi\n'
                printf '      sleep 240\n'
                printf '      notify "Downloading Decky Loader..."\n'
                printf '      wget -O /tmp/decky.sh https://github.com/SteamDeckHomebrew/decky-installer/releases/latest/download/install_release.sh\n'
                printf '      export SUDO_USER="$GAMING_USER"\n'
                printf '      if sh /tmp/decky.sh; then\n'
                printf '        notify "Decky Loader installed!"\n'
                printf '      else\n'
                printf '        notify "Decky install failed - will retry next boot."\n'
                printf '      fi\n'
                printf '      rm -f /tmp/decky.sh\n'
                printf '    ) &\n'
                printf 'else\n'
                printf '    if systemctl is-active --quiet pluginloader; then\n'
                printf '        systemctl disable gotunos-firstboot.service 2>/dev/null\n'
                printf '        rm -f /etc/systemd/system/gotunos-firstboot.service "$STATE" "$0"\n'
                printf '    else\n'
                printf '        ( sleep 60\n'
                printf '          wget -O /tmp/decky.sh https://github.com/SteamDeckHomebrew/decky-installer/releases/latest/download/install_release.sh\n'
                printf '          export SUDO_USER="$GAMING_USER"\n'
                printf '          if sh /tmp/decky.sh; then\n'
                printf '              notify "Decky Loader installed!"\n'
                printf '              systemctl disable gotunos-firstboot.service 2>/dev/null\n'
                printf '              rm -f /etc/systemd/system/gotunos-firstboot.service "$STATE" "$0"\n'
                printf '          fi\n'
                printf '          rm -f /tmp/decky.sh ) &\n'
                printf '    fi\n'
                printf 'fi\n'
            } > /mnt/usr/local/bin/gotunos-firstboot.sh
            chmod +x /mnt/usr/local/bin/gotunos-firstboot.sh
            cat > /mnt/etc/systemd/system/gotunos-firstboot.service << 'SVCEOF'
[Unit]
Description=GötünOS First Boot Setup
After=network-online.target graphical.target
Wants=network-online.target graphical.target
[Service]
Type=oneshot
RemainAfterExit=yes
ExecStart=/usr/local/bin/gotunos-firstboot.sh
[Install]
WantedBy=graphical.target
SVCEOF
            arch-chroot /mnt systemctl enable gotunos-firstboot.service >> "$LOG" 2>&1 || true

            # ── 16. Branding ──
            cat > /mnt/etc/os-release <<'EOF'
NAME="GötünOS"
ID=gotunos
ID_LIKE=arch
PRETTY_NAME="GötünOS"
ANSI_COLOR="0;36"
HOME_URL="https://gotunos.org"
EOF
            cat > /mnt/etc/lsb-release <<'EOF'
DISTRIB_ID=GötünOS
DISTRIB_RELEASE=12
DISTRIB_DESCRIPTION="GötünOS"
EOF
            mkdir -p /mnt/usr/share/gotunos-installer
            cat > /mnt/usr/share/gotunos-installer/rebrand.sh <<'EOF'
#!/bin/sh
cat > /etc/os-release <<'INNER'
NAME="GötünOS"
ID=gotunos
ID_LIKE=arch
PRETTY_NAME="GötünOS"
ANSI_COLOR="0;36"
HOME_URL="https://gotunos.org"
INNER
EOF
            chmod +x /mnt/usr/share/gotunos-installer/rebrand.sh
            cat > /mnt/etc/pacman.d/hooks/gotunos-branding.hook <<'EOF'
[Trigger]
Operation = Install
Operation = Upgrade
Type = Package
Target = filesystem
[Action]
Description = Re-applying GötünOS branding...
When = PostTransaction
Exec = /usr/share/gotunos-installer/rebrand.sh
EOF
            if [ -f /usr/share/gotunos-installer/gotunos_logo.txt ]; then
                cp /usr/share/gotunos-installer/gotunos_logo.txt /mnt/etc/issue
                cp /usr/share/gotunos-installer/gotunos_logo.txt /mnt/etc/motd
            fi

            # Preserve install log in the new system so it survives the live session ending
            mkdir -p /mnt/var/log
            cp "$LOG" /mnt/var/log/gotunos_install.log 2>/dev/null || true
            gum style --foreground 2 "Installation complete!"
            STEP=11
            ;;
        11)
            gum style --foreground 2 --border double --align center --width 60 "$(get_text SUCCESS_TITLE)"
            gum style "$(get_text SUCCESS_TEXT)"
            gum style --foreground 3 "$(get_text REBOOT_PROMPT)"
            REBOOT_PICK=$(gum choose "Reboot Now" "Exit to Shell")
            if [ "$REBOOT_PICK" = "Reboot Now" ]; then
                reboot
            else
                exit 0
            fi
            ;;
    esac
done
