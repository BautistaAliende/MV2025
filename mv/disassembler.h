#include "tiposyctes.h"
//#include "utiles.h"

#define IDENTACION_MAX 12
#define MAX_BYTES 8

void disassembler(dosBytes inicio, dosBytes inicioCS, dosBytes longCS, dosBytes entryPoint, int esV2, unByte memoria[MAXMEMORIA]);
void escribeInstruccion(unByte instruccion, int esV2);
void errorDissasembler(char msjError[]);
void escribeOperando(unByte op, int ir, unByte memoria[], int esV2);
void escribeRegistro(unByte registro, int esV2);
void escribeMemoria(cuatroBytes bytesMemoria, int esV2);
int longitudNum(long num);
void identar(int cant);

void identar(int cant){
    for(int i = 0; i < cant; i++)
        printf(" ");
}

int longitudNum(long num) {
    // Si es 0 ocupa un espacio pero no entra al while
    int i = (num == 0);
    // El signo "-" aporta a la longitud en caracteres
    if(num < 0) {
        num = -num;
        i++;
    }
    while(num > 0){
        num = num / 10;
        i++;
    }
    return i;
}

void errorDissasembler(char msjError[]) {
    printf("\n\n%s\n\n", msjError);
    //exit(1);
}

// Todos los registros deben ocupar siempre 3 caracteres
void escribeRegistro(unByte registro, int esV2){
    unByte sectorReg = (registro >> 6)&0x3;
    registro = registro&0x1F;//(registro >> 4)&0xF;

    //printf("Entre a escribe registro\n");
    //printf("Registro: %2x\n", registro);

    if(!esV2 && (( 7 <= registro && registro <= 9) || (18 <= registro && registro <= 25)|| (28 <= registro && registro <= 31) ))
      errorDissasembler("Error: Registro inválido para la versión de la máquina.\n");

    //sectorReg = 0; //: Registro completo
    if(sectorReg == 0 && registro >= 0xA && registro <= 0xF)
        printf("E");
    else
        printf(" ");

    switch(registro){
        case(0x0): printf("LAR"); break;
        case(0x1): printf("MAR"); break;
        case(0x2): printf("MBR"); break;
        case(0x3): printf("IP"); break;
        case(0x4): printf("OPC"); break;
        case(0x5): printf("OP1"); break;
        case(0x6): printf("OP2"); break;
        case(0x7): printf("SP"); break;
        case(0x8): printf("BP"); break;
        //case(0x9): printf("RESERVADO"); break;
        case(0xA): printf("A"); break;
        case(0xB): printf("B"); break;
        case(0xC): printf("C"); break;
        case(0xD): printf("D"); break;
        case(0xE): printf("E"); break;
        case(0xF): printf("F"); break;
        case(0x10): printf("AC"); break;
        case(0x11): printf("CC"); break;
        case(0x1A): printf("CS"); break;
        case(0x1B): printf("DS"); break;
        case(0x1C): printf("ES"); break;
        case(0x1D): printf("SS"); break;
        case(0x1E): printf("KS"); break;
        case(0x1F): printf("PS"); break;

        default: errorDissasembler("Error: Registro inválido");
    }

    if(registro >= 0xA && registro <= 0xF){
        if(sectorReg == 0 || sectorReg == 3)
            printf("X");
        else
            if(sectorReg == 1)
                printf("L");
            else
                printf("H");
    }

}

void escribeInstruccion(unByte instruccion, int esV2){
    if(!esV2 && (0x0B <= instruccion && instruccion <= 0x0E))
      errorDissasembler("Error: Instruccion invalida para la version de la maquina");

    switch(instruccion){
        case 0x00: printf("SYS ");  break;
        case 0x01: printf("JMP ");  break;
        case 0x02: printf("JZ  ");  break;
        case 0x03: printf("JP  ");  break;
        case 0x04: printf("JN  ");  break;
        case 0x05: printf("JNZ ");  break;
        case 0x06: printf("JNP ");  break;
        case 0x07: printf("JNN ");  break;
        case 0x08: printf("NOT ");  break;
        case 0x0B: printf("PUSH");  break; // V2
        case 0x0C: printf("POP ");  break; // V2
        case 0x0D: printf("CALL");  break; // V2
        case 0x0E: printf("RET ");  break; // V2
        case 0x0F: printf("STOP");  break;
        case 0x10: printf("MOV ");  break;
        case 0x11: printf("ADD ");  break;
        case 0x12: printf("SUB ");  break;
        case 0x13: printf("MUL ");  break;
        case 0x14: printf("DIV ");  break;
        case 0x15: printf("CMP ");  break;
        case 0x16: printf("SHL ");  break;
        case 0x17: printf("SHR ");  break;
        case 0x18: printf("SAR ");  break;
        case 0x19: printf("AND ");  break;
        case 0x1A: printf("OR  ");  break;
        case 0x1B: printf("XOR ");  break;
        case 0x1C: printf("SWAP ");  break;
        case 0x1D: printf("LDL ");  break;
        case 0x1E: printf("LDH ");  break;
        case 0x1F: printf("RND ");  break;
        default: errorDissasembler("Error: Codigo operacion invalido.");
    }
    printf(" ");
}

