#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <limits>

using namespace std;

struct person_node {
    int id;
    string name;
    string last_name;
    char gender; 
    int age;
    int id_father;
    bool is_dead;
    bool was_king;
    bool is_king;

    person_node* parent;
    person_node* first_child;   
    person_node* second_child;  

    person_node() {
        id = 0;
        gender = 'H';
        age = 0;
        id_father = 0;
        is_dead = false;
        was_king = false;
        is_king = false;
        parent = nullptr;
        first_child = nullptr;
        second_child = nullptr;
    }
};

void trim(string &s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t')) {
        s.pop_back();
    }
}

bool cargar_csv(const string &ruta, map<int, person_node*> &nodos, person_node* &root, person_node* &current_king) {
    ifstream file(ruta);
    if (!file.is_open()) {
        cout << "no se pudo abrir el archivo csv: " << ruta << endl;
        return false;
    }

    string line;
    
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

        person_node* p = new person_node();
        p->id = stoi(cols[0]);
        p->name = cols[1];
        p->last_name = cols[2];
        if (!cols[3].empty()) {
            p->gender = cols[3][0];
        }
        p->age = stoi(cols[4]);
        p->id_father = stoi(cols[5]);
        p->is_dead = (cols[6] == "1");
        p->was_king = (cols[7] == "1");
        p->is_king = (cols[8] == "1");

        nodos[p->id] = p;
    }

    root = nullptr;
    for (auto &par : nodos) {
        person_node* p = par.second;
        if (p->id_father == 0) {
            root = p;
        } else {
            auto it = nodos.find(p->id_father);
            if (it != nodos.end()) {
                person_node* padre = it->second;
                p->parent = padre;
                if (padre->first_child == nullptr) {
                    padre->first_child = p;
                } else if (padre->second_child == nullptr) {
                    padre->second_child = p;
                } else {
                
                    cout << "advertencia: padre con mas de dos hijos, id padre: " << padre->id << endl;
                }
            }
        }
        if (p->is_king) {
            current_king = p;
        }
    }

    if (root == nullptr) {
        cout << "no se encontro raiz (primer rey)" << endl;
        return false;
    }

    if (current_king == nullptr) {
        
        current_king = root;
        current_king->is_king = true;
    }

    return true;
}

void pre_order(person_node* node, vector<person_node*> &lista) {
    if (node == nullptr) return;
    lista.push_back(node);
    pre_order(node->first_child, lista);
    pre_order(node->second_child, lista);
}

void imprimir_persona(person_node* p, bool marcar_rey = false) {
    if (!p) return;
    cout << "id: " << p->id
         << " | nombre: " << p->name << " " << p->last_name
         << " | genero: " << p->gender
         << " | edad: " << p->age
         << " | vivo: " << (p->is_dead ? "no" : "si")
         << " | fue_rey: " << (p->was_king ? "si" : "no")
         << " | es_rey: " << (p->is_king ? "si" : "no");
    if (marcar_rey) cout << "  <-- rey actual";
    cout << endl;
}

void mostrar_arbol_simple(person_node* node, int nivel = 0) {
    if (node == nullptr) return;
    for (int i = 0; i < nivel; i++) cout << "  ";
    cout << "- " << node->name << " " << node->last_name
         << " (id " << node->id << ", genero " << node->gender << ", edad " << node->age
         << ", vivo " << (node->is_dead ? "no" : "si") << ", rey " << (node->is_king ? "si" : "no") << ")"
         << endl;
    mostrar_arbol_simple(node->first_child, nivel + 1);
    mostrar_arbol_simple(node->second_child, nivel + 1);
}

void obtener_lista_despues_de_rey(person_node* root, person_node* current_king, vector<person_node*> &despues) {
    vector<person_node*> todos;
    pre_order(root, todos);
    bool pasado_rey = false;
    for (auto p : todos) {
        if (p == current_king) {
            pasado_rey = true;
            continue;
        }
        if (!pasado_rey) continue;
        if (p->is_dead) continue;
        despues.push_back(p);
    }
}

person_node* encontrar_siguiente_rey(person_node* root, person_node* current_king) {
    vector<person_node*> despues;
    obtener_lista_despues_de_rey(root, current_king, despues);

    person_node* candidato_hombre = nullptr;
    for (auto p : despues) {
        if (p->gender == 'H' && !p->is_dead && p->age < 70) {
            candidato_hombre = p;
            break;
        }
    }

    if (candidato_hombre != nullptr) {
        return candidato_hombre;
    }

    person_node* candidata_mujer = nullptr;
    for (auto p : despues) {
        if (p->gender == 'M' && !p->is_dead && p->age > 15) {
            if (candidata_mujer == nullptr) {
                candidata_mujer = p;
            } else {
                if (p->age < candidata_mujer->age) {
                    candidata_mujer = p;
                }
            }
        }
    }

    return candidata_mujer;
}

