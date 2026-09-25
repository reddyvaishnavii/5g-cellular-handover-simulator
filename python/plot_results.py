import csv
from pathlib import Path

import matplotlib.pyplot as plt


DATA_FILE = Path(__file__).parent.parent / "data" / "simulation_results.csv"
OUTPUT_FILE = Path(__file__).parent.parent / "results" / "signal_strength.png"


def main():
    times = []
    cell_a_signal = []
    cell_b_signal = []

    with open(DATA_FILE, newline="") as file:
        rows = csv.DictReader(file)

        for row in rows:
            times.append(float(row["time"]))
            cell_a_signal.append(float(row["cell_a_signal"]))
            cell_b_signal.append(float(row["cell_b_signal"]))

    plt.figure(figsize=(10, 6))

    plt.plot(
        times,
        cell_a_signal,
        marker="o",
        label="Cell-A"
    )

    plt.plot(
        times,
        cell_b_signal,
        marker="o",
        label="Cell-B"
    )

    plt.xlabel("Time (seconds)")
    plt.ylabel("Signal Strength (dBm)")
    plt.title("Cellular Signal Strength During UE Mobility")
    plt.legend()
    plt.grid(True)

    plt.tight_layout()

    plt.savefig(OUTPUT_FILE, dpi=150)

    print(f"Saved plot to: {OUTPUT_FILE}")


if __name__ == "__main__":
    main()