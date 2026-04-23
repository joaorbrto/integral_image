import cv2
import numpy as np

H, W = 64, 64

# mesma imagem do C
img = np.fromfunction(lambda r, c: (r + c) & 0xFF, (H, W), dtype=np.int32)

# OpenCV integral (tem padding)
integral_cv = cv2.integral(img.astype(np.uint8))
integral_cv = integral_cv[1:, 1:]  # remove padding

# carrega matriz do C
integral_c = np.loadtxt("integral_c.txt", dtype=np.int64)

# ---------------------------
# 1. Validação pixel a pixel
# ---------------------------

diff = np.abs(integral_c - integral_cv)

if np.all(diff == 0):
    print("Matrizes iguais (pixel a pixel)")
else:
    print("Diferença encontrada")
    print("Erro máximo:", diff.max())

# ---------------------------
# 2. Validação das queries
# ---------------------------

def get_sum_opencv(r1, c1, r2, c2):
    if r1 > r2 or c1 > c2:
        return 0

    A = integral_cv[r2, c2]
    B = integral_cv[r1 - 1, c2] if r1 > 0 else 0
    C = integral_cv[r2, c1 - 1] if c1 > 0 else 0
    D = integral_cv[r1 - 1, c1 - 1] if (r1 > 0 and c1 > 0) else 0

    return int(A - B - C + D)

ok = True

with open("queries.txt") as f:
    for i, line in enumerate(f):
        r1, c1, r2, c2, c_val = map(int, line.split())

        py_val = get_sum_opencv(r1, c1, r2, c2)

        if py_val != c_val:
            print(f"❌ ERRO query {i}")
            print(f"C={c_val}, PY={py_val}")
            ok = False
            break

print("Validação queries:", "✅ OK" if ok else "❌ ERRO")