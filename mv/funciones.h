//#include "tiposyctes.h"
//#include "verificaciones.h"
//#include "utiles.h"
//include <string.h>
#include "sys.h"

//void operacionesAritmeticas(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2], unByte id);
//void sh(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2], char dir);
//void operacionesBitABit(cuatroBytes registros[CANTREGISTROS], unByte *memoria, dosBytes tabla[MAXSEGMENTOS][2], unByte id);
//void ld(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2], char bytes);

void SYS(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
    cuatroBytes opB;

    opB = dato(registros[6],registros,memoria,tabla);

    switch (opB) {
        case 1: sysRead(registros, memoria, tabla); break;
        case 2: sysWrite(registros, memoria, tabla); break;
        //case 3: sysStringRead(registros, memoria, tabla); break;
        //case 4: sysStringWrite(registros, memoria, tabla); break;
        //case 7: clearscreen(); break;
        case 15: break;
        default: {
            printf("Error: %d valor no válido para SYS.\n",opB);
            exit(1);
        }
    }
}

void JMP(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	cuatroBytes opB = dato(registros[6],registros,memoria,tabla);
	registros[3] = registros[26]+opB;
};
void JZ(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
	unByte dosMasSig = (registros[17]>>30)&0b0011; // 0b11?
	if (dosMasSig == 1)
    	JMP(registros,memoria,tabla);
};
void JP(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
	unByte dosMasSig = (registros[17]>>30)&0b0011;
	if (dosMasSig == 0)
    	JMP(registros,memoria,tabla);
};
void JN(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	unByte dosMasSig = (registros[17]>>30)&0b0011;
	if (dosMasSig == 2)
    	JMP(registros,memoria,tabla);
};
void JNZ(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	unByte dosMasSig = (registros[17]>>30)&0b0011;
	if (dosMasSig == 0 || dosMasSig == 2)
    	JMP(registros,memoria,tabla);
};
void JNP(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	unByte dosMasSig = (registros[17]>>30)&0b0011;
	if (dosMasSig == 1 || dosMasSig == 2)
    	JMP(registros,memoria,tabla);
};
void JNN(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	unByte dosMasSig = (registros[17]>>30)&0b0011;
	if (dosMasSig == 0 || dosMasSig == 1)
    	JMP(registros,memoria,tabla);
};
void NOT(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	unByte tipo = tipoDeOperando(registros[6]);
	int codigo = (registros[6]&0xF0)>>4;

	excepcionTipoDeOperandoInvalido(tipo);
	excepcionOperandoNulo(tipo);
	excepcionGuardarEnInmediato(tipo);//,"NOT");

	if (tipo == 1) {
        registros[codigo] = ~(registros[codigo]);
        cambiaCC(registros[codigo],registros);
	} else
    	if (tipo == 3) {
        	dosBytes indMem = indiceDeMemoria(registros[6],registros[codigo],tabla);

        	for(unByte i=0;i<4;i++) {
                //verificarDS(indMem+i,registros,tabla);
                verificarSegmento(codigo,indMem+i,registros,tabla);
                memoria[indMem+i] = ~memoria[indMem+i];
        	}
        	cambiaCC(memoria[indMem],registros);
    	}
}
void STOP(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
    printf("El programa llegó pacíficamente a su fin.\n");
	exit(0);
}
void MOV(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	unByte tipo = tipoDeOperando(registros[5]);

	cuatroBytes opB = dato(registros[6],registros, memoria,tabla);
	excepcionGuardarEnInmediato(tipo);//,"MOV");

	if (tipo==1) {
        int codigo = (registros[5]&0x1F);
    	registros[codigo] = opB;
	} else
    	if (tipo == 3) {
    	    int codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);

            for(unByte i=0;i<4;i++) {
                verificarSegmento(codigo,indMem+i,registros,tabla);
                memoria[indMem+i] = opB>>(8*(4-i-1));
            }
        }
}

