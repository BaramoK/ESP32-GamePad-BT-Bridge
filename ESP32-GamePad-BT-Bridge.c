#include <SPI.h>
#include <Usb.h>
#include <USBHub.h>
#include <XBOXRECV.h>
#include <BleGamepad.h>
#include <usb/usb_host.h>
#include <BLEDevice.h>

// Énumération des boutons de la manette Xbox 360
// Permet de mapper les boutons à des indices pour accéder facilement à leur état
enum {
    BUTTON_NONE = 0,   // Valeur pour aucun bouton pressé
    BUTTON_A,          // Bouton A (action principale)
    BUTTON_B,          // Bouton B (action secondaire/annulation)
    BUTTON_X,          // Bouton X (action alternative)
    BUTTON_Y,          // Bouton Y (action alternative secondaire)
    BUTTON_LB,         // Left Bumper - Bouton supérieur gauche
    BUTTON_RB,         // Right Bumper - Bouton supérieur droit
    BUTTON_LS,         // Left Stick - Pression sur le stick gauche
    BUTTON_RS,         // Right Stick - Pression sur le stick droit
    BUTTON_START,      // Bouton Start/Menu
    BUTTON_SELECT,     // Bouton Select/Back/View
    BUTTON_XBOX,       // Bouton Xbox central/Guide
    BUTTON_UP,         // D-pad direction haut
    BUTTON_DOWN,       // D-pad direction bas
    BUTTON_LEFT,       // D-pad direction gauche
    BUTTON_RIGHT,      // D-pad direction droite
    BUTTON_COUNT       // Compteur total pour allocation de mémoire
};

// Objets pour la communication USB et la gestion des manettes
USB Usb;                  // Contrôleur principal USB pour communiquer avec le shield
USBHub Hub(&Usb);         // Gestionnaire de hub USB pour augmenter le nombre de ports
XBOXRECV Xbox(&Usb);      // Récepteur sans fil pour manettes Xbox 360

// Configuration pour le périphérique Bluetooth émulant une manette
BleGamepadConfiguration bleConfig;  // Configuration personnalisée pour le périphérique BLE
BleGamepad bleGamepad("ESP32-BLE-Gamepad", "ESP32", 100);  // Nom, fabricant et % batterie

// Variables d'état pour la gestion de l'appairage Bluetooth
bool isPairing = false;               // Indicateur si mode appairage actif
bool isConnected = false;             // Indicateur si un appareil est connecté
unsigned long pairingStartTime = 0;   // Horodatage du début de l'appairage
const unsigned long PAIRING_TIMEOUT = 60000; // Temps max d'appairage (1 min)

// Structure regroupant toutes les données d'état de la manette Xbox
typedef struct {
    int16_t leftX;        // Position horizontale du joystick gauche (-32768 à 32767)
    int16_t leftY;        // Position verticale du joystick gauche (-32768 à 32767)
    int16_t rightX;       // Position horizontale du joystick droit (-32768 à 32767)
    int16_t rightY;       // Position verticale du joystick droit (-32768 à 32767)
    uint8_t leftTrigger;  // Valeur analogique de la gâchette gauche (0-255)
    uint8_t rightTrigger; // Valeur analogique de la gâchette droite (0-255)
    bool buttons[BUTTON_COUNT]; // Tableau des états de chaque bouton (true=pressé)
} XboxController;

XboxController controller; // Instance globale stockant l'état actuel de la manette

// Classe de gestion des événements de connexion/déconnexion Bluetooth
class BleGamepadCallbacks : public BLEServerCallbacks {
    // Méthode appelée quand un appareil se connecte au ESP32
    void onConnect(BLEServer* server) {
        isConnected = true;
        isPairing = false;
        Serial.println("Bluetooth device connected!");
    }

    // Méthode appelée quand un appareil se déconnecte du ESP32
    void onDisconnect(BLEServer* server) {
        isConnected = false;
        Serial.println("Bluetooth device disconnected!");
        // Redémarrer l'advertising pour permettre une nouvelle connexion
        server->startAdvertising();
    }
};

// Active le mode d'appairage Bluetooth pour permettre les nouvelles connexions
void startPairingMode() {
    if (!isPairing) {
        isPairing = true;
        pairingStartTime = millis();

        // Configuration du serveur BLE avec les callbacks
        BLEDevice::init("ESP32-BLE-Gamepad");
        BLEServer *pServer = BLEDevice::createServer();
        pServer->setCallbacks(new BleGamepadCallbacks());

        Serial.println("Mode appairage Bluetooth activé pour 1 minute");
        // Démarre l'advertising BLE pour être visible
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->start();
    }
}

// Vérifie si le temps d'appairage est dépassé pour arrêter le mode appairage
void checkPairingTimeout() {
    if (isPairing && (millis() - pairingStartTime > PAIRING_TIMEOUT)) {
        isPairing = false;
        if (!isConnected) {
            Serial.println("Timeout d'appairage atteint. Mode appairage désactivé.");
            BLEDevice::deinit(true);
        }
    }
}

