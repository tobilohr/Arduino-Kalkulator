# Arduino-Kalkulator

Dieses Projekt besteht aus einer PC-Konsolenanwendung und einem Arduino-Programm, die zusammen eine einfache Rechenaufgabe durchführen. Die PC-Anwendung validiert die Eingabe des Benutzers, sendet die eingegebenen Daten an den Arduino und empfängt das Ergebnis. Der Arduino verarbeitet die Berechnung und gibt das Ergebnis zurück.

Optional kann ein LCD-Display an den Arduino angeschlossen werden, welches die Berechnungen und Ergebnisse anzeigt. Dieses Display wird an die digitalen Pins des Arduino angeschlossen und bietet eine visuelle Darstellung der Ergebnisse direkt auf dem Gerät. Fehlerhafte Eingaben oder ungültige Berechnungen werden sowohl auf dem PC als auch auf dem LCD des Arduino angezeigt.

Funktionsweise:
Die PC-Anwendung fordert den Benutzer zur Eingabe einer mathematischen Berechnung auf (z.B. "34 * 72").
Die Eingabe wird auf ihre Gültigkeit überprüft und an den Arduino über die serielle Schnittstelle gesendet.
Der Arduino führt die Berechnung aus und gibt das Ergebnis zurück, das sowohl auf dem LCD (falls angeschlossen) als auch auf der Konsole angezeigt wird. Bei Fehlern wird eine Fehlermeldung ausgegeben.
Dieses System ermöglicht eine einfache, aber robuste Interaktion zwischen PC und Arduino zur Durchführung von Berechnungen in Echtzeit, wobei das LCD-Display optional eine visuelle Rückmeldung liefert.
