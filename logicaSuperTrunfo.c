// logicaSuperTrunfo.c
// Versão Final
#include <stdio.h>
#include <string.h>
#include <math.h>

// -------------------------------
// Super Trunfo - Cidades (Nível Mestre)
// -------------------------------

typedef struct {
    char estado;
    char codigo[5];
    char nome[50];
    int  populacao;
    float area;            // km2
    float pib;             // bilhões
    int  pontosTuristicos;
} Carta;

typedef enum {
    ATR_POP = 1,
    ATR_AREA = 2,
    ATR_PIB = 3,
    ATR_PTUR = 4,
    ATR_DENS = 5
} Atributo;

static void limparEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descarta */ }
}

static float densidade(const Carta *c) {
    return (c->area > 0.0f) ? ( (float)c->populacao / c->area ) : INFINITY;
}

static void imprimirCarta(const char *titulo, const Carta *c) {
    printf("\n%s\n", titulo);
    printf("Estado: %c\n", c->estado);
    printf("Codigo: %s\n", c->codigo);
    printf("Cidade: %s\n", c->nome);
    printf("Populacao: %d\n", c->populacao);
    printf("Area: %.2f km2\n", c->area);
    printf("PIB: %.2f bilhoes de reais\n", c->pib);
    printf("Pontos turisticos: %d\n", c->pontosTuristicos);
    float d = densidade(c);
    if (isinf(d)) {
        printf("Densidade: indefinida (area <= 0)\n");
    } else {
        printf("Densidade: %.2f hab/km2\n", d);
    }
}

static void menuAtributos(void) {
    printf("\n=== ATRIBUTOS ===\n");
    printf("1) Populacao (maior vence)\n");
    printf("2) Area (maior vence)\n");
    printf("3) PIB (maior vence)\n");
    printf("4) Pontos Turisticos (maior vence)\n");
    printf("5) Densidade Populacional (menor vence)\n");
}

static int compararUnico(const Carta *c1, const Carta *c2, Atributo atr) {
    // Retorna: -1 = empate, 0 = carta 1 vence, 1 = carta 2 vence
    switch (atr) {
        case ATR_POP:
            if (c1->populacao == c2->populacao) return -1;
            return (c1->populacao > c2->populacao) ? 0 : 1;
        case ATR_AREA:
            if (c1->area == c2->area) return -1;
            return (c1->area > c2->area) ? 0 : 1;
        case ATR_PIB:
            if (c1->pib == c2->pib) return -1;
            return (c1->pib > c2->pib) ? 0 : 1;
        case ATR_PTUR:
            if (c1->pontosTuristicos == c2->pontosTuristicos) return -1;
            return (c1->pontosTuristicos > c2->pontosTuristicos) ? 0 : 1;
        case ATR_DENS: {
            float d1 = densidade(c1), d2 = densidade(c2);
            if ((isinf(d1) && isinf(d2)) || d1 == d2) return -1;
            // regra: menor densidade vence
            return (d1 < d2) ? 0 : 1;
        }
        default:
            return -1;
    }
}

static const char* nomeAtributo(Atributo a) {
    switch (a) {
        case ATR_POP:  return "Populacao";
        case ATR_AREA: return "Area";
        case ATR_PIB:  return "PIB";
        case ATR_PTUR: return "Pontos Turisticos";
        case ATR_DENS: return "Densidade Populacional";
        default:       return "Desconhecido";
    }
}

