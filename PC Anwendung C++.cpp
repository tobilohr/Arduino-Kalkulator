#include <iostream>  // Für die Konsoleneingabe und -ausgabe
#include <sstream>   // Für Stringstreams (z.B. Formatierung von Eingaben)
#include <windows.h> // Für die Arbeit mit Windows-spezifischen APIs (z.B. COM-Ports)
#include <fstream>   // Zum Arbeiten mit Dateien (z.B. Log-Dateien)
#include <chrono>    // Für Zeitstempel (aktuelles Datum und Uhrzeit)
#include <iomanip>   // Für Zeitformatierung (z.B. "YYYY-MM-DD HH:MM:SS")

using namespace std;

// Funktion zum Überprüfen, ob die Log-Datei existiert
// Diese Funktion öffnet die Datei im Lesemodus und prüft, ob sie erfolgreich geöffnet wurde.
bool logFileExists(const string& filename) {
    ifstream file(filename);  // Datei im Lesemodus öffnen
    return file.good();       // Gibt true zurück, wenn die Datei existiert und gelesen werden kann
}

// Globale Variable für den Log-Datei-Stream, um alle Log-Nachrichten zu speichern
ofstream logFile;

// Funktion, um den aktuellen Zeitstempel als String zurückzugeben
// Der Zeitstempel wird im Format "YYYY-MM-DD HH:MM:SS" erstellt
string getCurrentTimestamp() {
    auto now = chrono::system_clock::now();                 // Aktuelle Zeit holen
    auto time_t_now = chrono::system_clock::to_time_t(now); // Konvertiere Zeit in time_t-Format
    stringstream ss;
    ss << put_time(localtime(&time_t_now), "%Y-%m-%d %H:%M:%S"); // Formatieren der Zeit
    return ss.str();
}

// Funktion, um Nachrichten mit Zeitstempel in die Log-Datei (und optional in die Konsole) zu schreiben
void logMessage(const string& message, bool toConsole = false) {
    string timestamp = getCurrentTimestamp();                       // Zeitstempel holen
    string logMessage = "[" + timestamp + "] " + message;         // Zeitstempel und Nachricht kombinieren
    logFile << logMessage << endl;                                  // Nachricht in die Log-Datei schreiben

    if (toConsole) {                                                // Wenn toConsole true ist
        cout << message << endl;                                    // Nachricht zusätzlich in die Konsole ausgeben
    }
}

// Funktion, um den COM-Port zu initialisieren
// Diese Funktion öffnet den angegebenen COM-Port und konfiguriert die Verbindungseinstellungen
HANDLE initSerialPort(const string& port) {
    HANDLE hSerial = CreateFile(port.c_str(),                     // Port-Name (z.B. "COM3")
                                GENERIC_READ | GENERIC_WRITE,     // Lese- und Schreibzugriff
                                0, NULL, OPEN_EXISTING, 0, NULL); // Öffnen eines bestehenden Ports

    if (hSerial == INVALID_HANDLE_VALUE) {                        // Überprüfen, ob der Port erfolgreich geöffnet wurde
        logMessage("Fehler: COM-Port nicht gefunden!", true);    // Fehler protokollieren und in die Konsole ausgeben
        return INVALID_HANDLE_VALUE;
    }

    DCB dcb = { sizeof(dcb) };                                    // Datenstruktur für die COM-Port-Konfiguration
    GetCommState(hSerial, &dcb);                                  // Aktuelle Port-Einstellungen abrufen
    dcb.BaudRate = CBR_9600;                                      // Baudrate auf 9600 setzen
    dcb.ByteSize = 8;                                             // 8 Datenbits
    dcb.StopBits = ONESTOPBIT;                                    // 1 Stoppbit
    dcb.Parity = NOPARITY;                                        // Keine Parität
    SetCommState(hSerial, &dcb);                                  // Neue Einstellungen anwenden

    return hSerial;                                               // Handle des geöffneten Ports zurückgeben
}

// Funktion, um den COM-Port auf Verfügbarkeit zu überprüfen
// Gibt true zurück, wenn der Port fehlerfrei verwendet werden kann
bool isPortAvailable(HANDLE hSerial) {
    COMSTAT comStat;                                              // Statusstruktur des COM-Ports
    DWORD dwError;                                                // Fehlercode
    return ClearCommError(hSerial, &dwError, &comStat);           // Fehler und Status überprüfen
}

