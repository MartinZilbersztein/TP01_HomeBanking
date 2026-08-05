#include <iostream>
#include <iomanip>
#include <ctime>
#include <windows.h>
#include <fstream>
#include <string>

#define record struct

using namespace std;

typedef char str25[26];
typedef char str24[25];
typedef char str5[6];
typedef char str3[4];

const int LINEAS = 25;
const int COLS = 80;
const int COTIZACION_USD = 1500;

enum Colores
{
  NEGRO = 0,
  AZUL = 1,
  VERDE = 2,
  CYAN = 3,
  ROJO = 4,
  MAGENTA = 5,
  AMARILLO = 6,
  BLANCO = 7,
  GRIS = 8,
  AZUL_CLARO = 9,
  VERDE_CLARO = 10,
  CYAN_CLARO = 11,
  ROJO_CLARO = 12,
  MAGENTA_CLARO = 13,
  AMARILLO_CLARO = 14,
  BLANCO_BRILLANTE = 15
};

enum Divisas
{
  USD,
  ARS
};
str3 divisasSimbolos[2] = {"u$d", "$"};

enum TipoMov
{
  DEBE = 'D',
  HABER = 'H'
};

enum MetodosTransferencia
{
  CBU,
  CELULAR
};

struct sUsuario
{
  int DNI;
  string nombre;
  int fechaNacimiento;
  string usuario;
  string clave;
  long long numeroCelular;
  string correoElectronico;
  string direccion;
  int numeroCuentaCA;
  long long CBU;
};

struct sMovimientoCA
{
  short dia;
  short mes;
  short anio;
  char tipoMov;
  str25 detalle;
  float importe;
};

struct sMovimientoTD
{
  short dia;
  short mes;
  short anio;
  str25 detalle;
  float importe;
};

struct sMovimientoTC
{
  short dia;
  short mes;
  short anio;
  str25 detalle;
  str5 cuota;
  float importe;
};

short busBinVecDNI(sUsuario vec[], int dim, int dni)
{
  int izq = 0;
  int der = dim - 1;
  while (izq <= der)
  {
    int medio = izq + (der - izq) / 2;
    if (vec[medio].DNI == dni)
      return medio;
    else if (vec[medio].DNI < dni)
      izq = medio + 1;
    else
      der = medio - 1;
  }
  return -1;
}

namespace Screen
{

  void setConsoleColor(WORD colText, WORD colBack)
  {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colText + 16 * colBack);
  }

  void _window(short izq, short sup, short der, short inf)
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SMALL_RECT window = csbi.srWindow;

    window.Left = izq - 1;
    window.Top = sup - 1;
    window.Right = der - 1;
    window.Bottom = inf - 1;
  }

  void _gotoxy(short x, short y)
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SMALL_RECT window = csbi.srWindow;
    COORD coord;

    coord.X = window.Left + x;
    coord.Y = window.Top + y;
    if (coord.X <= window.Right && coord.Y <= window.Bottom)
      SetConsoleCursorPosition(hConsole, coord);
  }

  int _wherex()
  {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
      return csbi.dwCursorPosition.X;
    return -1;
  }

  int _wherey()
  {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
      return csbi.dwCursorPosition.Y;
    return -1;
  }

  void ActualizaColores(WORD colTextAct, WORD colBackAc)
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD atributo = colTextAct | (colBackAc << 4);
    SetConsoleTextAttribute(hConsole, atributo);
  }

  WORD ObtenerColorTextoActual()
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
      return csbi.wAttributes & 0x000F;
    }
    return 0;
  }

  WORD ObtenerColorFondo()
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
      return (csbi.wAttributes >> 4) & 0x0F;
    }
    return -1;
  }

  void _textcolor(WORD color)
  {
    ActualizaColores(color, ObtenerColorFondo());
  }

  void _textbackground(WORD color)
  {
    ActualizaColores(ObtenerColorTextoActual(), color);
  }

  void _clrscr()
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SMALL_RECT window = csbi.srWindow;
    DWORD escritos;
    WORD atributo = ObtenerColorTextoActual() | (ObtenerColorFondo() << 4);
    int ancho = window.Right - 3 - window.Left;

    window.Left += 2;
    for (short y = window.Top + 2; y <= window.Bottom - 1; ++y)
    {
      COORD inicio_fila = {window.Left, y};
      FillConsoleOutputCharacter(hConsole, ' ', ancho, inicio_fila, &escritos);
      FillConsoleOutputAttribute(hConsole, atributo, ancho, inicio_fila, &escritos);
    }
    _gotoxy(1, 1);
  }

  void _clreol()
  {
    COORD coord;
    DWORD escrito;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(hStdOut, &info);

    coord = info.dwCursorPosition;
    int longitud = info.dwSize.X - coord.X;
    FillConsoleOutputCharacter(hStdOut, ' ', longitud, coord, &escrito);
    SetConsoleCursorPosition(hStdOut, coord);
  }

  void MnsgBox(short x, short y, string mensaje, char alinea, short ancho = 0)
  {
    _gotoxy(x, y);
    if (alinea == 'i')
      cout << setw(ancho) << left << mensaje;
    else
      cout << setw(ancho) << right << mensaje;
  }

  void MnsgBox(short x, short y, string mensaje)
  {
    _gotoxy(x, y);
    cout << mensaje;
  }

  void MnsgBox(short x, short y, short colorText, string mensaje)
  {
    _gotoxy(x, y);
    _textcolor(colorText);
    cout << mensaje;
  }

  void Marco(short x1, short y1, short x2, short y2, short colorTexto)
  {
    _textcolor(colorTexto);
    MnsgBox(x1, y1, "╔");
    MnsgBox(x2 - 1, y1, "╗");
    for (short i = 1; i < x2 - x1 - 1; i++)
      MnsgBox(x1 + i, y1, "═");
    for (short i = 1; i < y2 - y1 - 5; i++)
    {
      MnsgBox(x1, y1 + i, "║");
      MnsgBox(x2 - 1, y1 + i, "║");
    }
    MnsgBox(x1, y2 - 5, "╚");
    MnsgBox(x2 - 1, y2 - 5, "╝");
    for (short i = 1; i < x2 - x1 - 1; i++)
      MnsgBox(x1 + i, y2 - 5, "═");
  }

  string Separador(int ancho = 60, char car = '-')
  {
    return string(ancho, car);
  }

  void OcultarCursor()
  {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(consoleHandle, &info);
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
  }

  void MostrarCursor()
  {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(consoleHandle, &info);
    info.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &info);
  }

  void Espera(short tiempo)
  {
    _textcolor(7);
    MnsgBox(5, 20, "Espere " + to_string(tiempo / 1000) + " segundos");
    _textcolor(15);
    Sleep(tiempo);
    _gotoxy(5, 20);
    _clreol();
  }

  void BarraTitulo()
  {
    SetConsoleTitleA("Home Banking Haedo");
  }

  void Pausa(string mensg = "")
  {
    short i = 1;

    MnsgBox(12, 8, mensg);
    GetAsyncKeyState(VK_SPACE);
    while (not GetAsyncKeyState(VK_SPACE))
    {
      _textbackground(8);
      MnsgBox(5, 17, i, "Oprima la tecla");
      setConsoleColor(AZUL, VERDE);
      MnsgBox(21, 17, AMARILLO + 16 * VERDE, "ESPACIO");
      _textbackground(8);
      MnsgBox(29, 17, i++, "para continuar...");
      Sleep(1000);
      if (i == 16)
        i = 1;
    }
  }

  void BloquearCambioTamaño()
  {
    HWND hwnd = GetConsoleWindow();
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_MAXIMIZEBOX;
    style &= ~WS_SIZEBOX;
    SetWindowLong(hwnd, GWL_STYLE, style);
  }

  string Left(str25 cad, short cant)
  {
    return string(cad, cant);
  }

  int MenuNavegar(char aMenu[][25], short lIni, short lFin, short cIni)
  {
    int Menu = lIni;

    OcultarCursor();
    MnsgBox(6, 20, "Flecha ↑ ↓ para moverse sobre el menú");
    MnsgBox(3, 21, "Tecla ESCAPE seleccionar la opción del menú");
    _gotoxy(cIni, lIni);
    while (not GetAsyncKeyState(VK_ESCAPE))
    {
      Sleep(200);
      if (GetAsyncKeyState(VK_UP))
      {
        if (Menu == lIni)
        {
          _textcolor(15);
          _gotoxy(cIni, lIni);
          cout << aMenu[Menu - lIni];
          Menu = lFin;
          _textcolor(14);
          _gotoxy(cIni, lFin);
          cout << aMenu[Menu - lIni];
        }
        else
        {
          _textcolor(15);
          _gotoxy(cIni, _wherey());
          cout << aMenu[Menu - lIni];
          Menu--;
          _gotoxy(cIni, _wherey() - 1);
          _textcolor(14);
          cout << aMenu[Menu - lIni];
        }
      }
      else if (GetAsyncKeyState(VK_DOWN))
      {
        if (Menu == lFin)
        {
          _textcolor(15);
          _gotoxy(cIni, lFin);
          cout << aMenu[Menu - lIni];
          Menu = lIni;
          _gotoxy(cIni, lIni);
          _textcolor(14);
          cout << aMenu[Menu - lIni];
        }
        else
        {
          _textcolor(15);
          _gotoxy(cIni, _wherey());
          cout << aMenu[Menu - lIni];
          Menu++;
          _gotoxy(cIni, _wherey() + 1);
          _textcolor(14);
          cout << aMenu[Menu - lIni];
        }
      }
    }
    MostrarCursor();
    return Menu - lIni;
  }

}

