//include <tiposyctes.h>

void sysRead(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
    int cantCeldas = registros[12]&0xFFFF,
        tamanyoCeldas = (registros[12]>>16)&0xFFFF;

    //posicion inicial indicada en EDX
    long baseEdx = (registros[13]&0xFFFF0000)>>16;
    long offsetEdx = (registros[13]&0x0000FFFF);

    long indiceMemoria = tabla[baseEdx][0]+offsetEdx;
    long xxxx = indiceMemoria;
    unByte i,j;
    cuatroBytes datoEdx;
    char datoEdxStr[100];
    int formato;

    for(i=0;i<cantCeldas;i++) {
        formato = registros[10]&0x001F;
        printf("[%04X]: ",(unsigned int)xxxx+tamanyoCeldas*i);
        // Decimal
        if (formato&0x01)
            scanf("%d",&datoEdx);
        else {
            formato = formato>>1;
            // Caracteres
            if (formato&0x01) {
                datoEdx = 0;
                printf("'");
                fgets(datoEdxStr,tamanyoCeldas+1,stdin);
                // Elimina el salto de línea si existe
                if (strchr(datoEdxStr,'\n'))
                    datoEdxStr[strcspn(datoEdxStr, "\n")] = '\0';
                else
                    // Si no había '\n', limpia el buffer para evitar problemas
                    while (getchar() != '\n')
                        for(unByte k=0;k<tamanyoCeldas;k++) {
                            datoEdx = datoEdx<<8;
                            datoEdx += (unsigned char)datoEdxStr[k];
                        }
            } else {
                // Octal
                formato = formato>>1;
                if (formato&0x01) {
                    printf("0o");
                    scanf("%o",&datoEdx);
                } else {
                    // Hexadecimal
                    formato = formato>>1;
                    if (formato&0x01) {
                        printf("0x");
                        scanf("%X",&datoEdx);
                    } else {
                        // Binario
                        formato = formato>>1;
                        if (formato&0x01) {
                            printf("0b");
                            scanf("%s",datoEdxStr);
                            datoEdx = strtol(datoEdxStr,NULL,2);
                        }
                    }
                }
            }
        }

        for(j=0;j<tamanyoCeldas;j++){
            //verificarDS(indiceMemoria+tamanyoCeldas*i+j,registros,tabla);
            verificarSegmento(13,indiceMemoria+tamanyoCeldas*i+j,registros,tabla);
            memoria[indiceMemoria+tamanyoCeldas*i+j] = datoEdx>>(8*(tamanyoCeldas-j-1));
        }
    }
}

void sysWrite(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
    int cantCeldas = registros[12]&0xFFFF,
        tamanyoCeldas = (registros[12]>>16)&0xFFFF;

    //printf("EDX: %8x\n",registros[12]);

    //posicion inicial indicada en EDX
    long baseEdx = (registros[13]&0xFFFF0000)>>16;
    long offsetEdx = (registros[13]&0x0000FFFF);

    long indiceMemoria = tabla[baseEdx][0]+offsetEdx;
    long xxxx = indiceMemoria;
    unByte i,j;
    cuatroBytes datoEdx;
    int formato;

    for (i=0;i<cantCeldas;i++){

            verificarSegmento(13,indiceMemoria+tamanyoCeldas*i,registros,tabla);

            //printf("indMem: %2x\n",indiceMemoria+tamanyoCeldas*i);
            datoEdx = (memoria[indiceMemoria+tamanyoCeldas*i]&0x80) ? -1 : 0;
            //printf("tamanio celdas: %d\n",tamanyoCeldas);
            for(j=0;j<tamanyoCeldas;j++){
                datoEdx = datoEdx<<8;
                datoEdx += (unsigned char)memoria[indiceMemoria+tamanyoCeldas*i+j];
            }

            formato = registros[10]&0x001F;
            printf("[%04X]:",(unsigned int)xxxx+4*i);
            // Binario
            if (formato&0x10) {
                printf(" "); escribirBinario(datoEdx,tamanyoCeldas);
            }
            formato = formato<<1;
            // Hexadecimal
            if (formato&0x10)
                printf(" 0x%X",datoEdx);
            formato = formato<<1;
            // Octal
            if (formato&0x10)
                printf(" 0o%o",datoEdx);
            formato = formato<<1;
            // Caracteres
            if (formato&0x10) {
                char auxChar;
                printf(" ");
                for(j=0;j<tamanyoCeldas;j++) {
                    auxChar = (char)memoria[indiceMemoria+tamanyoCeldas*i+j];
                    printf("%c",(auxChar>=32 && auxChar<=126) ? auxChar : '.');
                }
            }
            formato = formato<<1;
            // Decimal
            if (formato&0x10)
                printf(" %d",datoEdx);
            printf("\n");
        }
}

void sysStringRead(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
    int maxChars = registros[0xC]&0xFFFF;
    if (maxChars == 0xFFFF)
        maxChars = MAXMEMORIA;//tabla[registros[0xC]>>16][1];
    long baseEdx = (registros[13]&0xFFFF0000)>>16;
    long offsetEdx = (registros[13]&0x0000FFFF);
    cuatroBytes indiceMemoria = tabla[baseEdx][0]+offsetEdx;
    unByte i=0;
    char str[maxChars];

    //printf("[%04X]: ",indiceMemoria);
    fgets(str,maxChars+1,stdin);
    while (i<maxChars && i<MAXMEMORIA && str[i]!='\n'){
        memoria[indiceMemoria+i] = str[i];
        i++;
    }
    memoria[indiceMemoria+i] = 0;
}

void sysStringWrite(cuatroBytes registros[CANTREGISTROS], unByte memoria[MAXMEMORIA], dosBytes tabla[MAXSEGMENTOS][2]) {
    long baseEdx = (registros[13]&0xFFFF0000)>>16;
    long offsetEdx = (registros[13]&0x0000FFFF);
    cuatroBytes i = tabla[baseEdx][0]+offsetEdx;

    //printf("[%04X]: ",i);
    while(memoria[i]!=0){
        printf("%c",memoria[i]);
        i++;
    }
    printf("\n");
}

void clearscreen() {
    printf("\033[2J");
    printf("\033[H");
}

void breakpoint(char* filename, dosBytes tamanyoMemoria, arregloFunciones *ops, cuatroBytes registros[CANTREGISTROS], unByte *memoria, dosBytes tabla[MAXSEGMENTOS][2]) {
    FILE *f;
    f = fopen(filename,"wb");
    if (f==NULL){
        printf("Error al abrir el vmi.\n");
        exit(1);
    }

    unByte version = 1;
    fwrite("VMX25",1,5,f);
    fwrite(&version,1,1,f);
    tamanyoMemoria = tamanyoMemoria>>10;
    fwrite(&tamanyoMemoria,2,1,f);
    cuatroBytes i;
    for(i=0;i<CANTREGISTROS;i++)
        fwrite(&(registros[26]),1,4,f);
    for(i=0;i<MAXSEGMENTOS;i++){
        fwrite(&(tabla[i][0]),1,2,f);
        fwrite(&(tabla[i][1]),1,2,f);
    }
    for(i=0;i<(tamanyoMemoria<<10);i++)
        fwrite(&(memoria[i]),1,1,f);

    char c;
    do
        scanf("%c",&c);
    while (c!='g' || c !='q' || c!='\n');

    if (c=='\n')
        unaInstruccion(ops,registros,memoria,tabla);
    else
    if (c=='q'){
        printf("Se hizo un breakpoint y se apretó q de quit.\n");
        exit(1);
    }
}




