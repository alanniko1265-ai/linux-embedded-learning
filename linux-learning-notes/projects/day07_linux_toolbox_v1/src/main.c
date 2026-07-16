#include <stdio.h>
#include <string.h>
#include "log.h"
#include "time_utils.h"
#define APP_NAME "linux_toolbox"
#define APP_VERSION "1.0.0"

static void print_help(void)
{
    printf("Usage:\n");
    printf("  linux_toolbox help\n");
    printf("  linux_toolbox version\n");
    printf("  linux_toolbox time\n");
    printf("  linux_toolbox log <message>\n");
}

static void print_version(void)
{
    printf("%s version %s\n", APP_NAME, APP_VERSION);
}
int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_help();
        return 1;
    }

    if (strcmp(argv[1], "help") == 0) {
        print_help();
        return 0;
    }

    if (strcmp(argv[1], "version") == 0) {
        print_version();
        return 0;
    }
	if(strcmp(argv[1],"time")==0){
		print_current_time();
		return 0;	
	}
    if(strcmp(argv[1],"log")==0){
	if(argc<3){
		printf("missing log message\n");
		print_help();
		return 1;

}	
	print_log(argv[2]);
	return 0;	
 }
    printf("unknown command: %s\n", argv[1]);
    print_help();
    return 1;
}