namespace FechaHora
{
  long GetTime(int &hh, int &mm, int &ss)
  {
    time_t rawtime;
    record tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    hh = timeinfo->tm_hour;
    mm = timeinfo->tm_min;
    ss = timeinfo->tm_sec;
    return timeinfo->tm_hour * 10000 + timeinfo->tm_min * 100 + timeinfo->tm_sec;
  }

  long GetDate(int &year, int &mes, int &dia, int &ds)
  {
    time_t rawtime;
    record tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    year = 1900 + timeinfo->tm_year;
    mes = 1 + timeinfo->tm_mon;
    dia = timeinfo->tm_mday;
    ds = 1 + timeinfo->tm_wday;
    return (1900 + timeinfo->tm_year) * 10000 + (1 + timeinfo->tm_mon) * 100 + timeinfo->tm_mday;
  }

  int FechaHoy()
  {
    int año, mes, dia, dsem;

    GetDate(año, mes, dia, dsem);
    cout << "Dia: " << setw(2) << dia << " Mes: " << setw(2) << mes
         << " Año:" << ' ' << setw(2) << año << endl;
    return dia * 10000 + mes * 100 + año;
  }

}

namespace EntradaSalida
{
  void mostrarTexto(string mensaje)
  {
    cout << mensaje << endl;
  }
  void obtenerEntrada(string &entrada, short x, short y, string mensaje)
  {
    Screen::MnsgBox(x, y, mensaje);
    getline(cin, entrada);
  }
  void obtenerEntrada(double &entrada, short x, short y, string mensaje)
  {
    Screen::MnsgBox(x, y, mensaje);
    cin >> entrada;
    cin.ignore();
  }
  void obtenerEntrada(int &entrada, short x, short y, string mensaje)
  {
    double entradaEntera;
    obtenerEntrada(entradaEntera, x, y, mensaje);
    entrada = entradaEntera;
  }

  void limpiarEstadoTeclas()
  {
    while (GetAsyncKeyState(VK_UP) & 0x8000 ||
           GetAsyncKeyState(VK_DOWN) & 0x8000 ||
           GetAsyncKeyState(VK_ESCAPE) & 0x8000 ||
           GetAsyncKeyState(VK_RETURN) & 0x8000 ||
           GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
      Sleep(20);
    }
  }

  void limpiarBufferEntrada()
  {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
  }
}

using namespace Screen;
using namespace EntradaSalida;

namespace Archivos
{
  void leerCA(sMovimientoCA vrMovimientosCA[], int &cardMovCA, int maxMovCA)
  {
    string nombreArchivo = "MovimientosCA.txt";
    ifstream archivo(nombreArchivo);

    int i = 0;

    while (archivo >> vrMovimientosCA[i].dia && i < maxMovCA)
    {
      archivo >> vrMovimientosCA[i].mes;
      archivo >> vrMovimientosCA[i].anio;
      archivo >> vrMovimientosCA[i].tipoMov;
      archivo.ignore();
      archivo.get(vrMovimientosCA[i].detalle, 25);
      archivo.ignore();
      archivo >> vrMovimientosCA[i].importe;
      archivo.ignore();

      i++;
    }

    archivo.close();

    cardMovCA = i;
  }

  void escribirCA(sMovimientoCA rMovimientoCA, char moneda)
  {
    string nombreArchivo = "MovimientosCA.txt";
    ofstream archivo(nombreArchivo, ios::app);

    archivo << endl;
    archivo << setw(2) << rMovimientoCA.dia;
    archivo << " " << setw(2) << rMovimientoCA.mes;
    archivo << " " << setw(4) << rMovimientoCA.anio;
    archivo << " " << setw(1) << rMovimientoCA.tipoMov;
    archivo << " " << left << setw(25) << rMovimientoCA.detalle << right;
    archivo << " " << setw(10) << rMovimientoCA.importe;

    archivo.close();
  }

  void agregarMovimientoVRCA(sMovimientoCA vrMovimientosCA[], int &cardMovCA, int i,
                             short dia, short mes, short anio, char tipoMov, str25 detalle, float importe)
  {
    sMovimientoCA rMovimientoCA;
    rMovimientoCA.dia = dia;
    rMovimientoCA.mes = mes;
    rMovimientoCA.anio = anio;
    rMovimientoCA.tipoMov = tipoMov;
    strcpy(rMovimientoCA.detalle, detalle);
    rMovimientoCA.importe = importe;

    vrMovimientosCA[cardMovCA + i] = rMovimientoCA;
  }

