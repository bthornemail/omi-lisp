#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
qemu-system-riscv64 \
  -machine virt \
  -m 512M \
  -nographic \
  -no-reboot \
  -drive file="$DIR/omi-riscv-vm.qcow2",format=qcow2,if=virtio \
  -kernel "$DIR/boot/vmlinuz-lts" \
  -initrd "$DIR/omi-riscv-initramfs.cpio" \
  -append 'console=ttyS0 rdinit=/init panic=1'
