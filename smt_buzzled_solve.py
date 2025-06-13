import z3
import plot_solutions as plotter

DEPTH_2_TILE_CNT = 19
DEPTH_2_FILTERS = [
    [0,4,9,14,18],
    [2,7,12,17],
    [5,10,15],
    [0,2,5],
    [1,4,7,10],
    [3,6,9,12,15],
    [8,11,14,17],
    [13,16,18],
    [15,17,18],
    [10,12,14,16],
    [5,7,9,11,13],
    [2,4,6,8],
    [0,1,3],
    [3,8,13],
    [1,6,11,16],
]


DEPTH_3_TILE_CNT = 37
DEPTH_3_FILTERS = [
    [0, 4, 11, 18, 25, 32, 36],
    [2, 8, 15, 22, 29, 35],
    [5, 12, 19, 26, 33],
    [9, 16, 23, 30],
    [0, 2, 5, 9],
    [1, 4, 8, 12, 16],
    [3, 7, 11, 15, 19, 23],
    [6, 10, 14, 18, 22, 26, 30],
    [13, 17, 21, 25, 29, 33],
    [20, 24, 28, 32, 35],
    [27, 31, 34, 36],
    [30, 33, 35, 36],
    [23, 26, 29, 32, 34],
    [16, 19, 22, 25, 28, 31],
    [9, 12, 15, 18, 21, 24, 27],
    [5, 8, 11, 14, 17, 20],
    [2, 4, 7, 10, 13],
    [0, 1, 3, 6],
    [6, 13, 20, 27],
    [3, 10, 17, 24, 31],
    [1, 7, 14, 21, 28, 34]
]

DEPTH_4_TILE_CNT = 61
DEPTH_4_FILTERS = [
    [0,4,12,21,30,39,48,56,60],
    [2,8,17,26,35,44,53,59],
    [5,13,22,31,40,49,57],
    [9,18,27,36,45,54],
    [14,23,32,41,50],
    [0,2,5,9,14],
    [1,4,8,13,18,23],
    [3,7,12,17,22,27,32],
    [6,11,16,21,26,31,36,41],
    [10,15,20,25,30,35,40,45,50],
    [19,24,29,34,39,44,49,54],
    [28,33,38,43,48,53,57],
    [37,42,47,52,56,59],
    [46,51,55,58,60],
    [50,54,57,59,60],
    [41,45,49,53,56,58],
    [32,36,40,44,48,52,55],
    [23,27,31,35,39,43,47,51],
    [14,18,22,26,30,34,38,42,46],
    [9,13,17,21,25,29,33,37],
    [5,8,12,16,20,24,28],
    [2,4,7,11,15,19],
    [0,1,3,6,10],
    [10,19,28,37,46],
    [6,15,24,33,42,51],
    [3,11,20,29,38,47,55],
    [1,7,16,25,34,43,52,58]
]

def solve_generic(weights, sums, rows, n_tiles):
    if len(weights) != n_tiles:
        return None

    if len(sums) != len(rows):
        return None

    solver = z3.Solver()

    var = [z3.Int(f"tile_{i}") for i in range(n_tiles)]
    tiles_are_0_or_value = [z3.Or(var[i] == 0, var[i] == weights[i]) for i in range(n_tiles)]

    solver.add(tiles_are_0_or_value)

    for i in range(len(rows)):
        row = rows[i]
        s = var[row[0]]
        for j in range(len(row[1:])):
            s += var[row[j + 1]]

        #print(s)
        solver.add(s == sums[i])

    #print(solver)
    if solver.check() == z3.sat:
        m = solver.model()

        solution = ""
        for v in var[::-1]:
            if m.eval(v).as_long() != 0:
                solution += "1"
            else:
                solution += "0"

        return solution
    else:
        return None


def solve_classic(w, s):
    return solve_generic(w, s, DEPTH_3_FILTERS, DEPTH_3_TILE_CNT)


def solve_expert(w, s):
    return solve_generic(w, s, DEPTH_4_FILTERS, DEPTH_4_TILE_CNT)


def solve_mini(w, s):
    return solve_generic(w, s, DEPTH_2_FILTERS, DEPTH_2_TILE_CNT)


def test_classic():
    weights_classic_59 = [2,5,1,3,1,2,3,4,1,2,4,4,4,5,5,5,4,1,3,3,5,5,5,5,1,2,3,3,3,2,2,2,5,4,1,5,5]
    sums_classic_59 = [5,11,10,11,2,10,21,14,14,9,6,11,6,9,18,12,14,6,11,10,18]
    solution_classic_59 = "0111011011001101000011111110111111000"

    s = solve_classic(weights_classic_59, sums_classic_59)
    print(s)
    print(solution_classic_59)
    print(s == solution_classic_59)

    plotter.plot_solutions([s], sums=sums_classic_59, weights=weights_classic_59)


def test_mini():
    weights_mini_65 = [1,4,5,3,5,4,1,3,4,1,3,5,2,1,1,3,4,1,3]
    sums_mini_65 = [3,10,10,10,10,7,6,1,3,6,14,6,5,1,10]
    solution_mini_65 = "0001111111011100111"

    s = solve_mini(weights_mini_65, sums_mini_65)
    print(s)
    print(solution_mini_65)
    print(s == solution_mini_65)

    plotter.plot_solutions([s], sums=sums_mini_65, weights=weights_mini_65)


def test_expert():
    weights_expert_65 = [3, 3, 1, 3, 4, 4, 5, 3, 2, 5, 3, 5, 1, 4, 4, 4, 4, 2, 1, 4, 3, 3, 2, 5, 1, 2, 3, 1, 1, 2, 4, 5, 2, 5, 1, 2, 2, 4, 1, 1, 3, 3, 1, 2, 1, 4, 1, 4, 1, 4, 2, 1, 3, 4, 2, 3, 3, 1, 4, 3, 5]
    sums_expert_65 = [13, 6, 11, 11, 7, 13, 8, 5, 17, 14, 6, 7, 14, 6, 9, 10, 2, 5, 13, 22, 8, 13, 8, 9, 14, 9, 10]
    solution_expert_65 = "1101100000001111100100010011001001010010011111110101001100010"

    s = solve_expert(weights_expert_65, sums_expert_65)
    print(s)
    print(solution_expert_65)
    print(s == solution_expert_65)

    plotter.plot_solutions([s], sums=sums_expert_65, weights=weights_expert_65)


if __name__ == "__main__":
    test_classic()
    test_mini()
    test_expert()
