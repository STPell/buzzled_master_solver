import numpy as np
import hexalattice.hexalattice as hx
import matplotlib.pyplot as plt

PLOT_MAP_DEPTH_3 = [7, 14, 21, 28, 4, 11, 18, 25, 32, 2, 8, 15, 22, 29, 35, 1, 5, 12, 19, 26, 33, 37, 3, 9, 16, 23, 30, 36, 6, 13, 20, 27, 34, 10, 17, 24, 31]
PLOT_MAP_DEPTH_2 = [4, 9, 14, 2, 7, 12, 17, 1, 5, 10, 15, 19, 3, 8, 13, 18, 6, 11, 16]
PLOT_MAP_DEPTH_4 = [11,20,29,38,47,7,16,25,34,43,52,4,12,21,30,39,48,56,2,8,17,26,35,44,53,59,1,5,13,22,31,40,49,57,61,3,9,18,27,36,45,54,60,6,14,23,32,41,50,58,10,19,28,37,46,55,15,24,33,42,51]

# Convert from numbering to python indexing
PLOT_MAP_DEPTH_3 = [i - 1 for i in PLOT_MAP_DEPTH_3]
PLOT_MAP_DEPTH_2 = [i - 1 for i in PLOT_MAP_DEPTH_2]
PLOT_MAP_DEPTH_4 = [i - 1 for i in PLOT_MAP_DEPTH_4]

NUM_TILES_DEPTH_3 = 37
NUM_TILES_DEPTH_2 = 19
NUM_TILES_DEPTH_4 = 61

SUM_POSITIONS_DEPTH_3 = [(-0.1, 3.5), (0.83, 3.0), (1.75, 2.5), (2.58, 2.0),
                         (3.1, 1.0), (3.1, 0.0), (3.1, -1.0), (3.1, -2.0),
                         (2.3, -2.4), (1.4, -2.9), (0.6, -3.4),
                         (-0.7, -3.4), (-1.5, -2.9), (-2.4, -2.4), (-3.2, -2.0),
                         (-3.2, -1.0), (-3.2, 0.0), (-3.2, 1.0),
                         (-2.7, 2.0), (-1.85, 2.5), (-0.9, 3.0)]

SUM_POSITIONS_DEPTH_2 = [(0, 2.6), (0.87, 2.1), (1.72, 1.6),
                         (2.33, 0.73), (2.33,-0.25), (2.33, -1.28),
                         (1.38, -1.77),(0.54, -2.23),
                         (-0.61, -2.3), (-1.44, -1.79), (-2.4, -1.27),
                         (-2.43, -0.29), (-2.43, 0.73),
                         (-1.72, 1.6), (-0.87, 2.1)]

YELLOW_HEX = "FFD700"
BLACK_HEX = "333333"
YELLOW_ARRAY = [1.0, 217/255, 0.0]
BLACK_ARRAY= [51/255, 51/255, 51/255]
STROKE_ARRAY = [0xAA/255,0xAA/255,0xAA/255]

YELLOW = 'Y'
BLACK = 'B'

def plot_solution_depth3(solution, solution_id, sums=None, weights=None):
    centers, _ = hx.create_hex_grid(n=50, do_plot=False, crop_circ=3, rotate_deg=90)
    centers_x = centers[:, 0]
    centers_y = centers[:, 1]

    colours = solution[PLOT_MAP_DEPTH_3]
    weight_colours = ['k' if i == YELLOW else 'w' for i in colours]
    colours = [YELLOW_ARRAY if i == YELLOW else BLACK_ARRAY for i in colours]
    edges = [STROKE_ARRAY for i in colours]

    hx.plot_single_lattice_custom_colors(centers_x, centers_y, face_color=colours,
                                         edge_color=edges,
                                         min_diam=0.9,
                                         plotting_gap=0.05,
                                         rotate_deg=90)

    ax = plt.gca()
    ax.axis('off')
    ax.set_title(f"{solution_id}")

    if weights is not None:
        weights = np.array(weights)[PLOT_MAP_DEPTH_3]

        for i in range(len(weights)):
            plt.text(centers_x[i], centers_y[i], weights[i], ha="center", va="center", c=weight_colours[i])

    if sums is None:
        ax.set_xlim([-3.1, 3.1])
        ax.set_ylim([-3.5, 3.5])
    else:
        ax.set_xlim([-3.3, 3.3])
        ax.set_ylim([-3.8, 3.8])
        for i in range(len(sums)):
            plt.text(SUM_POSITIONS_DEPTH_3[i][0], SUM_POSITIONS_DEPTH_3[i][1], sums[i])


