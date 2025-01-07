#include <LiquidCrystal.h>

// Initialisiere das LCD mit den Pins (RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
    Serial.begin(9600);   // Starte serielle Kommunikation
    lcd.begin(16, 2);     // LCD-Display: 16 Spalten, 2 Zeilen
    lcd.setCursor(0, 0);
    lcd.print("Bereit...");
}

void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n'); // Lese Eingabe bis zum Zeilenumbruch
        input.trim(); // Entferne Leerzeichen

        long operand1, operand2; // Operanden als long für Ganzzahlen
        char operation;

        // Versuche, Ganzzahlen zu parsen
        if (sscanf(input.c_str(), "%ld %c %ld", &operand1, &operation, &operand2) == 3) {
            // Berechne mit long (Ganzzahlen)
        }
        else {
            // Wenn das Format nicht passt, gib eine Fehlermeldung aus
            Serial.println("Fehler: Falsches Format!");

            lcd.clear();
            lcd.print("Fehler:");
            lcd.setCursor(0, 1);
            lcd.print("Falsches Format");
            return;
        }

        float result = 0.0;  // Ergebnis als Float
        bool valid = true;

        switch (operation) {
            case '+': result = operand1 + operand2; break;
            case '-': result = operand1 - operand2; break;
            case '*': result = operand1 * operand2; break;
            case '/': 
                if (operand2 != 0) result = (float)operand1 / (float)operand2; // Umwandlung in Float für Division
                else valid = false; // Verhindert Division durch Null
                break;
            default: valid = false; break;
        }

        lcd.clear(); // Lösche LCD-Inhalt
        lcd.setCursor(0, 0); // Erste Zeile
        if (valid) {
            String resultString = "Erg: " + String(result, 2); // Anzeige mit 2 Dezimalstellen
            Serial.println(resultString); // Sende Ergebnis zurück an PC

            lcd.print("Ergebnis:");
            lcd.setCursor(0, 1); // Zweite Zeile
            lcd.print(resultString);   // Ergebnis auf LCD anzeigen
        } else {
            Serial.println("Fehler: Ungueltig!");

            lcd.print("Fehler!");
            lcd.setCursor(0, 1);
            lcd.print("Ung. Ausdruck");
        }
    }
}
