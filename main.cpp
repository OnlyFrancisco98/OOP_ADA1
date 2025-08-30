#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Estructuras de datos
typedef struct {
    char curp[19];
    char *primerNombre;
    char *segundoNombre;
    char *apellidoPaterno;
    char *apellidoMaterno;
} Dependiente;

typedef struct {
    char *calle;
    char *numero;
    char *colonia;
    unsigned char distrito;
} Direccion;

typedef struct {
    char curp[19];
    char *primerNombre;
    char *segundoNombre;
    char *apellidoPaterno;
    char *apellidoMaterno;
    Direccion direccion;
    char telefono[11];
    char *email;
    Dependiente* dependientes[3];
    int numDependientes;
} Adulto;

// Prototipos de funciones
int validarFormatoCURP(const char *curp);
int calcularEdad(const char *curp);
int esMayusculaLetraODigito(char c);
void convertirAMayusculas(char *str);
int adultoExiste(Adulto **adultos, int totalAdultos, const char *curp);
int dependienteExiste(Adulto *adulto, const char *curp);
void liberarMemoria(Adulto **adultos, int totalAdultos);
void guardarEnArchivo(Adulto **adultos, int totalAdultos);
int cargarDesdeArchivo(Adulto ***adultos, int *totalAdultos);
void registrarAdulto(Adulto ***adultos, int *totalAdultos);
void añadirDependiente(Adulto **adultos, int totalAdultos);
void mostrarRegistros(Adulto **adultos, int totalAdultos);
char *leerCadenaDinamica(FILE *archivo);

// Función principal
int main() {
    Adulto **adultos = NULL;
    int totalAdultos = 0;
    int opcion;

    // Cargar datos existentes al iniciar
    if (cargarDesdeArchivo(&adultos, &totalAdultos)) {
        printf("Datos cargados correctamente. Total de adultos: %d\n", totalAdultos);
    } else {
        printf("No se encontraron datos previos o hubo un error. Iniciando con registro vacío.\n");
    }

    do {
        printf("\n=== SISTEMA DE REGISTRO DE VOTANTES ===\n");
        printf("1. Registrar adulto\n");
        printf("2. Añadir dependiente\n");
        printf("3. Mostrar registros\n");
        printf("4. Guardar y salir\n");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);
        while(getchar() != '\n'); // Limpiar buffer

        switch(opcion) {
            case 1:
                registrarAdulto(&adultos, &totalAdultos);
                guardarEnArchivo(adultos, totalAdultos); // Guardar después de cada operación
                break;
            case 2:
                if (totalAdultos > 0) {
                    añadirDependiente(adultos, totalAdultos);
                    guardarEnArchivo(adultos, totalAdultos); // Guardar después de cada operación
                } else {
                    printf("No hay adultos registrados. Registre un adulto primero.\n");
                }
                break;
            case 3:
                if (totalAdultos > 0) {
                    mostrarRegistros(adultos, totalAdultos);
                } else {
                    printf("No hay registros para mostrar.\n");
                }
                break;
            case 4:
                guardarEnArchivo(adultos, totalAdultos);
                printf("Datos guardados. Saliendo del sistema...\n");
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
    } while(opcion != 4);

    liberarMemoria(adultos, totalAdultos);
    return 0;
}

// Función para validar el formato del CURP
int validarFormatoCURP(const char *curp) {
    if (strlen(curp) != 18) return 0;
    
    // Los primeros 4 caracteres deben ser letras
    for (int i = 0; i < 4; i++) {
        if (!isalpha(curp[i])) return 0;
    }
    
    // Los siguientes 6 caracteres deben ser dígitos (fecha de nacimiento)
    for (int i = 4; i < 10; i++) {
        if (!isdigit(curp[i])) return 0;
    }
    
    // El resto debe seguir el formato estándar de CURP
    for (int i = 10; i < 18; i++) {
        if (!esMayusculaLetraODigito(curp[i])) return 0;
    }
    
    return 1;
}

