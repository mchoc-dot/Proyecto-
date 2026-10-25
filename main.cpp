#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

const int MAX = 9;
const int MIN_TAM = 3;
const int MINAS = 3;
const int MAX_MOV = MAX * MAX;
const string ARCHIVO = "historial.txt";

const char MINA = '*';
const char SIN_MINA = '.';
const char CERRADA = '?';
const char ABIERTA = '+';

bool leerEntero(const string &mensaje, int &valor) {
    string linea;
    cout << mensaje;
    if (!getline(cin, linea)) {
        cout << "\nFin de la entrada. Hasta luego." << endl;
        exit(0);
    }
    istringstream ss(linea);
    string resto;
    if (!(ss >> valor)) return false;
    if (ss >> resto) return false;
    return true;
}

int pedirDimension(const string &nombre) {
    int valor;
    while (true) {
        cout << "Numero de " << nombre << " (" << MIN_TAM << "-" << MAX << ")";
        if (!leerEntero(": ", valor)) {
            cout << "Error: escriba un numero, no letras." << endl;
        } else if (valor < MIN_TAM || valor > MAX) {
            cout << "Error: el tamano debe estar entre " << MIN_TAM << " y " << MAX << "." << endl;
        } else {
            return valor;
        }
    }
}

void pedirTamano(int &filas, int &columnas) {
    filas = pedirDimension("filas");
    columnas = pedirDimension("columnas");
}

void crearMatriz(char m[][MAX], int filas, int columnas, char simbolo) {
    for (int i = 0; i < filas; i++)
        for (int j = 0; j < columnas; j++)
            m[i][j] = simbolo;
}

void colocarMinas(char minas[][MAX], int filas, int columnas) {
    int colocadas = 0;
    while (colocadas < MINAS) {
        int f = rand() % filas;
        int c = rand() % columnas;
        if (minas[f][c] == MINA) continue;
        minas[f][c] = MINA;
        colocadas++;
    }
}

void mostrarTablero(const char m[][MAX], int filas, int columnas) {
    cout << "\n    ";
    for (int j = 0; j < columnas; j++) cout << j << " ";
    cout << endl;
    for (int i = 0; i < filas; i++) {
        cout << "  " << i << " ";
        for (int j = 0; j < columnas; j++) cout << m[i][j] << " ";
        cout << endl;
    }
    cout << endl;
}

void mostrarMovimientos(const int movFila[], const int movCol[], const char movRes[], int total) {
    if (total == 0) return;
    cout << "Movimientos:" << endl;
    for (int i = 0; i < total; i++)
        cout << "  mov " << i + 1 << " (" << movFila[i] << "," << movCol[i] << ") - "
             << movRes[i] << endl;
    cout << endl;
}

void revelarMinas(char tablero[][MAX], const char minas[][MAX], int filas, int columnas) {
    for (int i = 0; i < filas; i++)
        for (int j = 0; j < columnas; j++)
            if (minas[i][j] == MINA) tablero[i][j] = MINA;
}

bool dentroDelTablero(int f, int c, int filas, int columnas) {
    return f >= 0 && f < filas && c >= 0 && c < columnas;
}

bool abrirCasilla(char tablero[][MAX], const char minas[][MAX], int f, int c) {
    if (minas[f][c] == MINA) {
        tablero[f][c] = MINA;
        return true;
    }
    tablero[f][c] = ABIERTA;
    return false;
}

bool hayVictoria(int abiertas, int filas, int columnas) {
    return abiertas == filas * columnas - MINAS;
}

void guardarHistorial(int filas, int columnas, bool gano, int intentos) {
    ofstream archivo(ARCHIVO.c_str(), ios::app);
    if (!archivo.is_open()) {
        cout << "Aviso: no se pudo guardar el historial." << endl;
        return;
    }
    archivo << filas << " " << columnas << " " << (gano ? "gano" : "perdio")
            << " " << intentos << "\n";
}

void mostrarResumen() {
    int total = 0, ganadas = 0, perdidas = 0;
    ifstream archivo(ARCHIVO.c_str());
    if (archivo.is_open()) {
        int f, c, intentos;
        string resultado;
        while (archivo >> f >> c >> resultado >> intentos) {
            if (resultado == "gano") { ganadas++; total++; }
            else if (resultado == "perdio") { perdidas++; total++; }
        }
    }
    cout << "Partidas: " << total << " | Ganadas: " << ganadas
         << " | Perdidas: " << perdidas << endl;
}

void jugarPartida() {
    int filas, columnas;
    char minas[MAX][MAX];
    char tablero[MAX][MAX];
    int movFila[MAX_MOV];
    int movCol[MAX_MOV];
    char movRes[MAX_MOV];

    pedirTamano(filas, columnas);
    crearMatriz(minas, filas, columnas, SIN_MINA);
    crearMatriz(tablero, filas, columnas, CERRADA);
    colocarMinas(minas, filas, columnas);

    int intentos = 0;
    int abiertas = 0;

    while (true) {
        mostrarTablero(tablero, filas, columnas);
        mostrarMovimientos(movFila, movCol, movRes, intentos);

        int f;
        while (true) {
            if (!leerEntero("Fila (-1 = salir, -2 = ver minas): ", f)) {
                cout << "Error: escriba un numero, no letras." << endl;
            } else if (f == -2) {
                cout << "Matriz de minas (solo para probar):" << endl;
                mostrarTablero(minas, filas, columnas);
            } else {
                break;
            }
        }
        if (f == -1) {
            cout << "Juego terminado. No se guarda en el historial." << endl;
            return;
        }

        int c;
        while (!leerEntero("Columna: ", c)) {
            cout << "Error: escriba un numero, no letras." << endl;
        }

        if (!dentroDelTablero(f, c, filas, columnas)) {
            cout << "Error: la posicion esta fuera del tablero." << endl;
            continue;
        }
        if (tablero[f][c] != CERRADA) {
            cout << "Esa casilla ya esta abierta. Elija otra." << endl;
            continue;
        }

        bool habiaMina = abrirCasilla(tablero, minas, f, c);
        movFila[intentos] = f;
        movCol[intentos] = c;
        movRes[intentos] = tablero[f][c];
        intentos++;

        if (habiaMina) {
            cout << "\nPisaste una mina" << endl;
            revelarMinas(tablero, minas, filas, columnas);
            mostrarTablero(tablero, filas, columnas);
            mostrarMovimientos(movFila, movCol, movRes, intentos);
            guardarHistorial(filas, columnas, false, intentos);
            return;
        }

        abiertas++;
        if (hayVictoria(abiertas, filas, columnas)) {
            cout << "\nGanaste" << endl;
            revelarMinas(tablero, minas, filas, columnas);
            mostrarTablero(tablero, filas, columnas);
            mostrarMovimientos(movFila, movCol, movRes, intentos);
            guardarHistorial(filas, columnas, true, intentos);
            return;
        }
    }
}

int main() {
    srand(time(0));
    cout << "===== BUSCAMINAS =====" << endl;
    mostrarResumen();
    jugarPartida();
    system("pause");
    return 0;
}
