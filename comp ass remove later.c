// cs_moonlander.c
// Written by Charbel Badr (z5425323),
// on 30/09/2024
// TODO: Firstly, generate entities at user specified points on the board, 
// errors such as out of bound coordinates and other factors are considered.

#include <stdio.h>
#include <string.h>
// Provided Constant Definitions
#define BOARD_LEN 10
#define BOARD_MAX_LEVELS 32
#define BASE_OXY_RATE 1.0
#define MAX_PORTALS 16
#define TITLE_STR_LEN 25
#define INVALID_INDEX -1
#define COMMAND_QUIT 'q'

// TODO: Your #defines go here

// Provided Enum Definitions
enum entity {
    EMPTY,
    CHEESE,
    ROCK,
    LANDER,
    HOLE_DOWN,
    HOLE_UP,
    PORTAL
};

// TODO: Your enums go here

// Provided Struct Definitions
struct tile {
    enum entity entity;
};

// TODO: Your structs go here
struct cheese {
    int cheese_current;
    int cheese_transferred;
};

struct portal {
    int row_portal;
    int column_portal;
    int portal_call;
    int level;
};

struct player {
    double temp_base_rate;
    char command;
    char direction;
    int current_level;
    double current_oxy;
};
// Provided Function Prototypes
void init_board(
    struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS]
);
void print_board(
    struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS],
    int player_row,
    int player_col,
    int cheese_held,
    int cheese_lander,
    double oxy_capacity,
    double oxy_level,
    double oxy_rate,
    int current_level
);

// TODO: Your function prototypes go here
int valid_coordinate(int coordinate1, int coordinate2);
int valid_large(struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS], int row1, int col1, int row2, int col2);
int checking_refill(int row_start, int col_start, int mat_refill[8][2]);
int portal_pair(int portal_arr[8][5]);
int finding_portal(int row_start,int col_start,int portal_arr[8][5]);
int condition_up(int portal_arr[8][5],int row_start,int col_start,int row_found,struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS], int row, int col);
int hole_dir(int w_start,int col_start, 
struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS], 
int current_level, int row,int col);
int curr_hole(struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS],
 int row_start,int col_start,int current_level);
int portal_condition(struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS],
 int row_start,int col_start,int current_level,int row_portal,int column_portal,int level,
 int row, int col);
int coord_move(int row_start,int col_start,
 struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS],int current_level);
