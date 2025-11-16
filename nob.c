#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define DEBUG
#define RAYLIB
#define SRC_DIR "src"
#define BUILD_DIR "build"
#define FILE_NAME "main"
#define EXE_NAME "chess"

Cmd cmd = {0};

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    cmd_append(&cmd, "cc");
    cmd_append(&cmd, "-Wall");
    cmd_append(&cmd, "-Wextra");
#ifdef DEBUG
    cmd_append(&cmd, "-ggdb");
#endif
#ifdef SRC_DIR
    mkdir_if_not_exists(SRC_DIR);
#endif
#ifdef BUILD_DIR
    mkdir_if_not_exists(BUILD_DIR);
#endif
    cmd_append(&cmd, "-o", "./"BUILD_DIR"/"EXE_NAME, "./"SRC_DIR"/"FILE_NAME".c");
#ifdef RAYLIB
    cmd_append(&cmd, "-I./raylib-5.5_linux_amd64/include/");
    cmd_append(&cmd, "-L./raylib-5.5_linux_amd64/lib/");
    cmd_append(&cmd, "-l:libraylib.a");
    cmd_append(&cmd, "-lm");
#endif
    if (!cmd_run(&cmd)) return 1;

    if (argc > 1) {
        const char* program_name = shift(argv, argc);
        nob_log(INFO, "%s", program_name);
        while (argc > 0) {
            const char* arg = shift(argv, argc);
            if (strcmp(arg, "run") == 0) {
                cmd_append(&cmd, BUILD_DIR"/"EXE_NAME);
                if (!cmd_run(&cmd)) return 1;
            }
        }
    }
    return 0;
}
