#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <format>

static_assert(std::popcount(0xFULL) == 4);

#define BIT64(n) (UINT64_C(1) << (n))
#define ARRAY_LEN(a) ((sizeof(a))/(sizeof((a)[0])))

#define likely(x)    __builtin_expect (!!(x), 1)

typedef struct {
    uint8_t  y;
    uint8_t  b;
} tiles_t;

uint64_t filter_depth_3[] = {
    BIT64(0)|BIT64(4)|BIT64(11)|BIT64(18)|BIT64(25)|BIT64(32)|BIT64(36),
    BIT64(2)|BIT64(8)|BIT64(15)|BIT64(22)|BIT64(29)|BIT64(35),
    BIT64(5)|BIT64(12)|BIT64(19)|BIT64(26)|BIT64(33),
    BIT64(9)|BIT64(16)|BIT64(23)|BIT64(30),
    BIT64(0)|BIT64(2)|BIT64(5)|BIT64(9),
    BIT64(1)|BIT64(4)|BIT64(8)|BIT64(12)|BIT64(16),
    BIT64(3)|BIT64(7)|BIT64(11)|BIT64(15)|BIT64(19)|BIT64(23),
    BIT64(6)|BIT64(10)|BIT64(14)|BIT64(18)|BIT64(22)|BIT64(26)|BIT64(30),
    BIT64(13)|BIT64(17)|BIT64(21)|BIT64(25)|BIT64(29)|BIT64(33),
    BIT64(20)|BIT64(24)|BIT64(28)|BIT64(32)|BIT64(35),
    BIT64(27)|BIT64(31)|BIT64(34)|BIT64(36),
    BIT64(30)|BIT64(33)|BIT64(35)|BIT64(36),
    BIT64(23)|BIT64(26)|BIT64(29)|BIT64(32)|BIT64(34),
    BIT64(16)|BIT64(19)|BIT64(22)|BIT64(25)|BIT64(28)|BIT64(31),
    BIT64(9)|BIT64(12)|BIT64(15)|BIT64(18)|BIT64(21)|BIT64(24)|BIT64(27),
    BIT64(5)|BIT64(8)|BIT64(11)|BIT64(14)|BIT64(17)|BIT64(20),
    BIT64(2)|BIT64(4)|BIT64(7)|BIT64(10)|BIT64(13),
    BIT64(0)|BIT64(1)|BIT64(3)|BIT64(6),
    BIT64(6)|BIT64(13)|BIT64(20)|BIT64(27),
    BIT64(3)|BIT64(10)|BIT64(17)|BIT64(24)|BIT64(31),
    BIT64(1)|BIT64(7)|BIT64(14)|BIT64(21)|BIT64(28)|BIT64(34)
};

// Buzzled #52
tiles_t test_tiles_depth_3[] = {
    {.y=5, .b=2},
    {.y=2, .b=4},
    {.y=2, .b=3},
    {.y=2, .b=2},
    {.y=2, .b=2},
    {.y=3, .b=2},
    {.y=1, .b=5},
    {.y=6, .b=1},
    {.y=4, .b=2},
    {.y=2, .b=3},
    {.y=2, .b=2},
    {.y=2, .b=2},
    {.y=2, .b=3},
    {.y=4, .b=2},
    {.y=3, .b=4},
    {.y=3, .b=3},
    {.y=3, .b=2},
    {.y=3, .b=1},
    {.y=2, .b=2},
    {.y=4, .b=1},
    {.y=3, .b=3},
};

uint8_t yellow_tiles_test_depth_3[] = {5,2,2,2,
    2,3,1,6,
    4,2,2,
    2,2,4,3,
    3,3,3,
    2,4,3
};
uint64_t test_board_depth_3_valid   = 0b1000001111111010001110110010000111011;
uint64_t test_board_depth_3_invalid = 0b1000001111111010001110110010000111010;

//Buzzled #53
uint8_t yellow_tiles_53[] = {1,2,4,1,
    2,3,2,4,
    3,1,2,
    3,2,0,2,
    4,4,2,
    1,4,4
};
uint64_t sp_solution_53 = 0b1011001000101000000100111010110101110;

uint8_t yellow_tiles_54[] = {4,4,0,2,
    1,2,3,3,
    2,2,1,
    2,3,1,0,
    5,3,0,
    1,1,2
};
uint64_t sp_solution_54 = 0b1000101100000110100100100100110010100;

uint8_t yellow_tiles_51[] = {2,3,3,4,4,5,2,4,4,1,3,1,4,2,4,5,4,3,4,2,5};
uint8_t yellow_tiles_50[] = {2,3,3,1,2,3,5,3,3,2,2,0,1,3,6,4,5,1,2,5,4};
uint8_t yellow_tiles_49[] = {3,3,1,1,1,1,3,4,3,3,2,1,3,3,3,4,1,2,2,2,5};

