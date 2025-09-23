//#include "tiposyctes.h"
//#include "utiles.h"
#define MAXNOMBRE 10

unByte baseSegmento(unByte id, cuatroBytes registros[CANTREGISTROS]) {
    if (id<0 || id>CANTREGISTROS) {
        printf("Error: id de registro inválido.\n"); // Aparece sólo por error del programador
        exit(1);
    }
    cuatroBytes base = registros[id]>>16;
    if (base<0 || base>MAXSEGMENTOS) {
        printf("Error: Segmento inexistente.\n");
        exit(1);
    }
    return base;
}

void verificarSegmento(unByte id, cuatroBytes x, cuatroBytes registros[CANTREGISTROS], dosBytes tabla[MAXSEGMENTOS][2]) {
    // id representa al índice de registro
    int base = baseSegmento(id,registros);
    dosBytes min = tabla[base][0];
    cuatroBytes max = (unsigned short)tabla[base][0]+(unsigned short)tabla[base][1];

    if (x<min || x>=max) {
        switch(base) {
            case 0: printf("CS excedido - el programa llegó precipitadamente a su fin.\n"); break;
            case 1: printf("DS excedido.\n"); break;
            //case 2: printf("ES excedido.\n"); break;
            //case 3: printf("SS excedido.\n"); break;
            //case 4: printf("KS excedido.\n"); break;
        }
        exit(1);
    }
}
/*
    Habían subprogramas para verificar cada segmento en específico.
    They're gone now.
    Escribo esto por si hay que volver a ellas.
*/
void verificarStackOverflow(cuatroBytes x, cuatroBytes registros[CANTREGISTROS], dosBytes tabla[MAXSEGMENTOS][2]) {
    int baseSS = baseSegmento(3,registros);
    dosBytes min = tabla[baseSS][0];

    if (x<min) {
        printf("Error: Stack overflow.\n");
        exit(1);
    }
}
void verificarStackUnderflow(cuatroBytes x, cuatroBytes registros[CANTREGISTROS], dosBytes tabla[MAXSEGMENTOS][2]) {
    int baseSS = baseSegmento(3,registros);
    cuatroBytes max = (unsigned short)tabla[baseSS][0] + (unsigned short)tabla[baseSS][1];

    if (x>=max) {
        printf("Error: Stack overflow.\n");
        exit(1);
    }
}
void excepcionTipoDeOperandoInvalido(unByte tipo) {
    if (tipo<0 || tipo>3) {
        printf("Error: Tipo inválido de operando.\n");
        exit(1);
	}
}
void excepcionOperandoNulo(unByte tipo) {
    if (tipo == 0) {
        printf("Error: Tipo de operando nulo.\n");
        exit(1);
    }
}
void excepcionGuardarEnInmediato(unByte tipo, char nombre[MAXNOMBRE]) {
    if (tipo == 2) {
        printf("Error [%s]: Se intentó guardar un resultado en un operando inmediato.\n",nombre);
        exit(1);
	}
}
// excepción división por cero en funciones.h > operacionesAritmeticas(...)
// excepción instrucción inválidad en main.c > main(...) y disassembler.h > disassembler(...)



