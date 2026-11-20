/**
 * @file main.cpp
 * @brief Punto de entrada principal del videojuego Mini Aventura RPG.
 *
 * Capa de Arquitectura: Punto de Entrada / Arranque de la Aplicación.
 *
 * Propósito y Responsabilidades:
 * - Instanciar el objeto raíz del juego (`Game`), que actúa como el Controlador principal.
 * - Iniciar el ciclo de vida de la aplicación llamando a `Game::Run()`.
 * - Garantizar una terminación limpia devolviendo el código de salida estándar (0).
 *
 * Relaciones Principales:
 * - Se comunica exclusivamente con el Controlador principal `Game` ([Controladores/Game.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Controladores/Game.h)).
 * - Mantiene el punto de arranque desacoplado de bibliotecas gráficas y modelos de datos.
 */

#include "Controladores/Game.h"

/**
 * @brief Función de inicio del programa (entry point estándar en C++).
 *
 * Flujo de ejecución:
 * 1. Instancia el controlador `game` en el marco de pila local.
 *    El constructor de `Game` inicializa la ventana de Raylib (960x540),
 *    la tasa de refresco a 60 FPS, las estructuras del mapa, el jugador y las vistas.
 * 2. Invoca `game.Run()`, entrando en el bucle principal que procesa eventos, lógica y renderizado.
 * 3. Al cerrar la ventana o salir del juego, `game.Run()` finaliza, el destructor de `Game`
 *    libera los recursos gráficos y la función retorna 0 indicando cierre exitoso.
 *
 * @return int Código de salida del sistema operativo (0 para finalización normal).
 */
int main() {
    // Crea el controlador que administra los recursos del juego.
    Game game;
    // Ejecuta el bucle principal hasta que el jugador sale.
    game.Run();
    // Indica al sistema operativo que la ejecución terminó normalmente.
    return 0;
}
