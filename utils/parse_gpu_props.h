#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "mali_kbase_ioctl.h"

/*
 * GPU_ID2 bit layout from mali_kbase_gpu_id.h
 */

#define GPU_ID2_VERSION_STATUS_SHIFT 0
#define GPU_ID2_VERSION_MINOR_SHIFT 4
#define GPU_ID2_VERSION_MAJOR_SHIFT 12
#define GPU_ID2_PRODUCT_MAJOR_SHIFT 16
#define GPU_ID2_ARCH_REV_SHIFT 20
#define GPU_ID2_ARCH_MINOR_SHIFT 24
#define GPU_ID2_ARCH_MAJOR_SHIFT 28


#define GPU_ID2_VERSION_STATUS_MASK 0xf
#define GPU_ID2_VERSION_MINOR_MASK  0xff
#define GPU_ID2_VERSION_MAJOR_MASK  0xf

#define GPU_ID2_PRODUCT_MAJOR_MASK  0xf
#define GPU_ID2_ARCH_REV_MASK       0xf
#define GPU_ID2_ARCH_MINOR_MASK     0xf
#define GPU_ID2_ARCH_MAJOR_MASK     0xf


struct gpu_info {
    uint32_t gpu_id;

    unsigned arch_major;
    unsigned arch_minor;
    unsigned arch_rev;

    unsigned product_major;

    unsigned version_major;
    unsigned version_minor;
    unsigned version_status;

    const char *model;
};


/*
 * GPU_ID2_MODEL table from mali_kbase_gpu_id.h
 *
 * arch_major + product_major identifies the model
 */
static const char *gpu_model(unsigned arch, unsigned product)
{
    struct entry {
        unsigned arch;
        unsigned product;
        const char *name;
    };

    static const struct entry table[] = {

        {6,0,"Mali-TMIX"},
        {6,1,"Mali-THEX"},

        {7,0,"Mali-TSIX"},
        {7,1,"Mali-TNOX"},
        {7,2,"Mali-TGOX"},
        {7,3,"Mali-TDVX"},

        {9,0,"Mali-TTRX"},
        {9,1,"Mali-TNAX"},
        {9,2,"Mali-TBEX"},
        {9,4,"Mali-LBEX"},
        {9,5,"Mali-TBAX"},

        {10,2,"Mali-TODX"},
        {10,3,"Mali-TGRX"},
        {10,4,"Mali-TVAX"},
        {10,7,"Mali-LODX"},

        {11,2,"Mali-TTUX"},
        {11,3,"Mali-LTUX"},

        {12,0,"Mali-TTIX"},
        {12,1,"Mali-LTIX"},
    };


    for (unsigned i = 0; i < sizeof(table)/sizeof(table[0]); i++) {
        if (table[i].arch == arch &&
            table[i].product == product)
            return table[i].name;
    }

    return "Unknown Mali GPU";
}



void decode_gpu_id2(uint32_t id, struct gpu_info *gpu)
{
    memset(gpu, 0, sizeof(*gpu));

    gpu->gpu_id = id;


    gpu->version_status =
        (id >> GPU_ID2_VERSION_STATUS_SHIFT) &
        GPU_ID2_VERSION_STATUS_MASK;

    gpu->version_minor =
        (id >> GPU_ID2_VERSION_MINOR_SHIFT) &
        GPU_ID2_VERSION_MINOR_MASK;

    gpu->version_major =
        (id >> GPU_ID2_VERSION_MAJOR_SHIFT) &
        GPU_ID2_VERSION_MAJOR_MASK;


    gpu->product_major =
        (id >> GPU_ID2_PRODUCT_MAJOR_SHIFT) &
        GPU_ID2_PRODUCT_MAJOR_MASK;


    gpu->arch_rev =
        (id >> GPU_ID2_ARCH_REV_SHIFT) &
        GPU_ID2_ARCH_REV_MASK;

    gpu->arch_minor =
        (id >> GPU_ID2_ARCH_MINOR_SHIFT) &
        GPU_ID2_ARCH_MINOR_MASK;

    gpu->arch_major =
        (id >> GPU_ID2_ARCH_MAJOR_SHIFT) &
        GPU_ID2_ARCH_MAJOR_MASK;


    gpu->model =
        gpu_model(gpu->arch_major,
                  gpu->product_major);
}