// Función para calcular la edad a partir del CURP
int calcularEdad(const char *curp) {
    char anioStr[3], mesStr[3], diaStr[3];
    int anio, mes, dia;
    
    // Extraer fecha de nacimiento del CURP (posiciones 4-9: AAMMDD)
    strncpy(anioStr, curp + 4, 2);
    anioStr[2] = '\0';
    strncpy(mesStr, curp + 6, 2);
    mesStr[2] = '\0';
    strncpy(diaStr, curp + 8, 2);
    diaStr[2] = '\0';
    
    anio = atoi(anioStr);
    mes = atoi(mesStr);
    dia = atoi(diaStr);
    
    // Determinar el siglo (asumimos que si el año es mayor a 25, es del siglo 20)
    if (anio <= 25) {
        anio += 2000;
    } else {
        anio += 1900;
    }
    
    // Calcular edad respecto a la fecha de referencia (30/Agosto/2025)
    int edad = 2025 - anio;
    if (mes > 8 || (mes == 8 && dia > 30)) {
        edad--;
    }
    
    return edad;
}

// Función auxiliar para validar caracteres
int esMayusculaLetraODigito(char c) {
    return (isupper(c) || isdigit(c));
}

// Convertir string a mayúsculas
void convertirAMayusculas(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

// Verificar si un adulto ya existe
int adultoExiste(Adulto **adultos, int totalAdultos, const char *curp) {
    for (int i = 0; i < totalAdultos; i++) {
        if (strcmp(adultos[i]->curp, curp) == 0) {
            return 1;
        }
    }
    return 0;
}

// Verificar si un dependiente ya existe para un adulto
int dependienteExiste(Adulto *adulto, const char *curp) {
    for (int i = 0; i < adulto->numDependientes; i++) {
        if (strcmp(adulto->dependientes[i]->curp, curp) == 0) {
            return 1;
        }
    }
    return 0;
}

// Liberar memoria dinámica
void liberarMemoria(Adulto **adultos, int totalAdultos) {
    for (int i = 0; i < totalAdultos; i++) {
        free(adultos[i]->primerNombre);
        free(adultos[i]->segundoNombre);
        free(adultos[i]->apellidoPaterno);
        free(adultos[i]->apellidoMaterno);
        free(adultos[i]->direccion.calle);
        free(adultos[i]->direccion.numero);
        free(adultos[i]->direccion.colonia);
        free(adultos[i]->email);
        
        for (int j = 0; j < adultos[i]->numDependientes; j++) {
            free(adultos[i]->dependientes[j]->primerNombre);
            free(adultos[i]->dependientes[j]->segundoNombre);
            free(adultos[i]->dependientes[j]->apellidoPaterno);
            free(adultos[i]->dependientes[j]->apellidoMaterno);
            free(adultos[i]->dependientes[j]);
        }
        
        free(adultos[i]);
    }
    free(adultos);
}

// Función auxiliar para leer cadenas desde archivo
char *leerCadenaDinamica(FILE *archivo) {
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), archivo) == NULL) {
        return NULL;
    }
    
    // Eliminar el salto de línea
    buffer[strcspn(buffer, "\n")] = 0;
    
    return strdup(buffer);
}

