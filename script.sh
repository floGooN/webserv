#!/bin/bash

SERVER="localhost:8000"
SERVER2="localhost:3030"
SERVER3="localhost:8080"
NUM_CLIENTS=5  # Nombre de clients à simuler

# Liste des User-Agents pour simuler différents navigateurs
USER_AGENTS=(
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/108.0.0.0 Safari/537.36"
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Firefox/106.0"
    "Mozilla/5.0 (iPhone; CPU iPhone OS 15_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Safari/15.2"
    "Mozilla/5.0 (Linux; Android 12; SM-G991B) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.0.0 Mobile Safari/537.36"
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Opera/90.0.0.0"
)

# Fonction pour envoyer une requête HTTP
send_request() {
    local user_agent="$1"
    curl -s -A "$user_agent" "$SERVER"  &
	curl -s -A "$user_agent" "$SERVER2" &
	curl -s -A "$user_agent" "$SERVER3"
}

echo "📡 Envoi de $NUM_CLIENTS requêtes à $SERVER..."

# Boucle pour lancer plusieurs requêtes en parallèle
for ((i=0; i<NUM_CLIENTS; i++)); do
    send_request ;
done

# Attendre la fin de toutes les requêtes
wait

echo "✅ Toutes les requêtes ont été envoyées !"
