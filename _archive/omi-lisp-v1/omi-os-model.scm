;; omi-os-model.scm
;; OSI 7-layer model + Translator Internals
;; Pre-Lisp: control lattice before eval/apply

;; Layer 1: Physical - bit transmission
;;   - electrical/optical/radio signals
;;   - voltage levels, photon pulses

;; Layer 2: Data Link - MAC frames  
;;   - MAC addresses, frames, error detection (CRC)
;;   - Ethernet, ARP

;; Layer 3: Network - IP routing
;;   - logical addressing (IPv4/IPv6)
;;   - routing tables, NAT

;; Layer 4: Transport - TCP/UDP
;;   - reliable (TCP) vs unreliable (UDP) delivery
;;   - ports, sequence numbers, acknowledgments

;; Layer 5: Session - auth/checkpoints
;;   - authentication, TLS handshake
;;   - checkpoints, recovery

;; Layer 6: Presentation - encoding/translation
;;   QEMU TCG = "Tiny Code Generator"
;;   - guest instruction set -> host instruction set
;;   - Translation Block (TB): basic block of translated code
;;   - TCG Intermediate Representation (IR)
;;
;;   Key concepts:
;;   - tcg_gen_goto_tb(): emit branch to next TB
;;   - tcg_gen_exit_tb(): exit current TB, return to main loop
;;   - lookup_and_goto_ptr(): find next TB or JIT epilogue
;;   - CPU state optimization: record state in TB
;;   - Direct block chaining: patch jump slots
;;   - Self-modifying code: write-protect translated pages
;;   - Exception handling: longjmp() + SIGSEGV/SIGBUS
;;   - Software MMU: TLB for address translation

;; Layer 7: Application - HTTP/SMTP/DNS
;;   - user-facing protocols
;;   - QEMU monitor, VNC, SPICE

;; Build from model:
;; guix shell --manifest=omi-dev-manifest.scm
;; riscv64-linux-gnu-gcc -std=c99 -O2 -static omi_riscv_vm.c -o init