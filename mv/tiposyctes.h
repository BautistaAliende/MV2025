#define CANTREGISTROS 32
#define MAXMEMORIA 16384
#define MAXSEGMENTOS 2
#define LONGIDENTIFICADOR 5

typedef int8_t unByte;
typedef int16_t dosBytes;
typedef int32_t cuatroBytes;
typedef int64_t ochoBytes;

typedef void (*arregloFunciones)(cuatroBytes[CANTREGISTROS], unByte[MAXMEMORIA], dosBytes[MAXSEGMENTOS][2]);




