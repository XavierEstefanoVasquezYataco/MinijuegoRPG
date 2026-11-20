/**
 * @file NPC.h
 * @brief Definición del modelo de Personajes No Jugadores (NPCs) y Carteles del mundo.
 *
 * Capa de Arquitectura: Modelo (Model) - Entidades y Narrativa.
 *
 * Propósito y Responsabilidades:
 * - Representar a los habitantes de la aldea, guardianes, exploradores y carteles del supramundo.
 * - Almacenar atributos físicos (`body`, `isSolid`), estéticos (`art`, `bodyColor`, `lightColor`) y de rol (`type`).
 * - Generar dinámicamente las líneas de diálogo (`GetDialogue`) evaluando el progreso del héroe
 *   (apertura de compuertas y derrota del jefe final).
 * - Proveer la función de fábrica `CreateWorldNPCs()` para poblar la aldea y los caminos.
 *
 * Relaciones Principales:
 * - Utilizado por el Controlador ([Controladores/Game.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Controladores/Game.h)) para detectar interacciones (tecla `E`) y gestionar la conversación.
 * - Utilizado por las Vistas ([Vistas/DialogueView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/DialogueView.h) y [Vistas/MapView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/MapView.h)) para dibujar las cajas de diálogo y los avatares en el mundo.
 *
 * Observaciones Arquitectónicas:
 * - Contiene constantes de color y forma (`Rectangle body`, `Color bodyColor`) requeridas para
 *   su presencia en el mundo. No realiza llamadas a Raylib de dibujo directamente.
 */

#pragma once
#include <string>
#include <vector>
#include "raylib.h"

/**
 * @enum NPCType
 * @brief Categorías de personajes no jugadores presentes en el supramundo.
 *
 * - MENTOR: Aldor, guía de la aldea; explica controles y objetivos de las compuertas.
 * - HEALER: Elena, médica de la aldea; ofrece sanación interactiva mediante confirmación [S]/[N].
 * - GUARD: Rurik, centinela junto a la primera compuerta que advierte sobre los peligros.
 * - EXPLORER: Varek, aventurero veterano en las inmediaciones de las ruinas finales.
 * - SIGN: Carteles fijos en el suelo que proporcionan orientación y avisos de peligro.
 */
enum class NPCType {
    MENTOR,
    HEALER,
    GUARD,
    EXPLORER,
    SIGN
};

/**
 * @struct NPC
 * @brief Estructura de datos que encapsula el estado físico, visual y narrativo de un personaje o cartel.
 */
struct NPC {
    std::string id;                         ///< Identificador único del NPC (ej: "mentor", "healer", "sign_road").
    NPCType type = NPCType::SIGN;           ///< Rol funcional dentro del juego.
    std::string name;                       ///< Nombre mostrado en la cabecera del cuadro de diálogo.
    Rectangle body{ 0, 0, 24, 24 };         ///< Caja delimitadora en el mapa (24x24 px).
    int art = 0;                            ///< Identificador de sprite procedimental 12x12 (constante de PixelArt).
    Color bodyColor{ 255, 255, 255, 255 };  ///< Color principal de la vestimenta o material.
    Color lightColor{ 255, 255, 255, 255 }; ///< Color secundario o ribetes decorativos.
    bool isSolid = true;                    ///< Si es true, el jugador no puede atravesarlo físicamente.
    bool isSign = false;                    ///< Diferencia carteles de personas para el texto del aviso ("[E] Leer" vs "[E] Hablar").

    /**
     * @brief Comprueba si esta entidad debe ser tratada como un cartel u objeto inanimado.
     * @return true Si `isSign` es verdadero o su tipo es `NPCType::SIGN`.
     */
    bool IsSign() const { return isSign || type == NPCType::SIGN; }

    /**
     * @brief Calcula las coordenadas del centro del cuerpo en píxeles.
     *
     * Utilizado para calcular la distancia euclidiana hacia el jugador al evaluar si está en rango de interacción.
     *
     * @return Vector2 Coordenadas centrales (x + width/2, y + height/2).
     */
    Vector2 Center() const {
        return { body.x + body.width / 2.0f, body.y + body.height / 2.0f };
    }

    /**
     * @brief Obtiene la secuencia de líneas de diálogo según el progreso actual de la partida.
     *
     * Los diálogos progresan de acuerdo al avance del héroe en el mapa:
     * - Si `bossDefeated` es true, los NPCs felicitan al héroe por liberar la comarca.
     * - Si `gate2Open` es true, advierten sobre el Dragón y los peligros de las Ruinas.
     * - Si `gate1Open` es true, orientan hacia el Bosque Sombrío y el Lobo guardián.
     * - Si ninguna compuerta está abierta, ofrecen el tutorial y la bienvenida inicial.
     *
     * @param gate1Open true si la primera compuerta ha sido desbloqueada.
     * @param gate2Open true si la segunda compuerta ha sido desbloqueada.
     * @param bossDefeated true si el Dragón ha sido derrotado.
     * @return std::vector<std::string> Lista ordenada de oraciones que se mostrarán página a página.
     */
    std::vector<std::string> GetDialogue(bool gate1Open, bool gate2Open, bool bossDefeated) const;
};

/**
 * @brief Función factoría que instancia y posiciona todos los NPCs y carteles del mundo.
 *
 * Configura los 4 personajes de la aldea y los 4 carteles informativos con sus
 * posiciones mundiales en tiles, sprites y colores característicos.
 *
 * @return std::vector<NPC> Colección completa de personajes y carteles del mapa.
 */
std::vector<NPC> CreateWorldNPCs();
