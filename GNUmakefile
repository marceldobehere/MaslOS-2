all: MaslOS2.iso


.PHONY: kernel
kernel:
	$(MAKE) -C libm
	$(MAKE) -C kernel
	$(MAKE) -C modules
	$(MAKE) -C kernel-loader

MaslOS2.iso:
	$(MAKE) cleanObjFolder
	$(MAKE) kernel
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


clean:
	rm -rf iso_root MaslOS2.iso barebones.hdd
	$(MAKE) cleanObjFolder
	$(MAKE) -C libm clean
	$(MAKE) -C kernel clean
	$(MAKE) -C modules clean
	$(MAKE) -C kernel-loader clean	


clean2:
	rm -rf iso_root barebones.hdd
	$(MAKE) cleanObjFolder
	$(MAKE) -C libm clean
	$(MAKE) -C kernel clean
	$(MAKE) -C modules clean
	$(MAKE) -C kernel-loader clean


cleanObjFolder:
	rm -rf objects || true
	mkdir objects
	mkdir objects/kernel
	mkdir objects/kernel-loader
	mkdir objects/libm
	mkdir objects/modules
	mkdir objects/programs
	
	