void operacionesAritmeticas(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2], unByte id) {
	unByte tipo = tipoDeOperando(registros[5]);
	cuatroBytes codigo;
	cuatroBytes opB = dato(registros[6],registros,memoria,tabla);

	char oa[5];
	switch (id) {
        case 0: {strcpy(oa,"ADD"); break;};
        case 1: {strcpy(oa,"SUB"); break;};
        case 2: {strcpy(oa,"MUL"); break;};
        case 3: {strcpy(oa,"DIV"); break;};
        default: {
            printf("Error: Operación aritmética inválida.\n"); // Salta sólo por error del programador
            exit(1);
        }
	}
	excepcionGuardarEnInmediato(tipo);//,oa);

	if (id == 3 && opB == 0) {
        printf("Error: división por cero.\n");
        exit(1);
	}

	if (tipo == 1) {
        codigo = (registros[5]&0x1F);
    	//unByte sector = (registros[5]&0x0C)>>2;
    	//cuatroBytes mask = mascara(sector),
        //            auxReg = registros[codigo]&mask;
    	// Acomodar
    	//if (sector == 2)
        //    auxReg = auxReg>>8;
        // Operaciones

        cuatroBytes auxReg = registros[codigo];

        switch(id) {
            case 0: auxReg += opB; break;
            case 1: auxReg -= opB; break;
            case 2: auxReg *= opB; break;
            case 3: {
                registros[16] = auxReg%opB;
                auxReg = (cuatroBytes)auxReg/opB;
            };
        }
        // Propagar signos
        //if ((sector == 1 || sector == 2) && (auxReg&0x00000080))
        //
        //    auxReg = auxReg|0xFFFFFF00;
        //else if (sector == 3 && (auxReg&0x00008000))
        //    auxReg = auxReg|0xFFFF0000;

        // cambiaCC
        cambiaCC(auxReg,registros);

        // Reacomodar
        //if (sector == 2)
        //    auxReg = auxReg<<8;
        // Reevalúa
        registros[codigo] = auxReg;//(registros[codigo]&(~mask))+(auxReg&mask);
	} else
    	if (tipo==3) {
            codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
        	cuatroBytes auxMem = 0;

            //printf("indMem: %2x\n",indMem);

            /*
        	switch(codigo) {
                case 1: verificarDS(indMem,registros,tabla); verificarDS(indMem+3,registros,tabla); break;
                case 2: verificarES(indMem,registros,tabla); verificarES(indMem+3,registros,tabla); break;
        	}
        	*/
        	verificarSegmento(codigo,indMem,  registros,tabla);
        	verificarSegmento(codigo,indMem+3,registros,tabla);

        	for(int i=0;i<4;i++) {
            	auxMem = auxMem<<8;
            	auxMem += (unsigned char)memoria[indMem+i];
        	}
            switch (id) {
                case 0: auxMem += opB; break;
                case 1: auxMem -= opB; break;
                case 2: auxMem *= opB; break;
                case 3: {
                    registros[16] = auxMem%opB;
                    auxMem = (cuatroBytes)auxMem/opB;
                }
            }
        	cambiaCC(auxMem,registros);
        	//printf("indMem: %2x\n",indMem);
        	//printf("auxMem: %2x\n",auxMem);
        	for(int i=3;i>=0;i--) {
                memoria[indMem+i] = auxMem;
                auxMem = auxMem>>8;
        	}
    	}
}

void ADD(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	operacionesAritmeticas(registros,memoria,tabla,0);
}
void SUB(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	operacionesAritmeticas(registros,memoria,tabla,1);
}
void SWAP(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
    cuatroBytes codigo;
    int tipoA = tipoDeOperando(registros[5]),
        tipoB = tipoDeOperando(registros[6]);
    // B a A
    excepcionGuardarEnInmediato(tipoA);//,"SWAP");

    cuatroBytes auxA = dato(registros[5],registros,memoria,tabla);
    cuatroBytes auxB = dato(registros[6],registros,memoria,tabla);

    if(tipoA == 1) {
        codigo = (registros[5]&0x1F);
        //unByte sector = (opA&0x0C)>>2;
        //cuatroBytes mask = mascara(sector);

        //if (sector == 2)
        //    auxB = auxB<<8;
        registros[codigo] = auxB;//(registros[codigo]&(~mask))+(auxB&mask);
    } else
        if (tipoA == 3) {
            codigo = ((registros[5]>>16)&0x1F);
            dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
            for(unByte i=0;i<4;i++) {
                verificarSegmento(codigo,indMem+i,registros,tabla);
                memoria[indMem+i] = auxB>>(8*(4-i-1));
            }
        }
    // A a B
    excepcionGuardarEnInmediato(tipoB);//,"SWAP");

    if (tipoB == 1) {
        codigo = (registros[6]&0x1F);
        //unByte sector = (opB&0x0C)>>2;
        //cuatroBytes mask = mascara(sector);

        //if (sector == 2)
        //    auxA = auxA<<8;
        registros[codigo] = auxA;//(registros[codigo]&(~mask))+(auxA&mask);
    } else
        if (tipoB == 3) {
            codigo = ((registros[6])>>16&0x1F);
            dosBytes indMem = indiceDeMemoria(registros[6],registros[codigo],tabla);
            for(unByte i=0;i<4;i++) {
                //verificarDS(indMem+i,registros,tabla);
                verificarSegmento(codigo,indMem+i,registros,tabla);
                memoria[indMem+i] = auxA>>(8*(4-i-1));
            }
        }
}
void MUL(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	operacionesAritmeticas(registros,memoria,tabla,2);
}
void DIV(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
    operacionesAritmeticas(registros,memoria,tabla,3);
}

