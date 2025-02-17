#include <ncurses.h>
#include <stdlib.h>

#define ROWS 25
#define COLUMNS 80

int **alloc_memory(int row_len, int col_len);
int input_f(int **buff, int row_len, int col_len, char *filename);
void draw(int **buff, int row_len, int col_len);
void update(int **buff, int row_len, int col_len);
int is_alive(int **buff, int i, int j);
void release(int **buff, int row_len);

int main(int argc, char **argv) {
    initscr();
    cbreak();
    noecho();
    halfdelay(5);

    int error_flag = (argc != 2);
    int memory_flag = 0;
    int file_error_flag = 0;
    char key;
    int **data = NULL;

    if (!error_flag) {
        data = alloc_memory(ROWS, COLUMNS);
        if (data != NULL) {
            memory_flag = 1;
        } else {
            error_flag = 1;
        }
    }

    if (!error_flag) {
        file_error_flag = input_f(data, ROWS, COLUMNS, argv[1]);
        error_flag = file_error_flag;
    }

    if (!error_flag) {
        while (1) {
            key = getch();
            if (key == 'q') {
                break;
            } else if (key >= '1' && key <= '9') {
                halfdelay(key - '0');
            }
            draw(data, ROWS, COLUMNS);
            update(data, ROWS, COLUMNS);
        }
        draw(data, ROWS, COLUMNS);
        while ((key = getch()) != 'q') {
        }
    }

    if (error_flag) {
        if (file_error_flag) {
            printw("File doesn't exist or incorrect: %s", argv[1]);
        } else {
            printw("n/a");
        }
        while ((key = getch()) != 'q') {
        }
    }

    if (memory_flag) {
        release(data, ROWS);
    }
    endwin();
    return 0;
}

int **alloc_memory(int row_len, int col_len) {
    int **data = malloc(row_len * sizeof(int *));
    if (data != NULL) {
        for (int i = 0; i < row_len; ++i) {
            data[i] = malloc(col_len * sizeof(int));
            if (data[i] == NULL) {
                for (int j = 0; j < i; ++j) {
                    free(data[j]);
                }
                free(data);
                break;
            }
        }
    }
    return data;
}

int input_f(int **buff, int row_len, int col_len, char *filename) {
    FILE *file = fopen(filename, "r");
    int error_flag = 0;
    if (file == NULL) {
        error_flag = 1;
    } else {
        int open_flag = 1;
        for (int i = 0; i < row_len; i++) {
            for (int j = 0; j < col_len; j++) {
                char ch;
                if ((ch = fgetc(file)) == EOF && ch != '\n') {
                    fclose(file);
                    open_flag = 0;
                    error_flag = 1;
                    break;
                } else if (ch == '0' || ch == '1') {
                    buff[i][j] = ch - '0';
                }
            }
            if (error_flag) {
                break;
            }
        }
        if (open_flag) {
            fclose(file);
        }
    }
    return error_flag;
}

void draw(int **buff, int row_len, int col_len) {
    for (int i = 0; i < row_len; ++i) {
        for (int j = 0; j < col_len; ++j) {
            printw("%c", (buff[i][j]) ? 'X' : '.');
        }
        printw("\n");
    }
    move(0, 0);
}

void update(int **buff, int row_len, int col_len) {
    int error_flag = 0;
    int memory_flag = 0;

    int **temp_data = alloc_memory(row_len, col_len);
    if (temp_data != NULL) {
        memory_flag = 1;
    } else {
        error_flag = 1;
    }

    if (!error_flag) {
        for (int i = 0; i < row_len; ++i) {
            for (int j = 0; j < col_len; ++j) {
                temp_data[i][j] = is_alive(buff, i, j);
            }
        }
        for (int i = 0; i < row_len; ++i) {
            for (int j = 0; j < col_len; ++j) {
                buff[i][j] = temp_data[i][j];
            }
        }
    }

    if (memory_flag) {
        release(temp_data, row_len);
    }
}

int is_alive(int **buff, int i, int j) {
    int count = 0;
    int result = -1;

    for (int i_ind = -1; i_ind < 2; ++i_ind) {
        for (int j_ind = -1; j_ind < 2; ++j_ind) {
            int temp_i = ((i + i_ind >= 0) && (i + i_ind < ROWS)) ? i + i_ind : (ROWS - 1) - i;
            int temp_j = ((j + j_ind >= 0) && (j + j_ind < COLUMNS)) ? j + j_ind : (COLUMNS - 1) - j;
            if (buff[temp_i][temp_j] == 1 && !(i_ind == 0 && j_ind == 0)) {
                ++count;
            }
        }
    }
    if (buff[i][j] == 0) {
        if (count == 3) {
            result = 1;
        } else {
            result = 0;
        }
    } else {
        if (count <= 1 || count >= 4) {
            result = 0;
        } else {
            result = 1;
        }
    }

    return result;
}

void release(int **buff, int row_len) {
    for (int i = 0; i < row_len; ++i) {
        free(buff[i]);
    }
    free(buff);
}