// Cargar datos desde archivo
int cargarDesdeArchivo(Adulto ***adultos, int *totalAdultos) {
    FILE *archivo = fopen("registro_votantes.dat", "rb");
    if (archivo == NULL) {
        return 0; // No hay archivo previo
    }
    
    // Leer número total de adultos
    fread(totalAdultos, sizeof(int), 1, archivo);
    
    // Asignar memoria para el array de adultos
    *adultos = (Adulto**)malloc(*totalAdultos * sizeof(Adulto*));
    if (*adultos == NULL) {
        fclose(archivo);
        return 0;
    }
    
    for (int i = 0; i < *totalAdultos; i++) {
        // Asignar memoria para el adulto
        (*adultos)[i] = (Adulto*)malloc(sizeof(Adulto));
        if ((*adultos)[i] == NULL) {
            fclose(archivo);
            return 0;
        }
        
        // Leer datos del adulto
        fread((*adultos)[i]->curp, sizeof(char), 19, archivo);
        
        // Leer cadenas de longitud variable
        int len;
        fread(&len, sizeof(int), 1, archivo);
        (*adultos)[i]->primerNombre = (char*)malloc(len + 1);
        fread((*adultos)[i]->primerNombre, sizeof(char), len, archivo);
        (*adultos)[i]->primerNombre[len] = '\0';
        
        fread(&len, sizeof(int), 1, archivo);
        (*adultos)[i]->segundoNombre = (char*)malloc(len + 1);
        fread((*adultos)[i]->segundoNombre, sizeof(char), len, archivo);
        (*adultos)[i]->segundoNombre[len] = '\0';
        
        fread(&len, sizeof(int), 1, archivo);
        (*adultos)[i]->apellidoPaterno = (char*)malloc(len + 1);
        fread((*adultos)[i]->apellidoPaterno, sizeof(char), len, archivo);
        (*adultos)[i]->apellidoPaterno[len] = '\0';
        
        fread(&len, sizeof(int), 1, archivo);
        (*adultos)[i]->apellidoMaterno = (char*)malloc(len + 1);
        fread((*adultos)[i]->apellidoMaterno, sizeof(char), len, archivo);
        (*adultos)[i]->apellidoMaterno[len] = '\0';
        
        // Leer dirección
        fread(&len, sizeof(int), 1, archivo);
        (*adultos)[i]->direccion.calle = (char*)malloc(len + 1);
        fread((*adultos)[i]->direccion.calle, sizeof(char), len, archivo);
        (*adultos)[i]->direccion.calle[len] = '\0';
        
        fread(&len, sizeof(int), 1, archivo);
        (*adultos)[i]->direccion.numero = (char*)malloc(len + 1);
        fread((*adultos)[i]->direccion.numero, sizeof(char), len, archivo);
        (*adultos)[i]->direccion.numero[len] = '\0';
        
        fread(&len, sizeof(int), 1, archivo);
        (*adultos)[i]->direccion.colonia = (char*)malloc(len + 1);
        fread((*adultos)[i]->direccion.colonia, sizeof(char), len, archivo);
        (*adultos)[i]->direccion.colonia[len] = '\0';
        
        fread(&(*adultos)[i]->direccion.distrito, sizeof(unsigned char), 1, archivo);
        
        // Leer teléfono y email
        fread((*adultos)[i]->telefono, sizeof(char), 11, archivo);
        
        fread(&len, sizeof(int), 1, archivo);
        (*adultos)[i]->email = (char*)malloc(len + 1);
        fread((*adultos)[i]->email, sizeof(char), len, archivo);
        (*adultos)[i]->email[len] = '\0';
        
        // Leer número de dependientes
        fread(&(*adultos)[i]->numDependientes, sizeof(int), 1, archivo);
        
        // Leer dependientes
        for (int j = 0; j < (*adultos)[i]->numDependientes; j++) {
            (*adultos)[i]->dependientes[j] = (Dependiente*)malloc(sizeof(Dependiente));
            
            fread((*adultos)[i]->dependientes[j]->curp, sizeof(char), 19, archivo);
            
            fread(&len, sizeof(int), 1, archivo);
            (*adultos)[i]->dependientes[j]->primerNombre = (char*)malloc(len + 1);
            fread((*adultos)[i]->dependientes[j]->primerNombre, sizeof(char), len, archivo);
            (*adultos)[i]->dependientes[j]->primerNombre[len] = '\0';
            
            fread(&len, sizeof(int), 1, archivo);
            (*adultos)[i]->dependientes[j]->segundoNombre = (char*)malloc(len + 1);
            fread((*adultos)[i]->dependientes[j]->segundoNombre, sizeof(char), len, archivo);
            (*adultos)[i]->dependientes[j]->segundoNombre[len] = '\0';
            
            fread(&len, sizeof(int), 1, archivo);
            (*adultos)[i]->dependientes[j]->apellidoPaterno = (char*)malloc(len + 1);
            fread((*adultos)[i]->dependientes[j]->apellidoPaterno, sizeof(char), len, archivo);
            (*adultos)[i]->dependientes[j]->apellidoPaterno[len] = '\0';
            
            fread(&len, sizeof(int), 1, archivo);
            (*adultos)[i]->dependientes[j]->apellidoMaterno = (char*)malloc(len + 1);
            fread((*adultos)[i]->dependientes[j]->apellidoMaterno, sizeof(char), len, archivo);
            (*adultos)[i]->dependientes[j]->apellidoMaterno[len] = '\0';
        }
    }
    
    fclose(archivo);
    return 1;
}

