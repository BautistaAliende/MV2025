void unaInstruccion(arregloFunciones *ops, cuatroBytes registros[CANTREGISTROS], unByte *memoria, dosBytes tabla[MAXSEGMENTOS][2]) {
    dosBytes i;
    cuatroBytes opAux;

    //verificarCS(registros[5],registros,tabla);
    verificarSegmento(0,indiceDeMemoria(0,registros[3],tabla),registros,tabla);

    registros[4] = memoria[tabla[registros[3]>>16][0] + (registros[3]&0xFFFF)];
    //printf("%x\n",tabla[registros[5]>>16][0] + (registros[5]&0xFFFF));
    //printf("%x\n",tabla[registros[13]>>16][0] + (registros[13]&0x0000FFFF));

    //printf("%3x\n",(unsigned int)registros[5]);
    (registros[3])++;
    // Lee operando B
    opAux = (registros[4])&0xC0;  // Guarda tipo de operando.
    opAux = opAux<<24;
    registros[6] = 0;
    for(i=0;i<(((registros[4])>>6)&0x03);i++) {
        //verificarCS(registros[5],registros,tabla);
        verificarSegmento(0,indiceDeMemoria(0,registros[3],tabla),registros,tabla);
        registros[6] = registros[6]<<8;
        registros[6] += memoria[tabla[registros[3]>>16][0] + (registros[3]&0xFFFF)]&0x00FF;
        (registros[3])++;
    }
    registros[6] += opAux;
    // Lee operando A
    opAux = (registros[4])&0x30;
    opAux = opAux<<26;
    registros[5] = 0;
    for(i=0;i<(((registros[4])>>4)&0x03);i++) {
        //verificarCS(registros[5],registros,tabla);
        verificarSegmento(0,indiceDeMemoria(0,registros[3],tabla),registros,tabla);
        registros[5] = registros[5]<<8;
        registros[5] += memoria[tabla[registros[3]>>16][0] + (registros[3]&0xFFFF)]&0x00FF;
        (registros[3])++;
    }
    registros[5] += opAux;
    ops[(registros[4])&0x1F](registros[5], registros[6], registros, memoria, tabla);
    /*
    printf("Registros\n");
        printf("A  %8x\n",registros[10]);
        printf("B  %8x\n",registros[11]);
        printf("C  %8x\n",registros[12]);
        printf("D  %8x\n",registros[13]);
    printf("Memoria\n");
    printf("ind %8s %8s\n","DS","ES");
    for(int y=0;y<20;y++)
        printf("%3d | %2x \t %3d | %2x\n",y,memoria[tabla[registros[1]>>16][0]+y],y,memoria[tabla[registros[2]>>16][0]+y]);
    */
}

cuatroBytes getLong(FILE *f, unByte cantBytes) {
    if (cantBytes>4){
        printf("Error: se está queriendo usar un número de más de 4 bytes.\n"); // Salta sólo por error del programador
        exit(1);
    }
    cuatroBytes r = 0;
    unsigned char aux;

    for(unByte i=0;i<cantBytes;i++){
        r = r<<8;
        fread(&aux,1,1,f);
        r+=aux;
    }

    return r;
}

void verificarAlmacenamientoPS(cuatroBytes disponible, cuatroBytes posMem) {
    if (disponible-posMem<0) {
        printf("Error: Espacio insuficiente para el Param Segment.\n");
        exit(1);
    }
}

void ponerSegmentoEnTabla(unByte id, cuatroBytes posMem, unByte *indiceSegmento, dosBytes longS, cuatroBytes registros[CANTREGISTROS], dosBytes tabla[MAXSEGMENTOS][2]){
    unsigned long ls = longS;

    if(ls<=0)
        registros[id] = -1;
    else {
        registros[id] = (*indiceSegmento)<<16;
        tabla[(*indiceSegmento)][0] = posMem;
        tabla[(*indiceSegmento)][1] = ls;
        (*indiceSegmento)++;
    }


