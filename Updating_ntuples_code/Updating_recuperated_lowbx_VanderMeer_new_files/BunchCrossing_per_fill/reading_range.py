import pandas as pd

# Load the CSV file
input_file = "Bunch_crossing_2016.csv"  # Change this to your actual file name
output_file = "filtered_runs_2016_check.csv"

# Read the CSV file
df = pd.read_csv(input_file, sep=',')

# Filter rows where Bx < 1800
filtered_df = df[df["Bx"] < 2000]

# Save the filtered data to a new CSV file
filtered_df.to_csv(output_file, index=False)

print(f"Filtered runs saved to: {output_file}")

