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
        case 3: sysStringRead(registros, memoria, tabla); break;
        case 4: sysStringWrite(registros, memoria, tabla); break;
        case 7: clearscreen(); break;
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

	excepcionTipoDeOperandoInvalido(tipo);
	excepcionOperandoNulo(tipo);
	excepcionGuardarEnInmediato(tipo);//,"NOT");

	if (tipo == 1) {
        int codigo = (registros[6]&0x1F);
        registros[codigo] = ~(registros[codigo]);
        cambiaCC(registros[codigo],registros);
	} else
    	if (tipo == 3) {
    	    int codigo = ((registros[6]>>16)&0x1F);
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

	//printf("SOY MOV\n");
	if (tipo==1) {
        //printf("Opb: %8x\n", opB);
	    int codigo = (registros[5]&0x1F);
        char sector = (registros[5]>>6)&0x03;
        //printf("Sector: %8x\n", sector);
    	cuatroBytes mask = mascara(sector);
    	//printf("Mask: %8x\n", mask);
    	if (sector == 2)
            opB = opB<<8;
        //printf("RegistroCodigo NOT MASK: %8x\n", registros[codigo]&(~mask));
        //printf("OPB MASK: %8x\n", opB&mask);

    	registros[codigo] = (registros[codigo]&(~mask)) + (opB&mask);

	    //printf("Registro %4x\n", registros[5]);
        //int codigo = (registros[5]&0x1F);
    	//registros[codigo] = opB;
	} else
    	if (tipo == 3) {

    	    //printf("Entre a tipo3\n");
    	    int codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);

            unByte b = 4-((registros[5]>>22)&0x03);
            if (b==3)
                b--;

            for(unByte i=b-1;i>=0;i--) {
                verificarSegmento(codigo,indMem+i,registros,tabla);
                memoria[indMem+i] = opB;
                opB = opB>>8;
            }
        }
}

