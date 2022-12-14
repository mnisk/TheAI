/*
 * Copyright 2004-2008, Axel Dörfler, axeld@pinc-software.de. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <arch/system_info.h>

#include <string.h>

#include <KernelExport.h>
#include <OS.h>

#include <boot/kernel_args.h>
#include <cpu.h>
#include <kernel.h>
#include <smp.h>


enum cpu_vendor sCPUVendor;
uint32 sCPUModel;
int64 sCPUClockSpeed;


static bool
get_cpuid_for(cpuid_info *info, uint32 currentCPU, uint32 eaxRegister,
	uint32 forCPU)
{
	if (currentCPU != forCPU)
		return false;

	get_current_cpuid(info, eaxRegister, 0);
	return true;
}


status_t
get_cpuid(cpuid_info *info, uint32 eaxRegister, uint32 forCPU)
{
	uint32 numCPUs = (uint32)smp_get_num_cpus();
	cpu_status state;

	if (forCPU >= numCPUs)
		return B_BAD_VALUE;

	// prevent us from being rescheduled
	state = disable_interrupts();

	// ToDo: as long as we only run on pentium-class systems, we can assume
	//	that the CPU supports cpuid.

	if (!get_cpuid_for(info, smp_get_current_cpu(), eaxRegister, forCPU)) {
		smp_send_broadcast_ici(SMP_MSG_CALL_FUNCTION, (addr_t)info,
			eaxRegister, forCPU, (void *)get_cpuid_for, SMP_MSG_FLAG_SYNC);
	}

	restore_interrupts(state);
	return B_OK;
}


status_t
arch_system_info_init(struct kernel_args *args)
{
	// So far we don't have to care about heterogeneous x86 platforms.
	cpu_ent* cpu = get_cpu_struct();

	switch (cpu->arch.vendor) {
		case VENDOR_AMD:
			sCPUVendor = B_CPU_VENDOR_AMD;
			break;
		case VENDOR_CENTAUR:
			sCPUVendor = B_CPU_VENDOR_VIA;
			break;
		case VENDOR_CYRIX:
			sCPUVendor = B_CPU_VENDOR_CYRIX;
			break;
		case VENDOR_INTEL:
			sCPUVendor = B_CPU_VENDOR_INTEL;
			break;
		case VENDOR_NSC:
			sCPUVendor = B_CPU_VENDOR_NATIONAL_SEMICONDUCTOR;
			break;
		case VENDOR_RISE:
			sCPUVendor = B_CPU_VENDOR_RISE;
			break;
		case VENDOR_TRANSMETA:
			sCPUVendor = B_CPU_VENDOR_TRANSMETA;
			break;
		case VENDOR_HYGON:
			sCPUVendor = B_CPU_VENDOR_HYGON;
			break;
		default:
			sCPUVendor = B_CPU_VENDOR_UNKNOWN;
			break;
	}

	sCPUModel = (cpu->arch.extended_family << 20)
		| (cpu->arch.extended_model << 16) | (cpu->arch.type << 12)
		| (cpu->arch.family << 8) | (cpu->arch.model << 4) | cpu->arch.stepping;

	sCPUClockSpeed = args->arch_args.cpu_clock_speed;
	if (cpu->arch.vendor == VENDOR_INTEL) {
		cpuid_info cpuid;
		get_current_cpuid(&cpuid, 0, 0);
		uint32 maxBasicLeaf = cpuid.eax_0.max_eax;
		if (maxBasicLeaf >= 0x16) {
			get_current_cpuid(&cpuid, 0x16, 0);
			if (cpuid.regs.eax != 0) {
				sCPUClockSpeed = cpuid.regs.eax * 1000000LL;
				dprintf("found clock speed with CPUID.16h\n");
			}
		}
	}
	return B_OK;
}


void
arch_fill_topology_node(cpu_topology_node_info* node, int32 cpu)
{
	switch (node->type) {
		case B_TOPOLOGY_ROOT:
#if __i386__
			node->data.root.platform = B_CPU_x86;
#elif __x86_64__
			node->data.root.platform = B_CPU_x86_64;
#else
			node->data.root.platform = B_CPU_UNKNOWN;
#endif
			break;

		case B_TOPOLOGY_PACKAGE:
			node->data.package.vendor = sCPUVendor;
			node->data.package.cache_line_size = CACHE_LINE_SIZE;
			break;

		case B_TOPOLOGY_CORE:
			node->data.core.model = sCPUModel;
			node->data.core.default_frequency = sCPUClockSpeed;
			break;

		default:
			break;
	}
}


static void
get_frequency_for(void *_frequency, int cpu)
{
	uint64 *frequency = (uint64*)_frequency;

	bigtime_t timestamp = gCPU[cpu].arch.perf_timestamp;
	bigtime_t timestamp2 = system_time();
	if (timestamp2 - timestamp < 100) {
		*frequency = gCPU[cpu].arch.frequency;
		return;
	}

	uint64 mperf = gCPU[cpu].arch.mperf_prev;
	uint64 aperf = gCPU[cpu].arch.aperf_prev;
	uint64 mperf2 = x86_read_msr(IA32_MSR_MPERF);
	uint64 aperf2 = x86_read_msr(IA32_MSR_APERF);

	if (mperf2 == mperf)
		*frequency = 0;
	else {
		*frequency = (aperf2 - aperf) * sCPUClockSpeed / (mperf2 - mperf);
		gCPU[cpu].arch.mperf_prev = mperf2;
		gCPU[cpu].arch.aperf_prev = aperf2;
		gCPU[cpu].arch.perf_timestamp = timestamp2;
		gCPU[cpu].arch.frequency = *frequency;
	}
}


status_t
arch_get_frequency(uint64 *frequency, int32 cpu)
{
	if (x86_check_feature(IA32_FEATURE_APERFMPERF, FEATURE_6_ECX))
		call_single_cpu_sync(cpu, get_frequency_for, frequency);
	else
		*frequency = sCPUClockSpeed;

	return B_OK;
}


//	#pragma mark -


status_t
_user_get_cpuid(cpuid_info *userInfo, uint32 eaxRegister, uint32 cpuNum)
{
	cpuid_info info;
	status_t status;

	if (!IS_USER_ADDRESS(userInfo))
		return B_BAD_ADDRESS;

	status = get_cpuid(&info, eaxRegister, cpuNum);

	if (status == B_OK
		&& user_memcpy(userInfo, &info, sizeof(cpuid_info)) < B_OK)
		return B_BAD_ADDRESS;

	return status;
}

