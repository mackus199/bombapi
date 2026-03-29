#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ulfius.h>
#include "main.h"
#include "endpoints.h"

static struct _u_instance instance;
static volatile sig_atomic_t keep_running = 1;

static void handle_signal(int signal_value) {
    (void)signal_value;
    keep_running = 0;
}

int main(void) {
    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
        fprintf(stderr, "Failed to initialize bombapi on port %d\n", PORT);
        return EXIT_FAILURE;
    }

    register_endpoints(&instance);

    if (ulfius_start_framework(&instance) != U_OK) {
        fprintf(stderr, "Failed to start bombapi\n");
        ulfius_clean_instance(&instance);
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    printf("bombapi listening on http://localhost:%d\n", PORT);
    fflush(stdout);

    while (keep_running) {
        sleep(1);
    }

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);
    return EXIT_SUCCESS;
}