int main(void) {
    int row, col;
    printf("Please enter the [row] [col] of the lander: ");
    scanf("%d %d", &row, &col);
    int valid;
    valid = valid_coordinate(row, col);
    if (valid == 0) {
        row = 4;
        col = 4;
    }

    //initialising board (may have to remove later)
    struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS];
    init_board(board);
    board[row][col][0].entity = LANDER;
    print_board(board, INVALID_INDEX, INVALID_INDEX, 0, 0, 0.0, 0.0,
        BASE_OXY_RATE,0);

    //for other entities
    printf("Please enter cheese and rock locations:\n");
    char chars[13] = "r";
    int row1 = 0;
    int col1 = 0;
    int row2 = 0;
    int col2 = 0;

    //scanning until ctrld
    while(1) {
        scanf("%s", chars);
        
        if (strcmp(chars, "q") == 0) {
            break;
        }

        scanf("%d", &row1);
        scanf("%d", &col1);
        scanf("%d", &row2);
        scanf("%d", &col2);


        int valid1 = valid_coordinate(row1,col1);
        int valid2 = valid_coordinate(row2,col2);

        //check for Large rocks
        if (strcmp(chars,"R") == 0) {
            //see if input is in bounds
            if (valid1 == 0 || valid2 == 0) {
                printf("That is not a valid rock placement!\n");
                continue;
            }

            //see if range of data points is valid
            if (row2 < row1 || col2 < col1) {
                printf("That is not a valid rock placement!\n");
                continue;
            }

            //determine if there are any objects already in the grid
            int valid_rock = valid_large(board, row1, col1, row2, col2);
            if (valid_rock == 0) {
                printf("That is not a valid rock placement!\n");
                continue;
            }

            //placing rock after satisfying all conditions above
            for (int large_row = row1; large_row <= row2; large_row++) {
                for (int large_col = col1; large_col <= col2; large_col++) {
                    for (int depth = 0; depth < 32; depth ++) {
                        board[large_row][large_col][depth].entity = ROCK;
                    }
                }
            }
        }

        // check for rock
        if (strcmp(chars,"r") == 0) {
            if (valid1 == 0 || board[row1][col1][0].entity != EMPTY) {
                printf("That is not a valid rock placement!\n");
                continue;
            }
            for (int depth = 0; depth < 32; depth++) {
                board[row1][col1][depth].entity = ROCK;
            }
            
        }
        
        //check for cheese
        if (strcmp(chars,"c") == 0) {
            if (valid1 == 0 || board[row1][col1][0].entity != EMPTY) {
                printf("That is not a valid cheese placement!\n");
                continue;
            }
            for (int depth = 0; depth < 32; depth++) {
                board[row1][col1][depth].entity = CHEESE;
            }
        }
    }

    print_board(board, INVALID_INDEX, INVALID_INDEX, 0, 0, 0.0, 0.0,
        BASE_OXY_RATE,0);
    
    //initialising game parameters

    int cheese_quant;
    printf("Please enter the target qty of cheese: ");
    scanf("%d", &cheese_quant);
    while (cheese_quant < 0) {
    printf("The target qty of cheese must be >= 0!\n");
    scanf("%d", &cheese_quant);
    }
    
    double tank_capacity;
    printf("Please enter the player's oxygen tank capacity: ");
    scanf("%lf", &tank_capacity);
    while(tank_capacity < 0) {
        printf("The oxygen tank capacity must be >= 0!\n");
        scanf("%lf", &tank_capacity);
    }

    int row_start, col_start;
    printf("Please enter the [row] [col] of the player: ");
    scanf("%d %d", &row_start, &col_start);
    int valid_start = valid_coordinate(row_start,col_start);
    while (valid_start != 1 || board[row_start][col_start][0].entity != EMPTY) {
        printf("That is not a valid player placement!\n");
        scanf("%d %d", &row_start, &col_start);
        valid_start = valid_coordinate(row_start,col_start);
    }
    printf("<->        STARTING MOONLANDER        <->\n");
    print_board(board, row_start, col_start, 0, 0, 
    tank_capacity, tank_capacity, BASE_OXY_RATE,0);


    //determining coordinates for refill
    int x_lancer = row;
    int y_lancer = col;
    int skeleton_matrix[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},
                                {0,1},{1,-1},{1,0},{1,1}};
        int mat_refill[8][2];
    for (int i = 0; i < 8; i++) {
        mat_refill[i][0] = x_lancer + skeleton_matrix[i][0];
        mat_refill[i][1] = y_lancer + skeleton_matrix[i][1];
    }

    
    //player movement
    //double temp_base_rate = BASE_OXY_RATE;
    //char command = 'p';
    //char direction = 'w';
    //int player.current_level = 0;
    //double current_oxy = tank_capacity;
    //int cheese_current == 0;
    //int cheese_transferred == 0;
    //int row_portal = 0;
    //int portal.column_portal = 0;
    //int portal_call = 0;
    //creating portal

    struct player player = {BASE_OXY_RATE,'p','w',0,tank_capacity};
    struct cheese cheese = {0,0};
    struct portal portal = {0,0,0,0};
    int portal_arr[8][5] = {{-1,-1,-1,-1,-1}, {-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1},
                            {-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1}};

    while((int) player.command != 113) {
        
        //base command (movement and other abilities)
        scanf(" %c", &player.command);
        int as_ascii = (int) player.command;
        int temp_row = row_start;
        int temp_col = col_start;
        if (as_ascii == 119) {
            row_start -= 1;
            if (board[row_start][col_start][player.current_level].entity == PORTAL) {
                int valid_transport = 0;
                while (board[row_start][col_start][player.current_level].entity == PORTAL) {
                    int row_found = finding_portal(row_start,col_start,portal_arr);
                    int entry_portal = condition_up(portal_arr,row_start,col_start,row_found, board,-1,0);
                            if (entry_portal == 1) {
                                row_start = portal_arr[row_found][2] - 1;
                                col_start = portal_arr[row_found][3];
                                player.current_level = portal_arr[row_found][4];
                                valid_transport = 1;
                            } else if (entry_portal == 2) {
                                row_start = portal_arr[row_found][0] - 1;
                                col_start = portal_arr[row_found][1];
                                player.current_level = portal_arr[row_found][4];
                                valid_transport = 1;
                            } else {
                        row_start += 1;
                        break;
                    }
                }

            if (valid_transport == 1) {
                player.current_oxy -= player.temp_base_rate;
            }
            } else {
                player.current_oxy -= player.temp_base_rate;
            }
        } else if (as_ascii == 97) {
            col_start -= 1;
            if (board[row_start][col_start][player.current_level].entity == PORTAL) {
                int valid_transport = 0;
                while (board[row_start][col_start][player.current_level].entity == PORTAL) {
                    int row_found = finding_portal(row_start,col_start,portal_arr);
                    int entry_portal = condition_up(portal_arr,row_start,col_start,row_found, board,0,-1);
                    if (entry_portal == 1) {
                            row_start = portal_arr[row_found][2];
                            col_start = portal_arr[row_found][3] - 1;
                            player.current_level = portal_arr[row_found][4];
                            valid_transport = 1;
                    } else if (entry_portal == 2) {
                            row_start = portal_arr[row_found][0];
                            col_start = portal_arr[row_found][1] - 1;
                            player.current_level = portal_arr[row_found][4];
                            valid_transport = 1;
                    } else {
                        col_start += 1;
                        break;
                    }
                }
            if (valid_transport == 1) {
                player.current_oxy -= player.temp_base_rate;
            }
            } else {
                player.current_oxy -= player.temp_base_rate;
            }
        } else if (as_ascii == 115) {
            row_start += 1;
            if (board[row_start][col_start][player.current_level].entity == PORTAL) {
                int valid_transport = 0;
                while (board[row_start][col_start][player.current_level].entity == PORTAL) {
                    int row_found = finding_portal(row_start,col_start,portal_arr);
                    int entry_portal = condition_up(portal_arr,row_start,col_start,row_found, board,1,0);
                    if (entry_portal == 1) {
                            row_start = portal_arr[row_found][2] + 1;
                            col_start = portal_arr[row_found][3];
                            player.current_level = portal_arr[row_found][4];
                            valid_transport = 1;
                    } else if (entry_portal == 2) {
                            row_start = portal_arr[row_found][0] + 1;
                            col_start = portal_arr[row_found][1];
                            player.current_level = portal_arr[row_found][4];
                            valid_transport = 1;
                    } else {
                        row_start -= 1;
                        break;
                    }
                }
            if (valid_transport == 1) {
                player.current_oxy -= player.temp_base_rate;
            }
            } else {
                player.current_oxy -= player.temp_base_rate;
            }
        } else if (as_ascii == 100) {
            col_start += 1;
            if (board[row_start][col_start][player.current_level].entity == PORTAL) {
                int valid_transport = 0;
                while (board[row_start][col_start][player.current_level].entity == PORTAL) {
                    int row_found = finding_portal(row_start,col_start,portal_arr);
                    int entry_portal = condition_up(portal_arr,row_start,col_start,row_found, board,0,1);
                        if (entry_portal == 1) {
                            row_start = portal_arr[row_found][2];
                            col_start = portal_arr[row_found][3] + 1;
                            player.current_level = portal_arr[row_found][4];
                            valid_transport = 1;
                        } else if (entry_portal == 2) {
                            row_start = portal_arr[row_found][0];
                            col_start = portal_arr[row_found][1] + 1;
                            player.current_level = portal_arr[row_found][4];
                            valid_transport = 1;
                        } else {
                        col_start -= 1;
                        break;
                    }
                }
            if (valid_transport == 1) {
                player.current_oxy -= player.temp_base_rate;
            }
            } else {
                player.current_oxy -= player.temp_base_rate;
            }
        } else if (as_ascii == 113) {
            break; 
            } else if (as_ascii == 109) {
                // idea -- segment the grid into rows, each row will simply be transposed and placed on the column corresponding to to its row index -- implement as a function later
                    
                    int temp_mat[10][10][32];
                    for (int k = 0; k < 32; k++) {
                        for (int i = 0; i < BOARD_LEN; i++) {
                            for (int j = 0; j < BOARD_LEN; j++) {
                                if ((i == row_start) && (j == col_start)) {
                                    temp_mat[i][j][k] = 4;
                                } else if (board[i][j][k].entity == CHEESE) {
                                    temp_mat[i][j][k] = 1;
                                } else if (board[i][j][k].entity == ROCK) {
                                    temp_mat[i][j][k] = 2;
                                } else if (board[i][j][k].entity == LANDER) {
                                    temp_mat[i][j][k] = 3;
                                } else if (board[i][j][k].entity == EMPTY) {
                                    temp_mat[i][j][k] = 0;
                                } else if (board[i][j][k].entity == HOLE_UP) {
                                    temp_mat[i][j][k] = 5;
                                } else if (board[i][j][k].entity == HOLE_DOWN) {
                                    temp_mat[i][j][k] = 6;
                                } else if (board[i][j][k].entity == PORTAL) {
                                    temp_mat[i][j][k] = 7;
                                }
                            } 
                        }
                    }

                    for (int k = 0; k < 32; k ++) {
                        int counter = 0;
                        int x_copy = 0;
                        int y_copy = 0;
                        int x_true = 0;
                        int y_true = 9;
                        while(counter != 100) {
                            int curr = temp_mat[x_copy][y_copy][k];
                            if (curr == 0) {
                                board[x_true][y_true][k].entity = EMPTY;
                            } else if (curr == 1) {
                                board[x_true][y_true][k].entity = CHEESE;
                            } else if (curr == 2) {
                                board[x_true][y_true][k].entity = ROCK;
                            } else if (curr == 3) {
                                x_lancer = x_true;
                                y_lancer = y_true;
                                board[x_true][y_true][0].entity = LANDER;
                            } else if (curr == 4) {
                                row_start = x_true;
                                col_start = y_true;
                            } else if (curr == 5) {
                                board[x_true][y_true][k].entity = HOLE_UP;
                            } else if (curr == 6) {
                                board[x_true][y_true][k].entity = HOLE_DOWN;
                            } else if (curr == 7) {
                                board[x_true][y_true][k].entity = PORTAL;
                            }
                            if (y_copy == 9) {
                                y_true -= 1;
                                x_copy += 1;
                                y_copy = 0;
                                x_true = 0;
                            } else {
                            y_copy += 1;
                            x_true += 1;
                            }
                            counter += 1;
                        }
                    }


                    player.current_oxy -= 0.2 * tank_capacity;
                    player.temp_base_rate = player.temp_base_rate * 1.2;

                    // recreating LANDER grid
                    
                        for (int i = 0; i < 8; i++) {
                            mat_refill[i][0] = x_lancer + skeleton_matrix[i][0];
                            mat_refill[i][1] = y_lancer + skeleton_matrix[i][1];
                        }
            } else if (as_ascii == 104){
                scanf(" %c", &player.direction);
                int direction_int = (int) player.direction;
                int valid_jump = 0;
                    if (direction_int == 119) {
                        int direction = hole_dir(row_start,col_start,board,player.current_level,-1,0);
                        if (direction == 1) {
                            valid_jump = 1;
                            board[row_start - 1][col_start][player.current_level].entity = HOLE_DOWN;
                            board[row_start - 1][col_start][player.current_level + 1].entity = HOLE_UP;  
                        } } else if (direction_int == 97) {
                            int direction = hole_dir(row_start,col_start,board,player.current_level,0,-1);
                            if (direction == 1) {
                                board[row_start][col_start - 1][player.current_level].entity = HOLE_DOWN;
                                board[row_start][col_start - 1][player.current_level + 1].entity = HOLE_UP;
                                valid_jump = 1; 
                        } } else if (direction_int == 115) {
                            int direction = hole_dir(row_start,col_start,board,player.current_level,1,0);
                            if (direction == 1) {
                                board[row_start + 1][col_start][player.current_level].entity = HOLE_DOWN;
                                board[row_start + 1][col_start][player.current_level + 1].entity = HOLE_UP;  
                                valid_jump = 1;  
                        } } else if (direction_int == 100) {
                            int direction = hole_dir(row_start,col_start,board,player.current_level,0,1);
                            if (direction == 1) {
                                board[row_start][col_start + 1][player.current_level].entity = HOLE_DOWN;
                                board[row_start][col_start + 1][player.current_level + 1].entity = HOLE_UP; 
                                valid_jump = 1;
                            } }
                        if (valid_jump == 1) {
                            player.current_oxy -= player.temp_base_rate;
                        } } else if (as_ascii == 106) {
                            int current_hole = curr_hole(board,row_start,col_start,player.current_level);
                            if (current_hole == 1) {
                                player.current_level -= 1;
                                player.current_oxy -= player.temp_base_rate;
                            } else if (current_hole == 2) {
                                player.current_level += 1;
                                player.current_oxy -= player.temp_base_rate;
                            } } else if (as_ascii == 116) {
                            scanf(" %c", &player.direction);
                            scanf(" %d", &portal.level);
                            scanf(" %d", &portal.row_portal);
                            scanf(" %d", &portal.column_portal);
                            if (portal.portal_call == MAX_PORTALS) {
                                continue;
                            }
                            int next_portal;
                            int direction_portal = (int) player.direction;
                            int valid_portal = 0;
                            
                            if (direction_portal == 119) {
                                int portal_cond = portal_condition(board,row_start,col_start,player.current_level,portal.row_portal,portal.column_portal,portal.level,-1,0);
                                if (portal_cond == 1) {
                                    board[row_start - 1][col_start][player.current_level].entity = PORTAL;
                                    board[portal.row_portal][portal.column_portal][portal.level].entity = PORTAL;
                                    next_portal = portal_pair(portal_arr);
                                    portal_arr[next_portal][0] = row_start - 1;
                                    portal_arr[next_portal][1] = col_start;
                                    portal_arr[next_portal][2] = portal.row_portal;
                                    portal_arr[next_portal][3] = portal.column_portal;
                                    portal_arr[next_portal][4] = portal.level;
                                    valid_portal = 1;
                                }
                            } else if (direction_portal == 97) {
                                int portal_cond = portal_condition(board,row_start,col_start,player.current_level,portal.row_portal,portal.column_portal,portal.level,0,-1);
                                if (portal_cond == 1) {
                                    board[row_start][col_start - 1][player.current_level].entity = PORTAL;
                                    board[portal.row_portal][portal.column_portal][portal.level].entity = PORTAL;
                                    next_portal = portal_pair(portal_arr);
                                    portal_arr[next_portal][0] = row_start;
                                    portal_arr[next_portal][1] = col_start - 1;
                                    portal_arr[next_portal][2] = portal.row_portal;
                                    portal_arr[next_portal][3] = portal.column_portal;
                                    portal_arr[next_portal][4] = portal.level;
                                    valid_portal = 1;
                                } 
                            } else if (direction_portal == 115) {
                                int portal_cond = portal_condition(board,row_start,col_start,player.current_level,portal.row_portal,portal.column_portal,portal.level,1,0);
                                if (portal_cond == 1) {
                                    board[row_start + 1][col_start][player.current_level].entity = PORTAL;
                                    board[portal.row_portal][portal.column_portal][portal.level].entity = PORTAL;
                                    next_portal = portal_pair(portal_arr);
                                    portal_arr[next_portal][0] = row_start + 1;
                                    portal_arr[next_portal][1] = col_start;
                                    portal_arr[next_portal][2] = portal.row_portal;
                                    portal_arr[next_portal][3] = portal.column_portal;
                                    portal_arr[next_portal][4] = portal.level;
                                    valid_portal = 1;
                                } 
                            } else if (direction_portal == 100) {
                                int portal_cond = portal_condition(board,row_start,col_start,player.current_level,portal.row_portal,portal.column_portal,portal.level,0,1);
                                if (portal_cond == 1) {
                                    board[row_start][col_start + 1][player.current_level].entity = PORTAL;
                                    board[portal.row_portal][portal.column_portal][portal.level].entity = PORTAL;
                                    next_portal = portal_pair(portal_arr);
                                    portal_arr[next_portal][0] = row_start;
                                    portal_arr[next_portal][1] = col_start + 1;
                                    portal_arr[next_portal][2] = portal.row_portal;
                                    portal_arr[next_portal][3] = portal.column_portal;
                                    portal_arr[next_portal][4] = portal.level;
                                    valid_portal = 1; 
                                }
                            } if (valid_portal == 1) {
                                player.current_oxy -= player.temp_base_rate;
                                portal.portal_call += 2;
                            } } else {
                        printf("Command not recognised!\n");
                        continue;
                    }

        //determing whether i can move to a coordinate
        int move = coord_move(row_start,col_start,board,player.current_level);
        if (move == 1) {
            row_start = temp_row;
            col_start = temp_col;
            player.current_oxy += player.temp_base_rate;
        }

        if (board[row_start][col_start][player.current_level].entity == CHEESE) {
            cheese.cheese_current += 1;
            board[row_start][col_start][player.current_level].entity = EMPTY;
        }

        int bool = checking_refill(row_start,col_start,mat_refill);
        if (bool == 1) {
            player.current_oxy = tank_capacity;
            cheese.cheese_transferred += cheese.cheese_current;
            cheese.cheese_current = 0;
            player.temp_base_rate = BASE_OXY_RATE;
            if (cheese.cheese_transferred >= cheese_quant) {
                print_board(board, row_start, col_start, cheese.cheese_current, 
                cheese.cheese_transferred, tank_capacity, player.current_oxy, 
                player.temp_base_rate,player.current_level);
                printf("Congratulations, you won!\n");
                break;
            }
        }
        if (player.current_oxy <= 0) {
            print_board(board, row_start, col_start, cheese.cheese_current, 
            cheese.cheese_transferred, tank_capacity, player.current_oxy, 
            player.temp_base_rate,player.current_level);
            printf("Sorry, you ran out of oxygen and lost!\n");
            break;
        }
        print_board(board, row_start, col_start, cheese.cheese_current,
        cheese.cheese_current, tank_capacity, player.current_oxy, 
        player.temp_base_rate,player.current_level);
    }


    return 0;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////// YOUR FUNCTIONS //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int valid_coordinate(int coordinate1,int coordinate2) {
    int valid = 1;
    if (coordinate1 < 0 || coordinate1 > 9) {
        valid = 0;
    }
    if (coordinate2 < 0 || coordinate2 > 9) {
        valid = 0;
    }
    return valid;
}

