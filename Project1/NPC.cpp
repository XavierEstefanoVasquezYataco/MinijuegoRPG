#include "NPC.h"
#include "PixelArt.h"

std::vector<std::string> NPC::GetDialogue(bool gate1Open, bool gate2Open, bool bossDefeated) const {
    switch (type) {
    case NPCType::MENTOR:
        if (bossDefeated) {
            return {
                "¡Has derrotado al temible Dragon!",
                "La paz y la prosperidad han vuelto a reinar en toda nuestra comarca.",
                "Tu valentia sera recordada por siempre en los cantos de esta aldea."
            };
        }
        if (gate2Open) {
            return {
                "¡Las dos compuertas ancestrales han sido abiertas!",
                "El camino hacia las Ruinas Antiguas esta completamente despejado.",
                "Ten cuidado: los Esqueletos rondan los pasillos de piedra.",
                "Encuentra al Dragon en el sanctum final y destruyelo. ¡Todo el pueblo confia en ti!"
            };
        }
        if (gate1Open) {
            return {
                "¡Gran avance! Has vencido al Murcielago y quebrado la primera compuerta.",
                "Ahora puedes cruzar al Bosque Sombrio, donde merodean los astutos Goblins.",
                "Para entrar a las ruinas, busca y vence al feroz Lobo guardian de las sombras."
            };
        }
        // Inicio
        return {
            "Saludos, aprendiz. Soy Aldor, mentor de esta aldea.",
            "Para moverte usa WASD o las Flechas. Con [E] interactuas con personas y carteles.",
            "Presiona [M] para alternar el minimapa y [P] o [ESC] para pausar la partida.",
            "Tu objetivo es derrotar al Dragon en las ruinas del este. El paso esta sellado por dos compuertas.",
            "Para abrir la primera compuerta, debes derrotar al Murcielago guardian en la pradera."
        };

    case NPCType::HEALER:
        // La curandera ofrece recuperacion mediante pregunta de confirmacion Si/No
        return {
            "Bienvenido a la casa medica de la aldea, viajero.",
            "Deseas que restaure tu salud, mana y provisiones basicas de pociones?"
        };

    case NPCType::GUARD:
        if (gate1Open) {
            return {
                "¡Impresionante combate, heroe!",
                "El Murcielago guardian ha caido y la primera compuerta esta abierta.",
                "Puedes avanzar hacia el Bosque Sombrio, pero ten cuidado con los Goblins."
            };
        }
        return {
            "Alto ahi, viajero. Esta es la primera compuerta hacia el Bosque Sombrio.",
            "Un sello magico la mantiene cerrada. Solo se abrira si derrotas al Murcielago guardian de la pradera.",
            "El Murcielago patrulla los claros cercanos hacia el norte. ¡Ve con cuidado!"
        };

    case NPCType::EXPLORER:
        return {
            "¡Detente un momento, valiente aventurero!",
            "Mas adelante se extienden las peligrosas Ruinas Antiguas.",
            "Los Esqueletos custodian los pasillos y el Dragon final escupe llamaradas devastadoras.",
            "Revisa bien tu salud, tu mana y tus pociones antes de adentrarte en el sanctum.",
            "Si necesitas provisiones, regresa a la aldea segura con la curandera Elena."
        };

    case NPCType::SIGN:
    default:
        if (id == "sign_road") {
            return {
                "--- CAMINO PRINCIPAL AL ESTE ---",
                "Sigue la calzada empedrada para cruzar la Pradera hacia las compuertas.",
                "Presiona [M] para verificar tu posicion y el visor de camara en el minimapa."
            };
        }
        if (id == "sign_danger") {
            return {
                "--- PELIGRO: BOSQUE SOMBRIO ---",
                "Zona de emboscadas de Goblins. Bestias salvajes merodean fuera del sendero.",
                "El Lobo guardian vigila el paso hacia las Ruinas. No te desvies del camino."
            };
        }
        if (id == "sign_clinic") {
            return {
                "--- CLINICA DE ELENA ---",
                "Atencion medica gratuita para aventureros de la aldea.",
                "Pasa a descansar para restaurar salud, mana y reponer pociones de reserva."
            };
        }
        if (id == "sign_ruins") {
            return {
                "--- RUINAS DEL SANCTUM ---",
                "Santuario prohibido del Dragon Rojo.",
                "Peligro mortal. Solo heroes bien preparados deben traspasar este umbral."
            };
        }
        return { "..." };
    }
}

