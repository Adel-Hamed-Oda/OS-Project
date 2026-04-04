struct PCB {
    int pid;
    char *state;
    int program_counter;
    int *memoryboundaries; // [1, 2, 3] means I am occupying memory words 1, 2, and 3
};