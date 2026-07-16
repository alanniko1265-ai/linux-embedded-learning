# Day 7 - linux_toolbox_v1

This directory is for your Week 1 review project.

Implement the project yourself. Use this file only as the requirement list.

## Required features

Create a command-line tool named `linux_toolbox`.

It should support:

```text
./linux_toolbox version
./linux_toolbox time
./linux_toolbox log "message"
./linux_toolbox help
```

Expected behavior:

- `version`: print program name and version.
- `time`: print current local time.
- `log "message"`: print a timestamped INFO log.
- `help`: print supported commands.
- unknown command: print an error and usage hint.

## Required project structure

Suggested structure:

```text
day07_linux_toolbox_v1/
├── include/
│   ├── log.h
│   └── time_utils.h
├── src/
│   ├── main.c
│   ├── log.c
│   └── time_utils.c
├── Makefile
└── README.md
```

## Required build commands

At minimum, support:

```bash
make
make run
make clean
```

Optional:

```bash
make test
```

## Acceptance checks

```bash
make clean
make
./build/linux_toolbox version
./build/linux_toolbox time
./build/linux_toolbox log "device gateway started"
./build/linux_toolbox help
./build/linux_toolbox unknown
```

## Week 1 skills used

- multi-file C project
- header/source separation
- `gcc -Wall -g -O0`
- Makefile
- command-line arguments: `argc` and `argv`
- timestamp formatting
- simple error handling
