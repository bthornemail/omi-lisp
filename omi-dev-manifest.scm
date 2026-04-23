;; omi-dev-manifest.scm
;; Declarative dev environment for omi-lisp OSI model
;; guix shell --manifest=omi-dev-manifest.scm

;; omi-dev-manifest.scm
;; Declarative dev environment for omi-lisp kernel development
;; guix shell --manifest=omi-dev-manifest.scm

(use-modules (gnu packages))

(specifications->manifest
  (list
    "riscv64-linux-gnu-gcc"
    "guile"))

;; Build RISC-V kernel from C source:
;; guix shell --manifest=omi-dev-manifest.scm -- riscv64-linux-gnu-gcc -std=c99 -O2 -static omi_riscv_vm.c -o init