std::vector<NPC> CreateWorldNPCs() {
    std::vector<NPC> list;

    // 1. Mentor en la aldea (Aldor)
    NPC mentor;
    mentor.id = "mentor";
    mentor.type = NPCType::MENTOR;
    mentor.name = "Mentor Aldor";
    mentor.body = { 5 * 32 + 4.0f, 6 * 32 + 4.0f, 24, 24 };
    mentor.art = ART_ELDER;
    mentor.bodyColor = { 45, 65, 120, 255 };   // Tunica azul
    mentor.lightColor = { 215, 175, 55, 255 };  // Ribete dorado
    mentor.isSolid = true;
    list.push_back(mentor);

    // 2. Curandera en la aldea (Elena)
    NPC healer;
    healer.id = "healer";
    healer.type = NPCType::HEALER;
    healer.name = "Curandera Elena";
    healer.body = { 11 * 32 + 4.0f, 9 * 32 + 4.0f, 24, 24 };
    healer.art = ART_HEALER;
    healer.bodyColor = { 45, 145, 85, 255 };   // Verde sanador
    healer.lightColor = { 245, 245, 250, 255 }; // Blanco puro
    healer.isSolid = true;
    list.push_back(healer);

    // 3. Guardia cerca de la primera compuerta (Rurik)
    NPC guard;
    guard.id = "guard";
    guard.type = NPCType::GUARD;
    guard.name = "Guardia Rurik";
    guard.body = { 26 * 32 + 4.0f, 13 * 32 + 4.0f, 24, 24 };
    guard.art = ART_WARRIOR;
    guard.bodyColor = { 170, 180, 195, 255 };  // Armadura metalica
    guard.lightColor = { 220, 50, 50, 255 };    // Escarlata
    guard.isSolid = true;
    list.push_back(guard);

    // 4. Explorador en zona segura antes del tramo final (Varek)
    NPC explorer;
    explorer.id = "explorer";
    explorer.type = NPCType::EXPLORER;
    explorer.name = "Explorador Varek";
    explorer.body = { 41 * 32 + 4.0f, 13 * 32 + 4.0f, 24, 24 };
    explorer.art = ART_SCOUT;
    explorer.bodyColor = { 55, 145, 75, 255 };   // Verde bosque
    explorer.lightColor = { 210, 170, 100, 255 }; // Cuero
    explorer.isSolid = true;
    list.push_back(explorer);

    // 5. Cartel con indicaciones de camino (Salida de la Aldea)
    NPC signRoad;
    signRoad.id = "sign_road";
    signRoad.type = NPCType::SIGN;
    signRoad.name = "Cartel: Salida Este";
    signRoad.body = { 15 * 32 + 4.0f, 13 * 32 + 4.0f, 24, 24 };
    signRoad.art = ART_SIGN;
    signRoad.bodyColor = { 150, 95, 50, 255 };
    signRoad.lightColor = { 230, 205, 150, 255 };
    signRoad.isSolid = true;
    list.push_back(signRoad);

    // 6. Cartel con advertencia sobre zona peligrosa (Entrada al Bosque)
    NPC signDanger;
    signDanger.id = "sign_danger";
    signDanger.type = NPCType::SIGN;
    signDanger.name = "Cartel: Peligro Bosque";
    signDanger.body = { 30 * 32 + 4.0f, 13 * 32 + 4.0f, 24, 24 };
    signDanger.art = ART_SIGN;
    signDanger.bodyColor = { 150, 95, 50, 255 };
    signDanger.lightColor = { 230, 205, 150, 255 };
    signDanger.isSolid = true;
    list.push_back(signDanger);

    // 7. Cartel Clinica
    NPC signClinic;
    signClinic.id = "sign_clinic";
    signClinic.type = NPCType::SIGN;
    signClinic.name = "Cartel: Clinica";
    signClinic.body = { 10 * 32 + 4.0f, 11 * 32 + 4.0f, 24, 24 };
    signClinic.art = ART_SIGN;
    signClinic.bodyColor = { 150, 95, 50, 255 };
    signClinic.lightColor = { 230, 205, 150, 255 };
    signClinic.isSolid = true;
    list.push_back(signClinic);

    // 8. Cartel Ruinas
    NPC signRuins;
    signRuins.id = "sign_ruins";
    signRuins.type = NPCType::SIGN;
    signRuins.name = "Cartel: Ruinas";
    signRuins.body = { 43 * 32 + 4.0f, 13 * 32 + 4.0f, 24, 24 };
    signRuins.art = ART_SIGN;
    signRuins.bodyColor = { 150, 95, 50, 255 };
    signRuins.lightColor = { 230, 205, 150, 255 };
    signRuins.isSolid = true;
    list.push_back(signRuins);

    return list;
}
