/*
 * SKELETON ONLY - not authoritative.
 *
 * These definitions reflect the general shape kbase's ioctl interface has
 * taken historically. Every command number, struct field, and flag here
 * must be checked against the ACTUAL mali_kbase_ioctl.h /
 * mali_kbase_csf_ioctl.h / mali_base_kernel.h shipped in your device's
 * vendor kernel source (matching UK interface 44.10). Do not trust this
 * file's exact values - use it only as a map of "what kinds of things to
 * look for" in the real header.
 *
 * In particular, for a CSF/Valhall part like G615 you will also need the
 * CSF-specific ioctls (queue group create/terminate, KCPU queue
 * enqueue/register, tiler heap init) which have no JM equivalent and are
 * NOT sketched here - go straight to mali_kbase_csf_ioctl.h for those.
 */

#ifndef MALI_KBASE_IOCTL_SKELETON_H
#define MALI_KBASE_IOCTL_SKELETON_H

#include <linux/types.h>
#include <linux/ioctl.h>

#define KBASE_IOCTL_TYPE 0x80 /* VERIFY: has been stable historically but confirm */

/* --- Version check: always ioctl #0 in every kbase generation seen so far --- */
struct kbase_ioctl_version_check {
	__u16 major;
	__u16 minor;
};
#define KBASE_IOCTL_VERSION_CHECK \
	_IOWR(KBASE_IOCTL_TYPE, 0, struct kbase_ioctl_version_check)

/* --- Set flags: tells kbase what kind of context you want (JM vs CSF-aware userspace) --- */
struct kbase_ioctl_set_flags {
	__u32 create_flags; /* VERIFY: exact flag bit values for your version */
};
#define KBASE_IOCTL_SET_FLAGS \
	_IOW(KBASE_IOCTL_TYPE, 1, struct kbase_ioctl_set_flags) /* VERIFY command number */

/* --- GPU properties query: two-step, size-then-fetch pattern --- */
struct kbase_ioctl_get_gpuprops {
	__u64 buffer;
	__u32 size;
	__u32 flags; /* must be 0 */
};
#define KBASE_IOCTL_GET_GPUPROPS \
	_IOWR(KBASE_IOCTL_TYPE, 3, struct kbase_ioctl_get_gpuprops) /* VERIFY command number */

/* --- Memory allocation: shape varies a lot between kbase releases --- */
union kbase_ioctl_mem_alloc {
	struct {
		__u64 va_pages;
		__u64 commit_pages;
		__u64 extension;
		__u64 flags;
	} in;
	struct {
		__u64 flags;
		__u64 gpu_va;
	} out;
};
#define KBASE_IOCTL_MEM_ALLOC \
	_IOWR(KBASE_IOCTL_TYPE, 5, union kbase_ioctl_mem_alloc) /* VERIFY command number + struct shape */

#endif /* MALI_KBASE_IOCTL_SKELETON_H */
