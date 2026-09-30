import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load the CSV file
year = "2018"
cut_value = 2000
input_file = f"Bunch_crossing_{year}.csv"  # Change this to your actual file name
output_file = f"filtered_runs_{year}_bunchcrossing.csv"


# Read the CSV file
df = pd.read_csv(input_file, sep=',')

# Filter rows where Bx < 1800
filtered_df = df[df["Bx"] < cut_value]
filtered_df_above = df[df["Bx"] >= cut_value]
# Save the filtered data to a new CSV file
filtered_df.to_csv(output_file, index=False)


# Plot

# Example settings
xmin = 0
xmax = 2500
nbins = 25
bins = np.linspace(xmin, xmax, nbins + 1)


plt.figure(figsize=(8,5))
plt.hist(df["Bx"] , bins=nbins, color='blue' , range=(xmin, xmax), alpha=0.7, edgecolor='black')
plt.xlabel("Bx")
plt.grid(True)
plt.ylabel("Frequency")
plt.title(f"Number of colliding bunches : {year}")
# Stat box text
# Counts
x= df["Bx"].dropna()
in_range = x[(x >= xmin) & (x <= xmax)]
underflow = (x < xmin).sum()
overflow = (x > xmax).sum()
entries_in_range = len(in_range)
total_entries = len(x)
mean_in_range = in_range.mean()
mean_all = x.mean()

stats_text = (
    f"Entries in range = {entries_in_range}\n"
    f"Total entries = {total_entries}\n"
    f"Underflow = {underflow}\n"
    f"Overflow = {overflow}\n"
    f"Mean in range = {mean_in_range:.3f}\n"
    f"Mean all = {mean_all:.3f}"
)
plt.text(
    0.97, 0.97, stats_text,
    transform=plt.gca().transAxes,
    ha="right",
    va="top",
    bbox=dict(boxstyle="round", facecolor="white", alpha=0.8)
)
plt.savefig(f"figures/Bunch_crossing_{year}.png")

xmin = 0
xmax = 3000
nbins = 30
bins = np.linspace(xmin, xmax, nbins + 1)

plt.figure(figsize=(8,5))
plt.hist(filtered_df["Bx"] , bins=nbins,color='blue', range=(xmin, xmax), alpha=0.7, edgecolor='black')
plt.xlabel("Bx")
plt.ylabel("Frequency")
plt.grid(True)
plt.title(f"Number of colliding bunches < {cut_value}: {year} ")

# Counts
x= filtered_df["Bx"].dropna()
in_range = x[(x >= xmin) & (x <= xmax)]
underflow = (x < xmin).sum()
overflow = (x > xmax).sum()
entries_in_range = len(in_range)
total_entries = len(x)
mean_in_range = in_range.mean()
mean_all = x.mean()

stats_text = (
    f"Entries in range = {entries_in_range}\n"
    f"Total entries = {total_entries}\n"
    f"Underflow = {underflow}\n"
    f"Overflow = {overflow}\n"
    f"Mean in range = {mean_in_range:.3f}\n"
    f"Mean all = {mean_all:.3f}"
)
plt.text(
    0.97, 0.97, stats_text,
    transform=plt.gca().transAxes,
    ha="right",
    va="top",
    bbox=dict(boxstyle="round", facecolor="white", alpha=0.8)
)
plt.savefig(f"figures/Bunch_crossing_below_cut_{year}.png")

xmin = 0
xmax = 2500
nbins = 25
bins = np.linspace(xmin, xmax, nbins + 1)

plt.figure(figsize=(8,5))
plt.hist(filtered_df_above["Bx"] , bins=nbins, color='blue', range=(xmin, xmax), alpha=0.7, edgecolor='black')
plt.xlabel("Bx")
plt.ylabel("Frequency")
plt.grid(True)
plt.title(f"Number of colliding bunches > {cut_value}: {year} ")


# Counts
x= filtered_df_above["Bx"].dropna()
in_range = x[(x >= xmin) & (x <= xmax)]
underflow = (x < xmin).sum()
overflow = (x > xmax).sum()
entries_in_range = len(in_range)
total_entries = len(x)
mean_in_range = in_range.mean()
mean_all = x.mean()

stats_text = (
    f"Entries in range = {entries_in_range}\n"
    f"Total entries = {total_entries}\n"
    f"Underflow = {underflow}\n"
    f"Overflow = {overflow}\n"
    f"Mean in range = {mean_in_range:.3f}\n"
    f"Mean all = {mean_all:.3f}"
)
plt.text(
    0.97, 0.97, stats_text,
    transform=plt.gca().transAxes,
    ha="right",
    va="top",
    bbox=dict(boxstyle="round", facecolor="white", alpha=0.8)
)
plt.savefig(f"figures/Bunch_crossing_above_cut_{year}.png")

print(f"Filtered runs saved to: {output_file}")