  void leerTD(sMovimientoCA vrMovCA[], int &cardMovCA, int maxMovCA, sMovimientoTD vrMovTD[], int &cardMovTD)
  {
    sMovimientoCA rMovimientoCA;
    string nombreArchivo = "MovimientosTD.txt";
    ifstream archivo(nombreArchivo);

    int i = 0;

    while (archivo >> vrMovTD[i].dia && cardMovCA + i < maxMovCA)
    {
      archivo >> vrMovTD[i].mes;
      archivo >> vrMovTD[i].anio;
      archivo.ignore();
      archivo.get(vrMovTD[i].detalle, 26);
      archivo.ignore();
      archivo >> vrMovTD[i].importe;
      archivo.ignore();

      agregarMovimientoVRCA(
          vrMovCA, cardMovCA, i,
          vrMovTD[i].dia, vrMovTD[i].mes, vrMovTD[i].anio, DEBE, vrMovTD[i].detalle, vrMovTD[i].importe);

      i++;
    }

    archivo.close();

    cardMovTD = i;
    cardMovCA += i;
  }

  void leerTC(sMovimientoCA vrMovCA[], int &cardMovCA, int maxMovCA, sMovimientoTC vrMovTC[], int &cardMovTC)
  {
    sMovimientoCA rMovimientoCA;
    string nombreArchivo = "MovimientosTC.txt";
    ifstream archivo(nombreArchivo);

    int i = 0;
    int posMovCA = cardMovCA;

    while (archivo >> vrMovTC[i].dia && cardMovCA + i < maxMovCA)
    {
      archivo >> vrMovTC[i].mes;
      archivo >> vrMovTC[i].anio;
      archivo.ignore();
      archivo.get(vrMovTC[i].detalle, 25);
      archivo.ignore();
      archivo.get(vrMovTC[i].cuota, 5);
      archivo.ignore();

      archivo >> vrMovTC[i].importe;

      posMovCA = cardMovCA + i;

      agregarMovimientoVRCA(
          vrMovCA, cardMovCA, i,
          vrMovTC[i].dia, vrMovTC[i].mes, vrMovTC[i].anio, DEBE, vrMovTC[i].detalle, vrMovTC[i].importe);

      i++;
    }

    archivo.close();

    cardMovTC = i;
    cardMovCA += i;
  }
}

namespace Ordenar
{
  void agregarEnOrden(string vector[], string elem, short &card)
  {
    while (card > 1 && elem < vector[card - 1])
    {
      vector[card] = vector[card - 1];
      card--;
    }
    vector[card] = elem;
  }

  void agregarEnOrdenCA(sMovimientoCA vrMovCA[], sMovimientoCA elem, short &card)
  {
    int fechaElem = elem.anio * 10000 + elem.mes * 100 + elem.dia;

    while (card > 1 && fechaElem < vrMovCA[card - 1].anio * 10000 + vrMovCA[card - 1].mes * 100 + vrMovCA[card - 1].dia)
    {
      vrMovCA[card] = vrMovCA[card - 1];
      card--;
    }
    vrMovCA[card] = elem;
  }

  void ordXBurCA(sMovimientoCA vrMovimientosCA[], int card)
  {
    bool ordenado = false;
    int k = 0;
    int fecha1, fecha2;

    do
    {
      ordenado = true;
      k++;
      for (int i = 0; i < card - k; i++)
      {
        fecha1 = vrMovimientosCA[i].anio * 10000 +
                 vrMovimientosCA[i].mes * 100 +
                 vrMovimientosCA[i].dia;
        fecha2 = vrMovimientosCA[i + 1].anio * 10000 +
                 vrMovimientosCA[i + 1].mes * 100 +
                 vrMovimientosCA[i + 1].dia;
        if (fecha1 > fecha2)
        {
          swap(vrMovimientosCA[i], vrMovimientosCA[i + 1]);
          ordenado = false;
        }
      }
    } while (!ordenado);
  }

  void ordXBurTD(sMovimientoTD vrMovimientosTD[], int card)
  {
    bool ordenado = false;
    int k = 0;
    int fecha1, fecha2;

    do
    {
      ordenado = true;
      k++;
      for (int i = 0; i < card - k; i++)
      {
        fecha1 = vrMovimientosTD[i].anio * 10000 +
                 vrMovimientosTD[i].mes * 100 +
                 vrMovimientosTD[i].dia;
        fecha2 = vrMovimientosTD[i + 1].anio * 10000 +
                 vrMovimientosTD[i + 1].mes * 100 +
                 vrMovimientosTD[i + 1].dia;
        if (fecha1 > fecha2)
        {
          swap(vrMovimientosTD[i], vrMovimientosTD[i + 1]);
          ordenado = false;
        }
      }
    } while (!ordenado);
  }

  void ordXBurTC(sMovimientoTC vrMovimientosTC[], int card)
  {
    bool ordenado = false;
    int k = 0;
    int fecha1, fecha2;

    do
    {
      ordenado = true;
      k++;
      for (int i = 0; i < card - k; i++)
      {
        fecha1 = vrMovimientosTC[i].anio * 10000 +
                 vrMovimientosTC[i].mes * 100 +
                 vrMovimientosTC[i].dia;
        fecha2 = vrMovimientosTC[i + 1].anio * 10000 +
                 vrMovimientosTC[i + 1].mes * 100 +
                 vrMovimientosTC[i + 1].dia;
        if (fecha1 > fecha2)
        {
          swap(vrMovimientosTC[i], vrMovimientosTC[i + 1]);
          ordenado = false;
        }
      }
    } while (!ordenado);
  }
}

namespace Menues
{
  void Menu_Login(sUsuario vrUsuarios[], sUsuario &rUsuario, bool &correr);
  void Menu_Principal(sUsuario rUsuario, bool &correr);
  void Menu_Cuentas(sMovimientoCA vrMovimientosCA[], int cardMovCA);
  void Submenu_Cuentas(sMovimientoCA vrMovimientosCA[], int &cardMovCA, short divisa);
  void Menu_TransferirDinero(sMovimientoCA vrMovimientosCA[], int &cardMovCA);
  void Submenu_TransferirDinero(char metodo, sMovimientoCA vrMovimientosCA[], int &cardMovCA);
  void Menu_Tarjetas(sMovimientoTD vrMovimientosTD[], int &cardMovTD, sMovimientoTC vrMovimientosTC[], int &cardMovTC);
  void Submenu_TarjetaDebito(sMovimientoTD vrMovimientosTD[], int &cardMovTD);
  void Submenu_TarjetaCredito(sMovimientoTC vrMovimientosTC[], int &cardMovTC);
  void Menu_SimulacionPF();
  void Menu_Inversiones(sMovimientoCA vrMovimientosCA[], int &cardMovCA);
  void Submenu_PlazoFijo(sMovimientoCA vrMovimientosCA[], int &cardMovCA);
  void Submenu_FondoInversion(sMovimientoCA vrMovimientosCA[], int &cardMovCA);
  void Menu_Recargas(sMovimientoCA vrMovimientosCA[], int &cardMovCA);
  void Submenu_Recarga(char tipoRecarga, sMovimientoCA vrMovimientosCA[], int &cardMovCA);
  void Menu_MostrarCBU(sUsuario rUsuario);
  void Menu_DatosPersonales(sUsuario rUsuario);
  void Menu_ModificarClave(sUsuario &rUsuario);
  void Submenu_Deposito(sMovimientoCA vrMovimientosCA[], int &cardMovCA);
  void Menu_CrearCuenta(sUsuario &rUsuario);
  void Menu_MisCuentas(sUsuario rUsuario);
  void Menu_Compras(sMovimientoCA vrMovimientosCA[], int &cardMovCA);
  void Menu_Logout(bool &correr);
  void Menu_CompraVentaDolares(sMovimientoCA vrMovimientosCA[], int &cardMovCA);
  void Menu_GenerarToken();

