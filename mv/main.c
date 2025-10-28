#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "tiposyctes.h"
#include "verificaciones.h"
#include "utiles.h"
#include "mainutiles.h"
#include "funciones.h"
#include "disassembler.h"

int main(int argc, char *argv[]) {
    srand(time(NULL));
    // Verifica que se pasaron los parámetros adecuados
    if (argc<2) {
        printf("Uso (v1): vmx <archivo_origen.vmx> [-d]\n");
    	printf("Uso (v2): vmx <archivo_origen.vmx> <archivo_imagen.vmx> [m=M] [-d] [-p param1 param2 ... paramN]\n");
    	return 1;
	}
	// Empieza el programa

    unByte hayVmi, hayVmx;
    unByte v1 = 0, v2=1;

    int contArg=1;
    cuatroBytes tamanyoMemoria = 16384;

    cuatroBytes registros[CANTREGISTROS];
	unByte memoria[MAXMEMORIA];
	dosBytes tabla[MAXSEGMENTOS][2];
    dosBytes entryPoint = 0;

	dosBytes longCS;
    unByte hayD, hayM, hayP;

    hayVmx = hayVmi = 0;
    hayM = hayD = hayP = 0;

    FILE *vmx;
    // vmx
    if(contArg<argc && strcmp(argv[contArg]+strlen(argv[contArg])-4,".vmx")==0){
        vmx = fopen(argv[contArg],"rb");
       if (vmx == NULL) {
           printf("Error al abrir el vmx.\n");
           exit(1);
       }
       contArg++;
       hayVmx = 1;
    }

    FILE *vmi;
    // vmi
    if(contArg<argc && strcmp(argv[contArg]+strlen(argv[contArg])-4,".vmi")==0){
        vmi = fopen(argv[contArg],"rb");
       if (vmi == NULL) {
           printf("Error al abrir el vmi.\n");
           exit(1);
       }
       hayVmi = contArg;
       contArg++;
    }

    if (!hayVmx && !hayVmi) {
        printf("Error: no se recibieron vmx ni vmi.\n");
        exit(1);
    }

    // -d
    if(contArg<argc && strcmp(argv[contArg],"-d")==0){
        contArg++;
        hayD = 1;
    }

    // m
    if(contArg<argc && strncmp(argv[contArg],".vmx",2)==0){
        tamanyoMemoria = atoi(argv[contArg]+2)*1024;
        contArg++;
        hayM = 1;
    }

    // -p
    if(contArg<argc && strcmp(argv[contArg],"-p")==0){
        contArg++;
        hayP = 1;
    }
    if(contArg==argc && hayP) {
        printf("Error: hay flag p y no parámetros.\n");
        exit(1);
    }

    if(contArg<argc && !hayP){
        printf("Error: exceso de parámetros.\n");
        printf("Uso (v1): vmx <archivo_origen.vmx> [-d]\n");
    	printf("Uso (v2): vmx <archivo_origen.vmx> <archivo_imagen.vmx> [m=M] [-d] [-p param1 param2 ... paramN]\n");
        exit(1);
    }
    if (hayVmx == 1 && hayVmi == 0 &&
        hayM == 0 && hayP == 0)
        v1 = 1;
    // Inicializar la VM en casos de que...

    if (hayVmx){// ...haya vmx
        // Leer header
        unByte identificador[LONGIDENTIFICADOR];
        fread(&identificador,sizeof(identificador),1,vmx);
        if (memcmp(identificador,"VMX25",LONGIDENTIFICADOR)) {
            printf("Error: Identificador no deseado.\n");
            exit(1);
        }
        // Leer versión

        unByte version;
        fread(&version,sizeof(version),1,vmx);

        switch (version) {
            case 1: if (v1)
                        v2 = 0;
                    else {
                        printf("Error: Parámetros equivocados para la versión 1.\n");
                        exit(1);
                    } break;
            case 2: v1 = 0; break;
            default: printf("Error: Versión %d no soportada.\n", version);
                    exit(1);
        }

        longCS = getLong(vmx,2);
        if (longCS==0){
            printf("Error: CS con espacio 0.\n");
            exit(1);
        }

        //para la v1
        if (v1) {
            dosBytes i;
            if (longCS>tamanyoMemoria){
                printf("Error: el CS no cabe en la memoria.\n");
                exit(1);
            }
            tabla[0][0] = 0;
            tabla[0][1] = longCS; //tamaño del codigo (code segment)
            tabla[1][0] = longCS; //tamaño del codigo (code segment)
            tabla[1][1] = tamanyoMemoria - tabla[0][1]; //16kib - tamaño del codigo

            registros[0] = 0;
            registros[1] = 0;
            registros [2] = 0;

            registros[26] = 0; //CS -> 00 00 00 00
            registros[27] = 0x00010000; //DS -> 00 01 00 00

            registros[3] = registros[26]; //IP
            registros[4] = 0;//OPC
            registros[5] = 0;//OP1
            registros[6] = 0;//OP2
            registros[7] = registros[8] = registros[9] = -1;

            for(i=10;i<18;i++)
                registros[i] = 0;

            for(i=18;i<26;i++)
                registros[i] = -1;

            for(i=28;i<32;i++)
                registros[i] = -1;

            for(i=0;i<longCS;i++)
                memoria[i] = getLong(vmx,1);
        }else {
        //PARA LA V2
        // Para la v2

            dosBytes i,j;
            unByte indiceSegmento = 0;
            dosBytes longDS = getLong(vmx,2), //longCS calculada antes
                longES = getLong(vmx,2),
                longSS = getLong(vmx,2),
                longKS = getLong(vmx,2);
            entryPoint = getLong(vmx,2);
            cuatroBytes posMem = 0,
                        primerOffset = -1, // para el argv al fondo de la pila
                        memDisponible = tamanyoMemoria-longCS-longDS-longES-longSS-longKS;

            if(memDisponible<0){
                printf("Error: memoria insuficiente para todos los segmentos.\n");
                exit(1);
            }
            // Crea Param Segment (si se puede)
            if (hayP) {
                cuatroBytes offsets[argc-contArg];
                // Pone los string
                for(i=contArg;i<argc;i++){
                    for(j=0;j<strlen(argv[i]);j++) {
                        verificarAlmacenamientoPS(memDisponible,posMem);
                        memoria[posMem] = argv[i][j];
                        posMem++;
                    }
                    verificarAlmacenamientoPS(memDisponible,posMem);
                    memoria[posMem] = 0;
                    offsets[i-contArg] = posMem - strlen(argv[i]);
                    posMem++;
                }
                // Pone los punteros a cada string
                if (argc-contArg>0)
                    primerOffset = posMem;
                for(i=0;i<(argc-contArg);i++)
                    for(j=0;j<4;j++) {
                        verificarAlmacenamientoPS(memDisponible,posMem);
                        memoria[posMem] = offsets[i]>>(8*(4-j-1));
                        posMem++;
                    }
                tabla[0][0] = 0;
                tabla[0][1] = posMem;
                indiceSegmento++;
            }
            // Configura (crea, pone en tabla, en registros) los distintos segmentos
            ponerSegmentoEnTabla(30,posMem,&indiceSegmento,longKS,registros,tabla);
            ponerSegmentoEnTabla(26,posMem+longKS,&indiceSegmento,longCS,registros,tabla);

            for(i=0;i<longCS;i++)
                memoria[posMem+longKS+i] = getLong(vmx,1);

            for(i=0;i<longKS;i++)
                memoria[posMem+i] = getLong(vmx,1);

            posMem += longKS;
            posMem += longCS;

            ponerSegmentoEnTabla(1,posMem,&indiceSegmento,longDS,registros,tabla);
            posMem += longDS;
            ponerSegmentoEnTabla(2,posMem,&indiceSegmento,longES,registros,tabla);
            posMem += longES;
            ponerSegmentoEnTabla(3,posMem,&indiceSegmento,longSS,registros,tabla);
            posMem += longSS;
            registros[3] = registros[26] + entryPoint;

            cuatroBytes aux;
            // Pone puntero a argV
            verificarStackOverflow(posMem-4,registros,tabla);
            for(i=0;i<4;i++){
                memoria[posMem-1-i] = primerOffset;
                primerOffset = primerOffset>>8;
            }
            posMem -= 4;
            // Pone argc
            verificarStackOverflow(posMem-4,registros,tabla);
            aux = argc-contArg;
            for(i=0;i<4;i++){
                memoria[posMem-1-i] = aux;
                aux = aux>>8;
            }
            posMem -= 4;
            // Pone ret = -1
            verificarStackOverflow(posMem-4,registros,tabla);
            for(i=0;i<4;i++)
                memoria[posMem-1-i] = -1;
            posMem -= 4;
            registros[7] = registros[29] + tabla[registros[29]>>16][1] - 12;
        }
        fclose(vmx);
    }

    else { // ...haya sólo vmi
        v1 = 0;
        // Leer header
        unByte identificador[LONGIDENTIFICADOR];
        fread(&identificador,sizeof(identificador),1,vmi);
        if (memcmp(identificador,"VMI25",LONGIDENTIFICADOR)) {
            printf("Error: Identificador no deseado.\n");
            exit(1);
        }
        // Leer versión
        unByte version;
        fread(&version,sizeof(version),1,vmi);
        if (version != 1) {
            printf("Error: Identificador no deseado.\n");
            exit(1);
        }
        // Leer memoria
        tamanyoMemoria = getLong(vmi,2)*1024;
        // Leer registros
        unByte i;
        for(i=0;i<CANTREGISTROS;i++)
            registros[i] = getLong(vmi,4);
        // Leer tabla
        for(i=0;i<8;i++) {
            tabla[i][0] = getLong(vmi,2);
            tabla[i][1] = getLong(vmi,2);
        }
        // Leer memoria
        printf("%d\n",tamanyoMemoria);

        for(int i=0;i<tabla[3][0]+tabla[3][1];i++) {
            memoria[i] = getLong(vmi,1);
            //printf("%3d - %2x\n",i,memoria[i]);
        }

        longCS = tabla[registros[26]>>16][1];
        printf("longCS %d\n",longCS);
    }

    //for (int k=0;k<50;k++)
        //printf("%x\n",memoria[k]);

    //printf("Llegue aca2\n");
    //printf("D?: %d\n", hayD);
    if (hayD){
        //printf("Inicio: %d\n",tabla[registros[30]>>16][0]);
        //printf("InicioCs: %d\n",tabla[registros[26]>>16][0]);
        //printf("Longcs: %d\n",longCS);
        //printf("Entrypoint: %d\n",entryPoint);
        disassembler(tabla[registros[30]>>16][0],tabla[registros[26]>>16][0],longCS,entryPoint,v2,memoria);
    }
    // Declara variables
    //unByte v2=0;
    arregloFunciones ops[] = {SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN,
    	NOT, FEXC, FEXC, (v2) ? PUSH : FEXC, (v2) ? POP : FEXC, (v2) ? CALL : FEXC, (v2) ? RET : FEXC, STOP,
    	MOV, ADD, SUB, MUL, DIV, CMP, SHL,
    	SHR, SAR, AND, OR, XOR, SWAP, LDL, LDH, RND};
	// IPea
	cuatroBytes maxIP = registros[26]+tabla[registros[26]>>16][1];

    //printf("Tabla\n");
	//for(int iii=0;iii<MAXSEGMENTOS;iii++)
    //    printf("%d | %d - %d\n",iii,tabla[iii][0], tabla[iii][1]);


    while(registros[26]<=registros[3] && registros[3]<=maxIP) { // while el valor de IP sea menor al tamaño del CS
          //for (int reg=0; reg<32; reg++)
           // printf("%2d - %8x\n", reg, registros[reg]);
        //printf("%2x \n", registros[3]);
        //for (int mem=0; mem<10; mem++)
        //    printf("%2d - %8x\n", mem, memoria[tabla[1][0]+mem]);
        //for (int mem=39; mem<60; mem++)
        //    printf("%2d - %8x\n", mem, memoria[tabla[1][0]+mem]);

        unaInstruccion(ops,registros,memoria,tabla);

        if (!(registros[4]&0x1F) && registros[6] == 0xF)
            breakpoint(argv[contArg],tamanyoMemoria,ops,registros,memoria,tabla);

    }

	return 0;
}


