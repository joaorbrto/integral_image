#include <stdint.h>
#include <stdio.h>

/*
Integral Image (Summed Area Table)
=================================

Ideia principal
--------------
Dada uma matriz 2D "img" (pense como uma imagem em tons de cinza), o algoritmo
faz um pré-processamento que cria uma segunda matriz 2D "integral" onde:

    integral[r][c] = soma de todos os valores de img nas linhas 0..r e colunas 0..c

Ou seja, cada posição guarda a soma acumulada do retângulo desde a origem (0,0)
até o ponto (r,c).

Por que isso é útil?
-------------------
Depois que "integral" está pronta, a soma de QUALQUER retângulo dentro da matriz
pode ser obtida em tempo constante O(1) com apenas 4 leituras em "integral".

Complexidade
------------
- Construção (pré-processamento): O(H × W) porque percorre toda a matriz.
- Consulta de retângulo: O(1) porque usa apenas 4 acessos e poucas operações.

Restrições / adequação para embarcados
-------------------------------------
- Tudo é iterativo (sem recursão).
- Sem alocação dinâmica (sem malloc/free).
- Matrizes estáticas (memória previsível).

Observação de memória
---------------------
Este exemplo usa int. O consumo real depende de sizeof(int) (muito comum ser 4
bytes). Com H=W=64 temos 4096 elementos por matriz; duas matrizes = 8192 ints.
Então o total em bytes é 8192 * sizeof(int). (O requisito do trabalho menciona
"~8KB"; se precisar ajustar isso, a ideia é usar um tipo menor, ex.: int16_t,
desde que não haja overflow.)
*/

#define H 64
#define W 64

static void fill_img(int img[H][W])
{
    /*
     * Preenche "img" com um padrão determinístico só para termos dados.
     * Em um uso real, "img" viria de um sensor, arquivo, frame de câmera etc.
     */
    for (int r = 0; r < H; r++)
    {
        for (int c = 0; c < W; c++)
        {
            /* Ex.: valor simples baseado em (linha+coluna), limitado em 0..255 */
            img[r][c] = (r + c) & 0xFF;
        }
    }
}

/*
 * Construção da imagem integral (Summed Area Table).
 * integral[r][c] = soma de img[0..r][0..c].
 * Complexidade: O(H×W).
 */
static void build_integral(int img[H][W], int integral[H][W])
{
    for (int r = 0; r < H; r++)
    {
        /*
         * "row_running_sum" guarda a soma acumulada da linha r até a coluna c:
         *   row_running_sum(c) = img[r][0] + img[r][1] + ... + img[r][c]
         *
         * Isso ajuda a calcular integral[r][c] sem precisar somar tudo do zero.
         */
        int row_running_sum = 0;
        for (int c = 0; c < W; c++)
        {
            row_running_sum += img[r][c];

            /*
             * Fórmula:
             * integral[r][c] = (soma da linha r até c) + (soma acumulada acima)
             *
             * A parte "acima" é integral[r-1][c], que já representa a soma de
             * img[0..r-1][0..c].
             *
             * Portanto:
             *   integral[r][c] = row_running_sum + integral[r-1][c]
             * Com cuidado no caso r==0 (não existe linha acima).
             */
            integral[r][c] = row_running_sum + ((r > 0) ? integral[r - 1][c] : 0);
        }
    }
}

/*
 * Soma de uma sub-região retangular em O(1) usando 4 acessos.
 * Coordenadas inclusivas: (r1,c1) canto superior-esquerdo, (r2,c2) inferior-direito.
 * Retorna 0 para parâmetros inválidos.
 */
static int sum_region(int integral[H][W], int r1, int c1, int r2, int c2)
{
    /* Validação básica dos parâmetros para evitar acessos fora da matriz. */
    if (r1 < 0 || c1 < 0 || r2 < 0 || c2 < 0)
    {
        return 0;
    }
    if (r1 >= H || r2 >= H || c1 >= W || c2 >= W)
    {
        return 0;
    }
    if (r1 > r2 || c1 > c2)
    {
        return 0;
    }

    /*
     * Consulta O(1) via inclusão-exclusão.
     *
     * Queremos somar o retângulo:
     *   linhas r1..r2 e colunas c1..c2 (coordenadas INCLUSIVAS)
     *
     * Definimos:
     *   A = integral[r2][c2]          -> soma do retângulo (0,0) até (r2,c2)
     *   B = integral[r1-1][c2]        -> soma da faixa acima do retângulo alvo
     *   C = integral[r2][c1-1]        -> soma da faixa à esquerda do retângulo alvo
     *   D = integral[r1-1][c1-1]      -> área removida duas vezes (precisa somar de volta)
     *
     * Visualmente (X = área que queremos):
     *
     *   +---------------------+
     *   |          B          |
     *   |   +-------------+   |
     *   |   |      X      |   |
     *   |   +-------------+   |
     *   |          (C)        |
     *   +---------------------+
     *
     * Resultado:
     *   sum(X) = A - B - C + D
     *
     * Nos casos de borda (r1==0 ou c1==0), alguns termos não existem e viram 0.
     */

    const int A = integral[r2][c2];
    const int B = (r1 > 0) ? integral[r1 - 1][c2] : 0;
    const int C = (c1 > 0) ? integral[r2][c1 - 1] : 0;
    const int D = (r1 > 0 && c1 > 0) ? integral[r1 - 1][c1 - 1] : 0;
    return A - B - C + D;
}

