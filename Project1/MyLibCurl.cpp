#include "MyLibCurl.h"


namespace MyLibCurl {
    
    void SendPythonRequest(crow::websocket::connection& conn,  const std::string& document)//Funzione che permette di inviare al client tramite websocket un grafico inviato da parte del server python
    {
        curl_global_init(CURL_GLOBAL_ALL);//inizializza libcurl consentendo a libcurl di funzionare correttamente all'interno dell'applicazione
        std::string url = "http://localhost:5000/graph";//url server python
        CURL* curl = curl_easy_init();//Crea un oggetto di tipo puntatore a CURL , reppresenta una sessione curl
        if (curl) {
            
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());//Viene settato l'url per la curl
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");//Viene utilizzato per indicare il tipo di dati gestiti
            std::string response_str;
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);//set degli headers della chiamata curl
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, document.c_str());//Dati da inviare tramite post
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);// Imposta la funzione di callback per scrivere la risposta nella stringa
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_str);
            CURLcode res = curl_easy_perform(curl);//Esito della curl
            if (res == CURLE_OK) {
                              
                conn.send_binary(response_str);//Invio della risposta al client
            }
            else {
                conn.send_binary("errore");
                std::cout << "Server Python Offline"<<std::endl;
            }
            
            curl_easy_cleanup(curl);//Deallocazione dei vari puntatori relativi alla curl
            curl_slist_free_all(headers);
            curl_global_cleanup();
            
        }
        
        
    }
    size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data) {// Funzione di callback per scrivere i dati nella stringa
        size_t realsize = size * nmemb;
        data->append(ptr, realsize);
        return realsize;
    }

    void replaceSpaces(std::string& str) {//Funzione che rimpiazza gli spazi della stringa con %20 garantendo una formattazione adeguata 
        std::string replaceStr = "%20";
        std::string::size_type pos = 0;
        while ((pos = str.find(' ', pos)) != std::string::npos) {
            str.replace(pos, 1, replaceStr);
            pos += replaceStr.length();
        }
    }
    std::string gamesreturnValue(const std::string& url) {//Restituisce a differenza della SendPythonRequest una stringa in formato json leggible da parte del client non utilizza il websocket 

        CURL* curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            std::string response_str;
            std::ostringstream response_stream;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_str);

            CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            // Gestisce eventuali errori
            if (res == CURLE_OK) {

                return response_str;
            }
            

        }
        return "errore";
    }
}
