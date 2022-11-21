;; GNU Guix manifest with the software required to build this project
(use-modules
 (srfi srfi-1)
 (guix packages)
 (guix profiles)
 (gnu packages cross-base))

(define the-manifest
  (concatenate-manifests
   (list
    (packages->manifest
     (list
      (cross-binutils "arm-none-eabi")))
    (specifications->manifest
     (list
      "bash"
      "bison"
      "cmake"
      "coreutils"
      "diffutils"
      "gcc-toolchain"
      "gdb-arm-none-eabi"
      "git"
      "grep"
      "ninja"
      "nss-certs"
      "sed"
      "qemu")))))

the-manifest