// Guardar datos en archivo binario (para persistencia)
void guardarEnArchivo(Adulto **adultos, int totalAdultos) {
    FILE *archivo = fopen("registro_votantes.dat", "wb");
    if (archivo == NULL) {
        printf("Error al crear el archivo de datos.\n");
        return;
    }
    
    // Guardar número total de adultos
    fwrite(&totalAdultos, sizeof(int), 1, archivo);
    
    for (int i = 0; i < totalAdultos; i++) {
        // Guardar datos del adulto
        fwrite(adultos[i]->curp, sizeof(char), 19, archivo);
        
        // Guardar cadenas de longitud variable (longitud + contenido)
        int len = strlen(adultos[i]->primerNombre);
        fwrite(&len, sizeof(int), 1, archivo);
        fwrite(adultos[i]->primerNombre, sizeof(char), len, archivo);
        
        len = strlen(adultos[i]->segundoNombre);
        fwrite(&len, sizeof(int), 1, archivo);
        fwrite(adultos[i]->segundoNombre, sizeof(char), len, archivo);
        
        len = strlen(adultos[i]->apellidoPaterno);
        fwrite(&len, sizeof(int), 1, archivo);
        fwrite(adultos[i]->apellidoPaterno, sizeof(char), len, archivo);
        
        len = strlen(adultos[i]->apellidoMaterno);
        fwrite(&len, sizeof(int), 1, archivo);
        fwrite(adultos[i]->apellidoMaterno, sizeof(char), len, archivo);
        
        // Guardar dirección
        len = strlen(adultos[i]->direccion.calle);
        fwrite(&len, sizeof(int), 1, archivo);
        fwrite(adultos[i]->direccion.calle, sizeof(char), len, archivo);
        
        len = strlen(adultos[i]->direccion.numero);
        fwrite(&len, sizeof(int), 1, archivo);
        fwrite(adultos[i]->direccion.numero, sizeof(char), len, archivo);
        
        len = strlen(adultos[i]->direccion.colonia);
        fwrite(&len, sizeof(int), 1, archivo);
        fwrite(adultos[i]->direccion.colonia, sizeof(char), len, archivo);
        
        fwrite(&adultos[i]->direccion.distrito, sizeof(unsigned char), 1, archivo);
        
        // Guardar teléfono y email
        fwrite(adultos[i]->telefono, sizeof(char), 11, archivo);
        
        len = strlen(adultos[i]->email);
        fwrite(&len, sizeof(int), 1, archivo);
        fwrite(adultos[i]->email, sizeof(char), len, archivo);
        
        // Guardar número de dependientes
        fwrite(&adultos[i]->numDependientes, sizeof(int), 1, archivo);
        
        // Guardar dependientes
        for (int j = 0; j < adultos[i]->numDependientes; j++) {
            fwrite(adultos[i]->dependientes[j]->curp, sizeof(char), 19, archivo);
            
            len = strlen(adultos[i]->dependientes[j]->primerNombre);
            fwrite(&len, sizeof(int), 1, archivo);
            fwrite(adultos[i]->dependientes[j]->primerNombre, sizeof(char), len, archivo);
            
            len = strlen(adultos[i]->dependientes[j]->segundoNombre);
            fwrite(&len, sizeof(int), 1, archivo);
            fwrite(adultos[i]->dependientes[j]->segundoNombre, sizeof(char), len, archivo);
            
            len = strlen(adultos[i]->dependientes[j]->apellidoPaterno);
            fwrite(&len, sizeof(int), 1, archivo);
            fwrite(adultos[i]->dependientes[j]->apellidoPaterno, sizeof(char), len, archivo);
            
            len = strlen(adultos[i]->dependientes[j]->apellidoMaterno);
            fwrite(&len, sizeof(int), 1, archivo);
            fwrite(adultos[i]->dependientes[j]->apellidoMaterno, sizeof(char), len, archivo);
        }
    }
    
    fclose(archivo);
    
    // También guardar en archivo de texto para fácil lectura
    FILE *archivoTexto = fopen("registro_votantes.txt", "w");
    if (archivoTexto == NULL) {
        printf("Error al crear el archivo de texto.\n");
        return;
    }
    
    fprintf(archivoTexto, "=== REGISTRO DE VOTANTES Y DEPENDIENTES ===\n\n");
    
    for (int i = 0; i < totalAdultos; i++) {
        fprintf(archivoTexto, "ADULTO %d:\n", i+1);
        fprintf(archivoTexto, "  CURP: %s\n", adultos[i]->curp);
        fprintf(archivoTexto, "  Nombre: %s %s %s %s\n", 
                adultos[i]->primerNombre, 
                adultos[i]->segundoNombre,
                adultos[i]->apellidoPaterno,
                adultos[i]->apellidoMaterno);
        fprintf(archivoTexto, "  Dirección: %s %s, Col. %s, Distrito %d\n",
                adultos[i]->direccion.calle,
                adultos[i]->direccion.numero,
                adultos[i]->direccion.colonia,
                adultos[i]->direccion.distrito);
        fprintf(archivoTexto, "  Teléfono: %s\n", adultos[i]->telefono);
        fprintf(archivoTexto, "  Email: %s\n", adultos[i]->email);
        fprintf(archivoTexto, "  Dependientes: %d\n", adultos[i]->numDependientes);
        
        for (int j = 0; j < adultos[i]->numDependientes; j++) {
            fprintf(archivoTexto, "    DEPENDIENTE %d:\n", j+1);
            fprintf(archivoTexto, "      CURP: %s\n", adultos[i]->dependientes[j]->curp);
            fprintf(archivoTexto, "      Nombre: %s %s %s %s\n", 
                    adultos[i]->dependientes[j]->primerNombre, 
                    adultos[i]->dependientes[j]->segundoNombre,
                    adultos[i]->dependientes[j]->apellidoPaterno,
                    adultos[i]->dependientes[j]->apellidoMaterno);
        }
        fprintf(archivoTexto, "\n");
    }
    
    fclose(archivoTexto);
    printf("Datos guardados en 'registro_votantes.dat' y 'registro_votantes.txt'\n");
}