int main(void) {
    Carta c1 = {0}, c2 = {0};

    // ------- Cadastro Carta 1 -------
    printf("Cadastro da Carta 1\n");
    printf("--------------------\n");
    printf("Estado (A-H): ");
    scanf(" %c", &c1.estado);

    printf("Codigo (ex: A01): ");
    scanf(" %4s", c1.codigo);

    printf("Nome da cidade: ");
    limparEntrada(); // limpar resto da linha
    scanf(" %49[^\n]", c1.nome);

    printf("Populacao: ");
    scanf(" %d", &c1.populacao);

    printf("Area em km2: ");
    scanf(" %f", &c1.area);

    printf("PIB em bilhoes: ");
    scanf(" %f", &c1.pib);

    printf("Numero de pontos turisticos: ");
    scanf(" %d", &c1.pontosTuristicos);

    // ------- Cadastro Carta 2 -------
    printf("\nCadastro da Carta 2\n");
    printf("--------------------\n");
    printf("Estado (A-H): ");
    scanf(" %c", &c2.estado);

    printf("Codigo (ex: B02): ");
    scanf(" %4s", c2.codigo);

    printf("Nome da cidade: ");
    limparEntrada();
    scanf(" %49[^\n]", c2.nome);

    printf("Populacao: ");
    scanf(" %d", &c2.populacao);

    printf("Area em km2: ");
    scanf(" %f", &c2.area);

    printf("PIB em bilhoes: ");
    scanf(" %f", &c2.pib);

    printf("Numero de pontos turisticos: ");
    scanf(" %d", &c2.pontosTuristicos);

    // ------- Exibir cartas -------
    imprimirCarta("\n===== CARTA 1 =====", &c1);
    imprimirCarta("\n===== CARTA 2 =====", &c2);

    // ------- Menu Interativo -------
    int opcao;
    do {
        printf("\n=========== MENU ===========\n");
        printf("1) Comparar por um atributo\n");
        printf("2) Comparar por DOIS atributos (nivel Mestre)\n");
        printf("0) Sair\n");
        printf("Escolha: ");
        if (scanf(" %d", &opcao) != 1) { limparEntrada(); opcao = -1; }

        if (opcao == 1) {
            menuAtributos();
            printf("Escolha o atributo: ");
            int a;
            if (scanf(" %d", &a) != 1) { limparEntrada(); continue; }
            if (a < 1 || a > 5) { printf("Atributo invalido.\n"); continue; }

            int res = compararUnico(&c1, &c2, (Atributo)a);
            if (res == -1) {
                printf("\nEmpate em %s!\n", nomeAtributo((Atributo)a));
            } else {
                int iVencedor = (res == 0) ? 1 : 2;
                const Carta *v = (res == 0) ? &c1 : &c2;
                printf("\nVencedora: CARTA %d (%s) pelo atributo %s.\n",
                       iVencedor, v->nome, nomeAtributo((Atributo)a));
            }

        } else if (opcao == 2) {
            // ----- Escolha de dois atributos -----
            menuAtributos();
            int a1, a2;
            printf("Escolha o 1o atributo: ");
            if (scanf(" %d", &a1) != 1) { limparEntrada(); continue; }
            printf("Escolha o 2o atributo (diferente do 1o): ");
            if (scanf(" %d", &a2) != 1) { limparEntrada(); continue; }

            if (a1 < 1 || a1 > 5 || a2 < 1 || a2 > 5 || a1 == a2) {
                printf("Selecao invalida de atributos.\n");
                continue;
            }

            // ----- Comparacao dupla (encadeada + ternario) -----
            int pontosC1 = 0, pontosC2 = 0;

            int r1 = compararUnico(&c1, &c2, (Atributo)a1);
            (r1 == 0) ? (pontosC1++) : (r1 == 1 ? pontosC2++ : 0);

            int r2 = compararUnico(&c1, &c2, (Atributo)a2);
            (r2 == 0) ? (pontosC1++) : (r2 == 1 ? pontosC2++ : 0);

            printf("\nResultados parciais:\n");
            printf("- %s: %s\n", nomeAtributo((Atributo)a1),
                   (r1 == -1 ? "Empate" : (r1 == 0 ? "Carta 1" : "Carta 2")));
            printf("- %s: %s\n", nomeAtributo((Atributo)a2),
                   (r2 == -1 ? "Empate" : (r2 == 0 ? "Carta 1" : "Carta 2")));

            // decisão final
            if (pontosC1 == pontosC2) {
                // opcional: desempate adicional poderia ser adicionado aqui
                printf("\nEmpate geral! (%d x %d)\n", pontosC1, pontosC2);
            } else {
                int vencedor = (pontosC1 > pontosC2) ? 1 : 2;
                const Carta *v = (vencedor == 1) ? &c1 : &c2;
                printf("\nVencedora geral (dois atributos): CARTA %d (%s) por %d x %d\n",
                       vencedor, v->nome, pontosC1, pontosC2);
            }
        } else if (opcao == 0) {
            printf("Saindo...\n");
        } else {
            printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}
