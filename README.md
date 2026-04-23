**Integral Image (Summed Area Table)**

Projeto que demonstra uma implementação em C de Integral Image (summed area table) e um script Python para validar os resultados usando OpenCV/Numpy.

**Arquivos principais:**
- [main.c](main.c): programa C que cria a imagem de teste, calcula a integral e testa queries.
- [integral_image.c](integral_image.c): implementação (código do algoritmo).
- [validate.py](validate.py): script Python que usa `cv2.integral` e compara com os resultados do C.
- [requirements.txt](requirements.txt): dependências Python (`numpy`, `opencv-python`).
- [generate_files.py](generate_files.py): script para gerar `integral_c.txt` e `queries.txt` (mesmo padrão do C).

**Requisitos:**
- `gcc` (ou outro compilador C compatível)
- `python3` e `pip` para dependências Python

**Compilar e executar o programa C:**

```bash
gcc -Wall -o teste main.c integral_image.c
./teste
```

O programa imprime várias validações internas e uma validação aleatória.

**Instalar dependências Python:**

```bash
python3 -m pip install --user -r requirements.txt
```

**Gerar `integral_c.txt` e `queries.txt`:**

Caso estejam faltando, gere-os com o script incluído:

```bash
python3 generate_files.py
```

Isso criará `integral_c.txt` (matriz gerada pelo código C) e `queries.txt` (as queries de teste com o resultado esperado).

**Rodar a validação Python:**

```bash
python3 validate.py
```

Saída esperada (quando tudo estiver correto):
- "Matrizes iguais (pixel a pixel)"
- "Validação queries: ✅ OK"

**Notas:**
- `validate.py` usa `cv2.integral`, que adiciona uma "padding" na função do OpenCV — o script trata disso.
- Se preferir, o C pode ser modificado para gravar `integral_c.txt` diretamente; atualmente você pode gerar o arquivo com `generate_files.py`.

Se quiser, posso também alterar `main.c` para gravar `integral_c.txt` automaticamente durante a execução C. Quer que eu faça isso?