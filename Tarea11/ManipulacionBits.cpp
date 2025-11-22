#include <stdio.h>
#include <ctype.h> // para isprint() y tratar caracteres imprimibles
 
// Mensaje original a encriptar/descencriptar (arreglo de chars terminado en '\0')
char texto[] = "hola";
// Clave usada para la operación XOR (clave simple de un byte)
char clave_m = 45;

// Prototipos: declarar antes de su uso para C/C++
void muestra_mensaje(char *mensaje, int tam);
void imprime_como_string_seguro(char *mensaje, int tam);
 
// Encripta/Desencripta el buffer usando XOR con la clave.
// Imprime resultados parciales tras procesar cada byte.
void encripta(char *mensaje, char clave, int tam)
{
    int i;
    for(i=0; i<tam; i++)
    {
        // Antes de la operación: obtener el byte como unsigned para mostrar correctamente
        unsigned char antes = (unsigned char)mensaje[i];
        unsigned char despues = antes ^ (unsigned char)clave; // resultado del XOR

        // Aplicar el cambio al buffer
        mensaje[i] = (char)despues;

        // Mostrar información parcial de esta operación
        printf("  i=%d: antes=0x%02X ('%c') -> despues=0x%02X ('%c')\n",
               i,
               (unsigned int)antes,
               isprint(antes) ? antes : '.',
               (unsigned int)despues,
               isprint(despues) ? despues : '.');

        // Mostrar el estado parcial del mensaje en bytes tras procesar este byte
        printf("  estado parcial en bytes: ");
        muestra_mensaje(mensaje, tam);
    }
}
 
void desencripta(char *mensaje, char clave, int tam)
{
    encripta(mensaje, clave, tam);
}
 
void muestra_mensaje(char *mensaje, int tam)
{
    int i;
    for(i=0; i<tam; i++)
    {
        // Mostrar cada byte en formato hexadecimal (usar unsigned char para evitar signo)
        printf("0x%02X ", (unsigned char)mensaje[i]);
    }
    printf("\n");
}

// Imprime el buffer como una cadena segura: caracteres imprimibles se muestran,
// los no imprimibles se reemplazan por '.' para evitar cortar la salida.
void imprime_como_string_seguro(char *mensaje, int tam)
{
    int i;
    for(i=0; i<tam; i++)
    {
        unsigned char c = (unsigned char)mensaje[i];
        putchar(isprint(c) ? c : '.');
    }
    putchar('\n');
}
 
int main(void)
{
    // Mostrar mensaje original como cadena
    printf("Mensaje original: ");
    imprime_como_string_seguro(texto, 4);
    printf("En bytes: ");
    muestra_mensaje(texto, 4);
     
    encripta(texto,clave_m,4);
    // Mostrar resultado final encriptado de forma segura
    printf("Encriptado: ");
    imprime_como_string_seguro(texto, 4);
    printf("Encriptado en bytes: ");
    muestra_mensaje(texto, 4);
     
    desencripta(texto,clave_m,4);
    // Mostrar resultado final desencriptado de forma segura
    printf("Desencriptado: ");
    imprime_como_string_seguro(texto, 4);
    printf("Desencriptado en bytes: ");
    muestra_mensaje(texto, 4);
    
    // Espera para que la ventana no se cierre inmediatamente (presiona Enter)
    printf("Presiona Enter para salir...\n");
    fflush(stdout);
    getchar();

    return 0;
}