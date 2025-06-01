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
    BIT64(30)|BIT64(33)|BIT64(34)|BIT64(36),
    BIT64(23)|BIT64(26)|BIT64(29)|BIT64(32)|BIT64(34),
    BIT64(16)|BIT64(19)|BIT64(22)|BIT64(25)|BIT64(28)|BIT64(31),
    BIT64(9)|BIT64(12)|BIT64(15)|BIT64(18)|BIT64(21)|BIT64(24)|BIT64(27),
    BIT64(5)|BIT64(8)|BIT64(11)|BIT64(14)|BIT64(17)|BIT64(20),
    BIT64(2)|BIT64(4)|BIT64(7)|BIT64(10)|BIT64(13),
    BIT64(0)|BIT64(1)|BIT64(3)|BIT64(6),
    BIT64(6)|BIT64(13)|BIT64(20)|BIT64(27),
    BIT64(3)|BIT64(10)|BIT64(17)|BIT64(24)|BIT64(31),
    BIT64(1)|BIT64(7)|BIT64(14)|BIT64(21)|BIT64(28)|BIT64(31)
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
    2,4,3};
uint64_t test_board_depth_3_valid   = 0b1000001111111010001110110010000111011;
uint64_t test_board_depth_3_invalid = 0b1000001111111010001110110010000111010;

//Buzzled #53
uint8_t yellow_tiles_53[] = {1,2,4,1,
    2,3,2,4,
    3,1,2,
    3,2,0,2,
    4,4,2,
    1,4,4};

uint64_t sp_solution_53 = 0b1011001000101000000100111010110101110;

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

void test_system() {
    static_assert(ARRAY_LEN(yellow_tiles_test_depth_3) == ARRAY_LEN(filter_depth_3));

    std::cout << test_board(test_board_depth_3_valid, yellow_tiles_test_depth_3, filter_depth_3, ARRAY_LEN(filter_depth_3)) << '\n';
    std::cout << test_board(test_board_depth_3_invalid, yellow_tiles_test_depth_3, filter_depth_3, ARRAY_LEN(filter_depth_3)) << '\n';

    static_assert(ARRAY_LEN(yellow_tiles_53) == ARRAY_LEN(filter_depth_3));

    std::cout << test_board(sp_solution_53, yellow_tiles_53, filter_depth_3, ARRAY_LEN(filter_depth_3)) << '\n';
}


int main() {

    test_system();

    uint8_t* tiles = yellow_tiles_53;

    //Calculate the minimum valid board to cut down the search space
    //This is a board which has the first n bits set where n is the
    //the number of yellow tiles. To cover all the tiles we need to look
    //at the vertical columns which are the first 4 and last 3 elements
    //of the tiles array.
    uint8_t yellow_tile_cnt = 0;
    for (size_t i = 0; i < 4; i++) {
        yellow_tile_cnt += tiles[i];
    }

    for (size_t i = ARRAY_LEN(filter_depth_3) - 1; i > ARRAY_LEN(filter_depth_3) - 3 - 1; i--) {
        yellow_tile_cnt += tiles[i];
    }

    uint64_t lower_limit = BIT64(yellow_tile_cnt) - 1;

    //The absolute maximum valid board is the board with the top n bits of the
    //board set to 1.
    uint8_t black_tile_cnt = 37 - yellow_tile_cnt;
    uint64_t upper_limit = lower_limit << black_tile_cnt;

    //- Full space search took ~17.5 minutes against the test board
    //- Cut down search space of the test board is about 99.999% the
    //  size of the original space but it was worth attempting. Took
    //  approx 16.5 min to run.
    //- Only need to check the yellow tiles, drops our exec time to
    //  aprox 10.5 min.
    //- Add branch prediction hint: drops the exec time to ~9.75 min.

    //TODO: Parallelise the search to speed things up?

    //Iterate over every possible board, outputing valid solutions
    for (uint64_t board = lower_limit; board < upper_limit; board++) {
        if (test_board(board, tiles, filter_depth_3, ARRAY_LEN(filter_depth_3))) {
            std::cout << std::format("{:037b}", board) << '\n';
        }
    }
}