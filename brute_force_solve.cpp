#include <bit>
#include <bitset>
#include <cstdint>
#include <iostream>
#include <format>
#include <string>
#include <thread>
#include <vector>

static_assert(std::popcount(0xFULL) == 4);

#define BIT64(n) (UINT64_C(1) << (n))
#define ARRAY_LEN(a) ((sizeof(a))/(sizeof((a)[0])))

#define likely(x)    __builtin_expect (!!(x), 1)

#define NUM_THREADS 4

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

void test_system() {
    static_assert(ARRAY_LEN(yellow_tiles_test_depth_3) == ARRAY_LEN(filter_depth_3));

    std::cout << test_board(test_board_depth_3_valid, yellow_tiles_test_depth_3, filter_depth_3, ARRAY_LEN(filter_depth_3)) << '\n';
    std::cout << test_board(test_board_depth_3_invalid, yellow_tiles_test_depth_3, filter_depth_3, ARRAY_LEN(filter_depth_3)) << '\n';

    static_assert(ARRAY_LEN(yellow_tiles_53) == ARRAY_LEN(filter_depth_3));

    std::cout << test_board(sp_solution_53, yellow_tiles_53, filter_depth_3, ARRAY_LEN(filter_depth_3)) << '\n';
    std::cout << test_board(sp_solution_54, yellow_tiles_54, filter_depth_3, ARRAY_LEN(filter_depth_3)) << '\n';

}

/*************************************************************/
//https://web.archive.org/web/20130731200134/http://hackersdelight.org/hdcodetxt/snoob.c.txt

//Modified to use countr_zero instrinsic included in C++ 20, which
//compiles to a single x86 instruction

uint64_t next_set_of_n_elements(uint64_t x) {
    uint64_t smallest, ripple, new_smallest, ones;

                                              // x = xxx0 1111 0000
    smallest = x & -x;                        //     0000 0001 0000
    ripple = x + smallest;                    //     xxx1 0000 0000
    ones = x ^ ripple;                        //     0001 1111 0000
    ones = ones >> (2 + std::countr_zero(x)); //     0000 0000 0111
    return ripple | ones;                     //     xxx1 0000 0111
}
/*************************************************************/

typedef struct {
    uint8_t* tiles;
    uint64_t start;
    uint64_t end;
    std::vector<uint64_t>* results;
    std::mutex* results_mutex;
} search_task_t;

void bf_solve_thread(void *arg) {
    search_task_t* task = (search_task_t*) arg;

    uint8_t* tiles = task->tiles;
    uint64_t lower_limit = task->start;
    uint64_t upper_limit = task->end;

    //Iterate over every possible board, outputing valid solutions
    for (uint64_t board = lower_limit; board < upper_limit; board = next_set_of_n_elements(board)) {
    // for (uint64_t board = lower_limit; board < upper_limit; board++) {
        if (test_board(board, tiles, filter_depth_3, ARRAY_LEN(filter_depth_3))) {
            //std::cout << std::format("{:037b}", board) << '\n';
            task->results_mutex->lock();
            task->results->push_back(board);
            task->results_mutex->unlock();
        }
    }
}

int main(void) {

    //test_system();

    uint8_t* tiles = yellow_tiles_test_depth_3; //yellow_tiles_test_depth_3; //yellow_tiles_53;

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

    //OPTMISATION RECORDS.
    //Recorded based on the Buzzled #52 (yellow_tiles_test_depth_3)
    //- Full space search took ~17.5 minutes against the test board
    //- Cut down search space of the test board is about 99.999% the
    //  size of the original space but it was worth attempting. Took
    //  approx 16.5 min to run.
    //- Only need to check the yellow tiles, drops our exec time to
    //  aprox 10.5 min.
    //- Add branch prediction hint: drops the exec time to ~9.75 min.
    //- Turn on native architecture options on compiler: drops to ~4.5min
    //- Reduce search space by on making sure we only generate numbers with
    //  the exact required number of bits. Drops to ~4min.
    //- Improve number generation algorithm to remove division. Drops to ~90s.
    //- Change one step of number generation algorithm to use a C++ intrinsic
    //  which compiles down to a single x86 instruction TZCNT. Drops to ~59s.
    //- Parallelise but drop back to not caring about filtering for the right
    //  number of zeros gets us down to ~45s.
    //- Parallelise but begin caring about the right number of yellow tiles
    //  gets the exec time down to ~13s with 4 cores on my home machine.

    // Parallelise the search cause we gotta go fast
    search_task_t tasks[NUM_THREADS] = {0};
    std::vector<uint64_t> results;
    std::mutex results_mutex;
    std::thread threads[NUM_THREADS];

    // For now, just split the space into even chunks. Not all threads may do
    // the same amount of computation as the search space isn't necessarily
    // uniformly dense, but other options like pulling from a shared pool of
    // boards will probably tank each single threads performance.
    uint64_t interval = (upper_limit - lower_limit) / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; i++) {
        uint64_t start = lower_limit + (i * interval);
        int start_tile_cnt = std::popcount(start);
        if (start_tile_cnt < yellow_tile_cnt) {
            // Too few yellow tiles, incrementally add more to the end of the
            // board until we have the right number
            int j = 0;
            while (std::popcount(start) < yellow_tile_cnt) {
                start |= (1 << j);
                j++;
            }
        } else if (start_tile_cnt > yellow_tile_cnt) {
            // Too many yellow tiles, incrementally remove them from the end of
            // the board until we have the right number.
            int j = 0;
            while (std::popcount(start) > yellow_tile_cnt) {
                start &= ~(1 << j);
                j++;
            }
        }

        tasks[i].start = start;
        tasks[i].end = tasks[i].start + interval + 1;
        tasks[i].tiles = tiles;
        tasks[i].results = &results;
        tasks[i].results_mutex = &results_mutex;

        // std::cout << std::format("{:d}, start={:d}, end={:d}\n", i, tasks[i].start, tasks[i].end);

        threads[i] = std::thread(bf_solve_thread, (void*) &tasks[i]);
    }

    // Wait for all our child threads to finish their search
    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].join();
    }

    // Print all our hard earned results.
    for (uint64_t board: results) {
        std::cout << std::format("{:037b}", board) << '\n';
    }
}