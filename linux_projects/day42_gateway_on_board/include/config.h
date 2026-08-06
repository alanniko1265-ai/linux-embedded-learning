#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_TEXT_SIZE 128

struct GatewayConfig {
    int server_port;
    char device_name[CONFIG_TEXT_SIZE];
    char log_path[CONFIG_TEXT_SIZE];
};

void config_set_default(struct GatewayConfig *config);
int load_config(const char *path, struct GatewayConfig *config);

#endif