void print_gpu_info(struct gpu_info *gpu)
{
    printf("\nGPU information\n");
    printf("---------------------------\n");

    printf("GPU ID        : 0x%08x\n",
           gpu->gpu_id);

    printf("Model         : %s\n",
           gpu->model);

    printf("Architecture  : %u.%u\n",
           gpu->arch_major,
           gpu->arch_minor);

    printf("Architecture revision : %u\n",
           gpu->arch_rev);

    printf("Product major : %u\n",
           gpu->product_major);

    printf("GPU version   : %u.%u.%u\n",
           gpu->version_major,
           gpu->version_minor,
           gpu->version_status);
}

static const char *gpuprop_name(uint32_t id) {
  switch (id) {
  case KBASE_GPUPROP_PRODUCT_ID:
    return "PRODUCT_ID";
  case KBASE_GPUPROP_VERSION_STATUS:
    return "VERSION_STATUS";
  case KBASE_GPUPROP_MINOR_REVISION:
    return "MINOR_REVISION";
  case KBASE_GPUPROP_MAJOR_REVISION:
    return "MAJOR_REVISION";
  case KBASE_GPUPROP_GPU_FREQ_KHZ_MAX:
    return "GPU_FREQ_KHZ_MAX";
  case KBASE_GPUPROP_LOG2_PROGRAM_COUNTER_SIZE:
    return "LOG2_PROGRAM_COUNTER_SIZE";
  case KBASE_GPUPROP_TEXTURE_FEATURES_0:
    return "TEXTURE_FEATURES_0";
  case KBASE_GPUPROP_TEXTURE_FEATURES_1:
    return "TEXTURE_FEATURES_1";
  case KBASE_GPUPROP_TEXTURE_FEATURES_2:
    return "TEXTURE_FEATURES_2";
  case KBASE_GPUPROP_GPU_AVAILABLE_MEMORY_SIZE:
    return "GPU_AVAILABLE_MEMORY_SIZE";

  case KBASE_GPUPROP_L2_LOG2_LINE_SIZE:
    return "L2_LOG2_LINE_SIZE";
  case KBASE_GPUPROP_L2_LOG2_CACHE_SIZE:
    return "L2_LOG2_CACHE_SIZE";
  case KBASE_GPUPROP_L2_NUM_L2_SLICES:
    return "L2_NUM_L2_SLICES";

  case KBASE_GPUPROP_TILER_BIN_SIZE_BYTES:
    return "TILER_BIN_SIZE_BYTES";
  case KBASE_GPUPROP_TILER_MAX_ACTIVE_LEVELS:
    return "TILER_MAX_ACTIVE_LEVELS";

  case KBASE_GPUPROP_MAX_THREADS:
    return "MAX_THREADS";
  case KBASE_GPUPROP_MAX_WORKGROUP_SIZE:
    return "MAX_WORKGROUP_SIZE";
  case KBASE_GPUPROP_MAX_BARRIER_SIZE:
    return "MAX_BARRIER_SIZE";
  case KBASE_GPUPROP_MAX_REGISTERS:
    return "MAX_REGISTERS";
  case KBASE_GPUPROP_MAX_TASK_QUEUE:
    return "MAX_TASK_QUEUE";
  case KBASE_GPUPROP_MAX_THREAD_GROUP_SPLIT:
    return "MAX_THREAD_GROUP_SPLIT";
  case KBASE_GPUPROP_IMPL_TECH:
    return "IMPL_TECH";

  case KBASE_GPUPROP_RAW_SHADER_PRESENT:
    return "RAW_SHADER_PRESENT";
  case KBASE_GPUPROP_RAW_TILER_PRESENT:
    return "RAW_TILER_PRESENT";
  case KBASE_GPUPROP_RAW_L2_PRESENT:
    return "RAW_L2_PRESENT";
  case KBASE_GPUPROP_RAW_STACK_PRESENT:
    return "RAW_STACK_PRESENT";
  case KBASE_GPUPROP_RAW_L2_FEATURES:
    return "RAW_L2_FEATURES";
  case KBASE_GPUPROP_RAW_CORE_FEATURES:
    return "RAW_CORE_FEATURES";
  case KBASE_GPUPROP_RAW_MEM_FEATURES:
    return "RAW_MEM_FEATURES";
  case KBASE_GPUPROP_RAW_MMU_FEATURES:
    return "RAW_MMU_FEATURES";
  case KBASE_GPUPROP_RAW_AS_PRESENT:
    return "RAW_AS_PRESENT";
  case KBASE_GPUPROP_RAW_JS_PRESENT:
    return "RAW_JS_PRESENT";

  case KBASE_GPUPROP_RAW_JS_FEATURES_0:
    return "RAW_JS_FEATURES_0";
  case KBASE_GPUPROP_RAW_JS_FEATURES_1:
    return "RAW_JS_FEATURES_1";
  case KBASE_GPUPROP_RAW_JS_FEATURES_2:
    return "RAW_JS_FEATURES_2";
  case KBASE_GPUPROP_RAW_JS_FEATURES_3:
    return "RAW_JS_FEATURES_3";
  case KBASE_GPUPROP_RAW_JS_FEATURES_4:
    return "RAW_JS_FEATURES_4";
  case KBASE_GPUPROP_RAW_JS_FEATURES_5:
    return "RAW_JS_FEATURES_5";
  case KBASE_GPUPROP_RAW_JS_FEATURES_6:
    return "RAW_JS_FEATURES_6";
  case KBASE_GPUPROP_RAW_JS_FEATURES_7:
    return "RAW_JS_FEATURES_7";
  case KBASE_GPUPROP_RAW_JS_FEATURES_8:
    return "RAW_JS_FEATURES_8";
  case KBASE_GPUPROP_RAW_JS_FEATURES_9:
    return "RAW_JS_FEATURES_9";
  case KBASE_GPUPROP_RAW_JS_FEATURES_10:
    return "RAW_JS_FEATURES_10";
  case KBASE_GPUPROP_RAW_JS_FEATURES_11:
    return "RAW_JS_FEATURES_11";
  case KBASE_GPUPROP_RAW_JS_FEATURES_12:
    return "RAW_JS_FEATURES_12";
  case KBASE_GPUPROP_RAW_JS_FEATURES_13:
    return "RAW_JS_FEATURES_13";
  case KBASE_GPUPROP_RAW_JS_FEATURES_14:
    return "RAW_JS_FEATURES_14";
  case KBASE_GPUPROP_RAW_JS_FEATURES_15:
    return "RAW_JS_FEATURES_15";
  case KBASE_GPUPROP_RAW_TILER_FEATURES:
    return "RAW_TILER_FEATURES";
  case KBASE_GPUPROP_RAW_TEXTURE_FEATURES_0:
    return "RAW_TEXTURE_FEATURES_0";
  case KBASE_GPUPROP_RAW_TEXTURE_FEATURES_1:
    return "RAW_TEXTURE_FEATURES_1";
  case KBASE_GPUPROP_RAW_TEXTURE_FEATURES_2:
    return "RAW_TEXTURE_FEATURES_2";
  case KBASE_GPUPROP_RAW_GPU_ID:
    return "RAW_GPU_ID";
  case KBASE_GPUPROP_RAW_THREAD_MAX_THREADS:
    return "RAW_THREAD_MAX_THREADS";
  case KBASE_GPUPROP_RAW_THREAD_MAX_WORKGROUP_SIZE:
    return "RAW_THREAD_MAX_WORKGROUP_SIZE";
  case KBASE_GPUPROP_RAW_THREAD_MAX_BARRIER_SIZE:
    return "RAW_THREAD_MAX_BARRIER_SIZE";
  case KBASE_GPUPROP_RAW_THREAD_FEATURES:
    return "RAW_THREAD_FEATURES";
  case KBASE_GPUPROP_RAW_COHERENCY_MODE:
    return "RAW_COHERENCY_MODE";
  case KBASE_GPUPROP_COHERENCY_NUM_GROUPS:
    return "COHERENCY_NUM_GROUPS";
  case KBASE_GPUPROP_COHERENCY_NUM_CORE_GROUPS:
    return "COHERENCY_NUM_CORE_GROUPS";
  case KBASE_GPUPROP_COHERENCY_COHERENCY:
    return "COHERENCY_COHERENCY";
  case KBASE_GPUPROP_COHERENCY_GROUP_0:
    return "COHERENCY_GROUP_0";
  case KBASE_GPUPROP_COHERENCY_GROUP_1:
    return "COHERENCY_GROUP_1";
  case KBASE_GPUPROP_COHERENCY_GROUP_2:
    return "COHERENCY_GROUP_2";
  case KBASE_GPUPROP_COHERENCY_GROUP_3:
    return "COHERENCY_GROUP_3";
  case KBASE_GPUPROP_COHERENCY_GROUP_4:
    return "COHERENCY_GROUP_4";
  case KBASE_GPUPROP_COHERENCY_GROUP_5:
    return "COHERENCY_GROUP_5";
  case KBASE_GPUPROP_COHERENCY_GROUP_6:
    return "COHERENCY_GROUP_6";
  case KBASE_GPUPROP_COHERENCY_GROUP_7:
    return "COHERENCY_GROUP_7";
  case KBASE_GPUPROP_COHERENCY_GROUP_8:
    return "COHERENCY_GROUP_8";
  case KBASE_GPUPROP_COHERENCY_GROUP_9:
    return "COHERENCY_GROUP_9";
  case KBASE_GPUPROP_COHERENCY_GROUP_10:
    return "COHERENCY_GROUP_10";
  case KBASE_GPUPROP_COHERENCY_GROUP_11:
    return "COHERENCY_GROUP_11";
  case KBASE_GPUPROP_COHERENCY_GROUP_12:
    return "COHERENCY_GROUP_12";
  case KBASE_GPUPROP_COHERENCY_GROUP_13:
    return "COHERENCY_GROUP_13";
  case KBASE_GPUPROP_COHERENCY_GROUP_14:
    return "COHERENCY_GROUP_14";
  case KBASE_GPUPROP_COHERENCY_GROUP_15:
    return "COHERENCY_GROUP_15";
  case KBASE_GPUPROP_TEXTURE_FEATURES_3:
    return "TEXTURE_FEATURES_3";
  case KBASE_GPUPROP_RAW_TEXTURE_FEATURES_3:
    return "TEXTURE_FEATURES_3";
  case KBASE_GPUPROP_NUM_EXEC_ENGINES:
    return "NUM_EXEC_ENGINES";
  case KBASE_GPUPROP_RAW_THREAD_TLS_ALLOC:
    return "RAW_THREAD_TLS_ALLOC";
  case KBASE_GPUPROP_TLS_ALLOC:
    return "TLS_ALLOC";
  case KBASE_GPUPROP_RAW_GPU_FEATURES:
    return "RAW_GPU_FEATURES";

  default:
    return "UNKNOWN";
  }
}