  void clrFullScr()
  {
    system("CLS");
  }

  void pararFullScr()
  {
    MnsgBox(2, _wherey() + 2, "Oprima la tecla ESPACIO para continuar...");
    limpiarEstadoTeclas();
    while (not GetAsyncKeyState(VK_SPACE))
      Sleep(100);
  }

  void inicMostDeco()
  {
    limpiarEstadoTeclas();
    limpiarBufferEntrada();
    clrFullScr();
    _textbackground(AMARILLO);
    Marco(0, 0, COLS - 2, LINEAS - 2 + 4, AZUL_CLARO);
    MnsgBox(COLS - 16, 0, AZUL, "Banco Haedo");
    ActualizaColores(BLANCO_BRILLANTE, NEGRO);
  }

  void mostRotuloMenu(string mensaje)
  {
    MnsgBox(COLS - mensaje.length() - 5, 2, mensaje);
  }

  void obtenerOpcionSeleccionada(int &opcionSeleccionada, str24 opciones[], short numOpciones)
  {
    for (int i = 0; i < numOpciones; i++)
      MnsgBox(5, i + 1, i == 0 ? AMARILLO : BLANCO_BRILLANTE, opciones[i]);
    opcionSeleccionada = MenuNavegar(opciones, 1, numOpciones, 5);
  }

  void Menu_Login(sUsuario vrUsuarios[], sUsuario &rUsuario, bool &correr)
  {
    const int MAX_INTENTOS = 3;
    int contador = 0, seEncontro, dni;
    string usuario, clave;
    bool esValido = false;

    do
    {
      contador++;
      inicMostDeco();

      MnsgBox(2, 2, "Bienvenido al Sistema de Home Banking del Banco Haedo");
      obtenerEntrada(dni, 2, 4, "Ingrese su DNI     : ");
      obtenerEntrada(usuario, 2, 5, "Ingrese su usuario : ");
      obtenerEntrada(clave, 2, 6, "Ingrese su clave   : ");

      seEncontro = busBinVecDNI(vrUsuarios, 5, dni);
      esValido = (seEncontro != -1 && vrUsuarios[seEncontro].usuario == usuario && vrUsuarios[seEncontro].clave == clave);

      if (!esValido)
        MnsgBox(2, 12, "Datos incorrectos. Intento " + to_string(contador) + "/" + to_string(MAX_INTENTOS));
      Espera(3000);
    } while (contador < MAX_INTENTOS && !esValido);

    if (esValido)
      rUsuario = vrUsuarios[seEncontro];
    else
    {
      mostrarTexto("Ud. deberá dirigirse a un Cajero Automático o al propio Banco");
      Pausa();
    }

    correr = esValido;
  }

  void inicMenuPrincipal(string nombre,
                         sMovimientoCA vrMovimientosCA[], int &cardMovCA, int maxMovCA,
                         sMovimientoTD vrMovimientosTD[], int &cardMovTD,
                         sMovimientoTC vrMovimientosTC[], int &cardMovTC)
  {
    inicMostDeco();
    mostRotuloMenu("Bienvenido/a " + nombre);
    Archivos::leerCA(vrMovimientosCA, cardMovCA, maxMovCA);
    Archivos::leerTD(vrMovimientosCA, cardMovCA, maxMovCA, vrMovimientosTD, cardMovTD);
    Archivos::leerTC(vrMovimientosCA, cardMovCA, maxMovCA, vrMovimientosTC, cardMovTC);
    Ordenar::ordXBurCA(vrMovimientosCA, cardMovCA);
    Ordenar::ordXBurTC(vrMovimientosTC, cardMovTC);
    Ordenar::ordXBurTD(vrMovimientosTD, cardMovTD);
  }

  void Menu_Principal(sUsuario rUsuario, bool &correr)
  {
    const int NUM_OPCIONES = 19;
    const int MAX_MOV_TD = 20;
    const int MAX_MOV_TC = 20;
    const int MAX_MOV_CA = MAX_MOV_TD + MAX_MOV_TC + 25;
    int opcionSeleccionada, cardMovCA, cardMovTD, cardMovTC;
    sMovimientoCA vrMovimientosCA[MAX_MOV_CA];
    sMovimientoTD vrMovimientosTD[MAX_MOV_TD];
    sMovimientoTC vrMovimientosTC[MAX_MOV_TC];

    str24 opciones[NUM_OPCIONES] = {
        " [] Cuentas",
        "]>[ Transferir dinero",
        "[¯] Tarjetas",
        "$>U Compra/Venta dolares",
        "+$? Simulacion P.F.",
        "+$$ Inversion Plazo Fijo",
        "  ╦ Recargar",
        " +x Generar Token",
        "132 Mostrar CBU",
        " [] Mis Cuentas",
        "()/ Datos personales",
        "/** Modificar clave",
        "<O> Movimientos CA",
        "<O> Movimientos TD",
        "<O> Movimientos TC",
        "+[] Crear nueva cuenta",
        ">[] Depósito",
        "|^| Compras",
        "  ó Logout"};

    inicMenuPrincipal(rUsuario.nombre,
                      vrMovimientosCA, cardMovCA, MAX_MOV_CA,
                      vrMovimientosTD, cardMovTD,
                      vrMovimientosTC, cardMovTC);

    obtenerOpcionSeleccionada(opcionSeleccionada, opciones, NUM_OPCIONES);
    switch (opcionSeleccionada)
    {
    case 0:
      Menu_Cuentas(vrMovimientosCA, cardMovCA);
      break;
    case 1:
      Menu_TransferirDinero(vrMovimientosCA, cardMovCA);
      break;
    case 2:
      Menu_Tarjetas(vrMovimientosTD, cardMovTD, vrMovimientosTC, cardMovTC);
      break;
    case 3:
      Menu_CompraVentaDolares(vrMovimientosCA, cardMovCA);
      break;
    case 4:
      Menu_SimulacionPF();
      break;
    case 5:
      Menu_Inversiones(vrMovimientosCA, cardMovCA);
      break;
    case 6:
      Menu_Recargas(vrMovimientosCA, cardMovCA);
      break;
    case 7:
      Menu_GenerarToken();
      break;
    case 8:
      Menu_MostrarCBU(rUsuario);
      break;
    case 9:
      Menu_MisCuentas(rUsuario);
      break;
    case 10:
      Menu_DatosPersonales(rUsuario);
      break;
    case 11:
      Menu_ModificarClave(rUsuario);
      break;
    case 12:
      Menu_Cuentas(vrMovimientosCA, cardMovCA);
      break;
    case 13:
      Submenu_TarjetaDebito(vrMovimientosTD, cardMovTD);
      break;
    case 14:
      Submenu_TarjetaCredito(vrMovimientosTC, cardMovTC);
      break;
    case 15:
      Menu_CrearCuenta(rUsuario);
      break;
    case 16:
      Submenu_Deposito(vrMovimientosCA, cardMovCA);
      break;
    case 17:
      Menu_Compras(vrMovimientosCA, cardMovCA);
      break;
    case 18:
      Menu_Logout(correr);
      break;
    }
  }

