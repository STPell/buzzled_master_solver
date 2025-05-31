#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>
 
static_assert(std::popcount(0xFULL) == 4);

#define BIT64(n) (UINT64_C(1) << (n))
#define ARRAY_LEN(a) ((sizeof(a))/(sizeof((a)[0])))

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

uint64_t test_board_depth_3_valid   = 0b1000001111111010001110110010000111011;
uint64_t test_board_depth_3_invalid = 0b1000001111111010001110110010000111010;

// Board state represented with a binary number. The lower 37 bits indicate
// which tile is yellow/black indexed by order from top to bottom, left to right
// (see board_indexing.png). Thus we can extract which tiles we need to work with
// using bitwise operations. C++20 also provides a method for counting
bool test_board(uint64_t board, tiles_t* tiles, uint64_t* filters, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uint8_t tiles_y = std::popcount(board & filters[i]);
        uint8_t tiles_b = std::popcount((~board) & filters[i]);
        tiles_t* t = &tiles[i];

        //Possible speedup if I tell the compiler this is incredibly likely?
        if (t->y != tiles_y || t->b != tiles_b) {
            //If one of the tiles doesn't match the expected outcome the 
            //board is invalid, terminate check here for speed.
            return false;
        }
    }

    //If we make it here, the board is valid.
    return true;
}


void test_system() {
    static_assert(ARRAY_LEN(test_tiles_depth_3) == ARRAY_LEN(filter_depth_3));

    std::cout << test_board(test_board_depth_3_valid, test_tiles_depth_3, filter_depth_3, ARRAY_LEN(test_tiles_depth_3)) << '\n';
    std::cout << test_board(test_board_depth_3_invalid, test_tiles_depth_3, filter_depth_3, ARRAY_LEN(test_tiles_depth_3)) << '\n';
}


 
int main() {
    //test_system();

    uint64_t limit = BIT64(37);

    //Calculate the minimum viable board to cut down the search space
    //This is a board which has the first n bits set where n is the
    //the number of yellow tiles.
    uint8_t yellow_tile_cnt = 0;


    //The absolute maximum board is the board with the top n bits of the
    //board set to 1.

    for (uint64_t board = 0; board < limit; board++) {
        if (test_board(board, test_tiles_depth_3, filter_depth_3, ARRAY_LEN(test_tiles_depth_3))) {
            std::cout << board << '\n';
        }
    }
}