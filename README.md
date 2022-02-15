# JoyStickMatico V2.0
JoyStick controlado por Arduino, principalmente para su uso en Simuladores (Farming Simulator)

Desarrollado con Visual Code + PlatformIO en Framework Arduino

Desarrollado por Diego Maroto, Febrero 2022 - dmarofer@diegomaroto.net

https://bilbaomakers.org/

Licencia: GNU General Public License v3.0 - https://www.gnu.org/licenses/gpl-3.0.html


Desarrollado con la idea de multiplicar el numero de ejes y botones cuando es necesario tenerlos pero no
utilizarlos a la vez.

El control fisico dispone de un JoyStick de 3 ejes y un boton

Tambien dispone de una botonera matriz de 4x4 (no necesitan diodo)

Al Sistema operativo le presenta 6 ejes y 21 botones


Mapa de Controles Logicos. Referencia de los botones en el sistema operativo. En la libreria el primer indice es 0

MODO 1: LED APAGADO

MODO 2: LED ENCENDIDO

Haciendo HOLD (pulsado mas del tiempo configurado) el boton del JoyStick se alterna el modo de trabajo.

Fuera del modo los ejes logicos presentan estado centrado (0,0,0)

X,Y,Z - Ejes del JoyStick en modo 1

Rx,Ry,Rz - Ejes del JoyStick en modo 2

B1 - Click del boton fisico del JoyStick en cualquiera de los 2 modos

B2 - Doble click del boton del JoyStick en modo 1

B3 - Triple click del boton del JoyStick en modo 1

B4 - Doble click del boton del JoyStick en modo 2

B5 - Triple click del boton del JoyStick en modo 2

B[6-21] - Matriz 4x4 (123A456B789C*0#D)

B1, B2 y B4 emulan automaticamente una pulsacion (se desactivan al tiempo estbalecido)

B3 y B5 Cambian de estado cada vez que se activan (toggle)

B[6-21] Se pueden pulsar hasta 10 simultaneamente y reflejan el estado fisico del boton siempre (pulsado o no)


![](Imagen1.png)

![](Imagen2.jpg)

![](Imagen3.png)