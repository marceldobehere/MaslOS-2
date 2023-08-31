all: MaslOS2.iso


.PHONY: kernel
kernel:
	$(MAKE) -C libm
	$(MAKE) -C kernel
	$(MAKE) -C modules
	$(MAKE) -C programs
	$(MAKE) -C kernel-loader

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1
	make -C limine

MaslOS2.iso:
	$(MAKE) cleanObjFolder --silent
	$(MAKE) kernel
	rm -rf iso_root
	mkdir -p iso_root
	# cp modules/test/test.elf external/test.elf
	# cp modules/nothing-doer/nothing-doer.elf external/nothing-doer.elf
	
	for i in ./modules/*/; do \
		if [ -d "$$i" ]; \
		then \
			echo "$$(basename "$$i")"; \
			cp "$$i/$$(basename "$$i").elf" objects/external/modules/$$(basename "$$i").elf; \
		fi \
	done
	
	for i in ./programs/*/; do \
		if [ -d "$$i" ]; \
		then \
			echo "$$(basename "$$i")"; \
			cp "$$i/$$(basename "$$i").elf" objects/external/programs/$$(basename "$$i").elf; \
		fi \
	done
	
	$(MAKE) -C saf
	./saf/saf-make ./objects/external ./external/programs.saf
	
	
	
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


clean: clean2
	@rm -rf iso_root MaslOS2.iso barebones.hdd ./external/programs.saf
	


clean2:
	@rm -rf iso_root barebones.hdd ./external/programs.saf
	@$(MAKE) cleanObjFolder
	@$(MAKE) -C libm clean
	@$(MAKE) -C kernel clean
	@$(MAKE) -C modules clean
	@$(MAKE) -C programs clean
	@$(MAKE) -C kernel-loader clean
	@$(MAKE) -C saf clean


cleanObjFolder:
	@rm -rf objects || true
	@mkdir objects
	@mkdir objects/kernel
	@mkdir objects/kernel-loader
	@mkdir objects/libm
	@mkdir objects/modules
	@mkdir objects/programs
	@mkdir objects/external
	@mkdir objects/external/modules
	@mkdir objects/external/programs
	
	
