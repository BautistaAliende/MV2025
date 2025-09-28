//#include "tiposyctes.h"
//#include "verificaciones.h"

cuatroBytes dato(cuatroBytes op, cuatroBytes registros[CANTREGISTROS], unByte *memoria, dosBytes tabla[MAXSEGMENTOS][2]);
void cambiaCC(cuatroBytes resultado, cuatroBytes registros[CANTREGISTROS]);
cuatroBytes mascara(cuatroBytes sector);
unByte tipoDeOperando(cuatroBytes byte);
dosBytes indiceDeMemoria(cuatroBytes op, cuatroBytes registro, dosBytes tabla[MAXSEGMENTOS][2]);
void escribirBinarioComp2(cuatroBytes x);

cuatroBytes dato(cuatroBytes op, cuatroBytes registros[CANTREGISTROS], unByte *memoria, dosBytes tabla[MAXSEGMENTOS][2]) {
	char tipo = tipoDeOperando(op);


    cuatroBytes ret;

	excepcionOperandoNulo(tipo);
	excepcionTipoDeOperandoInvalido(tipo);

	switch(tipo) {
    	case 1: {
    	    int codigo = ((op)&0x1F);
        	op = registros[codigo];
        	break;
    	}
    	case 2: {
        	op = (op)&0x0000FFFF;
        	if ((op)&0x00008000)
                (op) = (op)|0xFFFF0000;
        	break;
    	}
    	case 3: {
    	    int codigo = ((op>>16)&0x1F);
            dosBytes indMem = indiceDeMemoria(op,registros[codigo],tabla);
            unByte b = 4;
            op = 0;

            for(int i=0;i<b;i++) {
                verificarSegmento(codigo,indMem+i,registros,tabla);
                op = (op)<<8;
                op = op + (unsigned char)memoria[indMem+i];
            }
            break;
    	}
	}
	ret = op;
	return ret;
}

void cambiaCC(cuatroBytes resultado, cuatroBytes registros[CANTREGISTROS]) {
	registros[17] = 0;
	if (resultado == 0)
    	registros[17] += 1;
	else if (resultado < 0)
    	registros[17] += 2;
	registros[17] = registros[17]<<30;
}

cuatroBytes mascara(cuatroBytes sector) {
    switch (sector) {
        case 0: return 0xFFFFFFFF;
        case 1: return 0x000000FF;
        case 2: return 0x0000FF00;
        case 3: return 0x0000FFFF;
        default: printf("Error: mascara llamada con valor inválido (<0 || >3)"); // Salta sólo por error del programador
                 exit(1);
    }
}
unByte tipoDeOperando(cuatroBytes byte) {
    cuatroBytes top = byte&0xC0000000;
    top = top>>30;
    top = top&0x00000003;
    return top;
}
dosBytes indiceDeMemoria(cuatroBytes op, cuatroBytes registro, dosBytes tabla[MAXSEGMENTOS][2]) {
    cuatroBytes baseEnReg = (registro&0xFFFF0000)>>16;
    cuatroBytes offsetEnReg = (registro&0x0000FFFF);
    cuatroBytes offsetInstruccion = (op&0x00FFFF);
    dosBytes indMem = tabla[baseEnReg][0]+offsetEnReg+offsetInstruccion;

    return indMem;
}

void escribirBinario(cuatroBytes x, char bytes) {
    int i,
        k = bytes*8-1;
    cuatroBytes mascara = 0x1;
    mascara = mascara<<k;

    printf("0b");
    while (k>=0 && !(x&mascara)) {
        k--;
        mascara = mascara>>1;
    }
    if (k<0)
        printf("0");
    else {
        for(i=k;i>=0;i--) {
            printf((x&mascara)?"1":"0");
            mascara = mascara>>1;
        }
    }
}