void CMP(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	unByte tipo = tipoDeOperando(registros[5]);
	cuatroBytes codigo, auxReg;

	cuatroBytes opB = dato(registros[6], registros,memoria,tabla);
	excepcionGuardarEnInmediato(tipo);//,"CMP");

	if (tipo==1) {
        codigo = (registros[5]&0x1F);
    	//char sector = (opA&0x0C)>>2;
    	//cuatroBytes mask = mascara(sector),
        //           auxReg = registros[codigo]&mask;
        //if (sector == 2)
        //    auxReg = auxReg>>8;
        //if ((sector == 1 || sector == 2) && (auxReg&0x00000080))
        //    auxReg = auxReg|0xFFFFFF00;
        //else if (sector == 3 && (auxReg&0x00008000))
        //    auxReg = auxReg|0xFFFF0000;
        auxReg = registros[codigo];
        auxReg -= opB;
        cambiaCC(auxReg,registros);
	} else
    	if (tipo==3) {
    	    codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
        	cuatroBytes auxMem = 0;

        	unByte b = 4;//-(registros[5]&0x03);
            //if (b==3) b--;
            /*
        	switch (codigo) {
                case 1: verificarDS(indMem,registros,tabla); verificarDS(indMem+3,registros,tabla); break;
                case 2: verificarES(indMem,registros,tabla); verificarES(indMem+3,registros,tabla); break;
        	}
        	*/
        	verificarSegmento(codigo,indMem,  registros,tabla);
        	verificarSegmento(codigo,indMem+3,registros,tabla);

        	if (memoria[indMem]&0x80)
                auxMem = -1;

        	for(int i=0;i<b;i++) {
            	auxMem = auxMem<<8;
            	auxMem += (unsigned char)memoria[indMem+i];
        	}
        	auxMem -= opB;
        	cambiaCC(auxMem,registros);
        }
}

void sh(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2], char dir) {
	unByte tipo = tipoDeOperando(registros[5]);
	cuatroBytes codigo, auxReg;
	cuatroBytes opB = dato(registros[6],registros,memoria,tabla);

	//if (dir==0){    // Salta sólo por error del programador
    //    printf("Error: shift id = 0.\n");
    //    exit(1);
	//}
	//excepcionGuardarEnInmediato(tipo,(dir<0)?"SHL":"SHR");

	if (tipo==1) {
        codigo = (registros[5]&0x1F);
    	//unByte sector = (opA&0x0C)>>2;
    	//cuatroBytes mask = mascara(sector),
        //            auxReg = registros[codigo]&mask;
        auxReg = registros[codigo];
        //if (sector == 2)
        //    auxReg = auxReg>>8;
        //if ((sector == 1 || sector == 2) && (auxReg&0x00000080))
        //    auxReg = auxReg|0xFFFFFF00;
        //else if (sector == 3 && (auxReg&0x00008000))
        //    auxReg = auxReg|0xFFFF0000;

        if (dir < 0)
            auxReg = auxReg << opB;
        else
            if (dir == 0)
                auxReg = auxReg >> opB;
            else{ //evito la propagación de signo del lenguaje C
                uint64_t auxauxReg = auxReg >> opB;
                auxReg = auxauxReg;
            }

        //if ((sector == 1 || sector == 2) && (auxReg&0x00000080))
        //    auxReg = auxReg|0xFFFFFF00;
        //else if (sector == 3 && (auxReg&0x00008000))
        //    auxReg = auxReg|0xFFFF0000;

        cambiaCC(auxReg,registros);
        //if (sector == 2)
        //    auxReg = auxReg<<8;
        registros[codigo] = auxReg;//(registros[codigo]&(~mask))+(auxReg&mask);
	} else
    	if (tipo==3) {
            codigo = ((registros[5]>>16)&0x1F);

            auxReg = registros[codigo];

        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
        	long auxMem = 0;

        	//verificarDS(indMem,registros,tabla);
        	//verificarDS(indMem+3,registros,tabla);
        	verificarSegmento(codigo,indMem,  registros,tabla);
        	verificarSegmento(codigo,indMem+3,registros,tabla);

        	for(int i=0;i<4;i++) {
            	auxMem = auxMem<<8;
            	auxMem += memoria[indMem+i];
        	}

        if (dir < 0)
            auxReg = auxReg << opB;
        else
            if (dir == 0)
                auxReg = auxReg >> opB;
            else{ //evito la propagación de signo del lenguaje C
                uint64_t auxauxReg = auxReg >> opB;
                auxReg = auxauxReg;
            }

        	cambiaCC(auxMem,registros);
        	for(char i=3;i>=0;i--) {
                memoria[indMem+i] = auxMem;
                auxMem = auxMem>>8;
        	}
    	}
}