def plot_solution_depth2(solution, solution_id, sums=None, weights=None):
    centers, _ = hx.create_hex_grid(n=50, do_plot=False, crop_circ=2, rotate_deg=90)
    centers_x = centers[:, 0]
    centers_y = centers[:, 1]

    colours = solution[PLOT_MAP_DEPTH_2]
    weight_colours = ['k' if i == YELLOW else 'w' for i in colours]
    colours = [YELLOW_ARRAY if i == YELLOW else BLACK_ARRAY for i in colours]
    edges = [STROKE_ARRAY for i in colours]

    hx.plot_single_lattice_custom_colors(centers_x, centers_y, face_color=colours,
                                         edge_color=edges,
                                         min_diam=0.9,
                                         plotting_gap=0.05,
                                         rotate_deg=90)

    ax = plt.gca()
    ax.axis('off')
    ax.set_title(f"{solution_id}")

    if weights is not None:
        weights = np.array(weights)[PLOT_MAP_DEPTH_2]

        for i in range(len(weights)):
            plt.text(centers_x[i], centers_y[i], weights[i], ha="center", va="center", c=weight_colours[i])

    if sums is None:
        ax.set_xlim([-3.1, 3.1])
        ax.set_ylim([-3.5, 3.5])
    else:
        ax.set_xlim([-3.3, 3.3])
        ax.set_ylim([-3.8, 3.8])
        for i in range(len(SUM_POSITIONS_DEPTH_2)):
            plt.text(SUM_POSITIONS_DEPTH_2[i][0], SUM_POSITIONS_DEPTH_2[i][1], sums[i], ha="center", va="center")


def plot_solution_depth4(solution, solution_id, sums=None, weights=None):
    centers, _ = hx.create_hex_grid(n=100, do_plot=False, crop_circ=4, rotate_deg=90)
    centers_x = centers[:, 0]
    centers_y = centers[:, 1]

    colours = solution[PLOT_MAP_DEPTH_4]
    weight_colours = ['k' if i == YELLOW else 'w' for i in colours]
    colours = [YELLOW_ARRAY if i == YELLOW else BLACK_ARRAY for i in colours]
    edges = [STROKE_ARRAY for i in colours]

    hx.plot_single_lattice_custom_colors(centers_x, centers_y, face_color=colours,
                                         edge_color=edges,
                                         min_diam=0.9,
                                         plotting_gap=0.05,
                                         rotate_deg=90)

    ax = plt.gca()
    ax.axis('off')
    ax.set_title(f"{solution_id}")

    if weights is not None:
        weights = np.array(weights)[PLOT_MAP_DEPTH_4]

        for i in range(len(weights)):
           plt.text(centers_x[i], centers_y[i], weights[i], ha="center", va="center", c=weight_colours[i])

    # if sums is None:
    #     ax.set_xlim([-3.1, 3.1])
    #     ax.set_ylim([-3.5, 3.5])
    # else:
    #     ax.set_xlim([-3.3, 3.3])
    #     ax.set_ylim([-3.8, 3.8])
    #     for i in range(len(SUM_POSITIONS_DEPTH_2)):
    #         plt.text(SUM_POSITIONS_DEPTH_2[i][0], SUM_POSITIONS_DEPTH_2[i][1], sums[i], ha="center", va="center")



def plot_solution(soln, soln_id, sums=None, weights=None):
    if len(soln) == NUM_TILES_DEPTH_2:
        plot_solution_depth2(soln, soln_id, sums=sums, weights=weights)
    elif len(soln) == NUM_TILES_DEPTH_3:
        plot_solution_depth3(soln, soln_id, sums=sums, weights=weights)
    else:
        plot_solution_depth4(soln, soln_id, sums=sums, weights=weights)