  void Menu_Cuentas(sMovimientoCA vrMovimientosCA[], int cardMovCA)
  {
    const int NUM_OPCIONES = 2;
    int opcionSeleccionada;
    str24 opciones[NUM_OPCIONES] = {
        "Dólares",
        "Pesos"};

    inicMostDeco();

    mostRotuloMenu("Seleccione la divisa");
    obtenerOpcionSeleccionada(opcionSeleccionada, opciones, NUM_OPCIONES);

    MnsgBox(2, 20, to_string(opcionSeleccionada));
    Submenu_Cuentas(vrMovimientosCA, cardMovCA, opcionSeleccionada);
  }

  float convertirMonedas(float importeEntrada, short divisaEntrada, short divisaSalida)
  {
    float importeSalida = importeEntrada;

    if (divisaEntrada == ARS && divisaSalida == USD)
      importeEntrada *= COTIZACION_USD;
    else if (divisaEntrada == USD && divisaSalida == ARS)
      importeEntrada /= COTIZACION_USD;

    return importeSalida;
  }

  void inicResumen(string nombreReporte)
  {
    _textcolor(AMARILLO);
    FechaHora::FechaHoy();
    MnsgBox(16, 3, nombreReporte + "  Banco Haedo");
    _textcolor(BLANCO);
    cout << endl;
  }

  void Submenu_Cuentas(sMovimientoCA vrMovimientosCA[], int &cardMovCA, short divisa)
  {
    float importe, saldoCA = 0;

    clrFullScr();

    inicResumen("Movimientos Caja de Ahorro");

    cout << Separador(75, '-')
         << endl
         << left
         << "Mov"
         << " " << setw(10) << "Fecha"
         << " T"
         << " " << setw(25) << "Detalle"
         << " " << setw(10) << "Debe"
         << " " << setw(10) << "Haber"
         << " Saldo"
         << endl
         << Separador(75, '-')
         << endl;
    for (int i = 0; i < cardMovCA; i++)
    {
      importe = convertirMonedas(vrMovimientosCA[i].importe, ARS, divisa);

      cout << right << setw(3) << i + 1
           << " " << setw(2) << vrMovimientosCA[i].dia << "-" << setw(2) << vrMovimientosCA[i].mes << "-" << setw(4) << vrMovimientosCA[i].anio
           << " " << vrMovimientosCA[i].tipoMov
           << " " << left << setw(25) << vrMovimientosCA[i].detalle;

      if (vrMovimientosCA[i].tipoMov == DEBE)
      {
        cout << " " << right << setw(10) << fixed << setprecision(2) << importe
             << " " << string(10, ' ');
        saldoCA -= importe;
      }
      else
      {
        cout << " " << string(10, ' ')
             << " " << right << setw(10) << fixed << setprecision(2) << importe;
        saldoCA += importe;
      }

      cout << " " << right << setw(10) << fixed << setprecision(2) << saldoCA
           << " " << endl;
    }
    cout << Separador(75, '-') << endl
         << setw(62) << "Saldo: " << right << setw(3) << divisasSimbolos[divisa] << right << setw(10) << fixed << setprecision(2) << saldoCA << endl
         << endl;

    pararFullScr();
  }

  void Menu_TransferirDinero(sMovimientoCA vrMovimientosCA[],
                             int &cardMovCA)
  {
    const int NUM_OPCIONES = 2;
    int opcionSeleccionada;
    str24 opciones[NUM_OPCIONES] = {
        "CBU",
        "Celular"};

    inicMostDeco();

    mostRotuloMenu("Seleccione método de transferencia.");
    obtenerOpcionSeleccionada(opcionSeleccionada, opciones, NUM_OPCIONES);
    MnsgBox(2, 20, to_string(opcionSeleccionada));

    switch (opcionSeleccionada)
    {
    case CBU:
      Submenu_TransferirDinero(CBU,
                               vrMovimientosCA,
                               cardMovCA);
      break;
    case CELULAR:
      Submenu_TransferirDinero(CELULAR,
                               vrMovimientosCA,
                               cardMovCA);
      break;
    }
  }

  void Submenu_TransferirDinero(char metodo,
                                sMovimientoCA vrMovimientosCA[],
                                int &cardMovCA)
  {
    string destinatario;
    double monto;
    inicMostDeco();

    if (metodo == CBU)
      MnsgBox(10, 3, "Transferencia por CBU seleccionada.");
    else
      MnsgBox(10, 3, "Transferencia por Celular seleccionada.");

    do
    {
      obtenerEntrada(destinatario, 10, 7, "Ingrese el destinatario: ");
    } while (destinatario == "");

    do
    {
      obtenerEntrada(monto, 10, 9, "Ingrese el monto a transferir: ");
    } while (monto <= 0);

    int anio, mes, dia, ds;
    FechaHora::GetDate(anio, mes, dia, ds);

    sMovimientoCA movimiento;

    movimiento.dia = dia;
    movimiento.mes = mes;
    movimiento.anio = anio;
    movimiento.tipoMov = DEBE;

    strcpy(movimiento.detalle, "Transferencia");

    movimiento.importe = monto;

    vrMovimientosCA[cardMovCA] = movimiento;
    cardMovCA++;

    Archivos::escribirCA(movimiento, ARS);

    MnsgBox(2, _wherey() + 2, "Transferencia realizada");

    pararFullScr();
  }

  void Menu_Tarjetas(sMovimientoTD vrMovimientosTD[], int &cardMovTD, sMovimientoTC vrMovimientosTC[], int &cardMovTC)
  {
    const int NUM_OPCIONES = 2;
    int opcionSeleccionada;
    str24 opciones[NUM_OPCIONES] = {
        "Tarjeta Débito",
        "Tarjeta Crédito"};

    inicMostDeco();

    mostRotuloMenu("Seleccione tarjeta");
    obtenerOpcionSeleccionada(opcionSeleccionada, opciones, NUM_OPCIONES);
    MnsgBox(2, 20, to_string(opcionSeleccionada));
    switch (opcionSeleccionada)
    {
    case 0:
      Submenu_TarjetaDebito(vrMovimientosTD, cardMovTD);
      break;

    case 1:
      Submenu_TarjetaCredito(vrMovimientosTC, cardMovTC);
      break;
    }
  }

