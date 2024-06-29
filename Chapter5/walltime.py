import matplotlib.pyplot as plt
import numpy as np

# ファイルからデータを読み込む
data = np.loadtxt('walltime.dat')

# X軸とY軸のデータ
x = data[:, 0]
y = data[:, 1]

# グラフの作成、図のサイズを指定
fig, ax = plt.subplots(figsize=(8, 8))  # サイズを8x6インチに設定
ax.plot(x, y, marker='o', linestyle='-', color='black',lw=3)  # ポイントを線で結ぶ

# 軸のラベル
ax.set_xlabel('# of process', fontsize=22)
ax.set_ylabel('Wallclock time [s]', fontsize=22)

# Y軸を対数スケールに設定
ax.set_yscale('log')

ax.tick_params(labelsize=14)  # 軸目盛のフォントサイズを設定

# タイトルの追加
ax.set_title('Sieve of Eratosthenes (ver.1)', fontsize=24)

# グリッドの追加
#ax.grid(True)

# レイアウトの自動調整
plt.tight_layout()

# PDFファイルとして保存
plt.savefig('wallclock_time.pdf')

# ショー関数を呼ばずにプロットを閉じる
plt.close()