int valid_large(struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS], int row1, int col1, int row2, int col2) {
    int valid = 1;
    for (int i = row1; i <= row2;++i) {
        for (int j = col1; j <= col2; ++j) {
            if (board[i][j][0].entity != EMPTY) {
                valid = 0;
                return valid;
            }
        }
    }
    return valid;
}

int checking_refill(int row_start, int col_start, int mat_refill[8][2]) {
    int bool = 0;
    for (int i = 0; i < 8; i++) {
        if (mat_refill[i][0] == row_start && mat_refill[i][1] == col_start) {
            bool = 1;
            return bool;
        }
    }
    return bool;
}

int portal_pair(int portal_arr[8][5]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 5; j++) {
            if (portal_arr[i][0] == -1) {
                return i;
            }
        }
    }
    return 0;;
}

int finding_portal(int row_start, int col_start, int portal_arr[8][5]) {
    for (int i = 0; i < 8; i++) {
        if ((portal_arr[i][0] == row_start) && (portal_arr[i][1] == col_start)) {
            return i;
        }
        if ((portal_arr[i][2] == row_start) && (portal_arr[i][3] == col_start)) {
            return i;
        }
    }
    return -1;
}


int condition_up(int portal_arr[8][5],int row_start,int col_start,int row_found,
                 struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS], int row,
                 int col) {
    int valid = 0;
    if ((portal_arr[row_found][0] == row_start) && (portal_arr[row_found][1] == col_start) && 
        (valid_coordinate(portal_arr[row_found][2] + row,portal_arr[row_found][3] + col) == 1) && 
        (board[portal_arr[row_found][2] + row ][portal_arr[row_found][3]+ col][portal_arr[row_found][4]].entity != ROCK) && 
        (board[portal_arr[row_found][2] + row][portal_arr[row_found][3] + col][portal_arr[row_found][4]].entity != LANDER)) {
            valid = 1;
    } else if ((portal_arr[row_found][2] == row_start) && (portal_arr[row_found][3] == col_start)
    && (valid_coordinate(portal_arr[row_found][0] + row,portal_arr[row_found][1] + col) == 1) && 
    (board[portal_arr[row_found][0] + row][portal_arr[row_found][1] + col][portal_arr[row_found][4]].entity != ROCK) && 
    (board[portal_arr[row_found][0] + row][portal_arr[row_found][1] + col][portal_arr[row_found][4]].entity != LANDER) ) {
        valid = 2;
    }
    return valid; 
}

