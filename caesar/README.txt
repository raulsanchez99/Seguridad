PRÁCTICA 1: Romper el cifrado Caesar

1._FICHEROS CONTENIDOS:

En este directorio encontrarás 2 códigos fuentes(caesar.c y breakcaesar.c) 
escritos en lenguaje C puro de  programación, así como sus ficheros objeto
y sus binarios para su cómoda ejecución.

Además, encontrarás un fichero 'metallica.txt' con texto plano largo y
a continuación su cifrado caesar con clave 24 para facilitarte poner a prueba
el programa principal

2._CÓMO EJECUTAR BREAKCAESAR:

Para ejecutar el programa pedido en el ejercicio, haremos uso de pasarle un
argumento que corresponderá con el texto cifrado con algoritmo Caesar. Recordar
que hará falta poner este argumento entre comillas para que se interprete como
como un sólo argumento y debemos situarnos en el directorio donde se encuentra
el binario.

EJEMPLO: ./breakcaesar 'EF JV KXJB FP XIBUXKAOB'

3._CÓMO EJECUTAR CAESAR:

He incluido en el directorio el programa Caesar el cual podría ayudar para 
conseguir cifrar de forma rápida texto que posteriormente quisieras probar
en breakcaesar. Una vez más hago uso de argumentos para el programa. En este
caso usaremos 2 argumentos en este orden: 

-Primero: número entero que represente la clave con la que queremos cifrar[0-25]
-Segundo: El texto en inglés que queremos cifrar entre comillas

EJEMPLO: ./caesar 23 'Hi, my name is Alexandre'



