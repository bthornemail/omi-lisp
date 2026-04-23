#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD="$ROOT/build-riscv"
INITROOT="$BUILD/initramfs"
ALPINE_BASE="${ALPINE_BASE:-https://dl-cdn.alpinelinux.org/alpine/v3.22/releases/riscv64}"
ALPINE_UBOOT="${ALPINE_UBOOT:-alpine-uboot-3.22.3-riscv64.tar.gz}"

mkdir -p "$BUILD"
rm -rf "$INITROOT"
mkdir -p "$INITROOT"

riscv64-linux-gnu-gcc -std=c99 -O2 -static "$ROOT/omi_riscv_vm.c" -o "$INITROOT/init"

(
  cd "$INITROOT"
  printf 'init\0' | cpio --null -ov --format=newc > "$BUILD/omi-riscv-initramfs.cpio"
)

if [ ! -s "$BUILD/$ALPINE_UBOOT" ]; then
  curl -L --fail --silent --show-error -o "$BUILD/$ALPINE_UBOOT" "$ALPINE_BASE/$ALPINE_UBOOT"
fi

if [ ! -s "$BUILD/boot/vmlinuz-lts" ]; then
  mkdir -p "$BUILD/boot"
  tar -xzf "$BUILD/$ALPINE_UBOOT" -C "$BUILD" ./boot/vmlinuz-lts
fi

qemu-img create -f qcow2 "$BUILD/omi-riscv-vm.qcow2" 128M >/dev/null

cat > "$BUILD/run-omi-riscv-vm.sh" <<'RUN'
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
RUN
chmod +x "$BUILD/run-omi-riscv-vm.sh"

printf 'built %s\n' "$BUILD/omi-riscv-vm.qcow2"
printf 'kernel %s\n' "$BUILD/boot/vmlinuz-lts"
printf 'run   %s\n' "$BUILD/run-omi-riscv-vm.sh"
