from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


ALGORITHM_ORDER = [
    "std_quick_sort",
    "std_merge_sort",
    "string_quick_sort",
    "string_merge_sort",
    "msd_radix_sort",
    "msd_radix_quick_sort",
]

LABELS = {
    "std_quick_sort": "QuickSort",
    "std_merge_sort": "MergeSort",
    "string_quick_sort": "String QuickSort",
    "string_merge_sort": "String MergeSort",
    "msd_radix_sort": "MSD Radix",
    "msd_radix_quick_sort": "MSD Radix + Quick",
}

COLORS = {
    "std_quick_sort": "#4C78A8",
    "std_merge_sort": "#72B7B2",
    "string_quick_sort": "#F58518",
    "string_merge_sort": "#E45756",
    "msd_radix_sort": "#54A24B",
    "msd_radix_quick_sort": "#B279A2",
}


def markdown_table(rows: list[list[object]], headers: list[str]) -> str:
    result = ["| " + " | ".join(headers) + " |"]
    result.append("| " + " | ".join(["---"] * len(headers)) + " |")

    for row in rows:
        result.append("| " + " | ".join(str(value) for value in row) + " |")

    return "\n".join(result) + "\n"


def prepare(raw: pd.DataFrame) -> pd.DataFrame:
    grouped = (
        raw.groupby(["case", "n", "algorithm"], as_index=False)
        .agg(time_us=("time_us", "mean"), char_ops=("char_ops", "mean"))
    )
    grouped["time_ms"] = grouped["time_us"] / 1000.0
    grouped["char_ops_k"] = grouped["char_ops"] / 1000.0
    grouped["algorithm"] = pd.Categorical(grouped["algorithm"], ALGORITHM_ORDER, ordered=True)
    return grouped.sort_values(["case", "n", "algorithm"])


def plot_metric(
    data: pd.DataFrame,
    case: str,
    metric: str,
    ylabel: str,
    title: str,
    output_path: Path,
) -> None:
    fig, ax = plt.subplots(figsize=(9, 5.2), dpi=150)
    subset = data[data["case"] == case]

    for algorithm in ALGORITHM_ORDER:
        line = subset[subset["algorithm"] == algorithm]
        if line.empty:
            continue
        ax.plot(
            line["n"],
            line[metric],
            marker="o",
            markersize=3,
            linewidth=1.8,
            color=COLORS[algorithm],
            label=LABELS[algorithm],
        )

    ax.set_title(title)
    ax.set_xlabel("Размер массива, строк")
    ax.set_ylabel(ylabel)
    ax.grid(True, alpha=0.25)
    ax.legend(ncols=2, fontsize=8)
    fig.tight_layout()
    fig.savefig(output_path)
    plt.close(fig)


def plot_char_ops_all_cases(data: pd.DataFrame, output_path: Path) -> None:
    case_order = ["random", "reversed", "almost_sorted", "common_prefix"]
    case_titles = {
        "random": "Случайный массив",
        "reversed": "Обратно отсортированный",
        "almost_sorted": "Почти отсортированный",
        "common_prefix": "Общий префикс",
    }

    fig, axes = plt.subplots(2, 2, figsize=(12, 7.2), dpi=150, sharex=True)
    axes = axes.flatten()

    for ax, case in zip(axes, case_order):
        subset = data[data["case"] == case]

        for algorithm in ALGORITHM_ORDER:
            line = subset[subset["algorithm"] == algorithm]
            if line.empty:
                continue
            ax.plot(
                line["n"],
                line["char_ops"],
                linewidth=1.6,
                color=COLORS[algorithm],
                label=LABELS[algorithm],
            )

        ax.set_title(case_titles[case])
        ax.set_xlabel("Размер массива, строк")
        ax.set_ylabel("Символьные операции")
        ax.grid(True, alpha=0.25)

    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc="lower center", ncols=3, fontsize=8)
    fig.suptitle("Анализ количества посимвольных сравнений", y=0.98)
    fig.tight_layout(rect=[0, 0.08, 1, 0.95])
    fig.savefig(output_path)
    plt.close(fig)