// Each board state represented with a binary number. The lower 37 bits indicate
// which tile is yellow/black indexed by order from top to bottom, left to right
// (see board_indexing.png). Thus we can extract which tiles we need to work with
// using bitwise operations. C++20 also provides a method for counting the number
// of bits set in any integer (which should compile down to the x86 POPCNT
// instruction). Since we set bits to be yellow, we can count the number of
// yellow tiles easily this way. We also only need to count the yellow tiles.
bool test_board(uint64_t board, uint8_t* tiles, uint64_t* filters, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uint8_t tiles_y = std::popcount(board & filters[i]);

        if (likely(tiles[i] != tiles_y)) {// || t->b != tiles_b) {
            //If one of the tiles doesn't match the expected outcome the
            //board is invalid, terminate check here for speed.
            return false;
        }
    }

    //If we make it here, the board is valid.
    return true;
}

int main(void) {
    uint8_t* tiles = yellow_tiles_50;


    //Generate the number of tiles per row from our filter array.
    uint8_t num_tiles_per_row[ARRAY_LEN(filter_depth_3)] = {0};
    for (size_t i = 0; i < ARRAY_LEN(num_tiles_per_row); i++) {
        num_tiles_per_row[i] = std::popcount(filter_depth_3[i]);
    }

    uint64_t set_tiles    = 0; //Records which tiles we have solved
    uint64_t board_yellow = 0; //Records which tiles are yellow
    uint64_t board_black  = 0; //Records which tiles are black
    bool new_row_set = false;
    uint64_t* filter = filter_depth_3;
    size_t num_rows = ARRAY_LEN(filter_depth_3);

    //Do the easy gimme's first before doing iterative solve
    for (size_t r = 0; r < num_rows; r++) {

        //If the row is all black tiles, colour them in.
        if (tiles[r] == 0) {
            board_black |= filter[r];
            set_tiles |= filter[r];
        } else if (tiles[r] == num_tiles_per_row[r]) {
            board_yellow |= filter[r];
            set_tiles |= filter[r];
        }
    }

    std::cout << std::format("{:037b}", set_tiles) << '\n';

    do {
        new_row_set = false;
        size_t curr_row = 0;

        while (curr_row < num_rows) {
            uint64_t currently_set_tiles = set_tiles & filter[curr_row];
            uint8_t init_set_tiles = std::popcount(currently_set_tiles);

            if (init_set_tiles > 0 && init_set_tiles < num_tiles_per_row[curr_row]) {
                //std::cout << "Dealing with row " << curr_row << '\n';
                //Only try and deal with a row, if we have some purchase on it
                uint64_t yellow_tiles_in_row = board_yellow & filter[curr_row];
                uint64_t black_tiles_in_row = board_black & filter[curr_row];
                uint8_t num_black_tiles_req = num_tiles_per_row[curr_row] - tiles[curr_row];

                //std::cout << std::format("Row needs {:d} yellow and {:d} black, has {:d} and {:d}", tiles[curr_row], num_black_tiles_req, yellow_tiles_in_row, std::popcount(black_tiles_in_row)) << '\n';

                if (tiles[curr_row] == std::popcount(yellow_tiles_in_row)) {
                    //We have enough yellow tiles in this row, fill in the rest with black
                    uint64_t tiles_to_fill = (yellow_tiles_in_row ^ filter[curr_row]) & filter[curr_row];
                    set_tiles |= tiles_to_fill;
                    board_black |= tiles_to_fill;
                    new_row_set = true;
                } else if (num_black_tiles_req == std::popcount(black_tiles_in_row)) {
                    //We have enough black tiles in this row, fill in the rest with yellow
                    uint64_t tiles_to_fill = (black_tiles_in_row ^ filter[curr_row]) & filter[curr_row];
                    set_tiles |= tiles_to_fill;
                    board_yellow |= tiles_to_fill;
                    new_row_set = true;
                }
            }

            curr_row++;
        }
        std::cout << std::format("{:037b}", set_tiles) << '\n';
    } while (new_row_set && std::popcount(set_tiles) != 37);
    //Keep going whilst we are making progress and we haven't solved the board

    uint64_t board = board_yellow;
    if (test_board(board, tiles, filter_depth_3, ARRAY_LEN(filter_depth_3))) {
        std::cout << "This tileset has one solution:" << '\n';
        std::cout << std::format("{:037b}", board) << '\n';
    } else {
        std::cout << "There exists more than one solution for this tile set" << '\n';
        std::cout << std::format("{:037b}", board) << '\n';
    }
    return 0;
}