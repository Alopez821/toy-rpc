#include "../rpc.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct
{
    reportable_t parent;

    int *arr;       // Arreglo de enteros
    int size;       // Tamaño del arreglo
    int target;     // Valor que se está buscando
    int res;        // Índice donde se encontró el valor buscado (-1 si no se encontró)
} my_struct_t;

void *parse_parameters(void *data)
{
    const char *buf = (const char *)(data);

    my_struct_t *d = (my_struct_t *)(malloc(sizeof(my_struct_t)));

    if (d)
    {
        int size;   // Tamaño del arreglo
        sscanf(buf, "%d %d", &size, &d->target);  // Se lee el tamaño del arreglo y el valor buscado
        d->size = size;
        d->arr = (int *)malloc(size * sizeof(int));  // Se asigna memoria para el arreglo
        // Se lee el arreglo de enteros
        for (int i = 0; i < size; i++) {
            sscanf(buf + (i*2*sizeof(int)) + 2*sizeof(int), "%d", &d->arr[i]);
        }
    }

    return (void *)d;
}

void *do_work(void *data)
{
    my_struct_t *d = (my_struct_t *)(data);

    int left = 0;             // Índice izquierdo de la búsqueda binaria
    int right = d->size - 1;  // Índice derecho de la búsqueda binaria
    // Búsqueda binaria
    while (left <= right) {
        int mid = left + (right - left) / 2;   // Índice medio del rango de búsqueda
        if (d->arr[mid] == d->target) {        // Si se encuentra el valor buscado
            d->res = mid;                      // Se almacena el índice donde se encontró
            return data;
        } else if (d->arr[mid] < d->target) {  // Si el valor medio es menor que el valor buscado
            left = mid + 1;                    // Se busca en la mitad derecha del rango
        } else {                               // Si el valor medio es mayor que el valor buscado
            right = mid - 1;                   // Se busca en la mitad izquierda del rango
        }
    }
    d->res = -1;  // Si no se encontró el valor buscado, se establece el índice en -1

    return data;
}

reportable_t *report(void *data)
{
    my_struct_t *d = (my_struct_t *)(data);

    d->parent.data = (char *)(malloc(255 * sizeof(char)));

    // Se genera un informe que indica si el valor buscado se encontró en el arreglo y en qué índice se encuentra
    if (d->res == -1) {
        snprintf(d->parent.data, 255, "%d not found in the array\n", d->target);
    } else {
        snprintf(d->parent.data, 255, "%d found at index %d\n", d->target, d->res);
    }
    d->parent.len = strlen(d->parent.data);

    return (reportable_t *)(data);
}

void clean_up(void *params, void *result, reportable_t *report)
{
    if (report && report->data)
    {
        free(report->data);
    }

    if (params)
    {
        my_struct_t *d = (my_struct_t *)(params);
        if (d->arr) {
            free(d->arr);   // Se libera la memoria asignada para el arreglo
        }
        free(params);       // Se libera la memoria asignada para la estructura my_struct_t
    }
}