def write_summary(data: pd.DataFrame, output_path: Path) -> None:
    max_n = int(data["n"].max())
    summary = data[data["n"] == max_n].copy()
    summary["time_ms"] = summary["time_ms"].round(3)
    summary["char_ops"] = summary["char_ops"].round(0).astype("int64")
    summary = summary[["case", "algorithm", "time_ms", "char_ops"]]
    summary["algorithm"] = summary["algorithm"].map(LABELS)
    summary.to_csv(output_path, index=False)


def write_markdown_tables(data: pd.DataFrame, tables_dir: Path) -> None:
    max_n = int(data["n"].max())
    summary = data[data["n"] == max_n].copy()
    summary["algorithm"] = summary["algorithm"].map(LABELS)
    summary["time_ms"] = summary["time_ms"].round(3)
    summary["char_ops"] = summary["char_ops"].round(0).astype("int64")
    summary = summary[["case", "algorithm", "time_ms", "char_ops"]]
    summary.columns = ["Тип данных", "Алгоритм", "Время, мс", "Символьные операции"]
    (tables_dir / "summary_n3000.md").write_text(
        markdown_table(summary.values.tolist(), list(summary.columns)),
        encoding="utf-8",
    )

    winners = (
        data[data["n"] == max_n]
        .sort_values(["case", "time_ms"])
        .groupby("case", observed=True)
        .first()
        .reset_index()
    )
    winners["algorithm"] = winners["algorithm"].map(LABELS)
    winners["time_ms"] = winners["time_ms"].round(3)
    winners = winners[["case", "algorithm", "time_ms"]]
    winners.columns = ["Тип данных", "Самый быстрый алгоритм", "Время, мс"]
    (tables_dir / "winners_n3000.md").write_text(
        markdown_table(winners.values.tolist(), list(winners.columns)),
        encoding="utf-8",
    )


def print_winners(data: pd.DataFrame) -> None:
    max_n = int(data["n"].max())
    winners = (
        data[data["n"] == max_n]
        .sort_values(["case", "time_ms"])
        .groupby("case", observed=True)
        .first()
        .reset_index()
    )

    print(f"\nFastest algorithms at n={max_n}")
    print("-" * 52)
    for _, row in winners.iterrows():
        label = LABELS[str(row["algorithm"])]
        print(f"{row['case']:<16} {label:<22} {row['time_ms']:>8.3f} ms")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", default="data/raw_results.csv")
    parser.add_argument("--figures-dir", default="analysis/figures")
    parser.add_argument("--tables-dir", default="analysis/tables")
    args = parser.parse_args()

    input_path = Path(args.input)
    figures_dir = Path(args.figures_dir)
    tables_dir = Path(args.tables_dir)
    figures_dir.mkdir(parents=True, exist_ok=True)
    tables_dir.mkdir(parents=True, exist_ok=True)

    raw = pd.read_csv(input_path)
    data = prepare(raw)

    plot_metric(
        data,
        case="random",
        metric="time_ms",
        ylabel="Среднее время, мс",
        title="Время сортировки на случайных строках",
        output_path=figures_dir / "time_random.png",
    )
    plot_metric(
        data,
        case="common_prefix",
        metric="time_ms",
        ylabel="Среднее время, мс",
        title="Время сортировки на строках с общим префиксом",
        output_path=figures_dir / "time_prefix.png",
    )
    plot_metric(
        data,
        case="common_prefix",
        metric="char_ops_k",
        ylabel="Символьные операции, тыс.",
        title="Символьные операции на строках с общим префиксом",
        output_path=figures_dir / "char_prefix.png",
    )
    plot_char_ops_all_cases(data, figures_dir / "char.png")

    write_summary(data, tables_dir / "summary_n3000.csv")
    write_markdown_tables(data, tables_dir)
    print_winners(data)


if __name__ == "__main__":
    main()
