# dev-docs/

## OSI Model Folder Structure

```
dev-docs/
├── 00-shared/           # Shared resources
├── 01-physical/         # RISC-V, QEMU virt, storage, graphics, devices
├── 02-datalink/         # MAC, frames
├── 03-network/         # IP, routing
├── 04-transport/       # TCP/UDP
├── 05-session/         # Auth, TLS
├── 06-presentation/    # YOUR octuple encoding + QEMU internals
├── 07-application/     # Your kernel
└── 99-build/          # Build and boot
```

## START HERE (In This Order)

1. **[99-build/bootable-runtime.md](./99-build/bootable-runtime.md)** - How to build from scratch
2. **[06-presentation/osi-model-why.md](./06-presentation/osi-model-why.md)** - Why OSI is structured
3. **[06-presentation/how-it-connects.md](./06-presentation/how-it-connects.md)** - HOW EVERYTHING WORKS
4. **[01-physical/storage-block-devices.md](./01-physical/storage-block-devices.md)** - NBD, storage daemon, block devices
5. **[01-physical/graphics-opengl-devices.md](./01-physical/graphics-opengl-devices.md)** - OpenGL, emulated devices
6. **[99-build/gdb-usage.md](./99-build/gdb-usage.md)** - GDB debugging

## Key Concept

Your octuple precision (Braille/Aegean/BOM) IS the presentation layer (Layer 6).