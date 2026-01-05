"""
Vizuelizacija rezultata benchmarka hash algoritama korišćenjem Seaborn
"""

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

# Postavke za lepe grafike
sns.set_theme(style="whitegrid")
plt.rcParams['figure.figsize'] = (16, 10)

# Učitavanje podataka
df = pd.read_csv('benchmark_results.csv')

# Kreiranje formatiranih labela za X-osu
def format_size(size):
    if size >= 1_000_000:
        return f"{size // 1_000_000}M"
    elif size >= 1_000:
        return f"{size // 1_000}k"
    return str(size)

df['Size_Label'] = df['Num_Keys'].apply(format_size)

# 1. THROUGHPUT GRAFICI - INSERT OPERACIJE
fig, axes = plt.subplots(2, 2, figsize=(20, 16))

# 1.1 Insert Throughput - Uniform Distribution
uniform_data = df[df['Distribution'] == 'uniform']
sns.lineplot(
    data=uniform_data,
    x='Num_Keys',
    y='Insert_Ops_Per_Sec',
    hue='Algorithm',
    marker='o',
    ax=axes[0, 0]
)
axes[0, 0].set_title('Insert Throughput - Uniform Distribution', fontsize=14, fontweight='bold')
axes[0, 0].set_xlabel('Number of Keys', fontsize=12)
axes[0, 0].set_ylabel('Throughput (Mops/s)', fontsize=12)
axes[0, 0].set_xscale('log')
axes[0, 0].legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=9)
axes[0, 0].grid(True, alpha=0.3)

# 1.2 Insert Throughput - Gaussian Distribution
gaussian_data = df[df['Distribution'] == 'gaussian']
sns.lineplot(
    data=gaussian_data,
    x='Num_Keys',
    y='Insert_Ops_Per_Sec',
    hue='Algorithm',
    marker='s',
    ax=axes[0, 1]
)
axes[0, 1].set_title('Insert Throughput - Gaussian Distribution', fontsize=14, fontweight='bold')
axes[0, 1].set_xlabel('Number of Keys', fontsize=12)
axes[0, 1].set_ylabel('Throughput (Mops/s)', fontsize=12)
axes[0, 1].set_xscale('log')
axes[0, 1].legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=9)
axes[0, 1].grid(True, alpha=0.3)

# 1.3 Lookup Throughput - Uniform Distribution
sns.lineplot(
    data=uniform_data,
    x='Num_Keys',
    y='Lookup_Ops_Per_Sec',
    hue='Algorithm',
    marker='o',
    ax=axes[1, 0]
)
axes[1, 0].set_title('Lookup Throughput - Uniform Distribution', fontsize=14, fontweight='bold')
axes[1, 0].set_xlabel('Number of Keys', fontsize=12)
axes[1, 0].set_ylabel('Throughput (Mops/s)', fontsize=12)
axes[1, 0].set_xscale('log')
axes[1, 0].legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=9)
axes[1, 0].grid(True, alpha=0.3)

# 1.4 Lookup Throughput - Gaussian Distribution
sns.lineplot(
    data=gaussian_data,
    x='Num_Keys',
    y='Lookup_Ops_Per_Sec',
    hue='Algorithm',
    marker='s',
    ax=axes[1, 1]
)
axes[1, 1].set_title('Lookup Throughput - Gaussian Distribution', fontsize=14, fontweight='bold')
axes[1, 1].set_xlabel('Number of Keys', fontsize=12)
axes[1, 1].set_ylabel('Throughput (Mops/s)', fontsize=12)
axes[1, 1].set_xscale('log')
axes[1, 1].legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=9)
axes[1, 1].grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('throughput_comparison.png', dpi=300, bbox_inches='tight')
print("✓ Saved: throughput_comparison.png")
plt.show()

# 2. MEMORY USAGE GRAFICI
fig, axes = plt.subplots(1, 2, figsize=(20, 8))

