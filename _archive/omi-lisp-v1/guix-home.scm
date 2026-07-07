;; guix-home.scm
;; Declarative home environment for omi-lisp development with Atom remote editing

(use-modules (gnu home)
             (gnu home services)
             (gnu home services shells)
             (gnu packages)
             (gnu packages atom)
             (gnu packages virtualization)
             (gnu packages engineering))

(home-environment
 (packages
  (specifications->packages
   (list
    "guile"
    "sbcl"
    "riscv64-linux-gnu-gcc"
    "qemu"
    "electron")))

 (environment-variables
  (list
   ;; RISC-V cross-compiler paths
   (simple-environment-variable "CC" "riscv64-linux-gnu-gcc")
   (simple-environment-variable "CROSS_PREFIX" "riscv64-linux-gnu-"))

 (shell-profile
  (list
   ;; Add omi-lisp project to PATH
   (local-file "omi-lisp" "./")))

 (services
  (list
   ;; SSH for remote editing
   (service home-shell-profile-service-type)

   ;; Atom remote editing daemon
   (home-service
    (name 'atom-remote)
    (configuration
     (list
      (computed-file
       "atom-remote.desktop"
       (with-output-to-string
         (lambda ()
           (format #t "[Desktop Entry]~%")
           (format #t "Name=Atom Remote~%")
           (format #t "Exec=atom --remote .~%"))))))))))

;; guix home reconfigure guix-home.scm