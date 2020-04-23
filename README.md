# Raymarching in C

## Compiling:

```fish
# load requirements
nix-shell
# run gcc with flags
gcc (pkg-config --cflags --libs gtk+-3.0 | string split " ") -lm -Wall -Wextra -o march.out main.c
```