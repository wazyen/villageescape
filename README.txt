Francisco Mesas Cervilla        194327        francisco.mesas01@estudiant.upf.edu
Francisco Moreno Jiménez        195650        francisco.moreno02@estudiant.upf.edu

VILLAGE ESCAPE

----------------------------------------------------------------------------------------------------------------------
           Cómo jugar
----------------------------------------------------------------------------------------------------------------------

El juego es multijugador cooperativo para 2 personas. Ambos jugadores tendrán que cooperar para llegar a la meta
evitando acercarse demasiado a ningún policía. El jugador 1, que en la pantalla dividida está a la izquierda,
avanza con W, retrocede con S, gira a la izquierda con A, gira a la derecha con D y corre con SHIFT izquierdo.
El jugador 2 se mueve con las flechas y el SHIFT derecho. Eventualmente, se puede jugar con mando. Para cambiar
el input del jugador 1 al mando, hay que pulsar X e ir probando hasta que eventualmente funcione. Esto es así
porque a veces determinábamos la id del mando como 1 pero luego, de forma aleatoria, nos aparecía un mando intruso
que el programa no reconocía y al final el mando que quería usar el jugador 1 no siempre acaba en la misma posición.
Para cambiar el input del jugador 2, es con la tecla C. Si usamos mando, nos movemos con el joystick izquierdo,
movemos la vista con el joystick derecho y corremos con R2 (está testeado con mandos de PS4 y no sabemos qué tecla
del mando de XBOX será). Se puede pausar la partida con la tecla P y salir del modo de pausa con la tecla Escape.


----------------------------------------------------------------------------------------------------------------------
           Qué hemos programado
----------------------------------------------------------------------------------------------------------------------

El juego se divide en stages:
Intro stage: stage inicial, que muestra la portada e instrucciones sobre el objetivo del juego y cómo jugar.
Loading stage: donde se cargan todos los recursos (aquellos que no han sido utilizados ya por la intro stage) antes
               de que comience el juego.
Game stage: la stage principal, en ella se desarrolla el juego.
Pause stage: stage que corresponde al menú de pausa.
End stage: stage a la que vamos cuando acaba el juego, tanto si ganamos como si perdemos.

Para empezar, el juego es completo. Hay una manera de ganar y de perder.

El juego está formado por una pantalla dividida donde en la pantalla izquierda se ve lo que ve el player 1 y en la
derecha se ve lo que está viendo el player 2. Esto hace que el juego sea multiplayer local.

La clase game básicamente lo que hace es llamar a los métodos render y update de la stage actual. También tiene un
par de onKeyPressed, para elegir si queremos ver las estadísticas de la GPU o no y para refrescar los shaders.
Pero la chicha está en la game stage. Cuando la inicializamos, inicializamos con ella el world y todos los objetos
que este incluye: props, agents y los players. Las tres entidades son leídas de ficheros txt externos, uno para cada
una. En ellos se define básicamente de qué tipo son y cuál es su posición y orientación.

Los agents son movidos por una AI muy básica que usa finite-state machines. Los agents van de un punto a otro (los
cuales están definidos en agents.txt) en línea recta parando 2 segundos a descansar al llegar a cada punto. No hay
nada de path finding.

Las colisiones entre agents no están controladas, las colisiones entre agent y player son imposibles porque a la que
te acercas ya te pillan y has perdido la partida y las colisiones entre agent y prop son imposibles porque los
agentes que hay creados están diseñados para que a lo largo de su trayectoria no topen con ningún prop. Las
colisiones entre los dos players y entre player y prop sí están controladas.

El shader que se usa para pintar los props es un phong que solo usa luz ambiental. Habíamos pensado en poner un punto
de luz en cada policía y pasarle el front vector de cada policía al shader para que cuando mayor sea el producto
escalar entre el front vector y el light vector, más ilumine la linterna (lo que haría una linterna real). Sin
embargo, no hemos tenido tiempo de implementar esto último.

Otra cosa que hemos implementado son las animaciones y las transiciones entre animaciones. En el update de la clase
character, cada vez que el character está idle actualizamos el idle_time, y con el walk_time y el run_time lo mismo.
Así, tenemos guardado el último momento en que hizo cada cosa. A la hora de hacer las animaciones, cogemos los dos
valores más altos (las dos últimas acciones que hizo el player) y blendeamos las animaciones de esas dos acciones,
usando como factor de interpolación la resta entre la última acción y la penúltima, es decir, el tiempo que hace que
el player dejó de hacer lo penúltimo para pasar a hacer lo último. Para hacer la transición más suave hemos pasado
este factor de interpolación por una función sinusoidal que hemos clampeado entre 0 y 1. Este sistema nos ha dado
buenos resultados.

Como parte de GUI, hemos proporcionado una brújula a cada player que le indica hacia dónde tiene que ir para escapar
y alcanzar su preciada libertad. El ángulo que forma la aguja de la brújula está calculado a partir del vector entre
la posición del player y la posición de la meta, además del yaw del player.

Como parte de audio, hemos puesto un sonido de fondo de unos grillos. Sin embargo, cuando uno de los dos players se
acerca a menos de cierta distancia a un policía, empieza a sonar una música de tensión. Creemos que la gracia es que
cuando suena ninguno de los players sabe quién de los dos es el que está cerca de un policía y eso hace que los dos
se pongan alerta. Creemos que este sencillo sistema es suficiente para dar una pequeña experiencia de inmersión de los
jugadores en el juego.

Como hemos explicado en el apartado anterior, es posible usar gamepad para jugar a este juego.

No hemos creído interesante añadir una funcionalidad para guardar y cargar partidas en un juego en el que las partidas
tienen un corto tiempo de vida. Es simplemente correr por la ciudad evitando a los guardias hasta que o te pillan
o llegas a la meta.

Podríamos haber hecho niveles de dificultad más fáciles en los cuales hay menos policías por la ciudad o más difíciles
aumentando la distancia a la que tienes que estar de un policía para que te pille, pero no lo hemos visto
demasiado interesante y no lo hemos hecho. Hemos preferido centrarnos en otras cosas más interesantes.