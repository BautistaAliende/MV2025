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
    	//printf("Uso (v2): vmx <archivo_origen.vmx> <archivo_imagen.vmx> [m=M] [-d] [-p param1 param2 ... paramN]\n");
    	return 1;
	}
	// Empieza el programa

    unByte hayVmx = 0;
    unByte v1 = 0;

   int contArg=1;
   cuatroBytes tamanyoMemoria = 16384;

   cuatroBytes registros[CANTREGISTROS];
	unByte memoria[MAXMEMORIA];
	dosBytes tabla[MAXSEGMENTOS][2];

	dosBytes longCS;
    hayD = 0;

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
    if (!hayVmx) {
        printf("Error: no se recibió vmx.\n");
        exit(1);
    }

    // -d
    if(contArg<argc && strcmp(argv[contArg],"-d")==0){
        contArg++;
        hayD = 1;
    }

    if (hayVmx == 1)
        v1 = 1;
    // Inicializar la VM en casos de que...
    if (hayVmx){
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

        if (version!=1){
            printf("La versión no es 1\n");
            Exit(1);
        }

        longCS = getLong(vmx,2);
        if (longCS==0){
            printf("Error: CS con espacio 0.\n");
            exit(1);
        }

        v1=1;

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
        }
        fclose(vmx);
    }

    if (hayD)
        disassembler(0,0,longCS,0,0,memoria);
    // Declara variables
    unByte v2=0;
    arregloFunciones ops[] = {
    	SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN,
    	NOT, FEXC, FEXC, (v2) ? PUSH : FEXC, (v2) ? POP : FEXC, (v2) ? CALL : FEXC, (v2) ? RET : FEXC, STOP,
    	MOV, ADD, SUB, MUL, DIV, CMP, SHL,
    	SHR, SAR, AND, OR, XOR, SWAP, LDL, LDH, RND
	};
	// IPea
	cuatroBytes opA, opB;
	cuatroBytes maxIP = registros[26]+tabla[registros[26]>>16][1];

    while(registros[26]<=registros[3] && registros[3]<=maxIP) { // while el valor de IP sea menor al tamaño del CS
        unaInstruccion(ops,registros,memoria,tabla);

    }
	return 0;
}

