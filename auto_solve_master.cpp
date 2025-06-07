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


size_t read_in_numbers(uint8_t* dest, size_t max_len, char* str) {
    char* p_start = str;
    char* p_read = NULL;
    size_t p_dest = 0;
    bool done = false;

    while (p_dest < max_len && !done) {
        dest[p_dest++] = (uint8_t)strtol(p_start, &p_read, 10);
        if (!*p_read) {
            done = true;
        }
        p_start = p_read + 1;

    }

    return p_dest;
}


bool deterministic_solve(uint8_t* tiles) {

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

        //If the row is all one type of tiles, colour them in.
        if (tiles[r] == 0) {
            //All black
            board_black |= filter[r];
            set_tiles |= filter[r];
        } else if (tiles[r] == num_tiles_per_row[r]) {
            //All yellow
            board_yellow |= filter[r];
            set_tiles |= filter[r];
        }
    }

    //std::cout << std::format("{:037b}", set_tiles) << '\n';

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
        //std::cout << std::format("{:037b}", set_tiles) << '\n';
    } while (new_row_set && std::popcount(set_tiles) != 37);
    //Keep going whilst we are making progress and we haven't solved the board

    uint64_t board = board_yellow;
    if (test_board(board, tiles, filter_depth_3, ARRAY_LEN(filter_depth_3))) {
        // std::cout << "This tileset has one solution:" << '\n';
        std::cout << std::format("{:037b}", board) << '\n';
        return true;
    } else {
        // std::cout << "There exists more than one solution for this tile set" << '\n';
        return false;
    }
}

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


void brute_force_solve(uint8_t* tiles) {
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


int main(int argc, char* argv[]) {
    uint8_t* tiles;

    if (argc != 2) {
        std::cout << "Error, too many/few arguments...\n";
        return 0;
    }

    size_t array_size = ARRAY_LEN(filter_depth_3);
    tiles = (uint8_t*) malloc(array_size);

    if (tiles == nullptr) {
        std::cout << "Failed to allocate tile array, giving up.\n";
        return 0;
    }

    if (read_in_numbers(tiles, array_size, argv[1]) != array_size) {
        std::cout << "Not enough constriaints added. Give up\n";
        free(tiles);
        return 0;
    }

    if (!deterministic_solve(tiles)) {
        brute_force_solve(tiles);
    }

    free(tiles);

}