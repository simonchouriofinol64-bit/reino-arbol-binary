#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct person_node {
    int id;
    string name;
    string last_name;
    char gender; // 'H' o 'M'
    int age;
    int id_father;
    bool is_dead;
    bool was_king;
    bool is_king;
};

void trim(string &s) {
    while (!s.empty() && (s.back() == '\r'  s.back() == '\n'  s.back() == ' ' || s.back() == '\t')) {
        s.pop_back();
    }
}

bool cargar_csv_basico(const string &ruta, vector<person_node> &lista) {
    ifstream file(ruta);
    if (!file.is_open()) {
        cout << "no se pudo abrir el archivo csv: " << ruta << endl;
        return false;
    }

    string line;

    // leer encabezado
    if (!getline(file, line)) {
        cout << "csv vacio" << endl;
        return false;
    }

    while (getline(file, line)) {
        trim(line);
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        vector<string> cols;

        while (getline(ss, token, ',')) {
            trim(token);
            cols.push_back(token);
        }

        if (cols.size() < 9) {
            cout << "linea invalida en csv: " << line << endl;
            continue;
        }

        person_node p;
        p.id = stoi(cols[0]);
        p.name = cols[1];
        p.last_name = cols[2];
        p.gender = cols[3].empty() ? 'H' : cols[3][0];
        p.age = stoi(cols[4]);
        p.id_father = stoi(cols[5]);
        p.is_dead = (cols[6] == "1");
        p.was_king = (cols[7] == "1");
        p.is_king = (cols[8] == "1");

        lista.push_back(p);
    }

    return true;
}

void imprimir_persona(const person_node &p) {
    cout << "id: " << p.id
         << " | nombre: " << p.name << " " << p.last_name
         << " | genero: " << p.gender
         << " | edad: " << p.age
         << " | vivo: " << (p.is_dead ? "no" : "si")
         << " | fue_rey: " << (p.was_king ? "si" : "no")
         << " | es_rey: " << (p.is_king ? "si" : "no")
         << endl;
}

int main() {
    string ruta_csv = "bin/family.csv";
    vector<person_node> personas;

    cout << "cargando datos desde " << ruta_csv << " ..." << endl;

    if (!cargar_csv_basico(ruta_csv, personas)) {
        cout << "no se pudieron cargar los datos" << endl;
        return 1;
    }

    cout << "datos cargados. listado de personas:" << endl;

    for (size_t i = 0; i < personas.size(); i++) {
        imprimir_persona(personas[i]);
    }

    cout << "fin de la version de lectura basica" << endl;

    return 0;
}
