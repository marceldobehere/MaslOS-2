#include <libm/cpuid/cpuid.h>
#include <stdint.h>

void memset(void* start, uint8_t value, uint64_t num)
{
    uint8_t* curr = (uint8_t*)start;
    
    for (int64_t rem = num; rem > 0; rem--)
        *curr++ = value;
}

static inline void cpuid(unsigned int op, unsigned int *eax, unsigned int *ebx,
			 				unsigned int *ecx, unsigned int *edx)
{
	*eax = op;
	*ecx = 0;

	asm volatile("cpuid"
	    : "=a" (*eax),
	      "=b" (*ebx),
	      "=c" (*ecx),
	      "=d" (*edx)
	    : "0" (*eax), "2" (*ecx)
	    : "memory");
}

unsigned int x86_family(unsigned int sig)
{
	unsigned int x86;

	x86 = (sig >> 8) & 0xf;

	if (x86 == 0xf)
		x86 += (sig >> 20) & 0xff;

	return x86;
}

unsigned int x86_model(unsigned int sig)
{
	unsigned int fam, model;

	fam = x86_family(sig);

	model = (sig >> 4) & 0xf;

	if (fam >= 0x6)
		model += ((sig >> 16) & 0xf) << 4;

	return model;
}

unsigned int x86_stepping(unsigned int sig)
{
	return sig & 0xf;
}


static void get_model_name(struct cpuinfo_x86 *c)
{
	unsigned int *v = (unsigned int *)c->x86_model_id;
	cpuid(0x80000002, &v[0], &v[1], &v[2], &v[3]);
	cpuid(0x80000003, &v[4], &v[5], &v[6], &v[7]);
	cpuid(0x80000004, &v[8], &v[9], &v[10], &v[11]);
	c->x86_model_id[48] = 0;
}

void cpu_detect(struct cpuinfo_x86 *c)
{
	/* Get vendor name */
	memset(c->x86_vendor_id, 0, sizeof(c->x86_vendor_id));
	cpuid(0x00000000, (unsigned int *)&c->cpuid_level,
	      (unsigned int *)&c->x86_vendor_id[0],
	      (unsigned int *)&c->x86_vendor_id[8],
	      (unsigned int *)&c->x86_vendor_id[4]);

	c->x86 = 4;
	/* Intel-defined flags: level 0x00000001 */
	if (c->cpuid_level >= 0x00000001) {
		unsigned int junk, tfms, cap0, misc;

		cpuid(0x00000001, &tfms, &misc, &junk, &cap0);
		c->x86		= x86_family(tfms);
		c->x86_model	= x86_model(tfms);
		c->x86_stepping	= x86_stepping(tfms);

		if (cap0 & (1<<19)) {
			c->x86_clflush_size = ((misc >> 8) & 0xff) * 8;
			c->x86_cache_alignment = c->x86_clflush_size;
		}
	}
}

void get_cpu_address_sizes(struct cpuinfo_x86 *c)
{
	unsigned int eax, ebx, ecx, edx;

	cpuid(0x80000008, &eax, &ebx, &ecx, &edx);

	c->x86_virt_bits = (eax >> 8) & 0xff;
	c->x86_phys_bits = eax & 0xff;

	c->x86_cache_bits = c->x86_phys_bits;
}