#!/usr/bin/env python3
"""
Gera `integral_c.txt` e `queries.txt` com o mesmo padrão usado em `main.c`.
Usar: python3 generate_files.py
"""
import numpy as np

H, W = 64, 64
img = np.fromfunction(lambda r, c: (r + c) & 0xFF, (H, W), dtype=np.int64)
integral = np.zeros((H, W), dtype=np.int64)
for r in range(H):
    row_running = 0
    for c in range(W):
        row_running += int(img[r, c])
        integral[r, c] = row_running + (integral[r - 1, c] if r > 0 else 0)

with open('integral_c.txt', 'w') as f:
    for r in range(H):
        f.write(' '.join(str(int(x)) for x in integral[r]) + '\n')

queries = [(4, 1, 3, 0), (0, 0, 1, 1), (10, 10, 20, 20), (0, 0, H - 1, W - 1), (5, 7, 5, 20)]
with open('queries.txt', 'w') as f:
    for (r1, c1, r2, c2) in queries:
        A = integral[r2, c2]
        B = integral[r1 - 1, c2] if r1 > 0 else 0
        C = integral[r2, c1 - 1] if c1 > 0 else 0
        D = integral[r1 - 1, c1 - 1] if (r1 > 0 and c1 > 0) else 0
        val = A - B - C + D
        f.write(f"{r1} {c1} {r2} {c2} {val}\n")

print('Arquivos gerados: integral_c.txt e queries.txt')
