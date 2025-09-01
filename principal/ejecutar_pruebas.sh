#!/bin/bash

# Script de pruebas automatizadas completo
echo "=== SISTEMA DE PRUEBAS AUTOMATIZADAS ==="
echo ""

# Función para mostrar separador
separador() {
    echo ""
    echo "===================================="
    echo ""
}

# Compilar el programa
echo "Compilando programa..."
gcc -o registro_votantes registro_votantes.c
if [ $? -ne 0 ]; then
    echo "Error al compilar el programa."
    exit 1
fi
echo "✓ Programa compilado exitosamente."
separador

# PRUEBA 1: Registro de adultos válidos
echo "PRUEBA 1: Registro de adultos válidos"
cat > test_input1.txt << EOF
1
GOME800101HDFTRR01
Juan
Carlos
Gomez
Perez
Calle Principal
123
Centro
5
5512345678
juan@email.com
1
RODR900215MDFMSS02
Maria
Elena
Rodriguez
Sanchez
Avenida Siempre Viva
456
Reforma
3
5512345679
maria@email.com
4
EOF

echo "Ejecutando prueba 1..."
./registro_votantes < test_input1.txt
echo "✓ Prueba 1 completada."
separador

# PRUEBA 2:  3 Registro de dependientes válidos
# Se intentan registrar un 4to CURP, pero como ya se tiene 3 registros, este debe ser rechazado
# Se intenta agregar un 2 depedientes a otro adulto
# El primer CURP es invalido para dependiente (mayor de 18 años)
# El segundo CURP tambien (menor de 10 años)
echo "PRUEBA 2: Registro de dependientes válidos"
cat > test_input2.txt << EOF
2
GOME800101HDFTRR01
MART100610HDFTRR03
Pedro
Luis
Martinez
Garcia
2
GOME800101HDFTRR01
LOPA120710MDFTRR04
Ana
Sofia
Lopez
Alvarez
2
GOME800101HDFTRR01
UXML121230MYNCXGA0
Ligia
Abigail
Uc
Mexico
2
GOME800101HDFTRR01
2
RODR900215MDFMSS02
MOUM020513HYNNCRA9
2
RODR900215MDFMSS02
UXMV230306MYNCXLA0
3
4
EOF

echo "Ejecutando prueba 2..."
./registro_votantes < test_input2.txt
echo "✓ Prueba 2 completada."
separador

# PRUEBA 3: Intentar registrar duplicados (debe fallar) 
# El primer CURP es duplicado
# El segundo CURP es un menor de 10 años
# El tercer CURP es valido
echo "PRUEBA 3: Intentar registrar duplicados"
cat > test_input3.txt << EOF
1
GOME800101HDFTRR01  
UXMV230306MYNCXLA0
MASH041123MYNYLNA6
Hanna
Virginia
May 
Sulub
28
64
Chuburna
1
9997432312
hannaMS@gmail.com
4
EOF

echo "Ejecutando prueba 3..."
./registro_votantes < test_input3.txt
echo "✓ Prueba 3 completada."
separador

# PRUEBA 4: Mostrar todos los registros
echo "PRUEBA 4: Mostrar registros completos"
cat > test_input4.txt << EOF
3
4
EOF

echo "Ejecutando prueba 4..."
./registro_votantes < test_input4.txt
echo "✓ Prueba 4 completada."
separador

# Mostrar archivos generados
echo "ARCHIVOS GENERADOS:"
echo "===================================="
echo "Contenido de registro_votantes.txt:"
echo "------------------------------------"
cat registro_votantes.txt

echo ""
echo "===================================="
echo "ESTADO DE ARCHIVOS:"
ls -la registro_votantes*
echo "===================================="

# Limpiar archivos de prueba
rm -f test_input1.txt test_input2.txt test_input3.txt test_input4.txt

echo ""
echo "=== TODAS LAS PRUEBAS COMPLETADAS ==="
echo "Revise los archivos generados para verificar el funcionamiento."