.PHONY: all
all: MaslOS2.iso

.PHONY: all-hdd
all-hdd: barebones.hdd

.PHONY: run
run: MaslOS2.iso
	qemu-system-x86_64 -M q35 -m 2G -cdrom MaslOS2.iso -boot d

.PHONY: run-uefi
run-uefi: ovmf-x64 MaslOS2.iso
	qemu-system-x86_64 -M q35 -m 2G -bios ovmf-x64/OVMF.fd -cdrom MaslOS2.iso -boot d

.PHONY: run-hdd
run-hdd: barebones.hdd
	qemu-system-x86_64 -M q35 -m 2G -hda barebones.hdd

.PHONY: run-hdd-uefi
run-hdd-uefi: ovmf-x64 barebones.hdd
	qemu-system-x86_64 -M q35 -m 2G -bios ovmf-x64/OVMF.fd -hda barebones.hdd

ovmf-x64:
	mkdir -p ovmf-x64
	cd ovmf-x64 && curl -o OVMF-X64.zip https://efi.akeo.ie/OVMF/OVMF-X64.zip && 7z x OVMF-X64.zip

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1
	make -C limine

.PHONY: kernel
kernel:
	$(MAKE) -C libm
	$(MAKE) -C kernel
	$(MAKE) -C modules
	$(MAKE) -C kernel-loader

MaslOS2.iso: limine kernel
	rm -rf iso_root
	mkdir -p iso_root
	cp modules/test/test.elf external/test.elf
	cp modules/nothing-doer/main.elf external/nothing-doer.elf
	cp kernel-loader/kernel.elf \
		limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin \
		external/* \
		iso_root/
		
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o MaslOS2.iso
		
	limine/limine-deploy MaslOS2.iso
	rm -rf iso_root

.PHONY: clean
clean:
	rm -rf iso_root MaslOS2.iso barebones.hdd
	$(MAKE) -C libm clean
	$(MAKE) -C kernel clean
	$(MAKE) -C modules clean
	$(MAKE) -C kernel-loader clean

.PHONY: distclean
distclean: clean
	rm -rf limine ovmf-x64
	$(MAKE) -C kernel distclean
