Francisco Mesas Cervilla        194327        francisco.mesas01@estudiant.upf.edu
Francisco Moreno Jim�nez        195650        francisco.moreno02@estudiant.upf.edu

VILLAGE ESCAPE

----------------------------------------------------------------------------------------------------------------------
           C�mo jugar
----------------------------------------------------------------------------------------------------------------------

El juego es multijugador cooperativo para 2 personas. Ambos jugadores tendr�n que cooperar para llegar a la meta
evitando acercarse demasiado a ning�n polic�a. El jugador 1, que en la pantalla dividida est� a la izquierda,
avanza con W, retrocede con S, gira a la izquierda con A, gira a la derecha con D y corre con SHIFT izquierdo.
El jugador 2 se mueve con las flechas y el SHIFT derecho. Eventualmente, se puede jugar con mando. Para cambiar
el input del jugador 1 al mando, hay que pulsar X e ir probando hasta que eventualmente funcione. Esto es as�
porque a veces determin�bamos la id del mando como 1 pero luego, de forma aleatoria, nos aparec�a un mando intruso
que el programa no reconoc�a y al final el mando que quer�a usar el jugador 1 no siempre acaba en la misma posici�n.
Para cambiar el input del jugador 2, es con la tecla C. Si usamos mando, nos movemos con el joystick izquierdo,
movemos la vista con el joystick derecho y corremos con R2 (est� testeado con mandos de PS4 y no sabemos qu� tecla
del mando de XBOX ser�). Se puede pausar la partida con la tecla P y salir del modo de pausa con la tecla Escape.


----------------------------------------------------------------------------------------------------------------------
           Qu� hemos programado
----------------------------------------------------------------------------------------------------------------------

El juego se divide en stages:
Intro stage: stage inicial, que muestra la portada e instrucciones sobre el objetivo del juego y c�mo jugar.
Loading stage: donde se cargan todos los recursos (aquellos que no han sido utilizados ya por la intro stage) antes
               de que comience el juego.
Game stage: la stage principal, en ella se desarrolla el juego.
Pause stage: stage que corresponde al men� de pausa.
End stage: stage a la que vamos cuando acaba el juego, tanto si ganamos como si perdemos.

Para empezar, el juego es completo. Hay una manera de ganar y de perder.

El juego est� formado por una pantalla dividida donde en la pantalla izquierda se ve lo que ve el player 1 y en la
derecha se ve lo que est� viendo el player 2. Esto hace que el juego sea multiplayer local.

La clase game b�sicamente lo que hace es llamar a los m�todos render y update de la stage actual. Tambi�n tiene un
par de onKeyPressed, para elegir si queremos ver las estad�sticas de la GPU o no y para refrescar los shaders.
Pero la chicha est� en la game stage. Cuando la inicializamos, inicializamos con ella el world y todos los objetos
que este incluye: props, agents y los players. Las tres entidades son le�das de ficheros txt externos, uno para cada
una. En ellos se define b�sicamente de qu� tipo son y cu�l es su posici�n y orientaci�n.

Los agents son movidos por una AI muy b�sica que usa finite-state machines. Los agents van de un punto a otro (los
cuales est�n definidos en agents.txt) en l�nea recta parando 2 segundos a descansar al llegar a cada punto. No hay
nada de path finding.

Las colisiones entre agents no est�n controladas, las colisiones entre agent y player son imposibles porque a la que
te acercas ya te pillan y has perdido la partida y las colisiones entre agent y prop son imposibles porque los
agentes que hay creados est�n dise�ados para que a lo largo de su trayectoria no topen con ning�n prop. Las
colisiones entre los dos players y entre player y prop s� est�n controladas.

El shader que se usa para pintar los props es un phong que solo usa luz ambiental. Hab�amos pensado en poner un punto
de luz en cada polic�a y pasarle el front vector de cada polic�a al shader para que cuando mayor sea el producto
escalar entre el front vector y el light vector, m�s ilumine la linterna (lo que har�a una linterna real). Sin
embargo, no hemos tenido tiempo de implementar esto �ltimo.

Otra cosa que hemos implementado son las animaciones y las transiciones entre animaciones. En el update de la clase
character, cada vez que el character est� idle actualizamos el idle_time, y con el walk_time y el run_time lo mismo.
As�, tenemos guardado el �ltimo momento en que hizo cada cosa. A la hora de hacer las animaciones, cogemos los dos
valores m�s altos (las dos �ltimas acciones que hizo el player) y blendeamos las animaciones de esas dos acciones,
usando como factor de interpolaci�n la resta entre la �ltima acci�n y la pen�ltima, es decir, el tiempo que hace que
el player dej� de hacer lo pen�ltimo para pasar a hacer lo �ltimo. Para hacer la transici�n m�s suave hemos pasado
este factor de interpolaci�n por una funci�n sinusoidal que hemos clampeado entre 0 y 1. Este sistema nos ha dado
buenos resultados.

Como parte de GUI, hemos proporcionado una br�jula a cada player que le indica hacia d�nde tiene que ir para escapar
y alcanzar su preciada libertad. El �ngulo que forma la aguja de la br�jula est� calculado a partir del vector entre
la posici�n del player y la posici�n de la meta, adem�s del yaw del player.

Como parte de audio, hemos puesto un sonido de fondo de unos grillos. Sin embargo, cuando uno de los dos players se
acerca a menos de cierta distancia a un polic�a, empieza a sonar una m�sica de tensi�n. Creemos que la gracia es que
cuando suena ninguno de los players sabe qui�n de los dos es el que est� cerca de un polic�a y eso hace que los dos
se pongan alerta. Creemos que este sencillo sistema es suficiente para dar una peque�a experiencia de inmersi�n de los
jugadores en el juego.

Como hemos explicado en el apartado anterior, es posible usar gamepad para jugar a este juego.

No hemos cre�do interesante a�adir una funcionalidad para guardar y cargar partidas en un juego en el que las partidas
tienen un corto tiempo de vida. Es simplemente correr por la ciudad evitando a los guardias hasta que o te pillan
o llegas a la meta.

Podr�amos haber hecho niveles de dificultad m�s f�ciles en los cuales hay menos polic�as por la ciudad o m�s dif�ciles
aumentando la distancia a la que tienes que estar de un polic�a para que te pille, pero no lo hemos visto
demasiado interesante y no lo hemos hecho. Hemos preferido centrarnos en otras cosas m�s interesantes.