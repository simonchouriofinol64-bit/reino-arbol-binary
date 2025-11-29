# reino-arbol-binary

proyecto final de arbol binario para sistema de sucesión de un rey

# sistema de sucesion de un reino

este proyecto implementa un sistema simple de sucesion de reyes usando un arbol binario en c++.

## compilacion

```bash
g++ src/main.cpp -o bin/reino

## formato del archivo csv

el archivo bin/family.csv tiene las columnas:

id,name,last_name,gender,age,id_father,is_dead,was_king,is_king

## uso del menu

al ejecutar el programa se muestra un menu con las opciones:

1. mostrar linea de sucesion
2. marcar muerte del rey actual
3. marcar que el rey actual pasa de 70 años
4. editar datos de una persona
5. mostrar el arbol simple
6. mostrar el rey actual
7. salir