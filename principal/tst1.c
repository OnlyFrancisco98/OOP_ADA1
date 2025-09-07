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
int agregarCiudadano(Adulto ***adultos, int *totalAdultos, const char *curp, const char *primerNombre, 
                    const char *segundoNombre, const char *apellidoPaterno, const char *apellidoMaterno,
                    const char *calle, const char *numero, const char *colonia, unsigned char distrito,
                    const char *telefono, const char *email);
int validarFormatoCURP(const char *curp);
int calcularEdad(const char *curp);
int esMayusculaLetraODigito(char c);
void convertirAMayusculas(char *str);
int adultoExiste(Adulto **adultos, int totalAdultos, const char *curp);
int validarTelefono(const char *telefono);
int validarDistrito(unsigned char distrito);
void liberarMemoria(Adulto **adultos, int totalAdultos);
void mostrarRegistros(Adulto **adultos, int totalAdultos);
void guardarEnArchivo(Adulto **adultos, int totalAdultos);

// Función principal
int main() {
    Adulto **adultos = NULL;
    int totalAdultos = 0;
    int resultado;
    
    printf("=== SISTEMA DE REGISTRO DE CIUDADANOS ===\n\n");
    
    // Adulto 1 - Válido
    resultado = agregarCiudadano(&adultos, &totalAdultos, 
        "GOME800101HDFTRR01", "Juan", "Carlos", "Gomez", "Perez",
        "Calle Principal", "123", "Centro", 5, "5512345678", "juan@email.com");    
    // Adulto 2 - Válido
    resultado = agregarCiudadano(&adultos, &totalAdultos, 
        "RODR900215MDFMSS02", "Maria", "Elena", "Rodriguez", "Sanchez",
        "Avenida Siempre Viva", "456", "Reforma", 3, "5512345679", "maria@email.com");
    
    // Adulto 3 - Válido
    resultado = agregarCiudadano(&adultos, &totalAdultos, 
        "LOPA950630MDFTRR03", "Laura", "Isabel", "Lopez", "Torres",
        "Calle Secundaria", "789", "Juarez", 7, "5512345680", "laura@email.com");

    mostrarRegistros(adultos, totalAdultos);
    
    // Intentar registrar un CURP duplicado 
    printf("\nIntentando registrar CURP duplicado\n");
    resultado = agregarCiudadano(&adultos, &totalAdultos, 
        "GOME800101HDFTRR01", "Juan", "Carlos", "Gomez", "Perez",
        "Otra Calle", "999", "Nueva Colonia", 2, "5598765432", "juan2@email.com");

    mostrarRegistros(adultos, totalAdultos);
    
    // Intentar registrar un menor de edad 
    printf("\nIntentando registrar menor de edad...\n");
    resultado = agregarCiudadano(&adultos, &totalAdultos, 
        "MART120610HDFTRR04", "Pedro", "Luis", "Martinez", "Garcia",
        "Calle Nueva", "321", "Norte", 4, "5512345681", "pedro@email.com");
    
    // Mostrar registros 
    mostrarRegistros(adultos, totalAdultos);
    
    // Intentar registrar con CURP incorrecto (debe fallar)
    printf("\nIntentando registrar con CURP incorrecto...\n");
    resultado = agregarCiudadano(&adultos, &totalAdultos, 
        "curpitoFake234", "Pepita", "Juanita", "Lopez", "Senorita",
        "Calle Falsa", "123", "Sur", 6, "5512345682", "ana@email.com");
    // Mostrar registros finales 
    printf("\nMostrando registros finales:\n");
    mostrarRegistros(adultos, totalAdultos);

    guardarEnArchivo(adultos, totalAdultos);
    
    // Liberar memoria<
    liberarMemoria(adultos, totalAdultos);
    
    return 0;
}