int hole_dir(int row_start,int col_start,
struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS], int current_level, int row,
int col) {
    int pass = 0;
    if ((board[row_start + row][col_start + col][current_level].entity == EMPTY) 
    && (board[row_start + row][col_start + col][current_level + 1].entity == EMPTY)){
        if (current_level != 31) {                      
        }
        pass = 1;
    }
return pass;
}

int curr_hole(struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS],
 int row_start,int col_start,int current_level) {
    int valid = 0;
    if ((board[row_start][col_start][current_level].entity == HOLE_UP) && (current_level != 0)) {
        valid = 1;
    } else if ((board[row_start][col_start][current_level].entity == HOLE_DOWN) && (current_level != 31)) {
        valid = 2;
    }
    return valid;
 }

 int portal_condition(struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS],
 int row_start,int col_start,int current_level,int row_portal,int column_portal,int level,
 int row, int col) {
    int valid = 0;
    if ((board[row_start + row][col_start + col][current_level].entity == EMPTY) 
    && (board[row_portal][column_portal][level].entity == EMPTY) && 
    ((row_start != row_portal) || (col_start != column_portal) || 
    (current_level != level))) {
        valid = 1;
    }
    return valid;
 }

 int coord_move(int row_start,int col_start,
 struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS],int current_level) {
    int valid = 0;
    if (valid_coordinate(row_start,col_start) == 0 || 
    ((board[row_start][col_start][current_level].entity != EMPTY) 
    && (board[row_start][col_start][current_level].entity != CHEESE) && 
        (board[row_start][col_start][current_level].entity != HOLE_UP) && 
        (board[row_start][col_start][current_level].entity != HOLE_DOWN) && 
        board[row_start][col_start][current_level].entity != PORTAL)) {
            valid = 1;
        }
    return valid;
 }