// Registrar un nuevo adulto
void registrarAdulto(Adulto ***adultos, int *totalAdultos) {
    char buffer[100];
    int edad;
    
    // Asignar memoria para el nuevo adulto
    Adulto *nuevoAdulto = (Adulto*)malloc(sizeof(Adulto));
    if (nuevoAdulto == NULL) {
        printf("Error de memoria.\n");
        return;
    }
    
    // Inicializar dependientes
    nuevoAdulto->numDependientes = 0;
    for (int i = 0; i < 3; i++) {
        nuevoAdulto->dependientes[i] = NULL;
    }
    
    printf("\n--- REGISTRO DE ADULTO ---\n");
    
    // Solicitar CURP
    do {
        printf("CURP (18 caracteres): ");
        scanf("%18s", nuevoAdulto->curp);
        while(getchar() != '\n'); // Limpiar buffer
        
        convertirAMayusculas(nuevoAdulto->curp);
        
        if (!validarFormatoCURP(nuevoAdulto->curp)) {
            printf("Formato de CURP inválido. Intente de nuevo.\n");
            continue;
        }
        
        if (adultoExiste(*adultos, *totalAdultos, nuevoAdulto->curp)) {
            printf("Este CURP ya está registrado. Intente con otro.\n");
            continue;
        }
        
        edad = calcularEdad(nuevoAdulto->curp);
        if (edad < 18) {
            printf("El adulto debe ser mayor de 18 años. Edad calculada: %d\n", edad);
        } else {
            printf("Edad válida: %d años\n", edad);
            break;
        }
    } while (1);
    
    // Solicitar nombres
    printf("Primer nombre: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoAdulto->primerNombre = strdup(buffer);
    
    printf("Segundo nombre: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoAdulto->segundoNombre = strdup(buffer);
    
    printf("Apellido paterno: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoAdulto->apellidoPaterno = strdup(buffer);
    
    printf("Apellido materno: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoAdulto->apellidoMaterno = strdup(buffer);
    
    // Solicitar dirección
    printf("Calle: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoAdulto->direccion.calle = strdup(buffer);
    
    printf("Número: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoAdulto->direccion.numero = strdup(buffer);
    
    printf("Colonia: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoAdulto->direccion.colonia = strdup(buffer);
    
    printf("Distrito (1-255): ");
    scanf("%hhu", &nuevoAdulto->direccion.distrito);
    while(getchar() != '\n'); // Limpiar buffer
    
    // Solicitar teléfono
    printf("Teléfono (10 dígitos): ");
    scanf("%10s", nuevoAdulto->telefono);
    while(getchar() != '\n'); // Limpiar buffer
    
    // Solicitar email
    printf("Email: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoAdulto->email = strdup(buffer);
    
    // Agregar el nuevo adulto al arreglo
    *adultos = (Adulto**)realloc(*adultos, (*totalAdultos + 1) * sizeof(Adulto*));
    (*adultos)[*totalAdultos] = nuevoAdulto;
    (*totalAdultos)++;
    
    printf("Adulto registrado exitosamente.\n");
}

// Añadir dependiente a un adulto
void añadirDependiente(Adulto **adultos, int totalAdultos) {
    char curpAdulto[19], curpDependiente[19];
    char buffer[100];
    int edad, encontrado = 0;
    Adulto *adulto = NULL;
    
    printf("\n--- AÑADIR DEPENDIENTE ---\n");
    
    // Buscar al adulto por CURP
    do {
        printf("Ingrese el CURP del adulto: ");
        scanf("%18s", curpAdulto);
        while(getchar() != '\n'); // Limpiar buffer
        
        convertirAMayusculas(curpAdulto);
        
        for (int i = 0; i < totalAdultos; i++) {
            if (strcmp(adultos[i]->curp, curpAdulto) == 0) {
                adulto = adultos[i];
                encontrado = 1;
                break;
            }
        }
        
        if (!encontrado) {
            printf("No se encontró un adulto con ese CURP. Intente de nuevo.\n");
        }
    } while (!encontrado);
    
    // Verificar que no tenga más de 3 dependientes
    if (adulto->numDependientes >= 3) {
        printf("Este adulto ya tiene el máximo de 3 dependientes.\n");
        return;
    }
    
    // Solicitar CURP del dependiente
    do {
        printf("CURP del dependiente (18 caracteres): ");
        scanf("%18s", curpDependiente);
        while(getchar() != '\n'); // Limpiar buffer
        
        convertirAMayusculas(curpDependiente);
        
        if (!validarFormatoCURP(curpDependiente)) {
            printf("Formato de CURP inválido. Intente de nuevo.\n");
            continue;
        }
        
        // Verificar si el dependiente ya existe para cualquier adulto
        for (int i = 0; i < totalAdultos; i++) {
            if (dependienteExiste(adultos[i], curpDependiente)) {
                printf("Este CURP ya está registrado como dependiente de otro adulto.\n");
                return;
            }
        }
        
        edad = calcularEdad(curpDependiente);
        if (edad < 10) {
            printf("El dependiente debe ser mayor de 10 años. Edad calculada: %d\n", edad);
            printf("No se puede registrar.\n");
            return;
        } else if (edad >= 18) {
            printf("El dependiente debe ser menor de 18 años. Edad calculada: %d\n", edad);
            printf("No se puede registrar como dependiente.\n");
            return;
        } else {
            printf("Edad válida: %d años\n", edad);
            break;
        }
    } while (1);
    
    // Crear nuevo dependiente
    Dependiente *nuevoDependiente = (Dependiente*)malloc(sizeof(Dependiente));
    if (nuevoDependiente == NULL) {
        printf("Error de memoria.\n");
        return;
    }
    
    strcpy(nuevoDependiente->curp, curpDependiente);
    
    // Solicitar nombres del dependiente
    printf("Primer nombre del dependiente: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoDependiente->primerNombre = strdup(buffer);
    
    printf("Segundo nombre del dependiente: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoDependiente->segundoNombre = strdup(buffer);
    
    printf("Apellido paterno del dependiente: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoDependiente->apellidoPaterno = strdup(buffer);
    
    printf("Apellido materno del dependiente: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    nuevoDependiente->apellidoMaterno = strdup(buffer);
    
    // Agregar el dependiente al adulto
    adulto->dependientes[adulto->numDependientes] = nuevoDependiente;
    adulto->numDependientes++;
    
    printf("Dependiente registrado exitosamente.\n");
}

// Mostrar todos los registros
void mostrarRegistros(Adulto **adultos, int totalAdultos) {
    printf("\n=== REGISTROS ACTUALES ===\n");
    
    for (int i = 0; i < totalAdultos; i++) {
        printf("\nADULTO %d:\n", i+1);
        printf("  CURP: %s\n", adultos[i]->curp);
        printf("  Nombre: %s %s %s %s\n", 
               adultos[i]->primerNombre, 
               adultos[i]->segundoNombre,
               adultos[i]->apellidoPaterno,
               adultos[i]->apellidoMaterno);
        printf("  Dirección: %s %s, Col. %s, Distrito %d\n",
               adultos[i]->direccion.calle,
               adultos[i]->direccion.numero,
               adultos[i]->direccion.colonia,
               adultos[i]->direccion.distrito);
        printf("  Teléfono: %s\n", adultos[i]->telefono);
        printf("  Email: %s\n", adultos[i]->email);
        printf("  Dependientes: %d\n", adultos[i]->numDependientes);
        
        for (int j = 0; j < adultos[i]->numDependientes; j++) {
            printf("    DEPENDIENTE %d:\n", j+1);
            printf("      CURP: %s\n", adultos[i]->dependientes[j]->curp);
            printf("      Nombre: %s %s %s %s\n", 
                   adultos[i]->dependientes[j]->primerNombre, 
                   adultos[i]->dependientes[j]->segundoNombre,
                   adultos[i]->dependientes[j]->apellidoPaterno,
                   adultos[i]->dependientes[j]->apellidoMaterno);
        }
    }
}