# 2.1 Memory Usage - Uniform
sns.lineplot(
    data=uniform_data,
    x='Num_Keys',
    y='Memory_Usage_MB',
    hue='Algorithm',
    marker='o',
    ax=axes[0]
)
axes[0].set_title('Memory Usage - Uniform Distribution', fontsize=14, fontweight='bold')
axes[0].set_xlabel('Number of Keys', fontsize=12)
axes[0].set_ylabel('Memory Usage (MB)', fontsize=12)
axes[0].set_xscale('log')
axes[0].set_yscale('log')
axes[0].legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=9)
axes[0].grid(True, alpha=0.3)

# 2.2 Memory Usage - Gaussian
sns.lineplot(
    data=gaussian_data,
    x='Num_Keys',
    y='Memory_Usage_MB',
    hue='Algorithm',
    marker='s',
    ax=axes[1]
)
axes[1].set_title('Memory Usage - Gaussian Distribution', fontsize=14, fontweight='bold')
axes[1].set_xlabel('Number of Keys', fontsize=12)
axes[1].set_ylabel('Memory Usage (MB)', fontsize=12)
axes[1].set_xscale('log')
axes[1].set_yscale('log')
axes[1].legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=9)
axes[1].grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('memory_usage_comparison.png', dpi=300, bbox_inches='tight')
print("✓ Saved: memory_usage_comparison.png")
plt.show()

# 3. HEATMAP - INSERT PERFORMANCE PO ALGORITMU I VELIČINI
fig, axes = plt.subplots(1, 2, figsize=(20, 8))

# 3.1 Uniform Heatmap
pivot_uniform = uniform_data.pivot_table(
    values='Insert_Ops_Per_Sec',
    index='Algorithm',
    columns='Size_Label',
    aggfunc='mean'
)
# Reorder columns by size
size_order = ['1k', '10k', '100k', '1M', '5M', '10M']
pivot_uniform = pivot_uniform[[col for col in size_order if col in pivot_uniform.columns]]

sns.heatmap(
    pivot_uniform,
    annot=True,
    fmt='.2f',
    cmap='YlOrRd',
    ax=axes[0],
    cbar_kws={'label': 'Mops/s'}
)
axes[0].set_title('Insert Throughput Heatmap - Uniform', fontsize=14, fontweight='bold')
axes[0].set_xlabel('Number of Keys', fontsize=12)
axes[0].set_ylabel('Algorithm', fontsize=12)

# 3.2 Gaussian Heatmap
pivot_gaussian = gaussian_data.pivot_table(
    values='Insert_Ops_Per_Sec',
    index='Algorithm',
    columns='Size_Label',
    aggfunc='mean'
)
pivot_gaussian = pivot_gaussian[[col for col in size_order if col in pivot_gaussian.columns]]

sns.heatmap(
    pivot_gaussian,
    annot=True,
    fmt='.2f',
    cmap='YlGnBu',
    ax=axes[1],
    cbar_kws={'label': 'Mops/s'}
)
axes[1].set_title('Insert Throughput Heatmap - Gaussian', fontsize=14, fontweight='bold')
axes[1].set_xlabel('Number of Keys', fontsize=12)
axes[1].set_ylabel('Algorithm', fontsize=12)

plt.tight_layout()
plt.savefig('performance_heatmap.png', dpi=300, bbox_inches='tight')
print("✓ Saved: performance_heatmap.png")
plt.show()

# 4. COMPARISON: UNIFORM vs GAUSSIAN
fig, axes = plt.subplots(2, 1, figsize=(16, 12))

# 4.1 Insert Performance Comparison
for algo in df['Algorithm'].unique():
    algo_data = df[df['Algorithm'] == algo]
    uniform_perf = algo_data[algo_data['Distribution'] == 'uniform']['Insert_Ops_Per_Sec'].values
    gaussian_perf = algo_data[algo_data['Distribution'] == 'gaussian']['Insert_Ops_Per_Sec'].values
    
    if len(uniform_perf) > 0 and len(gaussian_perf) > 0:
        axes[0].plot(
            algo_data['Num_Keys'].unique()[:len(uniform_perf)],
            uniform_perf,
            marker='o',
            label=f'{algo} (Uniform)',
            linestyle='-'
        )
        axes[0].plot(
            algo_data['Num_Keys'].unique()[:len(gaussian_perf)],
            gaussian_perf,
            marker='s',
            label=f'{algo} (Gaussian)',
            linestyle='--',
            alpha=0.7
        )

