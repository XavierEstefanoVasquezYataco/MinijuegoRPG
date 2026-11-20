/**
 * @file PixelArt.h
 * @brief Rasterizador procedural de sprites 12x12 en Pixel Art sin dependencias de texturas externas.
 *
 * Capa de Arquitectura: Vista (View) - Renderizado Procedural y Recursos Visuales.
 *
 * Propósito y Responsabilidades:
 * - Proveer identificadores simbólicos para todas las apariencias de criaturas, héroes, NPCs y carteles.
 * - Declarar la función de trazado `DrawPixelArt` que dibuja en pantalla sprites vectorizados mediante primitivas.
 * - Permitir la personalización dinámica de colores primarios y secundarios (`body`, `light`) y efecto de espejo horizontal (`flipX`).
 *
 * Relaciones Principales:
 * - Utilizado por [Vistas/MapView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/MapView.h) para dibujar enemigos y NPCs en el supramundo.
 * - Utilizado por [Vistas/CombatUI.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/CombatUI.h) para renderizar al héroe de espaldas y a los monstruos en combate.
 * - Utilizado por [Vistas/TitleView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/TitleView.h) para mostrar las miniaturas de los héroes en la selección.
 * - Utilizado por [Vistas/DialogueView.h](file:///C:/Users/Xavier/source/repos/MiniAventuraRPG_v2/Project1/Vistas/DialogueView.h) para el recuadro del avatar del hablante.
 */

#pragma once
#include "raylib.h"

// ---------- Identificadores de sprites procedimentales 12x12 ----------

/// Sprite del Slime (masa gelatinosa con ojos).
constexpr int ART_SLIME = 0;

/// Sprite del Murciélago (alas desplegadas y colmillos).
constexpr int ART_BAT = 1;

/// Sprite del Humanoide / Goblin / Esqueleto (silueta bípeda).
constexpr int ART_HUMANOID = 2;

/// Sprite de Bestia salvaje / Lobo de sombras (cuadrúpedo acechante).
constexpr int ART_BEAST = 3;

/// Sprite del Dragón rojo (jefe imponente con cuernos y vientre blindado).
constexpr int ART_DRAGON = 4;

/// Sprite del héroe visto de espaldas para la arena de combate.
constexpr int ART_HERO_BACK = 5;

/// Sprite del Guerrero con casco y hombreras.
constexpr int ART_WARRIOR = 6;

/// Sprite de la Maga con capucha mística y báculo.
constexpr int ART_MAGE = 7;

/// Sprite de la Exploradora con tocado y carcaj.
constexpr int ART_SCOUT = 8;

/// Sprite del Anciano / Mentor Aldor con barba blanca y túnica.
constexpr int ART_ELDER = 9;

/// Sprite de la Curandera Elena con cofia y vestido medicinal.
constexpr int ART_HEALER = 10;

/// Sprite del Cartel de madera grabado.
constexpr int ART_SIGN = 11;

/**
 * @brief Dibuja un sprite procedimental de 12x12 celdas escalado en coordenadas específicas.
 *
 * Itera sobre la matriz ASCII asociada al identificador `art`, asigna a cada carácter
 * un color de la paleta temática y dibuja rectángulos sólidos mediante `DrawRectangle`.
 *
 * @param art Índice del sprite (0..11). Si está fuera de rango, no realiza ninguna acción.
 * @param x Coordenada X superior izquierda de dibujo en la pantalla.
 * @param y Coordenada Y superior izquierda de dibujo en la pantalla.
 * @param scale Factor de escalado de cada píxel individual (ej. 2 para 24x24 px, 5 para 60x60 px, 12 para 144x144 px).
 * @param body Color asignado a los píxeles identificados con 'b' (cuerpo principal).
 * @param light Color asignado a los píxeles identificados con 'l' (brillos y ribetes).
 * @param flipX Si es true, invierte horizontalmente las columnas (efecto espejo).
 */
void DrawPixelArt(int art, float x, float y, int scale,
    Color body, Color light, bool flipX = false);