void SHL(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
	sh(registros,memoria,tabla,-1);
}
void SHR(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
    sh(registros,memoria,tabla,1);
}
void SAR(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
    sh(registros,memoria,tabla,0);
}

void operacionesBitABit(cuatroBytes registros[CANTREGISTROS], unByte *memoria, dosBytes tabla[MAXSEGMENTOS][2], unByte id) {
	unByte tipo = tipoDeOperando(registros[5]);
	cuatroBytes codigo;
	cuatroBytes opB = dato(registros[6],registros,memoria,tabla);

	char ol[5];
	switch(id) {
        case 0: {strcpy(ol,"AND"); break;}
        case 1: {strcpy(ol,"OR"); break;}
        case 2: {strcpy(ol,"XOR"); break;}
        default: {
            printf("Error: Operación lógica inválida.\n"); // Salta sólo por error del programador
            exit(1);
        }
	}
	excepcionGuardarEnInmediato(tipo);//,ol);

	if (tipo==1) {
        codigo = (registros[5]&0x1F);
    	//unByte sector = (opA&0x0C)>>2;
    	//cuatroBytes mask = mascara(sector),
        cuatroBytes auxReg = registros[codigo];//&mask;
        // Acomodar
        //if (sector == 2)
        //   auxReg = auxReg>>8;
        // Operaciones
        switch(id) {
            case 0: auxReg = auxReg&opB; break;
            case 1: auxReg = auxReg|opB; break;
            case 2: auxReg = auxReg^opB;
        }
        // Propagar signos
        //if ((sector == 1 || sector == 2) && (auxReg&0x00000080))
        //    auxReg = auxReg|0xFFFFFF00;
        //else if (sector == 3 && (auxReg&0x00008000))
        //    auxReg = auxReg|0xFFFF0000;
        // cambiaCC
        cambiaCC(auxReg,registros);
        // Reacomodar
        //if (sector == 2)
        //    auxReg = auxReg<<8;
        // Reevalúa
        registros[codigo] = auxReg;//(registros[codigo]&(~mask))+(auxReg&mask);
	} else
    	if (tipo==3) {
            codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
        	int auxMem = 0;

        	unByte b = 4;//-(registros[5]&0x03);
            //if (b==3) b--;

        	//verificarDS(indMem,registros,tabla);
        	//verificarDS(indMem+3,registros,tabla);
        	verificarSegmento(codigo,indMem,  registros,tabla);
        	verificarSegmento(codigo,indMem+3,registros,tabla);

        	for(int i=0;i<b;i++) {
            	auxMem = auxMem<<8;
            	auxMem += memoria[indMem+i];
        	}
        	switch(id) {
                case 0: auxMem = auxMem&opB; break;
                case 1: auxMem = auxMem|opB; break;
                case 2: auxMem = auxMem^opB;
            }
        	cambiaCC(auxMem,registros);
        	for(char i=b-1;i>=0;i--) {
                memoria[indMem+i] = auxMem;
                auxMem = auxMem>>8;
        	}
    	}
}

void AND(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
    operacionesBitABit(registros,memoria,tabla,0);
}
void OR(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
	operacionesBitABit(registros,memoria,tabla,1);
}
void XOR(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
	operacionesBitABit(registros,memoria,tabla,2);
}

