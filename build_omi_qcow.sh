#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD="$ROOT/build"
INITROOT="$BUILD/initramfs"
KERNEL_URL="${KERNEL_URL:-https://dl-cdn.alpinelinux.org/alpine/v3.22/releases/x86_64/netboot/vmlinuz-lts}"

mkdir -p "$BUILD"
rm -rf "$INITROOT"
mkdir -p "$INITROOT"

cc -std=c99 -O2 -static "$ROOT/omi_header8_runtime.c" -o "$INITROOT/init"

(
  cd "$INITROOT"
  printf 'init\0' | cpio --null -ov --format=newc > "$BUILD/omi-initramfs.cpio"
)

if [ ! -s "$BUILD/vmlinuz-lts" ]; then
  curl -L --fail --silent --show-error -o "$BUILD/vmlinuz-lts" "$KERNEL_URL"
fi

qemu-img create -f qcow2 "$BUILD/omi-vm.qcow2" 128M >/dev/null

cat > "$BUILD/run-omi-vm.sh" <<'RUN'
#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
qemu-system-x86_64 \
  -nographic \
  -no-reboot \
  -m 256M \
  -drive file="$DIR/omi-vm.qcow2",format=qcow2,if=virtio \
  -kernel "$DIR/vmlinuz-lts" \
  -initrd "$DIR/omi-initramfs.cpio" \
  -append 'console=ttyS0 quiet panic=1'
RUN
chmod +x "$BUILD/run-omi-vm.sh"

printf 'built %s\n' "$BUILD/omi-vm.qcow2"
printf 'run   %s\n' "$BUILD/run-omi-vm.sh"
