# OMI VM QEMU Boot

This repo now has two VM layers:

- `omi_vm_loader.lisp`: symbolic OMI VM loader for host-side Lisp testing.
- `omi_header8_runtime.c`: boot/PID-1 machine substrate that runs the HEADER8 lattice directly in QEMU.
- `omi_riscv_vm.c`: RISC-V PID-1 runtime with BOM, four control-plane points, binary lexer masks, ECC witnesses, HEADER8, and dot-notation structure.

Build:

```sh
chmod +x build_omi_qcow.sh
./build_omi_qcow.sh
```

Run:

```sh
./build/run-omi-vm.sh
```

The generated `build/omi-vm.qcow2` is attached as the VM disk. The current boot path uses a kernel plus OMI initramfs because that is the smallest reliable boot path without installing a full guest OS or bootloader. The OMI runtime runs as PID 1 and emits the HEADER8/rewrite witness at boot.

Dot notation is interpreted in the boot runtime. For example, the byte stream `(A.B)` emits:

```text
emit=STRUCT.LIST-OPEN dot=STRUCT.LIST(depth=1)
emit=TOKEN.ASCII dot=STRUCT.ATOM(value=0x41)
emit=STRUCT.DOT dot=STRUCT.DOT(left=0x41)
emit=TOKEN.ASCII dot=STRUCT.PAIR(left=0x41,right=0x42)
emit=STRUCT.LIST-CLOSE dot=STRUCT.CLOSE(depth=1)
```

Host Lisp test:

```sh
sbcl --noinform --non-interactive --load omi_vm_loader.lisp --eval '(format t "~S~%" (omi-vm-self-test))'
```

## RISC-V Control Plane Build

The RISC-V path is the preferred VM target for OMI:

```sh
chmod +x build_omi_riscv_qcow.sh
./build_omi_riscv_qcow.sh
./build-riscv/run-omi-riscv-vm.sh
```

The four control points are:

```text
BOM      0xEF        frame sync / beginning of message
ESC      0x1B        mode shift
BOUNDARY 0x1C..0x1F FS / GS / RS / US
ECC      0xE0..0xEE binary error-correction witness
```

The RISC-V runtime emits the raw binary byte, lexer mask, control-plane state, HEADER8, Hamming(8,4) ECC witnesses, and dot-notation structure on each step.