void operacionesAritmeticas(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2], unByte id) {
	unByte tipo = tipoDeOperando(registros[5]);
	cuatroBytes codigo;
	cuatroBytes opB = dato(registros[6],registros,memoria,tabla);

    //printf("Sub OPB: %d\n", opB);

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
        //printf("SUB Reg5 osea OP1: %8x\n", registros[5]);
        //printf("SUB RegCodigo: %8x\n", registros[codigo]);
    	unByte sector = (registros[5]>>6)&0x03;
    	//printf("SUB Sector: %8x\n", sector);
    	cuatroBytes mask = mascara(sector);
    	//printf("SUB Mascara: %8x\n", mask);
        cuatroBytes auxReg = registros[codigo]&mask;
    	// Acomodar
    	if (sector == 2)
            auxReg = auxReg>>8;
        // Operaciones
        //printf("Sub OPA: %d\n", auxReg);
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
        //un solo byte
        if ((sector == 1 || sector == 2) && (auxReg&0x00000080))      //
            auxReg = auxReg|0xFFFFFF00;
        else
            if (sector == 3 && (auxReg&0x00008000))
                auxReg = auxReg|0xFFFF0000;

        // cambiaCC
        cambiaCC(auxReg,registros);

        // Reacomodar
        if (sector == 2)
            auxReg = auxReg<<8;
        // Reevalúa
        registros[codigo] = (registros[codigo]&(~mask))+(auxReg&mask);
	} else
    	if (tipo==3) {
    	    //printf("OpB = %d\n", opB);

            codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
        	cuatroBytes auxMem = 0;

            //printf("indMem: %2x\n",indMem);
        	verificarSegmento(codigo,indMem,  registros,tabla);
        	verificarSegmento(codigo,indMem+3,registros,tabla);

        	unByte b = 4-((registros[5]>>22)&0x03);
            if (b == 3)
                b--;

        	for(int i=0;i<4;i++) {
            	auxMem = auxMem<<8;
            	auxMem += (unsigned char)memoria[indMem+i];
        	}
            //printf("AuxMem = %d\n", auxMem);
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
        	for(char i=b-1;i>=0;i--) {
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
        unByte sector = (registros[5]>>6)&0x03;
        cuatroBytes mask = mascara(sector);

        if (sector == 2)
            auxB = auxB<<8;
        registros[codigo] = (registros[codigo]&(~mask))+(auxB&mask);
    } else
        if (tipoA == 3) {
            unByte b = 4-((registros[5]>>22)&0x03);
            if (b==3)
                b--;
            codigo = ((registros[5]>>16)&0x1F);
            dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
            for(unByte i=0;i<b;i++) {
                verificarSegmento(codigo,indMem+i,registros,tabla);
                memoria[indMem+i] = auxB>>(8*(4-i-1));
            }
        }
    // A a B
    excepcionGuardarEnInmediato(tipoB);//,"SWAP");

    if (tipoB == 1) {
        codigo = (registros[6]&0x1F);
        unByte sector = (registros[6]>>6)&0x03;
        cuatroBytes mask = mascara(sector);

        if (sector == 2)
           auxA = auxA<<8;
        registros[codigo] = (registros[codigo]&(~mask))+(auxA&mask);
    } else
        if (tipoB == 3) {

            unByte b = 4-((registros[6]>>22)&0x03);
            if (b==3)
                b--;

            codigo = ((registros[6])>>16&0x1F);
            dosBytes indMem = indiceDeMemoria(registros[6],registros[codigo],tabla);
            for(unByte i=0;i<b;i++) {
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

    //printf("CMP opB: %d\n", opB);
	if (tipo==1) {
        codigo = (registros[5]&0x1F);
        unByte sector = (registros[5]>>6)&0x03;
    	cuatroBytes mask = mascara(sector),
        auxReg = registros[codigo]&mask;
        if (sector == 2)
            auxReg = auxReg>>8;
        if ((sector == 1 || sector == 2) && (auxReg&0x00000080))
            auxReg = auxReg|0xFFFFFF00;
        else if (sector == 3 && (auxReg&0x00008000))
            auxReg = auxReg|0xFFFF0000;
        //auxReg = registros[codigo];

        //printf("CMP AuxReg: %d\n", auxReg);

        auxReg -= opB;

        cambiaCC(auxReg,registros);
	} else
    	if (tipo==3) {
    	    codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
        	cuatroBytes auxMem = 0;

            unByte b = 4-((registros[5]>>22)&0x03);
            if (b==3)
                b--;

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

	if (tipo==1) {
        codigo = (registros[5]&0x1F);
        unByte sector = (registros[5]>>6)&0x03;
    	cuatroBytes mask = mascara(sector),
        auxReg = registros[codigo]&mask;

        if (sector == 2)
            auxReg = auxReg>>8;
        if ((sector == 1 || sector == 2) && (auxReg&0x00000080))
            auxReg = auxReg|0xFFFFFF00;
        else if (sector == 3 && (auxReg&0x00008000))
            auxReg = auxReg|0xFFFF0000;

        if (dir < 0)
            auxReg = auxReg << opB;
        else
            if (dir == 0)
                auxReg = auxReg >> opB;
            else{ //evito la propagación de signo del lenguaje C
                uint64_t auxauxReg = auxReg >> opB;
                auxReg = auxauxReg;
            }

        if ((sector == 1 || sector == 2) && (auxReg&0x00000080))
            auxReg = auxReg|0xFFFFFF00;
        else if (sector == 3 && (auxReg&0x00008000))
            auxReg = auxReg|0xFFFF0000;

        cambiaCC(auxReg,registros);
        if (sector == 2)
            auxReg = auxReg<<8;
        registros[codigo] = auxReg;//(registros[codigo]&(~mask))+(auxReg&mask);
	} else
    	if (tipo==3) {
            codigo = ((registros[5]>>16)&0x1F);
            //printf("OpB: %d\n", opB);
            auxReg = registros[codigo];

        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
        	cuatroBytes auxMem = 0;

        	verificarSegmento(codigo,indMem,  registros,tabla);
        	verificarSegmento(codigo,indMem+3,registros,tabla);

            unByte b = 4-((registros[5]>>22)&0x03);
            if (b==3)
                b--;

        	for(int i=0;i<b;i++) {
            	auxMem = auxMem<<8;
            	auxMem |= memoria[indMem+i];
                //printf("Lee: %2x\n", memoria[indMem+i]);
                //printf("AuxMem queda: %8x\n",auxMem);
        	}

        if (dir < 0){
            auxMem = auxMem << opB;
        }
        else
            if (dir == 0){
                //printf("Antes AUXREG: %8x\n", auxReg);
                auxMem = auxMem >> opB;
                //printf("Dsp AUXREG: %8x\n", auxReg);
            }
            else{ //evito la propagación de signo del lenguaje C
                //printf("AUXMEM: %8x\n", auxMem);
                uint32_t auxauxMem = auxMem;
                //printf("AuxAuxMem: %8x\n", auxauxMem);
                auxauxMem = auxauxMem >> opB;
                //printf("AuxAuxMem: %8x\n", auxauxMem);
                auxMem = auxauxMem;
                //printf("AUXMEM: %8x\n",auxMem);
            }

        	cambiaCC(auxMem,registros);
        	for(char i=b-1;i>=0;i--) {
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

    //printf("OperandoB: %d\n", opB);

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
    	unByte sector = (registros[5]>>6)&0x03;
    	cuatroBytes mask = mascara(sector);
        cuatroBytes auxReg = registros[codigo]&mask;
        // Acomodar
        if (sector == 2)
           auxReg = auxReg>>8;
        // Operaciones
        switch(id) {
            case 0: auxReg = auxReg&opB; break;
            case 1: auxReg = auxReg|opB; break;
            case 2: auxReg = auxReg^opB;
        }
        // Propagar signos
        if ((sector == 1 || sector == 2) && (auxReg&0x00000080))
            auxReg = auxReg|0xFFFFFF00;
        else if (sector == 3 && (auxReg&0x00008000))
            auxReg = auxReg|0xFFFF0000;
        // cambiaCC
        cambiaCC(auxReg,registros);
        // Reacomodar
        if (sector == 2)
            auxReg = auxReg<<8;
        // Reevalúa
        registros[codigo] = (registros[codigo]&(~mask))+(auxReg&mask);
	} else
    	if (tipo==3) {
            codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
        	int auxMem = 0;

        	unByte b = 4-((registros[5]>>22)&0x03);
            if (b==3)
                b--;

        	verificarSegmento(codigo,indMem,  registros,tabla);
        	verificarSegmento(codigo,indMem+3,registros,tabla);

            //printf("indMem2: %d\n", indMem);
        	for(int i=0;i<b;i++) {
            	auxMem = auxMem<<8;
            	auxMem |= memoria[indMem+i];

        	}

            //printf("OperandoA: %d\n", auxMem);

        	switch(id) {
                case 0: auxMem = auxMem&opB; break;
                case 1: auxMem = auxMem|opB; break;
                case 2: auxMem = auxMem^opB;
            }

            //printf("Resultado de OpA: %d\n", auxMem);

        	cambiaCC(auxMem,registros);
        	//printf("b: %d\n", b);

        	for(char i=b-1;i>=0;i--) {
                //printf("indMem: %d\n", indMem);
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
    	unByte sector = (registros[5]>>6)&0x03;
    	cuatroBytes mask = (bytes<0) ? 0xFFFF0000 : 0x0000FFFF;

    	//ojo
    	if (sector == 0){
            if (bytes<0)
                opB = opB<<16;
            registros[codigo] = (registros[codigo]&(~mask)) + (opB&mask);
        }else {
           printf("Error [%s]: Se intentó cargar 2 bytes en una sección de registro.\n",(bytes<0)?"LDH":"LDL");
           exit(1);
        }
	}
	 else
    	if (tipo==3) {
            codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);
        	char a = (bytes<0) ? 0 : 2, // 1: LDL ; -1: LDH
                 b = a+2;

        	for(unByte i=b-1;i>=a;i--) {
            	verificarSegmento(codigo,indMem+i,registros,tabla);
            	memoria[indMem+i] = opB;
            	opB = opB>>8;
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
    	unByte sector = (registros[5]>>6)&0x03;
        cuatroBytes mask = mascara(sector);

    	if (sector == 2)
            opB = opB<<8;
    	registros[codigo] = (registros[codigo]&(~mask)) + (opB&mask);
	} else
    	if (tipo==3) {
            codigo = ((registros[5]>>16)&0x1F);
        	dosBytes indMem = indiceDeMemoria(registros[5],registros[codigo],tabla);

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
    registros[7] -= 4;
    unByte baseSP = baseSegmento(29,registros);

    cuatroBytes indMem = tabla[baseSP][0]+(registros[7]&0xFFFF);

    verificarStackOverflow(indMem,registros,tabla);

    cuatroBytes opB = dato(registros[6],registros,memoria,tabla);

    //printf("Opb: %8x\n", opB);
    for(unByte i=3; i>=0; i--){
        verificarStackOverflow(indMem,registros,tabla);
        memoria[indMem+i] = opB;
        opB = opB>>8;
    }

}
void POP(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
    int baseSP = registros[7]>>16;
    int indMem = tabla[baseSP][0] + (registros[7]&0xFFFF);
    uint32_t dato = 0;

    for(unByte i=0;i<4;i++){
        verificarStackUnderflow(indMem+i,registros,tabla);
        dato = dato<<8;
        dato |= (unsigned char) memoria[indMem+i];
    }

    int tipo = tipoDeOperando(registros[6]);
    int codigo = registros[6]&0x1F;
    //printf("Tipo: %d\n", tipo);
    //printf("Codigo: %d\n", codigo);
    if (tipo==1) {
        int sector = (registros[6]>>6)&0x03;
        if (sector == 2)
            dato = dato<<8;
        int mask = mascara(sector);

        registros[codigo] = (registros[codigo]&(~mask))+(dato&mask);

        registros[7] += 4;
    } else
    if (tipo==3) {
        dosBytes indMem = indiceDeMemoria(registros[7],registros[codigo],tabla);

        for(unByte i=3;i>=0;i--) {
            memoria[indMem+i] = dato;
            dato = dato>>8;
        }

        registros[7] += 4;
    }
}
void CALL(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
    cuatroBytes aux = registros[6];
    //printf("Reg3: %2x\n", registros[3]);
    registros[6] = 0x1000003;
    PUSH(registros, memoria,tabla);
    registros[6] = aux;
    JMP(registros,memoria,tabla);
}
void RET(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]){
    registros[6] = (0x1000003);
    POP(registros, memoria, tabla);
}