  void Submenu_TarjetaDebito(sMovimientoTD vrMovimientosTD[], int &cardMovTD)
  {
    float totalTD = 0;
    clrFullScr();

    inicResumen("Movimientos Tarjeta Débito");

    cout << Separador()
         << endl
         << "Mov."
         << " " << setw(10) << "Fecha"
         << " " << setw(25) << "Detalle"
         << " " << "Importe"
         << endl
         << Separador()
         << endl;
    for (int i = 0; i < cardMovTD; i++)
    {
      cout << setw(4) << i + 1
           << " " << setw(2) << vrMovimientosTD[i].dia << "-" << setw(2) << vrMovimientosTD[i].mes << "-" << setw(4) << vrMovimientosTD[i].anio
           << " " << setw(25) << vrMovimientosTD[i].detalle
           << " " << setw(10) << vrMovimientosTD[i].importe
           << endl;
      totalTD += vrMovimientosTD[i].importe;
    }
    cout << Separador() << endl
         << setw(42) << "Total TD: $"
         << setw(10) << totalTD
         << endl
         << Separador() << endl;

    pararFullScr();
  }

  void Submenu_TarjetaCredito(sMovimientoTC vrMovimientosTC[], int &cardMovTC)
  {
    float totalTC = 0;
    clrFullScr();

    inicResumen("Movimientos Tarjeta Crédito");

    cout << Separador()
         << endl
         << "Mov."
         << " " << setw(10) << "Fecha"
         << " " << setw(25) << "Detalle"
         << " " << setw(5) << "Cuota"
         << " " << "Importe"
         << endl
         << Separador()
         << endl;
    for (int i = 0; i < cardMovTC; i++)
    {
      cout << setw(4) << i + 1
           << " " << setw(2) << vrMovimientosTC[i].dia << "-" << setw(2) << vrMovimientosTC[i].mes << "-" << setw(4) << vrMovimientosTC[i].anio
           << " " << setw(25) << vrMovimientosTC[i].detalle
           << " " << setw(5) << vrMovimientosTC[i].cuota
           << " " << setw(10) << vrMovimientosTC[i].importe
           << endl;
      totalTC += vrMovimientosTC[i].importe;
    }
    cout << Separador() << endl
         << setw(42) << "Total TC: $"
         << setw(10) << totalTC
         << endl
         << Separador() << endl;

    pararFullScr();
  }

  void Menu_Recargas(sMovimientoCA vrMovimientosCA[], int &cardMovCA)
  {
    const int NUM_OPCIONES = 2;
    int opcionSeleccionada;

    str24 opciones[NUM_OPCIONES] =
        {
            "Recargar celular",
            "Recargar SUBE"};

    inicMostDeco();

    mostRotuloMenu("Recargas");

    obtenerOpcionSeleccionada(opcionSeleccionada, opciones, NUM_OPCIONES);

    switch (opcionSeleccionada)
    {
    case 0:
      Submenu_Recarga('C', vrMovimientosCA, cardMovCA);
      break;

    case 1:
      Submenu_Recarga('S', vrMovimientosCA, cardMovCA);
      break;
    }
  }

  void Menu_SimulacionPF()
  {
    double capital;
    double tasaAnual;
    double interes;
    double montoFinal;
    int dias;

    inicMostDeco();

    mostRotuloMenu("SIMULACION PLAZO FIJO");

    do
    {
      obtenerEntrada(capital, 2, 5, "Capital a invertir: $");
    } while (capital <= 0);

    do
    {
      obtenerEntrada(dias, 2, 6, "Plazo (dias): ");
    } while (dias <= 0);

    tasaAnual = 35;

    interes = capital * (tasaAnual / 100.0) * dias / 365.0;
    montoFinal = capital + interes;

    MnsgBox(2, 9, "Capital invertido : $" + to_string(capital));
    MnsgBox(2, 10, "Tasa anual        : " + to_string(tasaAnual) + "%");
    MnsgBox(2, 11, "Interes generado  : $" + to_string(interes));
    MnsgBox(2, 12, "Monto al vencimiento: $" + to_string(montoFinal));

    pararFullScr();
  }

  void Menu_Inversiones(sMovimientoCA vrMovimientosCA[], int &cardMovCA)
  {
    const int NUM_OPCIONES = 2;
    int opcionSeleccionada;

    str24 opciones[NUM_OPCIONES] =
        {
            "Plazo Fijo",
            "Fondo de Inversion"};

    inicMostDeco();

    mostRotuloMenu("INVERSIONES");
    obtenerOpcionSeleccionada(opcionSeleccionada, opciones, NUM_OPCIONES);

    switch (opcionSeleccionada)
    {
    case 0:
      Submenu_PlazoFijo(vrMovimientosCA, cardMovCA);
      break;

    case 1:
      Submenu_FondoInversion(vrMovimientosCA, cardMovCA);
      break;
    }
  }

  void Submenu_PlazoFijo(sMovimientoCA vrMovimientosCA[], int &cardMovCA)
  {
    double monto;

    inicMostDeco();

    mostRotuloMenu("PLAZO FIJO");

    do
    {
      obtenerEntrada(monto, 2, 5, "Capital a invertir: $");
    } while (monto <= 0);

    sMovimientoCA mov;

    int anio, mes, dia, ds;
    FechaHora::GetDate(anio, mes, dia, ds);

    mov.dia = dia;
    mov.mes = mes;
    mov.anio = anio;
    mov.tipoMov = DEBE;

    strcpy(mov.detalle, "Plazo fijo");
    mov.importe = monto;

    vrMovimientosCA[cardMovCA++] = mov;
    Archivos::escribirCA(mov, ARS);

    MnsgBox(2, 9, "Plazo fijo constituido correctamente.");

    pararFullScr();
  }

  void Submenu_FondoInversion(sMovimientoCA vrMovimientosCA[], int &cardMovCA)
  {
    double monto;

    inicMostDeco();

    mostRotuloMenu("FONDO DE INVERSION");

    do
    {
      obtenerEntrada(monto, 2, 5, "Capital a invertir: $");
    } while (monto <= 0);

    sMovimientoCA mov;

    int anio, mes, dia, ds;
    FechaHora::GetDate(anio, mes, dia, ds);

    mov.dia = dia;
    mov.mes = mes;
    mov.anio = anio;
    mov.tipoMov = DEBE;

    strcpy(mov.detalle, "Fondo inversion");
    mov.importe = monto;

    vrMovimientosCA[cardMovCA++] = mov;
    Archivos::escribirCA(mov, ARS);

    MnsgBox(2, 9, "Inversion realizada correctamente.");

    pararFullScr();
  }

  void Submenu_Recarga(char tipoRecarga,
                       sMovimientoCA vrMovimientosCA[],
                       int &cardMovCA)
  {
    string dato;
    double importe;

    inicMostDeco();

    if (tipoRecarga == 'C')
      MnsgBox(10, 3, "Recarga de celular");
    else
      MnsgBox(10, 3, "Recarga SUBE");

    if (tipoRecarga == 'C')
      obtenerEntrada(dato, 10, 6, "Numero de celular: ");
    else
      obtenerEntrada(dato, 10, 6, "Numero de tarjeta SUBE: ");

    do
    {
      obtenerEntrada(importe, 10, 8, "Importe: ");
    } while (importe <= 0);

    int anio, mes, dia, ds;
    FechaHora::GetDate(anio, mes, dia, ds);

    sMovimientoCA movimiento;

    movimiento.dia = dia;
    movimiento.mes = mes;
    movimiento.anio = anio;
    movimiento.tipoMov = DEBE;

    if (tipoRecarga == 'C')
      strcpy(movimiento.detalle, "Recarga celular");
    else
      strcpy(movimiento.detalle, "Recarga SUBE");

    movimiento.importe = importe;

    vrMovimientosCA[cardMovCA] = movimiento;
    cardMovCA++;

    Ordenar::ordXBurCA(vrMovimientosCA, cardMovCA);

    Archivos::escribirCA(movimiento, ARS);

    MnsgBox(10, 12, "Recarga realizada correctamente.");

    pararFullScr();
  }