def test_plot():
    solution1 = np.array(['Y', 'Y', 'B', 'Y', 'Y', 'Y', 'B', 'B', 'B', 'B', 'Y', 'B', 'B', 'Y', 'Y', 'B', 'Y', 'Y', 'Y', 'B', 'B', 'B', 'Y', 'B', 'Y', 'Y', 'Y','Y','Y','Y','Y', 'B','B','B','B','B','Y'])
    plot_solution(solution1)
    plt.show()


def plot_solutions(solutions, sums=None, weights=None):
    i = 0
    if len(solutions) > 10:
        solutions_t = solutions

        while len(solutions_t) > 10:
            solutions = solutions_t[:10]
            solutions_t = solutions_t[10:]

            for s in solutions:
                i += 1
                solution = [YELLOW if c == '1' else BLACK for c in s]
                solution.reverse()
                plot_solution(np.array(solution), i, sums=sums)
            plt.show()

        for s in solutions_t:
            i += 1
            solution = [YELLOW if c == '1' else BLACK for c in s]
            solution.reverse()
            plot_solution(np.array(solution), i, sums=sums)
        plt.show()

    else:
        for s in solutions:
            i += 1
            solution = [YELLOW if c == '1' else BLACK for c in s]
            solution.reverse()
            plot_solution(np.array(solution), i, sums=sums, weights=weights)
        plt.show()

if __name__ == "__main__":
    solutions_52 = ["0110011000111011001110010110001110011", "0110111000010011001110111010001111001", "0111010000111010001110110111001010011", "1000001111111010001110110010000111011"]
    solutions_53 = ["1011001000101000000100111010110101110"]

    solutions_52_new = ["0001011111111000001110100110001010111", "0001111001110011101110100010000011111", "0001111001111011000110100110001010111", "0001111011110001001110100010101011101", "0001111011110010001100110011100011011", "0001111101011010001110100010011110011", "0011011000111011101110000110001010111", "0011011010111010001100010111101010011", "0011111000010011101110101010001011101", "0011111000011010001110111110001000111", "0011111000011010001111110010001110011", "0011111000011010011100110011101010011", "0011111000011011001010110010101111001", "0011111010010010001100111011101011001", "0100111001110011001110110010000111011", "0110011000111011001110010110001110011", "0110111000010011001110111010001111001", "0111010000111010001110110111001010011", "1000001111111010001110110010000111011", "1000011010111111001100010010101110011", "1000011100111001111110100010001010111", "1000011100111011101010110010000111011", "1000011100111011101110000010011110011", "1000011110011110001100111010001110011", "1000011110101011001110010110001110011", "1000011110110001101110101010001011101", "1000011110110010101100111011000011011", "1000011110111000001110111110001000111", "1000011110111000001111110010001110011", "1000011110111000011100110011101010011", "1000011110111001001010110010101111001", "1000011110111010000100111111001010011", "1000111000110011101110111010000001111", "1000111000111001011110110010101000111", "1000111000111011000110111110001000111", "1000111000111011000111110010001110011", "1000111000111011001110010010111100011", "1000111000111011001110110000001111110", "1000111000111011010100110011101010011", "1000111010110001001110111010101001101", "1000111010110011000100111011101011001", "1000111010110011001100010011101111010", "1000111100001011011110110010001110011", "1000111100011010001110111010011100011", "1000111110000011001110111010001111001", "1000111110010010001100111011001111010", "1001010000111111101110100010001010111", "1001010010111110001100110011101010011", "1001010100111010101110100011011010011", "1001010110101010001110110111001010011", "1001110000110011101110100011001011110", "1001110000111010001110110011111000011", "1001110010100011001110110011101011001", "1010001010111011001110010010101111001", "1010001110011010001110111010001111001", "1011000010111010001110110011101011001", "1100010000111111001110110010001110011", "1100110000110011001110110011001111010"]

    solutions_54 = ["1000101100000110100100100100110010100"]

    solutions_51 = ["0010011101100101100110111001111110111"]
    solutions_50 = ["0010011101100101100110111001111110111"]

    solutions = solutions_50

    solutions = ["1000001111111010001110110010000111011"]

    print(len(solutions))

    plot_solutions(solutions, sums=sums)#, weights=weights)

