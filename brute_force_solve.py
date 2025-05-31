import numpy as np
import hexalattice.hexalattice as hx
import matplotlib.pyplot as plt
import itertools

NUM_SQUARES = 37

TILES_SIDE1 = [
    [1, 5, 12, 19, 26, 33, 37],
    [3, 9, 16, 23, 30, 36],
    [6, 13, 20, 27, 34],
    [10, 17, 24, 31]
]
TILES_SIDE2 = [
    [1, 3, 6, 10],
    [2, 5, 9, 13, 17],
    [4, 8, 12, 16, 20, 24],
    [7, 11, 15, 19, 23, 27, 31]
]
TILES_SIDE3 = [
    [14, 18, 22, 26, 30, 34],
    [21, 25, 29, 33, 36],
    [28, 32, 35, 37]
]

TILES_SIDE4 = [
    [31, 34, 35, 37],
    [24, 27, 30, 33, 35],
    [17, 20, 23, 26, 29, 32],
    [10, 13, 16, 19, 22, 25, 28],
]

TILES_SIDE5 = [
    [6, 9, 12, 15, 18, 21],
    [3, 5, 8, 11, 14],
    [1, 2, 4, 7]
]

TILES_SIDE6 = [
    [7, 14, 21, 28],
    [4, 11, 18, 25, 32],
    [2, 8, 15, 22, 29, 32]
]

TILES = [TILES_SIDE1, TILES_SIDE2, TILES_SIDE3, TILES_SIDE4, TILES_SIDE5, TILES_SIDE6]
PLOT_MAP = [7, 14, 21, 28, 4, 11, 18, 25, 32, 2, 8, 15, 22, 29, 35, 1, 5, 12, 19, 26, 33, 37, 3, 9, 16, 23, 30, 36, 6, 13, 20, 27, 34, 10, 17, 24, 31]


# Convert from numbering to python indexing
TILES = [[[e - 1 for e in e_l] for e_l in t] for t in TILES]
PLOT_MAP = [i - 1 for i in PLOT_MAP]

YELLOW = 'Y'
BLACK = 'B'

def count_tiles(board, tiles):
    yellow_cnt = np.sum(board[tiles] == YELLOW)
    black_cnt = len(tiles) - yellow_cnt
    return (yellow_cnt, black_cnt)



def evaluate_board(board, constraints):
    for side in range(len(TILES)):
        for element in range(len(TILES[side])):
            t = count_tiles(board, TILES[side][element])
            if constraints[side][element] != t:
                #print(f"Fail on {side}, {element}: {t} not {constraints[side][element]}")
                return False
    
    # If we make it here this board is a good solution
    return True


def base_test():
    constraints = [
        [(5, 2), (2, 4), (2, 3), (2, 2)],
        [(2, 2), (3, 2), (1, 5), (6, 1)],
        [(4, 2), (2, 3), (2, 2)],
        [(2, 2), (2, 3), (4, 2), (3, 4)],
        [(3, 3), (3, 2), (3, 1)],
        [(2, 2), (4, 1), (3, 3)]
    ]
    solution1 = np.array(['Y', 'Y', 'B', 'Y', 'Y', 'Y', 'B', 'B', 'B', 'B', 'Y', 'B', 'B', 'Y', 'Y', 'B', 'Y', 'Y', 'Y', 'B', 'B', 'B', 'Y', 'B', 'Y', 'Y', 'Y','Y','Y','Y','Y', 'B','B','B','B','B','Y'])
    solution2 = np.array(['Y', 'Y', 'B', 'Y', 'Y', 'Y', 'B', 'B', 'B', 'B', 'Y', 'B', 'B', 'Y', 'Y', 'B', 'Y', 'Y', 'Y', 'B', 'B', 'B', 'Y', 'B', 'Y', 'Y', 'Y','Y','Y','Y','Y', 'B','B','B','B','B','B'])
    
    print(evaluate_board(solution1, constraints)) #Should be True
    print(evaluate_board(solution2, constraints)) #Should be false


def plot_solution(solution):  
    centers, _ = hx.create_hex_grid(n=50, do_plot=False, crop_circ=3, rotate_deg=90)
    centers_x = centers[:, 0]
    centers_y = centers[:, 1]

    colours = solution[PLOT_MAP]
    colours = ['y' if i == 'Y' else 'k' for i in colours]
    hx.plot_single_lattice_custom_colors(centers_x, centers_y, face_color=colours,
                                         edge_color=colours,
                                         min_diam=0.9,
                                         plotting_gap=0.05,
                                         rotate_deg=90)


def test_plot():
    solution1 = np.array(['Y', 'Y', 'B', 'Y', 'Y', 'Y', 'B', 'B', 'B', 'B', 'Y', 'B', 'B', 'Y', 'Y', 'B', 'Y', 'Y', 'Y', 'B', 'B', 'B', 'Y', 'B', 'Y', 'Y', 'Y','Y','Y','Y','Y', 'B','B','B','B','B','Y'])
    plot_solution(solution1)
    plt.show()


def brute_force_search():
    constraints = [
        [(5, 2), (2, 4), (2, 3), (2, 2)],
        [(2, 2), (3, 2), (1, 5), (6, 1)],
        [(4, 2), (2, 3), (2, 2)],
        [(2, 2), (2, 3), (4, 2), (3, 4)],
        [(3, 3), (3, 2), (3, 1)],
        [(2, 2), (4, 1), (3, 3)]
    ]

    i = 0
    correct_boards = []
    a = ("Y" * 37) + ("B" * 37)
    combinations = itertools.combinations(a, 37)

    while i < 10000000:
        i += 1
        next(combinations)

    while i < 1000000000:
        board = np.array(next(combinations))
        if evaluate_board(board, constraints):
            correct_boards.append(board)
            print(board)
        i += 1
        
    print(correct_boards)


if __name__ == "__main__":
    #base_test()

    #brute_force_search()
    #print(TILES)
    solutions = [54885035123, 59096134777, 62399147603, 70854861883]
    for s in solutions:
        solution = ['Y' if c == '1' else 'B' for c in "{:37b}".format(s)]
        solution.reverse()
        plot_solution(np.array(solution))

    plt.show()