void escribeMemoria(cuatroBytes bytesMemoria, int esV2){
    dosBytes offset = (bytesMemoria&0x0FFFF); //16 Bits del offset
    unByte codReg = ((bytesMemoria>>16)&0x1F);
    unByte registro = codReg;//(codReg&0x1F);
    unByte tamCelda = (bytesMemoria>>22)&0x3;
    int espacioOcupadoOffset;
    unByte offsetNEGATIVO = 0;

    if(!esV2 && tamCelda != 0)
        errorDissasembler("Error: Tamaño de celda invalido para esta version");

    // 2 por el tamanio [] y 1 por el operando de memoria (w, b)
    int espacioOcupadoAdicional = 2 + ((esV2 && tamCelda != 0)? 1 : 0);
    // 3 por el tamanio del registro
    espacioOcupadoAdicional += (registro != 0x1B)? 3 : 0;
    // agregar el " + ", " - "
    espacioOcupadoAdicional += (offset != 0 && registro != 0x1B)? 3 : 0;
    // Si el offset es 0, luego longitudNum(0) = 1. Por eso el if
    if(offset == 0 && registro != 0x1B)
        espacioOcupadoOffset = 0;
    else
        espacioOcupadoOffset = longitudNum(offset);

    identar(IDENTACION_MAX - espacioOcupadoOffset - espacioOcupadoAdicional);

    if(esV2 && tamCelda != 0)
        switch(tamCelda){
            case(0x1):  printf("l"); break;
            case(0x2):  printf("w"); break;
            case(0x3):  printf("b"); break;
            default:    errorDissasembler("Error: Tamaño celda invalido\n");
        }

    if(registro == 0x1B)
        printf("[%d", offset); //Es el DS, por lo tanto escribe: [offset]
    else {
        printf("[");
        escribeRegistro(codReg, esV2);

        if(offset&0x08000){
            offsetNEGATIVO = 1;
            offset = 0xFFFF0000 | offset;
            offset = -offset;
        }


        // Se agrega el offset si se puede
        if(offset != 0)
            printf("%s%d", !offsetNEGATIVO? " + ": " - ", offset);
    }
    printf("]");
}

// Decodifica las operaciones A y B.
// op es un numero binario con 2 bits [00, 01, 10, 11]
void escribeOperando(unByte op, int ir, unByte memoria[], int esV2){
    dosBytes inmediato;
    cuatroBytes bytesMemoria;

    switch(op&0xF){
        // (00) Ninguno
        case(0x0): break;
        // (01) Registro
        case(0x1):  identar(IDENTACION_MAX - 3);
                    //printf("Entre al case bitch \n");
                    //printf("memir: %2x\n", memoria[ir]);
                    escribeRegistro(memoria[ir], esV2);
                    break;
        // (10) Inmediato
        case(0x2):  inmediato = ((memoria[ir]&0xFF) << 8) + (memoria[ir + 1]&0xFF);
                    identar(IDENTACION_MAX - longitudNum(inmediato));
                    printf("%d",inmediato);
                    break;
        // (11) Memoria
        case(0x3):  bytesMemoria = (memoria[ir] << 16) + (memoria[ir + 1] << 8);
                    bytesMemoria += (memoria[ir + 2]&0xFF);
                    escribeMemoria(bytesMemoria, esV2);
                    break;
    }
}

// inicio tendrá, si es posible, la dirección de inicio de KS; de lo contrario, su valor será inicioCS.
void disassembler(dosBytes inicio, dosBytes inicioCS, dosBytes longCS, dosBytes entryPoint, int esV2, unByte memoria[MAXMEMORIA]){
    unByte bitsA, bitsB, codOp;
    int i, nroBytes, cantBytesLeidos;
    //printf("inicio: %d\n", inicio);
    int ir = inicio>-1 ? inicio : inicioCS;
    //printf("ir: %d\n", ir);
    printf("\n\n");
    //printf("Dissasembler 221\n");

    while(ir < inicioCS){
        printf(" [%04X]", ir);
        i = ir;
        cantBytesLeidos = 0;

        // Escribir en hexadecimal los char
        while(i < inicioCS && memoria[i] != 0 && cantBytesLeidos < 7){
            printf("%02X ",memoria[i]&0xFF);
            cantBytesLeidos++;
            i++;
        }

        if(memoria[i] == 0)
            printf("00 ");
        else
            if(cantBytesLeidos == 7)
                printf(".. ");

        for(i = 0; i < 7 - cantBytesLeidos; i++)
            printf("   ");

        printf("\t| \"");

        // Escribir los caracteres
        while(ir < inicioCS && memoria[ir] != 0){
            printf("%c", memoria[ir]);
            ir++;
        }
        printf("\"\n");
        ir++;
    }

    ir = inicioCS;

    // empieza la diversion

    while(ir < inicioCS+longCS){
        printf(ir==inicioCS+entryPoint ? ">" : " ");

        // Direccion memoria fisica, y el codigo de operacion
        printf("[%04X] %02X ", ir, memoria[ir]&0xFF);

        // Decodifica la instruccion
        bitsB = (memoria[ir] >> 6)&0x3;
        bitsA = (memoria[ir] >> 4)&0x3;
        codOp = memoria[ir]&0x1F;
        ir++;

        nroBytes = bitsB + bitsA;

        // Escribe las instrucciones en hexadecimal
        for(i = ir; i < ir + nroBytes; i++)
            printf("%02X ",memoria[i]&0xFF);

        // Identa los espacios restantes respecto al maximo total: (memoria + memoria) = 6 bytes
        for(i = 0; i < 8 - nroBytes; i++)
            printf("   ");

        printf("\t| ");

        // Escribe las instrucciones con letras
        escribeInstruccion(codOp, esV2);
        escribeOperando(bitsA, ir + bitsB, memoria, esV2);

        if(bitsA != 0)
            printf(", ");

        escribeOperando(bitsB, ir, memoria, esV2);
        printf("\n");

        // Incrementa la posicion de la memoria fisica
        ir += nroBytes;
    }
    printf("\n\n");
}