axes[0].set_title('Insert Throughput: Uniform vs Gaussian Distribution', fontsize=14, fontweight='bold')
axes[0].set_xlabel('Number of Keys', fontsize=12)
axes[0].set_ylabel('Throughput (Mops/s)', fontsize=12)
axes[0].set_xscale('log')
axes[0].legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=8, ncol=2)
axes[0].grid(True, alpha=0.3)

# 4.2 Lookup Performance Comparison
for algo in df['Algorithm'].unique():
    algo_data = df[df['Algorithm'] == algo]
    uniform_perf = algo_data[algo_data['Distribution'] == 'uniform']['Lookup_Ops_Per_Sec'].values
    gaussian_perf = algo_data[algo_data['Distribution'] == 'gaussian']['Lookup_Ops_Per_Sec'].values
    
    if len(uniform_perf) > 0 and len(gaussian_perf) > 0:
        axes[1].plot(
            algo_data['Num_Keys'].unique()[:len(uniform_perf)],
            uniform_perf,
            marker='o',
            label=f'{algo} (Uniform)',
            linestyle='-'
        )
        axes[1].plot(
            algo_data['Num_Keys'].unique()[:len(gaussian_perf)],
            gaussian_perf,
            marker='s',
            label=f'{algo} (Gaussian)',
            linestyle='--',
            alpha=0.7
        )

axes[1].set_title('Lookup Throughput: Uniform vs Gaussian Distribution', fontsize=14, fontweight='bold')
axes[1].set_xlabel('Number of Keys', fontsize=12)
axes[1].set_ylabel('Throughput (Mops/s)', fontsize=12)
axes[1].set_xscale('log')
axes[1].legend(bbox_to_anchor=(1.05, 1), loc='upper left', fontsize=8, ncol=2)
axes[1].grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('distribution_comparison.png', dpi=300, bbox_inches='tight')
print("✓ Saved: distribution_comparison.png")
plt.show()

# 5. STATISTIKE
print("\n" + "="*80)
print("SUMMARY STATISTICS")
print("="*80)

print("\nBest Insert Performance (Uniform):")
best_uniform_insert = uniform_data.loc[uniform_data['Insert_Ops_Per_Sec'].idxmax()]
print(f"  {best_uniform_insert['Algorithm']}: {best_uniform_insert['Insert_Ops_Per_Sec']:.2f} Mops/s @ {format_size(int(best_uniform_insert['Num_Keys']))} keys")

print("\nBest Insert Performance (Gaussian):")
best_gaussian_insert = gaussian_data.loc[gaussian_data['Insert_Ops_Per_Sec'].idxmax()]
print(f"  {best_gaussian_insert['Algorithm']}: {best_gaussian_insert['Insert_Ops_Per_Sec']:.2f} Mops/s @ {format_size(int(best_gaussian_insert['Num_Keys']))} keys")

print("\nBest Lookup Performance (Uniform):")
best_uniform_lookup = uniform_data.loc[uniform_data['Lookup_Ops_Per_Sec'].idxmax()]
print(f"  {best_uniform_lookup['Algorithm']}: {best_uniform_lookup['Lookup_Ops_Per_Sec']:.2f} Mops/s @ {format_size(int(best_uniform_lookup['Num_Keys']))} keys")

print("\nBest Lookup Performance (Gaussian):")
best_gaussian_lookup = gaussian_data.loc[gaussian_data['Lookup_Ops_Per_Sec'].idxmax()]
print(f"  {best_gaussian_lookup['Algorithm']}: {best_gaussian_lookup['Lookup_Ops_Per_Sec']:.2f} Mops/s @ {format_size(int(best_gaussian_lookup['Num_Keys']))} keys")

print("\nLowest Memory Usage:")
best_memory = df.loc[df['Memory_Usage_MB'].idxmin()]
print(f"  {best_memory['Algorithm']}: {best_memory['Memory_Usage_MB']:.2f} MB @ {format_size(int(best_memory['Num_Keys']))} keys")

print("\n" + "="*80)
print("All visualizations saved successfully!")
print("="*80)
