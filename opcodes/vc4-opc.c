#define _GNU_SOURCE

#include "opcode/vc4.h"

#include "../libvc4/vc4_arch.c"
#include "../libvc4/vc4_decode.c"
#include "../libvc4/vc4_util.c"
#include "../libvc4/eval.c"

struct vc4_info *vc4_info;

void vc4_load_opcode_info(void)
{
  char *arch;

  if (vc4_info == NULL) {
    arch = getenv("VC4_ARCH");
    if (arch == NULL)
      arch = "/home/marmar01/src/rpi/videocoreiv/videocoreiv.arch";
    vc4_info = vc4_read_arch_file(arch);

    vc4_get_opcodes(vc4_info);
  }
}
