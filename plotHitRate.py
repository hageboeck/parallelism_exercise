import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# List of CSV file names
files = ['barbarian_NPC_hit_rate.csv', 'cleric_NPC_hit_rate.csv', 'rogue_NPC_hit_rate.csv', 'wizard_NPC_hit_rate.csv', 'NPC_barbarian_hit_rate.csv', 'NPC_cleric_hit_rate.csv', 'NPC_rogue_hit_rate.csv', 'NPC_wizard_hit_rate.csv']

# Process each file
for file_name in files:
    # Read the CSV file without any index column
    data = pd.read_csv(file_name, header=None)
    
    # Remove the last column from the DataFrame
    data = data.iloc[:, :-1]
    
    # Create a heatmap
    plt.figure(figsize=(10, 8))
    ax = sns.heatmap(data, annot=False, cmap='coolwarm', cbar_kws={'label': 'Hit Rate'})
    
    # Title and labels
    plt.title(f'{file_name.split("_")[0].capitalize()} hit rate against {file_name.split("_")[1]}s')
    plt.ylabel('NPC level')
    plt.xlabel('PC level')

    # Adjusting the tick labels for rows and columns to start from 1 instead of 0
    ax.set_xticklabels([int(item.get_text())+1 for item in ax.get_xticklabels()])
    ax.set_yticklabels([int(item.get_text())+1 for item in ax.get_yticklabels()])

    # Invert the y-axis to display the higher levels at the top
    plt.gca().invert_yaxis()
    
    # Save the plot as a PNG file
    plt.savefig(f'{file_name.split("_")[0]}_{file_name.split("_")[1]}_hit_rate.png')
    
    # Close the plot figure to free up memory
    plt.close()