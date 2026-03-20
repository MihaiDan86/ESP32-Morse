📡 ESP32 Morse Code Web Controller

Acest proiect transformă un ESP32 într-un server web interactiv care transmite mesaje în cod Morse folosind un LED, controlat în timp real din browser.

🚀 Funcționalități

🌐 Server web pe ESP32 (fără backend extern)

💬 Trimitere mesaj direct din browser

⚡ Control viteză transmisie (dot time)

🔵 Redare cod Morse prin LED

📊 Feedback live în interfață (progres caracter cu caracter)

🔄 Actualizare în timp real prin WebSocket

🧠 Cum funcționează

ESP32 se conectează la WiFi

Pornește un server web pe portul 80

Browserul se conectează prin WebSocket (/ws)

Utilizatorul introduce un mesaj + viteză

ESP32:

convertește textul în cod Morse

aprinde/stinge LED-ul conform semnalului

trimite progresul înapoi către browser

🛠️ Hardware necesar

ESP32

LED (sau LED onboard – GPIO 2)

Rezistor (220Ω recomandat)

🔌 Conexiuni
Componentă	Pin ESP32
LED	GPIO 2
📦 Librării necesare

Instalează următoarele librării în Arduino IDE:

WiFi.h

ESPAsyncWebServer

AsyncTCP

⚙️ Configurare

Modifică datele WiFi în cod:

const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";
▶️ Utilizare

Încarcă codul pe ESP32

Deschide Serial Monitor

Copiază IP-ul afișat (ex: 192.168.1.100)

Accesează IP-ul în browser

Introdu mesajul și apasă Trimite

🖥️ Interfață Web

Input pentru mesaj

Slider pentru viteză (50ms – 500ms)

Highlight live al caracterului transmis:

🟢 activ

⚪ transmis

🔤 în așteptare

🔤 Cod Morse suportat

Litere: A–Z

Numere: 0–9

Spațiu între cuvinte

⚡ Task-uri FreeRTOS

Proiectul folosește multitasking:

morseTask rulează pe un thread separat

Permite întreruperea mesajului în timp real

🔄 Protocol WebSocket

Mesaj trimis către ESP32:

TEXT|VITEZA

Exemplu:

HELLO|200

Mesaje trimise către browser:

POS:x → poziția curentă

DONE → finalizare

🧪 Exemplu

Input:

SOS

Output LED:

... --- ...
💡 Posibile îmbunătățiri

🔊 Buzzer pentru sunet Morse

📱 UI responsive pentru mobil

💾 salvare mesaje favorite

🌍 suport pentru mai multe limbi

📡 control din internet (port forwarding / cloud)

📜 Licență

MIT License – liber de utilizat și modificat

🤝 Contribuții

Pull request-urile sunt binevenite!
Dacă ai idei sau îmbunătățiri, deschide un issue 🚀

👨‍💻 Autor

Proiect realizat cu ESP32 + WebSockets + FreeRTOS ❤️