// Funktion, um Daten an den COM-Port zu senden
// Gibt true zurück, wenn die Daten erfolgreich gesendet wurden
bool sendToSerialPort(HANDLE hSerial, const string& data) {
    DWORD bytesWritten;                                           // Anzahl der geschriebenen Bytes
    return WriteFile(hSerial, data.c_str(), data.size(), &bytesWritten, NULL);
}

// Funktion, um Daten vom COM-Port zu empfangen
// Gibt die empfangenen Daten als String zurück
string readFromSerialPort(HANDLE hSerial) {
    char buffer[256];                                             // Puffer für empfangene Daten
    DWORD bytesRead;                                              // Anzahl der gelesenen Bytes
    return ReadFile(hSerial, buffer, sizeof(buffer), &bytesRead, NULL) ? string(buffer, bytesRead) : "";
}

int main() {
    const string logFileName = "communication_log.txt";          // Name der Log-Datei

    // Überprüfen, ob die Log-Datei existiert, und gegebenenfalls eine neue erstellen
    if (!logFileExists(logFileName)) {
        logMessage("Log-Datei existiert noch nicht. Neue Log-Datei wird erstellt.");
    } else {
        logMessage("Log-Datei existiert bereits. Neue Einträge werden angehängt.");
    }

    // Öffne die Log-Datei im Anhängemodus, um neue Einträge hinzuzufügen
    logFile.open(logFileName, ios::app);

    // Protokolliere den Start des Programms
    logMessage("Programm gestartet.");

    // Initialisiere den COM-Port (z.B. COM3)
    HANDLE hSerial = initSerialPort("COM3");
    if (hSerial == INVALID_HANDLE_VALUE) return 1;               // Beende das Programm bei Fehler

    string eingabe;                                              // Variable für Benutzereingaben

    // Haupt-Schleife für die Benutzereingabe
    while (true) {
        cout << "Gib einen Ausdruck ein (z.B. 34 * 72) oder 'exit' zum Beenden: ";
        getline(cin, eingabe);                                  // Lese die Eingabe des Benutzers

        if (eingabe == "exit") {                               // Überprüfen, ob der Benutzer "exit" eingegeben hat
            logMessage("Benutzer hat 'exit' eingegeben. Programm wird beendet.");
            break;                                              // Schleife verlassen
        }

        stringstream ss(eingabe);                              // Stringstream für die Verarbeitung der Eingabe
        int zahl1, zahl2;
        char operation;

        // Überprüfen auf Division durch Null vor der Validierung der Eingabe
        if (ss >> zahl1 >> operation >> zahl2) {
            if (operation == '/' && zahl2 == 0) {              // Prüfen auf Division durch Null
                logMessage("Fehler: Division durch 0 ist nicht erlaubt!");
                cout << "Fehler: Division durch 0 ist nicht erlaubt!" << endl;
                continue;                                      // Eingabe erneut anfordern
            }
        } 

        // Überprüfen, ob die Eingabe gültig ist
        bool istValide = (ss.fail() == false);
        if (istValide) {
            stringstream data;                                // Daten für den COM-Port vorbereiten
            data << zahl1 << " " << operation << " " << zahl2;

            // Logge die zu sendenden Daten
            logMessage("Sende an COM-Port: " + data.str());

            if (isPortAvailable(hSerial) && sendToSerialPort(hSerial, data.str())) {
                logMessage("Daten wurden an COM3 gesendet.");

                string arduinoAntwort = readFromSerialPort(hSerial); // Antwort vom Arduino lesen
                if (!arduinoAntwort.empty()) {
                    logMessage("Antwort vom Arduino erhalten: " + arduinoAntwort);
                    cout << "Antwort vom Arduino: " << arduinoAntwort << endl;
                }
            } else {
                logMessage("Fehler beim Senden an COM-Port!", true);
                cout << "Fehler beim Senden an COM-Port!" << endl;
            }
        } else {
            logMessage("Fehler: Ungültige Eingabe!");
            cout << "Fehler: Ungültige Eingabe!" << endl;
        }
    }

    // Protokolliere das Schließen des COM-Ports
    logMessage("COM-Port wird geschlossen.");
    CloseHandle(hSerial);                                       // COM-Port schließen

    // Protokolliere das Ende des Programms
    logMessage("Programm beendet.");
    logFile.close();                                           // Log-Datei schließen
    return 0;
}
