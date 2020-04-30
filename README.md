# Raymarching in C

Visit the website at [renders.datenvorr.at](https://renders.datenvorr.at)

![infinite spheres](renders/mandelbulb-pow3.png)

## Compiling:
in fish:
```fish
# load requirements
nix-shell --run fish
# run gcc with flags
gcc (pkg-config --cflags --libs gtk+-3.0 | string split " ") -lm -Wall -Wextra -o march.out main.c
```

## Running

give it the number of threads you want to use
```bash
./march.out <num threads>
```