  void Menu_ModificarClave(sUsuario &rUsuario)
  {
    string nuevaClave, claveActual;
    inicMostDeco();
    mostRotuloMenu("Modificar Clave");

    obtenerEntrada(claveActual, 2, 5, "Ingrese clave actual: ");
    if (claveActual == rUsuario.clave)
    {
      obtenerEntrada(nuevaClave, 2, 7, "Ingrese nueva clave : ");
      if (nuevaClave != "")
      {
        rUsuario.clave = nuevaClave;
        MnsgBox(2, 10, "Clave modificada con éxito.");
      }
      else
      {
        MnsgBox(2, 10, "La clave no puede estar vacía.");
      }
    }
    else
    {
      MnsgBox(2, 10, "Clave actual incorrecta.");
    }
    pararFullScr();
  }

  void Submenu_Deposito(sMovimientoCA vrMovimientosCA[], int &cardMovCA)
  {
    double monto;

    inicMostDeco();

    mostRotuloMenu("DEPOSITO");

    do
    {
      obtenerEntrada(monto, 2, 5, "Monto a depositar: $");
    } while (monto <= 0);

    sMovimientoCA mov;

    int anio, mes, dia, ds;
    FechaHora::GetDate(anio, mes, dia, ds);

    mov.dia = dia;
    mov.mes = mes;
    mov.anio = anio;
    mov.tipoMov = 'H';

    strcpy(mov.detalle, "Deposito");
    mov.importe = monto;

    vrMovimientosCA[cardMovCA++] = mov;
    Ordenar::ordXBurCA(vrMovimientosCA, cardMovCA);
    Archivos::escribirCA(mov, ARS);

    MnsgBox(2, 9, "Deposito realizado correctamente.");

    pararFullScr();
  }

  void Menu_CrearCuenta(sUsuario &rUsuario)
  {
    const int NUM_OPCIONES_TIPO = 2;
    int tipoOpcion;
    str24 opcionesTipo[NUM_OPCIONES_TIPO] = {
        "CA (Caja de Ahorro)",
        "CC (Cuenta Corriente)"};

    inicMostDeco();
    mostRotuloMenu("CREAR NUEVA CUENTA - TIPO");
    obtenerOpcionSeleccionada(tipoOpcion, opcionesTipo, NUM_OPCIONES_TIPO);

    const int NUM_OPCIONES_MONEDA = 2;
    int monedaOpcion;
    str24 opcionesMoneda[NUM_OPCIONES_MONEDA] = {
        "Pesos ($)",
        "Dólares (u$s)"};

    inicMostDeco();
    mostRotuloMenu("CREAR NUEVA CUENTA - MONEDA");
    obtenerOpcionSeleccionada(monedaOpcion, opcionesMoneda, NUM_OPCIONES_MONEDA);

    inicMostDeco();
    mostRotuloMenu("CREAR NUEVA CUENTA");

    int correlativo = (rUsuario.DNI % 90) + 10;
    string nroCuentaCreada = "009-00000" + to_string(correlativo);

    MnsgBox(2, 4, "Titular: " + rUsuario.nombre);
    MnsgBox(2, 6, "Tipo de cuenta: " + string(tipoOpcion == 0 ? "CA" : "CC"));
    MnsgBox(2, 7, "Moneda        : " + string(monedaOpcion == 0 ? "Pesos ($)" : "Dólares (u$s)"));

    MnsgBox(2, 10, "Cuenta creada nro. " + nroCuentaCreada + ",");

    pararFullScr();
  }

  void Menu_MisCuentas(sUsuario rUsuario)
  {
    clrFullScr();
    inicResumen("Mis Cuentas - Datos Bancarios");

    cout << Separador(78, '-')
         << endl
         << left
         << setw(16) << "Nro. Cta"
         << setw(8) << "Tipo"
         << setw(10) << "Moneda"
         << setw(15) << "Saldo"
         << setw(15) << "Estado"
         << endl
         << Separador(78, '-')
         << endl;

    cout << left
         << setw(16) << rUsuario.numeroCuentaCA
         << setw(8) << "CA"
         << setw(10) << "$"
         << right << setw(10) << fixed << setprecision(2) << 150000.00
         << "     " << left << setw(15) << "Activa"
         << endl;

    cout << left
         << setw(16) << rUsuario.numeroCuentaCA + 1
         << setw(8) << "CC"
         << setw(10) << "u$s"
         << right << setw(10) << fixed << setprecision(2) << 450.00
         << "     " << left << setw(15) << "Activa"
         << endl;

    cout << Separador(78, '-') << endl;
    pararFullScr();
  }

  void Menu_Compras(sMovimientoCA vrMovimientosCA[], int &cardMovCA)
  {
    string comercio;
    double monto;

    inicMostDeco();
    mostRotuloMenu("COMPRAS");

    do
    {
      obtenerEntrada(comercio, 2, 5, "Establecimiento / Comercio: ");
    } while (comercio == "");

    do
    {
      obtenerEntrada(monto, 2, 7, "Monto de la compra: $");
    } while (monto <= 0);

    sMovimientoCA mov;
    int anio, mes, dia, ds;
    FechaHora::GetDate(anio, mes, dia, ds);

    mov.dia = dia;
    mov.mes = mes;
    mov.anio = anio;
    mov.tipoMov = DEBE;

    string detalleStr = "Compra: " + comercio;
    if (detalleStr.length() > 25)
      detalleStr = detalleStr.substr(0, 25);
    strcpy(mov.detalle, detalleStr.c_str());
    mov.importe = monto;

    vrMovimientosCA[cardMovCA++] = mov;
    Ordenar::ordXBurCA(vrMovimientosCA, cardMovCA);
    Archivos::escribirCA(mov, ARS);

    MnsgBox(2, 10, "Compra registrada correctamente.");

    pararFullScr();
  }

