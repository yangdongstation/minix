#!/bin/sh
#
# MINIX 3 Setup Script for RISC-V 64-bit
#
# This script is run during system installation to configure
# the newly installed MINIX system.
#

# Configuration
HOSTNAME="minix-riscv64"
ROOT_DEV="/dev/c0d0"
ROOT_PART="${ROOT_DEV}p0"
USR_PART="${ROOT_DEV}p1"
HOME_PART="${ROOT_DEV}p2"

# Colors (if terminal supports them)
if [ -t 1 ]; then
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    YELLOW='\033[1;33m'
    BLUE='\033[0;34m'
    NC='\033[0m'
else
    RED=''
    GREEN=''
    YELLOW=''
    BLUE=''
    NC=''
fi

echo_info() {
    echo "${GREEN}[INFO]${NC} $1"
}

echo_warn() {
    echo "${YELLOW}[WARN]${NC} $1"
}

echo_error() {
    echo "${RED}[ERROR]${NC} $1"
}

# Print banner
print_banner() {
    cat << 'EOF'

  __  __ ___ _   _ _____  __  _____
 |  \/  |_ _| \ | |_ _\ \/ / |___ /
 | |\/| || ||  \| || | \  /    |_ \
 | |  | || || |\  || | /  \   ___) |
 |_|  |_|___|_| \_|___/_/\_\ |____/

       RISC-V 64-bit Edition

EOF
    echo "Welcome to MINIX 3 Setup"
    echo "========================"
    echo ""
}

# Check if running as root
check_root() {
    if [ "$(id -u)" != "0" ]; then
        echo_error "This script must be run as root"
        exit 1
    fi
}

# Detect hardware
detect_hardware() {
    echo_info "Detecting hardware..."

    # Get CPU info
    if [ -f /proc/cpuinfo ]; then
        NCPU=$(grep -c "^processor" /proc/cpuinfo 2>/dev/null || echo 1)
    else
        NCPU=1
    fi
    echo "  CPUs detected: $NCPU"

    # Get memory
    if [ -f /proc/meminfo ]; then
        MEM=$(grep "MemTotal" /proc/meminfo 2>/dev/null | awk '{print $2}')
        if [ -n "$MEM" ]; then
            MEM_MB=$((MEM / 1024))
            echo "  Memory: ${MEM_MB} MB"
        fi
    fi

    # Detect disks
    echo "  Disks:"
    for disk in /dev/c[0-9]d[0-9]; do
        if [ -b "$disk" ]; then
            echo "    $disk"
        fi
    done
}

# Configure network
configure_network() {
    echo ""
    echo_info "Network Configuration"
    echo ""

    echo "Configure network? (y/n) [n]: "
    read answer

    if [ "$answer" = "y" ] || [ "$answer" = "Y" ]; then
        echo "Enter hostname [$HOSTNAME]: "
        read new_hostname
        if [ -n "$new_hostname" ]; then
            HOSTNAME="$new_hostname"
        fi

        echo "$HOSTNAME" > /etc/hostname
        echo "127.0.0.1 localhost $HOSTNAME" > /etc/hosts

        echo "Use DHCP? (y/n) [y]: "
        read use_dhcp

        if [ "$use_dhcp" != "n" ] && [ "$use_dhcp" != "N" ]; then
            # DHCP configuration
            cat > /etc/rc.conf.d/network << EOF
# Network configuration
ifconfig_eth0="DHCP"
EOF
        else
            echo "Enter IP address: "
            read ip_addr
            echo "Enter netmask [255.255.255.0]: "
            read netmask
            netmask="${netmask:-255.255.255.0}"
            echo "Enter gateway: "
            read gateway

            cat > /etc/rc.conf.d/network << EOF
# Network configuration
ifconfig_eth0="inet $ip_addr netmask $netmask"
defaultrouter="$gateway"
EOF
        fi

        echo_info "Network configured"
    fi
}

