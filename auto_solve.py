from selenium import webdriver
from selenium.webdriver.common.by import By
import time
import subprocess
import plot_solutions as plotter
import smt_buzzled_solve as solver

MASTER_AUTOSOLVE_PROGRAM = "./auto_solve_master"
RESULTS_FILE_NAME = "results.log"

URL = "https://puzzmallow.com/buzzled"
MASTER_BUZZLE_BUTTON_TEXT = "Master"
MINI_BUZZLE_BUTTON_TEXT = "Mini"
CLASSIC_BUZZLE_BUTTON_TEXT = "Classic"
EXPERT_BUZZLE_BUTTON_TEXT = "Expert"


'3\n3\n4\n 2\n4\n5\n2 \n1\n3\n3\n4 \n1\n2\n2 \n2\n5\n2 \n2\n3\n3\n1'
MASTER_SVG_TEXT_ORDER = [18,19,20,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17]
MASTER_PUZZLE_SUM_CNT = 21
TILES_PER_ROW_DEPTH_3 = [4,5,6,7,6,5,4,4,5,6,7,6,5,4,4,5,6,7,6,5,4] #In SVG Order

MINI_SVG_TEXT_ORDER = [13,14,0,1,2,3,4,5,6,7,8,9,10,11,12]


def select_puzzle(page, button_text):
    buttons = page.find_elements(By.CLASS_NAME, "control")

    for button in buttons:
        if button.text == button_text:
            button.click()
            return True

    print("Couldn't find the button to push :(")
    return False


def process_master(page):
    if not select_puzzle(page, MASTER_BUZZLE_BUTTON_TEXT):
        # We couldn't find the puzzle we wanted
        return

    svg = page.find_element(By.ID, "puzzle-svg")
    sums = svg.text.split('\n')

    if len(sums) != MASTER_PUZZLE_SUM_CNT:
        # We don't have the right number of sums, give up
        return

    sums_svg_order = [int(s) for s in sums]
    sums_prog_order = ["" for s in range(len(sums))]

    # Our program expects the sums to be the number of yellows in the row.
    # => We need to conver the black labels to yellow for later processing
    labels = svg.find_elements(By.CLASS_NAME, "label")

    if len(labels) != len(MASTER_SVG_TEXT_ORDER):
        # We have too many or too few labels than sums, which is not right, give up.
        return

    for i in range(len(sums)):
        dest_idx = MASTER_SVG_TEXT_ORDER[i]
        is_black = "black" in labels[i].find_element(By.TAG_NAME, "path").get_attribute("class")
        if is_black:
            sums_prog_order[dest_idx] = str(TILES_PER_ROW_DEPTH_3[i] - sums_svg_order[i])
        else:
            sums_prog_order[dest_idx] = str(sums_svg_order[i])

    sum_argument = ",".join(sums_prog_order)

    result = subprocess.check_output([MASTER_AUTOSOLVE_PROGRAM, sum_argument])
    try:
        result = result.decode('utf8')
    except UnicodeDecodeError:
        return

    results = result.strip().split("\n")

    current_date = time.strftime("%Y%m%d")
    with open(RESULTS_FILE_NAME, "a+") as res_file:
        res_file.write(f"{current_date};{sum_argument};{";".join(results)}\n")

    print(f"Master has: {len(results)} unique solutions")

    return results, sums_prog_order


def sum_tiles_in_row(w, tiles):
    return sum(w[t] for t in tiles)


