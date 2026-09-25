import csv
from pathlib import Path


DATA_FILE = Path(__file__).parent.parent / "data" / "simulation_results.csv"


def load_results():
    with open(DATA_FILE, newline="") as file:
        return list(csv.DictReader(file))


def count_handovers(rows):
    handovers = 0

    for previous, current in zip(rows, rows[1:]):
        if previous["serving_cell"] != current["serving_cell"]:
            handovers += 1

    return handovers


def main():
    rows = load_results()

    if not rows:
        print("No simulation data found.")
        return

    handovers = count_handovers(rows)

    first_time = rows[0]["time"]
    last_time = rows[-1]["time"]

    avg_cell_a_utilization = sum(
        float(row["cell_a_utilization"])
        for row in rows
    ) / len(rows)

    avg_cell_b_utilization = sum(
        float(row["cell_b_utilization"])
        for row in rows
    ) / len(rows)

    print("=== Simulation Analysis ===")
    print(f"Simulation duration: {last_time} seconds")
    print(f"Number of handovers: {handovers}")

    print(
        f"Average Cell-A utilization: "
        f"{avg_cell_a_utilization * 100:.2f}%"
    )

    print(
        f"Average Cell-B utilization: "
        f"{avg_cell_b_utilization * 100:.2f}%"
    )

    print(
        f"Initial serving cell: "
        f"{rows[0]['serving_cell']}"
    )

    print(
        f"Final serving cell: "
        f"{rows[-1]['serving_cell']}"
    )


if __name__ == "__main__":
    main()