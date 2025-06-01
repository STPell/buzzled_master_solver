import numpy as np
import hexalattice.hexalattice as hx
import matplotlib.pyplot as plt

PLOT_MAP = [7, 14, 21, 28, 4, 11, 18, 25, 32, 2, 8, 15, 22, 29, 35, 1, 5, 12, 19, 26, 33, 37, 3, 9, 16, 23, 30, 36, 6, 13, 20, 27, 34, 10, 17, 24, 31]

# Convert from numbering to python indexing
PLOT_MAP = [i - 1 for i in PLOT_MAP]

YELLOW = 'Y'
BLACK = 'B'

def plot_solution(solution):
    centers, _ = hx.create_hex_grid(n=50, do_plot=False, crop_circ=3, rotate_deg=90)
    centers_x = centers[:, 0]
    centers_y = centers[:, 1]

    colours = solution[PLOT_MAP]
    colours = ['y' if i == YELLOW else 'k' for i in colours]
    hx.plot_single_lattice_custom_colors(centers_x, centers_y, face_color=colours,
                                         edge_color=colours,
                                         min_diam=0.9,
                                         plotting_gap=0.05,
                                         rotate_deg=90)


def test_plot():
    solution1 = np.array(['Y', 'Y', 'B', 'Y', 'Y', 'Y', 'B', 'B', 'B', 'B', 'Y', 'B', 'B', 'Y', 'Y', 'B', 'Y', 'Y', 'Y', 'B', 'B', 'B', 'Y', 'B', 'Y', 'Y', 'Y','Y','Y','Y','Y', 'B','B','B','B','B','Y'])
    plot_solution(solution1)
    plt.show()


if __name__ == "__main__":
    solutions_52 = ["0110011000111011001110010110001110011", "0110111000010011001110111010001111001", "0111010000111010001110110111001010011", "1000001111111010001110110010000111011"]
    solutions_53 = ["1011001000101000000110111010110101110", "1011001000101000000100111010110101110"]
    solutions = solutions_53
    for s in solutions:
        solution = [YELLOW if c == '1' else BLACK for c in s]
        solution.reverse()
        plot_solution(np.array(solution))

    plt.show()

