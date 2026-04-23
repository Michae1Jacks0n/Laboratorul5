#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>
using namespace std;

class Utilizator;             
Utilizator* gasesteUser(int id);

struct Mesaj {
    int expeditorId;
    string text;
    string timp;
};

struct Conversatie {
    int idDestinatar;
    vector<Mesaj> mesaje;
};

class Utilizator {
public:
    int id;
    string nume;
    vector<Conversatie> conversatii;

    Utilizator(int id, string nume) {
        this->id = id;
        this->nume = nume;
    }

    Conversatie* gasesteConversatie(int idDest) {
        for (auto &c : conversatii) {
            if (c.idDestinatar == idDest)
                return &c;
        }
        return NULL;
    }

    void creareConversatie(int idDest) {
        if (gasesteConversatie(idDest) == NULL) {
            Conversatie c;
            c.idDestinatar = idDest;
            conversatii.push_back(c);
        }
    }

    void adaugaMesaj(int idDest, string mesaj, int expeditorReal) {
        Conversatie* c = gasesteConversatie(idDest);
        if (c == NULL) {
            creareConversatie(idDest);
            c = gasesteConversatie(idDest);
        }

        Mesaj m;
        m.expeditorId = expeditorReal;
        m.text = mesaj;

        time_t now = time(0);
        struct tm* tm_info = localtime(&now);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H:%M:%S", tm_info);
        m.timp = buffer;

        c->mesaje.push_back(m);
    }

    void afisareConversatie(int idDest, int idCurent) {
        Conversatie* c = gasesteConversatie(idDest);
        if (c == NULL) {
            cout << "Nu exista o conversatie" << endl;
            return;
        }

        cout << endl << "------------------------Conversatia" << endl;

        for (auto m : c->mesaje) {
            if (m.expeditorId == idCurent) {
                cout << setw(60) << ("Tu: " + m.text) << endl;
            } else {
                Utilizator* u = gasesteUser(m.expeditorId);

                if (u)
                    cout << u->nume << ": " << m.text << endl;
                else
                    cout << "User necunoscut: " << m.text << endl;
            }
        }
    }
};

vector<Utilizator*> utilizatori;

Utilizator* gasesteUser(int id) {
    for (auto u : utilizatori)
        if (u->id == id)
            return u;
    return NULL;
}

void salveaza() {
    ofstream f("utilizatori.txt");

    for (auto u : utilizatori) {
        f << u->id << " " << u->nume << endl;

        for (auto c : u->conversatii) {
            f << "C " << c.idDestinatar << endl;

            for (auto m : c.mesaje) {
                f << "M " << m.expeditorId << " " << m.timp << " " << m.text << endl;
            }
        }

        f << "END" << endl;
    }

    f.close();
}

void incarca() {
    ifstream f("utilizatori.txt");
    if (!f) return;

    int id;
    string nume;

    while (f >> id >> nume) {
        Utilizator* u = new Utilizator(id, nume);
        string tip;

        while (f >> tip) {
            if (tip == "END") break;

            if (tip == "C") {
                int idDest;
                f >> idDest;

                Conversatie c;
                c.idDestinatar = idDest;

                while (f >> tip) {
                    if (tip != "M") {
                        for (int i = tip.size() - 1; i >= 0; i--)
                            f.putback(tip[i]);
                        break;
                    }

                    Mesaj m;
                    f >> m.expeditorId;
                    f >> m.timp;
                    getline(f, m.text);
                    c.mesaje.push_back(m);
                }

                u->conversatii.push_back(c);
            }
        }

        utilizatori.push_back(u);
    }

    f.close();
}

int main() {
    incarca();

    int o;
    Utilizator* curent = NULL;

    do {
        cout << "1 - Inregistrare\n2 - Autentificare\n3 - Trimite mesaj\n4 - Vizualizare conversatie\n5 - Sterge cont\n0 - Exit\n";
        cin >> o;

        if (o == 1) {
            int id;
            string nume;
            cout << "ID: "; cin >> id;

            if (gasesteUser(id)) {
                cout << "ID existent" << endl;
            } else {
                cout << "Nume: "; cin >> nume;
                utilizatori.push_back(new Utilizator(id, nume));
                salveaza();
            }
        }

        else if (o == 2) {
            int id;
            cout << "ID: "; cin >> id;

            curent = gasesteUser(id);
            if (!curent) cout << "Nu exista asa ID" << endl;
            else cout << "Autentificat" << endl;
        }

        else if (o == 3) {
            if (!curent) {
                cout << "Nu esti autentificat" << endl;
            } else {
                int idDest;
                string msg;
                cout << "Destinatar ID: ";
                cin >> idDest;
                Utilizator* dest = gasesteUser(idDest);
                if (!dest) {
                    cout << "User inexistent" << endl;
                } else if (dest->id == curent->id) {
                    cout << "Nu poti trimite mesaj tie insuti" << endl;
                } else {
                    cin.ignore();
                    cout << "Mesajul: ";
                    getline(cin, msg);
                    if (msg.empty()) {
                        cout << "Mesaj gol" << endl;
                    } else {
                        curent->adaugaMesaj(idDest, msg, curent->id);
                        dest->adaugaMesaj(curent->id, msg, curent->id);
                        salveaza();
                    }
                }
            }
        }

        else if (o == 4) {
            if (!curent) {
                cout << "Nu esti autentificat" << endl;
            } else {
                int idDest;
                cout << "ID destinatar: ";
                cin >> idDest;
                curent->afisareConversatie(idDest, curent->id);
            }
        }

        else if (o == 5) {
            if (!curent) {
                cout << "Nu esti autentificat" << endl;
            } else {
                for (int i = 0; i < utilizatori.size(); i++) {
                    if (utilizatori[i]->id == curent->id) {
                        delete utilizatori[i];
                        utilizatori.erase(utilizatori.begin() + i);
                        break;
                    }
                }
                curent = NULL;
                salveaza();
                cout << "Deleted" << endl;
            }
        }

    } while (o != 0);

    return 0;
}
