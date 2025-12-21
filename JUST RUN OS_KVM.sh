#!/bin/bash
set -e

./cDisk.sh
qemu-system-x86_64 -machine q35 -accel kvm -m 1G -cpu qemu64 -serial stdio -drive file=disk.img -boot d -cdrom MaslOS2.iso
# -soundhw ac97