  void Menu_CompraVentaDolares(sMovimientoCA vrMovimientosCA[], int &cardMovCA)
  {
    const int NUM_OPCIONES = 2;
    int opcionSeleccionada;
    str24 opciones[NUM_OPCIONES] = {
        "Comprar Dólares",
        "Vender Dólares"};

    inicMostDeco();
    mostRotuloMenu("COMPRA / VENTA DE DÓLARES");
    obtenerOpcionSeleccionada(opcionSeleccionada, opciones, NUM_OPCIONES);

    double monto;
    inicMostDeco();
    if (opcionSeleccionada == 0)
    {
      mostRotuloMenu("COMPRAR DÓLARES");
      do
      {
        obtenerEntrada(monto, 2, 5, "Ingrese el monto en ARS: $");
      } while (monto <= 0);

      sMovimientoCA mov;
      int anio, mes, dia, ds;
      FechaHora::GetDate(anio, mes, dia, ds);

      mov.dia = dia;
      mov.mes = mes;
      mov.anio = anio;
      mov.tipoMov = DEBE;
      strcpy(mov.detalle, "Compra Dolares");
      mov.importe = monto;

      vrMovimientosCA[cardMovCA++] = mov;
      Ordenar::ordXBurCA(vrMovimientosCA, cardMovCA);
      Archivos::escribirCA(mov, ARS);
    }
    else
    {
      mostRotuloMenu("VENDER DÓLARES");
      do
      {
        obtenerEntrada(monto, 2, 5, "Ingrese el monto en USD: u$d ");
      } while (monto <= 0);

      sMovimientoCA mov;
      int anio, mes, dia, ds;
      FechaHora::GetDate(anio, mes, dia, ds);

      mov.dia = dia;
      mov.mes = mes;
      mov.anio = anio;
      mov.tipoMov = HABER;
      strcpy(mov.detalle, "Venta Dolares");
      mov.importe = monto * COTIZACION_USD;

      vrMovimientosCA[cardMovCA++] = mov;
      Ordenar::ordXBurCA(vrMovimientosCA, cardMovCA);
      Archivos::escribirCA(mov, ARS);
    }

    MnsgBox(2, 9, "Operación realizada correctamente.");
    pararFullScr();
  }

  void Menu_GenerarToken()
  {
    inicMostDeco();
    mostRotuloMenu("GENERAR TOKEN");

    MnsgBox(5, 18, "Presione ESPACIO para regresar al Menú Principal.");

    bool salir = false;
    while (!salir)
    {
      int token = rand() % 900000 + 100000;

      MnsgBox(10, 7, "Token actual: ");
      _textcolor(AMARILLO_CLARO);
      MnsgBox(25, 7, to_string(token));
      _textcolor(BLANCO_BRILLANTE);

      int totalBloques = 30;
      MnsgBox(10, 9, "[");
      MnsgBox(11 + totalBloques, 9, "]");

      for (int porcentaje = 10; porcentaje <= 100; porcentaje += 10)
      {
        if (GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_ESCAPE))
        {
          salir = true;
          break;
        }

        int bloques = (porcentaje * totalBloques) / 100;

        _textcolor(BLANCO_BRILLANTE);
        string barra = "";
        for (int b = 0; b < totalBloques; b++)
        {
          if (b < bloques)
            barra += "█";
          else
            barra += " ";
        }
        MnsgBox(11, 9, barra);

        string porcStr = to_string(porcentaje) + "%";
        if (porcentaje < 100)
          porcStr = " " + porcStr;

        MnsgBox(13 + totalBloques, 9, porcStr);

        Sleep(900);
      }

      if (salir)
        break;

      Sleep(1000);
    }

    limpiarEstadoTeclas();
    pararFullScr();
  }

  void Menu_MostrarCBU(sUsuario rUsuario)
  {
    inicMostDeco();

    mostRotuloMenu("Mostrar CBU");

    string cbu = to_string(rUsuario.CBU);

    while (cbu.length() < 22)
      cbu = "0" + cbu;

    MnsgBox(5, 4, "Titular:");
    MnsgBox(20, 4, rUsuario.nombre);

    MnsgBox(5, 6, "Numero de Cuenta:");
    MnsgBox(26, 6, to_string(rUsuario.numeroCuentaCA));

    MnsgBox(5, 8, "CBU:");
    MnsgBox(20, 8, cbu.substr(0, 8) + " " + cbu.substr(8, 14));

    pararFullScr();
  }

  void Menu_DatosPersonales(sUsuario rUsuario)
  {
    inicMostDeco();

    mostRotuloMenu("Datos personales");

    MnsgBox(5, 3, "Apellido y Nombre:");
    MnsgBox(30, 3, rUsuario.nombre);

    string fecha = to_string(rUsuario.fechaNacimiento);

    while (fecha.length() < 6)
      fecha = "0" + fecha;

    fecha = fecha.substr(0, 2) + "/" +
            fecha.substr(2, 2) + "/" +
            fecha.substr(4, 2);

    MnsgBox(5, 5, "Fecha de nacimiento:");
    MnsgBox(30, 5, fecha);
    MnsgBox(5, 7, "Usuario:");
    MnsgBox(30, 7, rUsuario.usuario);

    MnsgBox(5, 9, "Clave:");
    MnsgBox(30, 9, rUsuario.clave);

    MnsgBox(5, 11, "Número de celular:");
    MnsgBox(30, 11, to_string(rUsuario.numeroCelular));

    MnsgBox(5, 13, "Email:");
    MnsgBox(30, 13, rUsuario.correoElectronico);

    MnsgBox(5, 15, "Domicilio:");
    MnsgBox(30, 15, rUsuario.direccion);

    MnsgBox(5, 17, "N° Cuenta C.A.:");
    MnsgBox(30, 17, to_string(rUsuario.numeroCuentaCA));

    string cbu = to_string(rUsuario.CBU);
    while (cbu.length() < 22)
      cbu = "0" + cbu;

    MnsgBox(5, 19, "CBU:");
    MnsgBox(30, 19, cbu.substr(0, 8) + " " + cbu.substr(8, 14));

    pararFullScr();
  }

  void Menu_Logout(bool &correr)
  {
    inicMostDeco();

    MnsgBox(10, 5, "Sesión Cerrada");
    MnsgBox(10, 7, "Gracias por operar con el");
    _textcolor(VERDE_CLARO);
    MnsgBox(10, 8, "Banco Haedo");
    _textcolor(BLANCO_BRILLANTE);

    pararFullScr();
    correr = false;
  }
}

void SistemaHomeBanking()
{
  const int NUM_USUARIOS = 5;
  bool correr;
  sUsuario vrUsuarios[NUM_USUARIOS] = {
      {12345678, "Juan Perez", 150398, "juanp", "clave123", 1234567890, "juanperez@gmail.com", "Calle 123", 987654321, 1122334455},
      {23456789, "Maria Gomez", 210701, "mariag", "clave456", 2345678901, "mariagomez@gmail.com", "Calle 456", 876543210, 2233445566},
      {34567890, "Carlos Lopez", 90495, "carlosl", "clave789", 3456789012, "carloslopez@gmail.com", "Calle 789", 765432109, 3344556677},
      {45678901, "Ana Torres", 300199, "anat", "clave012", 4567890123, "anatorres@gmail.com", "Calle 012", 654321098, 4455667788},
      {56789012, "Luis Ramirez", 120600, "luisr", "clave345", 5678901234, "luisramirez@gmail.com", "Calle 012", 543210987, 5566778899}};
  sUsuario rUsuario;

  SetConsoleOutputCP(CP_UTF8);

  BarraTitulo();
  Menues::Menu_Login(vrUsuarios, rUsuario, correr);
  while (correr)
    Menues::Menu_Principal(rUsuario, correr);
}

int main()
{
  SistemaHomeBanking();
  return 0;
}