// Lit toutes les entrées de la manette Xbox et met à jour la structure controller
void readControllerInput() {
    // Lecture des positions des joysticks
    controller.leftX = Xbox.getAnalogHat(LeftHatX);       // Axe X du joystick gauche
    controller.leftY = Xbox.getAnalogHat(LeftHatY);       // Axe Y du joystick gauche
    controller.rightX = Xbox.getAnalogHat(RightHatX);     // Axe X du joystick droit
    controller.rightY = Xbox.getAnalogHat(RightHatY);     // Axe Y du joystick droit

    // Lecture des gâchettes analogiques
    controller.leftTrigger = Xbox.getAnalogButton(LT);    // Gâchette gauche (LT)
    controller.rightTrigger = Xbox.getAnalogButton(RT);   // Gâchette droite (RT)

    // Lecture de l'état de tous les boutons
    controller.buttons[BUTTON_A] = Xbox.getButtonPress(A);
    controller.buttons[BUTTON_B] = Xbox.getButtonPress(B);
    controller.buttons[BUTTON_X] = Xbox.getButtonPress(X);
    controller.buttons[BUTTON_Y] = Xbox.getButtonPress(Y);
    controller.buttons[BUTTON_LB] = Xbox.getButtonPress(LB);
    controller.buttons[BUTTON_RB] = Xbox.getButtonPress(RB);
    controller.buttons[BUTTON_LS] = Xbox.getButtonPress(LS);
    controller.buttons[BUTTON_RS] = Xbox.getButtonPress(RS);
    controller.buttons[BUTTON_START] = Xbox.getButtonPress(START);
    controller.buttons[BUTTON_SELECT] = Xbox.getButtonPress(BACK);
    controller.buttons[BUTTON_XBOX] = Xbox.getButtonPress(GUIDE);
    controller.buttons[BUTTON_UP] = Xbox.getButtonPress(UP);
    controller.buttons[BUTTON_DOWN] = Xbox.getButtonPress(DOWN);
    controller.buttons[BUTTON_LEFT] = Xbox.getButtonPress(LEFT);
    controller.buttons[BUTTON_RIGHT] = Xbox.getButtonPress(RIGHT);

    // Détection de la pression prolongée sur le bouton Xbox/Guide pour l'appairage
    static unsigned long guideButtonPressTime = 0;  // Heure à laquelle le bouton a été pressé
    if (controller.buttons[BUTTON_XBOX]) {
        if (guideButtonPressTime == 0) {
            guideButtonPressTime = millis();  // Enregistrer le moment où le bouton est pressé
        } else if (millis() - guideButtonPressTime > 3000) {  // 3 secondes de pression
            startPairingMode();  // Activer le mode appairage
            guideButtonPressTime = 0;  // Réinitialiser le compteur
        }
    } else {
        guideButtonPressTime = 0;  // Réinitialiser si le bouton est relâché
    }
}

// Transmet les données de la manette Xbox via Bluetooth
void sendControllerData() {
    if (isConnected && bleGamepad.isConnected()) {
        // Envoyer les positions des joysticks et gâchettes
        bleGamepad.setAxes(
            controller.leftX,       // X gauche
            controller.leftY,       // Y gauche
            controller.rightX,      // X droit
            controller.rightY,      // Y droit
            controller.leftTrigger, // Gâchette gauche
            controller.rightTrigger,// Gâchette droite
            0, 0                    // Axes supplémentaires non utilisés
        );

        // Mise à jour de l'état des boutons sur le périphérique Bluetooth
        for (int i = BUTTON_A; i < BUTTON_COUNT; i++) {
            if (controller.buttons[i]) {
                bleGamepad.press(i);    // Active le bouton si pressé
            } else {
                bleGamepad.release(i);  // Désactive le bouton si relâché
            }
        }

        // Envoie le rapport d'état complet
        bleGamepad.sendReport();
    }
}

// Initialisation du système
void setup() {
    Serial.begin(115200);  // Communication série pour le débogage
    Serial.println("Starting ESP32-S3 USB Host");

    // Configuration du périphérique Bluetooth émulant une manette
    bleConfig.setAutoReport(false);  // Désactive les rapports automatiques
    bleConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD);  // Type: manette de jeu
    bleConfig.setButtonCount(BUTTON_COUNT);  // Nombre de boutons disponibles
    bleConfig.setWhichAxes(true, true, true, true, true, true, false, false);  // Axes actifs
    bleConfig.setAxesMin(0x0000);  // Valeur minimale des axes
    bleConfig.setAxesMax(0x7FFF);  // Valeur maximale des axes
    bleGamepad.begin(&bleConfig);  // Initialisation avec la configuration
    // Configuration de l'hôte USB pour l'ESP32-S3
    usb_host_config_t host_config = {
        .skip_phy_setup = false,            // Configuration physique requise
        .intr_flags = ESP_INTR_FLAG_LEVEL1, // Niveau de priorité des interruptions
    };

    // Installation du pilote USB Host
    if (usb_host_install(&host_config) != ESP_OK) {
        Serial.println("ESP32-S3 USB Host initialization failed");
        while (1); // Boucle infinie en cas d'échec
    }

    // Initialisation du shield USB Host
    if (Usb.Init() == -1) {
        Serial.println("USB Host Shield did not start");
        while (1); // Boucle infinie en cas d'échec
    }

    Serial.println("ESP32-S3 USB Host initialized successfully");
    Serial.println("Pour lancer l'appairage Bluetooth, maintenez le bouton Xbox/Guide pendant 3 secondes");

    // Activation initiale du mode appairage
    startPairingMode();
}

// Boucle principale du programme
void loop() {
    Usb.Task();  // Traitement des événements USB

    // Vérification du timeout d'appairage
    checkPairingTimeout();

    // Traitement des données de la manette si connectée
    if (Xbox.Xbox360Connected[0]) {  // Vérifie si la manette #0 est connectée
        readControllerInput();     // Lecture des entrées
        sendControllerData();      // Transmission via Bluetooth
        delay(10);                 // Délai pour stabiliser la communication BLE
    }
}