void mostrar_linea_sucesion(person_node* root, person_node* current_king) {
    vector<person_node*> despues;
    obtener_lista_despues_de_rey(root, current_king, despues);

    cout << "rey actual:" << endl;
    imprimir_persona(current_king, true);
    cout << endl;

    cout << "linea de sucesion (solo vivos, en orden genealogico despues del rey):" << endl;
    int pos = 1;
    for (auto p : despues) {
        if (!p->is_dead) {
            cout << pos << ". ";
            imprimir_persona(p);
            pos++;
        }
    }
    if (pos == 1) {
        cout << "no hay sucesores vivos" << endl;
    }
}

void cambiar_rey(person_node* root, person_node* &current_king, bool por_muerte) {
    if (current_king == nullptr) {
        cout << "no hay rey actual" << endl;
        return;
    }

    if (por_muerte) {
        current_king->is_dead = true;
        cout << "el rey " << current_king->name << " ha muerto" << endl;
    } else {
        cout << "el rey " << current_king->name << " ha pasado de 70 anos" << endl;
    }

    current_king->was_king = true;
    current_king->is_king = false;

    person_node* nuevo_rey = encontrar_siguiente_rey(root, current_king);

    if (nuevo_rey == nullptr) {
        cout << "no se encontro nuevo rey o reina" << endl;
        current_king = nullptr;
        return;
    }

    nuevo_rey->is_king = true;
    current_king = nuevo_rey;

    cout << "nuevo rey asignado automaticamente:" << endl;
    imprimir_persona(current_king, true);
}


person_node* buscar_por_id(map<int, person_node*> &nodos, int id) {
    auto it = nodos.find(id);
    if (it != nodos.end()) return it->second;
    return nullptr;
}

void editar_persona(map<int, person_node*> &nodos) {
    int id;
    cout << "ingrese id de la persona a editar: ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    person_node* p = buscar_por_id(nodos, id);
    if (!p) {
        cout << "no se encontro persona con ese id" << endl;
        return;
    }

    cout << "datos actuales:" << endl;
    imprimir_persona(p);

    cout << "ingrese nuevo nombre (enter para mantener): ";
    string nuevo;
    getline(cin, nuevo);
    if (!nuevo.empty()) p->name = nuevo;

    cout << "ingrese nuevo apellido (enter para mantener): ";
    getline(cin, nuevo);
    if (!nuevo.empty()) p->last_name = nuevo;

    cout << "ingrese nuevo genero (H/M, enter para mantener): ";
    string gen;
    getline(cin, gen);
    if (!gen.empty()) p->gender = gen[0];

    cout << "ingrese nueva edad (o -1 para mantener): ";
    int nueva_edad;
    cin >> nueva_edad;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (nueva_edad >= 0) p->age = nueva_edad;

    cout << "esta muerto? (0 no, 1 si, -1 mantener): ";
    int v;
    cin >> v;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (v == 0) p->is_dead = false;
    else if (v == 1) p->is_dead = true;

    cout << "fue rey? (0 no, 1 si, -1 mantener): ";
    cin >> v;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (v == 0) p->was_king = false;
    else if (v == 1) p->was_king = true;

    cout << "es rey? (0 no, 1 si, -1 mantener): ";
    cin >> v;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (v == 0) p->is_king = false;
    else if (v == 1) p->is_king = true;

    cout << "persona actualizada:" << endl;
    imprimir_persona(p);
}

int main() {
    string ruta_csv = "bin/family.csv";

    map<int, person_node*> nodos;
    person_node* root = nullptr;
    person_node* current_king = nullptr;

    if (!cargar_csv(ruta_csv, nodos, root, current_king)) {
        cout << "error cargando datos. verifique el archivo csv." << endl;
        return 1;
    }

    int opcion = 0;

    while (true) {
        cout << endl;
        cout << "===== menu reino =====" << endl;
        cout << "1. mostrar linea de sucesion" << endl;
        cout << "2. marcar muerte del rey actual y asignar nuevo rey" << endl;
        cout << "3. marcar que el rey actual pasa de 70 anos y asignar nuevo rey" << endl;
        cout << "4. editar datos de una persona" << endl;
        cout << "5. mostrar arbol simple" << endl;
        cout << "6. mostrar rey actual" << endl;
        cout << "7. salir" << endl;
        cout << "opcion: ";
        cin >> opcion;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (opcion == 1) {
            if (current_king == nullptr) {
                cout << "no hay rey actual" << endl;
            } else {
                mostrar_linea_sucesion(root, current_king);
            }
        } else if (opcion == 2) {
            if (current_king == nullptr) {
                cout << "no hay rey actual" << endl;
            } else {
                cambiar_rey(root, current_king, true);
            }
        } else if (opcion == 3) {
            if (current_king == nullptr) {
                cout << "no hay rey actual" << endl;
            } else {
                cambiar_rey(root, current_king, false);
            }
        } else if (opcion == 4) {
            editar_persona(nodos);
        } else if (opcion == 5) {
            mostrar_arbol_simple(root);
        } else if (opcion == 6) {
            if (current_king) imprimir_persona(current_king, true);
            else cout << "no hay rey actual" << endl;
        } else if (opcion == 7) {
            cout << "saliendo del programa..." << endl;
            break;
        } else {
            cout << "opcion invalida" << endl;
        }
    }


    for (auto &par : nodos) {
        delete par.second;
    }

    return 0;
}