////////////////////////////////////////////////////////////////////////////////
////////////////////////////// PROVIDED FUNCTIONS //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Initialises the board to EMPTY for each tile
void init_board(struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS]) {
    for (int row = 0; row < BOARD_LEN; row++) {
        for (int col = 0; col < BOARD_LEN; col++) {
            for (int depth = 0; depth < BOARD_MAX_LEVELS; depth++) {
                board[row][col][depth].entity = EMPTY;
            }
        }
    }
    return;
}

// Prints a line commonly used when printing the board, line length scales with
// the BOARD_LEN constant
void print_board_line(void) {
    printf("+");
    for (int col = 0; col < BOARD_LEN; col++) {
        printf("---+");
    }
    printf("\n");
    return;
}

// Prints the header of the cs_moonlander board
void print_board_header(void) {
    char title_str[TITLE_STR_LEN + 1] = "C S   M O O N L A N D E R";

    printf("|");
    for (int i = (4 * BOARD_LEN - TITLE_STR_LEN) / 2; i > 0; i--) {
        printf(" ");
    }

    printf("%s", title_str);

    for (int i = (4 * BOARD_LEN - TITLE_STR_LEN) / 2; i > 0; i--) {
        printf(" ");
    }
    printf("|\n");

    return;
}

// Prints the player stats when requested by the user
void print_player_stats(
    int cheese_held,
    int cheese_lander,
    double oxy_capacity,
    double oxy_level,
    double oxy_rate
) {
    printf("Player Cheese: %d     Lander Cheese: %d\n",
            cheese_held, cheese_lander);
    printf("Oxy: %.2lf / %.2lf  @  %.6lf / move\n", oxy_level,
            oxy_capacity, oxy_rate);
    return;
}

