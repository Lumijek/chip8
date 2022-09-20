# Chip8 Interpreter

Fully functioning(I think) chip8 implementation (without sound).

**Steps to run (for mac os)**:
1. Go to the root directory of the project.

2. Create and enter `build` directory with `mkdir build && cd build`.

3. Make project with `cmake .. && make`

4. Run game with `./game`

Currently nothing is in place to load ROM directly from command line so you will have to go into the project and change the 3rd line in the `main` function of `main.c` to specify where your ROM file is (the `load_rom` function).