/* Implementação de referência (O(área)) para validar as consultas. */
static int naive_sum_region(int img[H][W], int r1, int c1, int r2, int c2)
{
    /*
     * Soma "ingênua": percorre cada célula do retângulo e acumula.
     * Serve como referência para validar a versão O(1).
     * Complexidade: O(área) = O((r2-r1+1) × (c2-c1+1)).
     */
    int sum = 0;
    for (int r = r1; r <= r2; r++)
    {
        for (int c = c1; c <= c2; c++)
        {
            sum += img[r][c];
        }
    }
    return sum;
}

static uint32_t lcg_next(uint32_t *state)
{
    /*
     * Gerador pseudo-aleatório simples (LCG) para criar retângulos de teste.
     * É determinístico: com a mesma semente, gera sempre a mesma sequência.
     *
     * Não é para criptografia; é apenas para testes.
     */
    *state = (*state * 1664525u) + 1013904223u;
    return *state;
}

int main(void)
{
    /*
     * "static" aqui coloca as matrizes em memória estática (não na pilha),
     * evitando estouro de stack em ambientes mais restritos.
     */
    static int img[H][W];
    static int integral[H][W];

    fill_img(img);

    /* Pré-processamento: constrói a tabela integral para permitir consultas O(1). */
    build_integral(img, integral);

    /*
     * Exemplos de consultas com retângulos (r1,c1)->(r2,c2).
     * Coordenadas são inclusivas (ambos os cantos entram na soma).
     */
    struct Query
    {
        int r1, c1, r2, c2;
    } queries[] = {
        {0, 0, 0, 0},
        {0, 0, 1, 1},
        {10, 10, 20, 20},
        {0, 0, H - 1, W - 1},
        {5, 7, 5, 20},
    };

    printf("Integral Image (Summed Area Table) - %dx%d\n", H, W);
    for (unsigned i = 0; i < (unsigned)(sizeof(queries) / sizeof(queries[0])); i++)
    {
        const int r1 = queries[i].r1;
        const int c1 = queries[i].c1;
        const int r2 = queries[i].r2;
        const int c2 = queries[i].c2;
        /*
         * Compara a soma O(1) (usando integral) com a soma ingênua (varrendo img).
         * Se bater, sabemos que a fórmula de inclusão-exclusão está correta.
         */
        const int fast = sum_region(integral, r1, c1, r2, c2);
        const int slow = naive_sum_region(img, r1, c1, r2, c2);
        printf("Q%u: (%d,%d) -> (%d,%d)  O(1)=%d  naive=%d  %s\n",
               i + 1, r1, c1, r2, c2, fast, slow, (fast == slow) ? "OK" : "ERRO");
    }

    /*
     * Validação adicional com retângulos pseudo-aleatórios.
     * A ideia é pegar muitos casos diferentes para aumentar confiança.
     */
    uint32_t rng = 1u;
    int ok = 1;
    for (int t = 0; t < 200; t++)
    {
        int r1 = (int)(lcg_next(&rng) % H);
        int r2 = (int)(lcg_next(&rng) % H);
        int c1 = (int)(lcg_next(&rng) % W);
        int c2 = (int)(lcg_next(&rng) % W);
        /* Normaliza para garantir r1<=r2 e c1<=c2. */
        if (r1 > r2)
        {
            int tmp = r1;
            r1 = r2;
            r2 = tmp;
        }
        if (c1 > c2)
        {
            int tmp = c1;
            c1 = c2;
            c2 = tmp;
        }

        const int fast = sum_region(integral, r1, c1, r2, c2);
        const int slow = naive_sum_region(img, r1, c1, r2, c2);
        if (fast != slow)
        {
            /* Se der erro, imprime o caso que falhou e finaliza. */
            ok = 0;
            printf("Falha: (%d,%d)->(%d,%d) O(1)=%d naive=%d\n", r1, c1, r2, c2, fast, slow);
            break;
        }
    }

    printf("Validacao aleatoria: %s\n", ok ? "OK" : "ERRO");
    return ok ? 0 : 1;
}