// Prints the cs_moonlander board
void print_board(
    struct tile board[BOARD_LEN][BOARD_LEN][BOARD_MAX_LEVELS],
    int player_row,
    int player_col,
    int cheese_held,
    int cheese_lander,
    double oxy_capacity,
    double oxy_level,
    double oxy_rate,
    int current_level) {

    print_board_line();
    print_board_header();
    print_board_line();
    for (int row = 0; row < BOARD_LEN; row++) {
        for (int col = 0; col < BOARD_LEN; col++) {
            printf("|");
            if (row == player_row && col == player_col) {
                printf("0.0");
            } else if (board[row][col][current_level].entity == ROCK) {
                printf("^^^");
            } else if (board[row][col][current_level].entity == LANDER) {
                printf("/|\\");
            } else if (board[row][col][current_level].entity == CHEESE) {
                printf("<(]");
            } else if (board[row][col][current_level].entity == HOLE_UP) {
                printf("/O\\");
            } else if (board[row][col][current_level].entity == HOLE_DOWN) {
                printf("\\O/");
            } else if (board[row][col][current_level].entity == PORTAL) {
                printf("~~~");
            } else {
                printf("   ");
            }
        }
        printf("|\n");
        print_board_line();
    }
    print_player_stats(cheese_held, cheese_lander, oxy_capacity, oxy_level,
            oxy_rate);
    print_board_line();
    return;
}