void ld(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2], char bytes) {
	unByte tipo = tipoDeOperando(registros[5]);
	cuatroBytes codigo;
	cuatroBytes opB = dato(registros[6],registros,memoria,tabla);

	if (bytes==0){  // Salta sólo por error del programador
        printf("Error: LD id = 0.\n");
        exit(1);
	}
	excepcionGuardarEnInmediato(tipo);//,(bytes<0)?"LDH":"LDL");

	if (tipo==1) {
         codigo = (registros[5]&0x1F);
    	//unByte sector = (registros[5]&0x0C)>>2;
    	cuatroBytes mask = (bytes<0) ? 0xFFFF0000 : 0x0000FFFF;

    	//ojo
    	int sector =0;
    	if (sector == 0)
            if (bytes<0)
                opB = opB<<16;
        registros[codigo] = (registros[codigo]&(~mask)) + (opB&mask);
        //} else {
        //   printf("Error [%s]: Se intentó cargar 2 bytes en una sección de registro.\n",(bytes<0)?"LDH":"LDL");
        //    exit(1);
    }
	 else
    	if (tipo==3) {
            codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
        	char a = (bytes<0) ? 0 : 2, // 1: LDL ; -1: LDH
                 b = a+2;
            /*
        	for(int i=b;i>a;i--) {
            	memoria[indMem+i] = opB;
            	opB = opB>>8;
        	}
        	*/
        	for(unByte i=a;i<b;i++) {
                //verificarDS(indMem+i,registros,tabla);
            	verificarSegmento(codigo,indMem+i,registros,tabla);
            	memoria[indMem+i] = opB>>(8*(2-i-1));
        	}
    	}
}

void LDL(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
	ld(registros,memoria,tabla,1);
}
void LDH(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
    ld(registros,memoria,tabla,-1);
}

void RND(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
	double randomAux = (double)rand()/(RAND_MAX+1.0);
	char tipo = (registros[5]&0x01000000)>>24;
    cuatroBytes codigo;
    cuatroBytes opB = dato(registros[6],registros,memoria,tabla);
	opB = (cuatroBytes)(randomAux*opB);

	excepcionGuardarEnInmediato(tipo);//,"RND");

	if (tipo==1) {
        codigo = (registros[5]&0x1F);
    	//unByte sector = (opA&0x0C)>>2;
        //cuatroBytes mask = mascara(sector);

    	//if (sector == 2)
        //    opB = opB<<8;
    	registros[codigo] = opB;//(registros[codigo]&(~mask)) + (opB&mask);
	} else
    	if (tipo==3) {
            codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
            /*
        	for(char i=3;i>=0;i--) {
            	memoria[indMem+i] = opB;
            	opB = opB>>8;
            }
            */
            for(unByte i=0;i<4;i++) {
                //verificarDS(indMem+i,registros,tabla);
            	verificarSegmento(codigo,indMem+i,registros,tabla);
            	memoria[indMem+i] = opB>>(8*(4-i-1));
            }
    	}
}

// 09
void FEXC(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
	printf("Error: función no existente\n");
	exit(1);
}
//0A
void PUSH(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
    registros[6] -= 4;
    unByte baseSP = baseSegmento(6,registros);
    cuatroBytes indMem = tabla[baseSP][0]+(registros[6]&0xFFFF);
    verificarStackOverflow(indMem,registros,tabla);

    cuatroBytes opB = dato(registros[6],registros,memoria,tabla);

    for(unByte i=3; i>=0; i--){
        memoria[indMem+i] = opB;
        opB = opB>>8;
    }
}
void POP(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
    int baseSP = registros[6]>>16;
    int indMem = tabla[baseSP][0] + (registros[6]&0xFFFF);
    uint32_t dato = 0;

    for(unByte i=0;i<4;i++){
        verificarStackUnderflow(indMem+i,registros,tabla);
        dato = dato<<8;
        dato |= (unsigned char) memoria[indMem+i];
    }

    int tipo = tipoDeOperando(registros[6]);
    int codigo = (registros[6]&0xF0)>>4;

    if (tipo==1) {
        int sector = (registros[6]&0x0C)>>2;
        if (sector == 2)
            dato = dato<<8;
        int mask = mascara(sector);

        registros[codigo] = (registros[codigo]&(~mask))+(dato&mask);

        registros[6] += 4;
    } else
    if (tipo==3) {
        dosBytes indMem = indiceDeMemoria(registros[6],registros[codigo],tabla);

        for(unByte i=3;i>=0;i--) {
            memoria[indMem+i] = dato;
            dato = dato>>8;
        }

        registros[6] += 4;
    }
}
void CALL(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
    //cuatroBytes auxOpB = (0x40000050);
    PUSH(registros,memoria,tabla);
    JMP(registros,memoria,tabla);
}
void RET(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
    //cuatroBytes auxOpB = (0x40000050);
    POP(registros, memoria, tabla);
}




