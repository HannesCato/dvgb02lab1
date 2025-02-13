Detta projekt är en enkel implementation av en tidsserver och en klient. Kommunikationen sker vid UDP RFC868 formatet för tidsprotokoll. 
Servern skickar tillbaka tiden till klienten som konverterar det och gör det läsbart 

Kör server och klient
1. Kör make för att kompilera
2. Kör sudo ./timeserver om port 37 används
3. Kör sudo ./client för att köra klienten 