// Función para agregar ciudadano (adulto) con datos predefinidos - Retorna 1 si éxito, 0 si error
int agregarCiudadano(Adulto ***adultos, int *totalAdultos, const char *curp, const char *primerNombre, 
                    const char *segundoNombre, const char *apellidoPaterno, const char *apellidoMaterno,
                    const char *calle, const char *numero, const char *colonia, unsigned char distrito,
                    const char *telefono, const char *email) {
    
    // Validar formato de CURP
    if (!validarFormatoCURP(curp)) {
        return 0;
    }
    
    // Verificar si el adulto ya existe
    if (adultoExiste(*adultos, *totalAdultos, curp)) {
        return 0;
    }
    
    // Validar edad (debe ser mayor de 18 años)
    int edad = calcularEdad(curp);
    if (edad < 18) {
        return 0;
    }
    
    // Validar teléfono
    if (!validarTelefono(telefono)) {
        return 0;
    }
    
    // Validar distrito
    if (!validarDistrito(distrito)) {
        return 0;
    }
    
    // Asignar memoria para el nuevo adulto
    Adulto *nuevoAdulto = (Adulto*)malloc(sizeof(Adulto));
    if (nuevoAdulto == NULL) {
        return 0;
    }
    
    // Inicializar dependientes
    nuevoAdulto->numDependientes = 0;
    for (int i = 0; i < 3; i++) {
        nuevoAdulto->dependientes[i] = NULL;
    }
    
    // Copiar datos proporcionados
    strcpy(nuevoAdulto->curp, curp);
    nuevoAdulto->primerNombre = strdup(primerNombre);
    nuevoAdulto->segundoNombre = strdup(segundoNombre);
    nuevoAdulto->apellidoPaterno = strdup(apellidoPaterno);
    nuevoAdulto->apellidoMaterno = strdup(apellidoMaterno);
    
    nuevoAdulto->direccion.calle = strdup(calle);
    nuevoAdulto->direccion.numero = strdup(numero);
    nuevoAdulto->direccion.colonia = strdup(colonia);
    nuevoAdulto->direccion.distrito = distrito;
    
    strcpy(nuevoAdulto->telefono, telefono);
    nuevoAdulto->email = strdup(email);
    
    // Agregar el nuevo adulto al arreglo
    *adultos = (Adulto**)realloc(*adultos, (*totalAdultos + 1) * sizeof(Adulto*));
    (*adultos)[*totalAdultos] = nuevoAdulto;
    (*totalAdultos)++;
    
    return 1;
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

// Validar formato de teléfono (10 dígitos)
int validarTelefono(const char *telefono) {
    if (strlen(telefono) != 10) {
        return 0;
    }
    
    for (int i = 0; i < 10; i++) {
        if (!isdigit(telefono[i])) {
            return 0;
        }
    }
    
    return 1;
}

// Validar distrito (1-9)
int validarDistrito(unsigned char distrito) {
    return (distrito >= 1 && distrito <= 9);
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

// Mostrar todos los registros
void mostrarRegistros(Adulto **adultos, int totalAdultos) {
    
    if (totalAdultos == 0) {
        printf("No hay registros para mostrar.\n");
        return;
    }
    
    printf("Total de adultos registrados: %d\n", totalAdultos);
    
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
    }
}

void guardarEnArchivo(Adulto **adultos, int totalAdultos) {
    // Guardar en archivo binario (.dat)
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
    }
    
    fclose(archivo);
    
    // Guardar en archivo de texto (.txt) para fácil lectura
    FILE *archivoTexto = fopen("registro_votantes.txt", "w");
    if (archivoTexto == NULL) {
        printf("Error al crear el archivo de texto.\n");
        return;
    }
    
    fprintf(archivoTexto, "=== REGISTRO DE VOTANTES ===\n\n");
    fprintf(archivoTexto, "Total de adultos registrados: %d\n\n", totalAdultos);
    
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
        fprintf(archivoTexto, "\n");
    }
    
    fclose(archivoTexto);
}