# Configure timezone
configure_timezone() {
    echo ""
    echo_info "Timezone Configuration"
    echo ""

    echo "Set timezone? (y/n) [n]: "
    read answer

    if [ "$answer" = "y" ] || [ "$answer" = "Y" ]; then
        echo "Available timezones:"
        echo "  1) UTC"
        echo "  2) US/Eastern"
        echo "  3) US/Pacific"
        echo "  4) Europe/London"
        echo "  5) Europe/Paris"
        echo "  6) Asia/Tokyo"
        echo "  7) Asia/Shanghai"
        echo ""
        echo "Select timezone [1]: "
        read tz_choice

        case "$tz_choice" in
            2) TZ="US/Eastern" ;;
            3) TZ="US/Pacific" ;;
            4) TZ="Europe/London" ;;
            5) TZ="Europe/Paris" ;;
            6) TZ="Asia/Tokyo" ;;
            7) TZ="Asia/Shanghai" ;;
            *) TZ="UTC" ;;
        esac

        echo "TZ=$TZ" >> /etc/profile
        echo_info "Timezone set to $TZ"
    fi
}

# Set root password
set_root_password() {
    echo ""
    echo_info "Root Password"
    echo ""

    echo "Set root password? (y/n) [y]: "
    read answer

    if [ "$answer" != "n" ] && [ "$answer" != "N" ]; then
        passwd root
    else
        echo_warn "Root password not set - system may be insecure"
    fi
}

# Create user account
create_user() {
    echo ""
    echo_info "User Account"
    echo ""

    echo "Create a regular user account? (y/n) [n]: "
    read answer

    if [ "$answer" = "y" ] || [ "$answer" = "Y" ]; then
        echo "Enter username: "
        read username

        if [ -n "$username" ]; then
            # Add user
            useradd -m -s /bin/sh "$username"
            passwd "$username"

            # Add to wheel group
            echo "Add $username to wheel group (sudo access)? (y/n) [y]: "
            read wheel_answer
            if [ "$wheel_answer" != "n" ] && [ "$wheel_answer" != "N" ]; then
                usermod -G wheel "$username"
            fi

            echo_info "User $username created"
        fi
    fi
}

# Configure services
configure_services() {
    echo ""
    echo_info "Service Configuration"
    echo ""

    # Enable RISC-V platform init
    if [ -f /etc/rc.d/riscv64_init ]; then
        chmod +x /etc/rc.d/riscv64_init
    fi

    # Basic services
    cat > /etc/rc.conf.d/services << EOF
# MINIX services
riscv64_init="YES"
syslogd="YES"
cron="YES"
sshd="NO"
EOF

    echo_info "Services configured"
}

# Finalize installation
finalize() {
    echo ""
    echo_info "Finalizing installation..."

    # Sync filesystems
    sync

    # Update /etc/fstab if needed
    if [ ! -f /etc/fstab ]; then
        cat > /etc/fstab << EOF
# Device        Mountpoint      FStype  Options         Dump    Pass#
$ROOT_PART      /               mfs     rw              1       1
$USR_PART       /usr            mfs     rw              1       2
$HOME_PART      /home           mfs     rw              1       3
none            /tmp            mfs     rw,-s=64m       0       0
proc            /proc           procfs  rw              0       0
EOF
    fi

    echo_info "Installation complete!"
}

# Print completion message
print_completion() {
    echo ""
    echo "========================================"
    echo "  MINIX 3 RISC-V 64-bit Setup Complete"
    echo "========================================"
    echo ""
    echo "You can now reboot into your new system."
    echo ""
    echo "To start MINIX in QEMU:"
    echo "  qemu-system-riscv64 -machine virt -m 256M \\"
    echo "    -bios default -nographic \\"
    echo "    -kernel /boot/kernel \\"
    echo "    -drive file=disk.img,format=raw,id=hd0 \\"
    echo "    -device virtio-blk-device,drive=hd0"
    echo ""
    echo "Default login: root (with password you set)"
    echo ""
}

# Main menu
main_menu() {
    while true; do
        echo ""
        echo "Setup Menu"
        echo "=========="
        echo "1) Quick setup (recommended)"
        echo "2) Custom setup"
        echo "3) Exit"
        echo ""
        echo "Select option [1]: "
        read choice

        case "$choice" in
            2)
                detect_hardware
                configure_network
                configure_timezone
                set_root_password
                create_user
                configure_services
                finalize
                print_completion
                break
                ;;
            3)
                echo "Setup cancelled."
                exit 0
                ;;
            *)
                # Quick setup
                detect_hardware
                set_root_password
                configure_services
                finalize
                print_completion
                break
                ;;
        esac
    done
}

# Main
main() {
    print_banner
    check_root
    main_menu
}

main "$@"