def process_classic(page):
    # The classic puzzle is the same size as the master.
    if not select_puzzle(page, CLASSIC_BUZZLE_BUTTON_TEXT):
        # We couldn't find the puzzle we wanted
        return

    svg = page.find_element(By.ID, "puzzle-svg")
    encoded = svg.text.split('\n')
    sums = encoded[1:]
    weights_str = encoded[0].split(' ')

    # Change weights from SVG order into our order
    weights = [0 for i in range(solver.DEPTH_3_TILE_CNT)]
    for i in range(solver.DEPTH_3_TILE_CNT):
        weights[plotter.PLOT_MAP_DEPTH_3[i]] = int(weights_str[i])

    if len(sums) != MASTER_PUZZLE_SUM_CNT:
        # We don't have the right number of sums, give up
        return

    sums_svg_order = [int(s) for s in sums]
    sums_prog_order = [0 for s in range(len(sums))]

    # Our program expects the sums to be the number of yellows in the row.
    # => We need to convert the black labels to yellow for later processing
    labels = svg.find_elements(By.CLASS_NAME, "label")

    if len(labels) != len(MASTER_SVG_TEXT_ORDER):
        # We have too many or too few labels than sums, which is not right, give up.
        return

    for i in range(len(sums)):
        dest_idx = MASTER_SVG_TEXT_ORDER[i]
        is_black = "black" in labels[i].find_element(By.TAG_NAME, "path").get_attribute("class")
        if is_black:
            #Need to sum the total possible elements in a row and subtract required sum
            #to get the sum of yellow tiles needed.
            sums_prog_order[dest_idx] = sum_tiles_in_row(weights, solver.DEPTH_3_FILTERS[dest_idx]) - sums_svg_order[i]
        else:
            # Can just the use sum provided to us
            sums_prog_order[dest_idx] = sums_svg_order[i]

    s = solver.solve_classic(weights, sums_prog_order)
    if s is not None:
        plotter.plot_solutions([s], sums=sums_prog_order, weights=weights)


def process_mini(page):
    if not select_puzzle(page, MINI_BUZZLE_BUTTON_TEXT):
        # We couldn't find the puzzle we wanted
        return

    svg = page.find_element(By.ID, "puzzle-svg")
    encoded = svg.text.split('\n')
    sums = encoded[1:]
    weights_str = encoded[0].split(' ')

    # Change weights from SVG order into our order
    weights = [0 for i in range(solver.DEPTH_2_TILE_CNT)]
    for i in range(solver.DEPTH_2_TILE_CNT):
        weights[plotter.PLOT_MAP_DEPTH_2[i]] = int(weights_str[i])

    if len(sums) != len(solver.DEPTH_2_FILTERS):
        # We don't have the right number of sums, give up
        return

    sums_svg_order = [int(s) for s in sums]
    sums_prog_order = [0 for s in range(len(sums))]

    # Our program expects the sums to be the number of yellows in the row.
    # => We need to convert the black labels to yellow for later processing
    labels = svg.find_elements(By.CLASS_NAME, "label")

    if len(labels) != len(MINI_SVG_TEXT_ORDER):
        # We have too many or too few labels than sums, which is not right, give up.
        return

    for i in range(len(sums)):
        dest_idx = MINI_SVG_TEXT_ORDER[i]
        is_black = "black" in labels[i].find_element(By.TAG_NAME, "path").get_attribute("class")
        if is_black:
            #Need to sum the total possible elements in a row and subtract required sum
            #to get the sum of yellow tiles needed.
            sums_prog_order[dest_idx] = sum_tiles_in_row(weights, solver.DEPTH_2_FILTERS[dest_idx]) - sums_svg_order[i]
        else:
            # Can just the use sum provided to us
            sums_prog_order[dest_idx] = sums_svg_order[i]

    s = solver.solve_mini(weights, sums_prog_order)
    if s is not None:
        plotter.plot_solutions([s], sums=sums_prog_order, weights=weights)


def main():
    driver = webdriver.Firefox()
    driver.get(URL)

    i = 0
    buttons = driver.find_elements(By.CLASS_NAME, "control")
    while len(buttons) == 0 and i < 20:
        time.sleep(1.0)
        buttons = driver.find_elements(By.CLASS_NAME, "control")
        i += 1

    if len(buttons) == 0:
        driver.close()
        print("Failed to find the main puzzle. Giving up")
        return

    master_solutions, master_sums = process_master(driver)

    driver.close()

    plotter.plot_solutions(master_solutions, sums=master_sums)



if __name__ == "__main__":
    main()