static uint64_t read_value(uint8_t *buf, size_t size) {
  uint64_t v = 0;

  for (size_t i = 0; i < size; i++)
    v |= ((uint64_t)buf[i]) << (i * 8);

  return v;
}

void parse_gpuprops(void *buffer, size_t length) {
  uint8_t *buf = buffer;
  size_t off = 0;

  while (off + 4 <= length) {

    uint32_t key = *(uint32_t *)(buf + off);

    off += 4;

    uint32_t id = key >> 2;
    uint32_t size_code = key & 0x3;

    size_t value_size;

    switch (size_code) {
    case KBASE_GPUPROP_VALUE_SIZE_U8:
      value_size = 1;
      break;

    case KBASE_GPUPROP_VALUE_SIZE_U16:
      value_size = 2;
      break;

    case KBASE_GPUPROP_VALUE_SIZE_U32:
      value_size = 4;
      break;

    case KBASE_GPUPROP_VALUE_SIZE_U64:
      value_size = 8;
      break;

    default:
      printf("Invalid size code %u\n", size_code);
      return;
    }

    if (off + value_size > length) {
      printf("Truncated property %u\n", id);
      return;
    }

    uint64_t value = read_value(buf + off, value_size);

    off += value_size;

    printf("%3u %-32s = 0x%016llx (%llu)\n", id, gpuprop_name(id),
           (unsigned long long)value, (unsigned long long)value);

    /* Some useful decoding */
    switch (id) {

    case 55: /* RAW_GPU_ID */
      printf("       GPU ID: 0x%08llx\n", (unsigned long long)value);
      
      struct gpu_info gpu;

      
      decode_gpu_id2(value, &gpu);

      print_gpu_info(&gpu);
      break;

    case 25: /* RAW_SHADER_PRESENT */
      printf("       Shader cores: %d\n", __builtin_popcountll(value));
      break;

    case 27: /* RAW_L2_PRESENT */
      printf("       L2 slices: %d\n", __builtin_popcountll(value));
      break;

    case 6: /* GPU_FREQ_KHZ_MAX */
      printf("       Max frequency: %llu kHz\n", (unsigned long long)value);
      break;
    }
  }
}