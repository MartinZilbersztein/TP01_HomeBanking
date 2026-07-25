#include <iostream>
#include <iomanip>
#include <ctime>
// #include <string>
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
const int COLS   = 80;
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

enum Divisas { USD, ARS };
str3 divisasSimbolos[2] = { "u$d", "$" };

enum TipoMov {
  DEBE = 'D',
  HABER = 'H'
};

enum MetodosTransferencia {
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
  long long numeroCelular; // 15 dígitos según E.164. Alternativa str15
  string correoElectronico;
  string direccion;
  int numeroCuentaCA;
  long long CBU; // 22 dígitos según BCRA. Alternativa str22
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
      return medio; // Elemento encontrado
    else if (vec[medio].DNI < dni)
      izq = medio + 1; // Buscar en la mitad derecha
    else
      der = medio - 1; // Buscar en la mitad izquierda
  }
  return -1; // Elemento no encontrado
}



/* Según GPT, hay que agregar esto para que funcione _window
 *
 // LINEAS DE
 // LÍNEA OBLIGATORIA: Aplica los cambios visuales en la consola real
 // SetConsoleWindowInfo(hConsole, TRUE, &window);
*/



/* FUNCIONES BRINDADAS POR EL PROFESOR */

// TP1-2026: HOME BANKING.

// Módulos para manejo de la Consola Pantalla y Teclado.
// Módulos para obtener la Fecha y Hora del Sistema.

// Estos módulos deberán ser utilizados como herramientas productivas para el
// TP1-HomeBanking.
// El proceso es <<Interactivo>>, por lo que el código se duplica con respecto a
// un proceso en <<Batch>> o por Lotes, debido a que en el proceso interactivo
// además de contar con el código que resuelve el problema lógico, se requiere del
// código de la interface con el usuario.
// Los módulos se encuentran dentro del del namespace o espacio de nombres que
// correspondan a los módulos relacionados con la pantalla y/o teclado es decir,
// con la Consola. Así tenemos en esta entrega los namespace de <<Screen>> y
// <<FechaHora>>. Cada grupo de trabajo podrá optar por utilizar otros namespace.
// Podemos copiar el <<namespace Screen>> completo con todos los módulos allí
// definidos; como así también el <<namespace FechaHora>>.
// Los módulos dentro de un namespace que utilicemos deberán estar acompañados
// por el ámbito al que pertenece. Por ejemplo: Screen::_gotoxy(x,y);
// Screen::_textcolor(n); Screen::_clrscr(); Screen::MnsgBox(x,y,Mensaje);
// Existe una forma de NO indicar el ámbito a la función definida, si utilizamos
// la siguiente sentencia: <<using namespace Screen>>, entonces luego de esto
// podemos utilizar esas funciones como se indica a continuación:
// _gotoxy(x,y); _textcolor(n); _clrscr(); MnsgBox(x,y,Mensaje);
// Los namespace tienen la siguiente sintaxis:

// namespace NomEspacioNombre {
//   Cabecera Mod1() {
//     Cuerpo del Mod1;
//   } // Fin Mod1
//
//   Cabecera Mod2() {
//     Cuerpo del Mod2;
//   } // Fin Mod2
//   ...
//   ...
// } // Fin namespace Screen

// Los nombres de módulos que inician con el caracter underscore o guión bajo o
// subrayado, tienen el mismo nombre de los módulos de Borland, por lo que el
// underscore indican que no son los módulos de Borland. El compilador a usar
// NO es el de Borland sino el TDM-GCC-64 GNU-GCC Compiler
// C++ Compiler x86_64-w64-mingw32-g++.exe

namespace Screen
{

  void setConsoleColor(WORD colText, WORD colBack)
  {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colText + 16 * colBack);
  } // setConsoleColor

  void _window(short izq, short sup, short der, short inf)
  { // Clon de window()
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SMALL_RECT window = csbi.srWindow;

    window.Left = izq - 1;
    window.Top = sup - 1;
    window.Right = der - 1;
    window.Bottom = inf - 1;
  } // _window

  void _gotoxy(short x, short y)
  { // Clon de gotoxy()
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SMALL_RECT window = csbi.srWindow;
    COORD coord;

    coord.X = window.Left + x;
    coord.Y = window.Top + y;
    if (coord.X <= window.Right && coord.Y <= window.Bottom)
      SetConsoleCursorPosition(hConsole, coord);
  } // _gotoxy

  int _wherex()
  {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
      return csbi.dwCursorPosition.X;
    return -1;
  } // _wherex

  int _wherey()
  {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
      return csbi.dwCursorPosition.Y;
    return -1;
  } // _wherey

  void ActualizaColores(WORD colTextAct, WORD colBackAc)
  {
    // Clon de textcolor() y textbackground() combinados.
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // En Windows, el fondo se desplaza 4 bits a la izq. (se multiplica por 16)
    WORD atributo = colTextAct | (colBackAc << 4);
    SetConsoleTextAttribute(hConsole, atributo);
  } // ActualizaColores

  WORD ObtenerColorTextoActual()
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
      // Aplica una máscara para quedarse únicamente con el color del texto
      return csbi.wAttributes & 0x000F;
    }
    return 0; // Retorna 0 (negro) si hubo un error
  } // ObtenerColorTextoActual

  WORD ObtenerColorFondo()
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
      // El color de fondo está en los primeros 4 bits de la parte alta
      // Por lo tanto, dividimos entre 16 para extraerlo
      return (csbi.wAttributes >> 4) & 0x0F;
    }
    return -1; // Retorna -1 si ocurre un error
  } // ObtenerColorFondo

  void _textcolor(WORD color)
  {
    ActualizaColores(color, ObtenerColorFondo());
  } // _textcolor

  void _textbackground(WORD color)
  {
    ActualizaColores(ObtenerColorTextoActual(), color);
  } // _textbackground

  void _clrscr()
  { // Clon de _clrscr() LOCALIZADO (Solo limpia área de la ventana)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SMALL_RECT window = csbi.srWindow;
    DWORD escritos;
    WORD atributo = ObtenerColorTextoActual() | (ObtenerColorFondo() << 4);
    // Calcular ancho y alto de nuestra ventana
    int ancho = window.Right - 3 - window.Left;

    window.Left += 2;
    // Limpiar fila por fila dentro de los límites coordenados
    for (short y = window.Top + 2; y <= window.Bottom - 1; ++y)
    {
      COORD inicio_fila = {window.Left, y};
      // Llena la fila actual con espacios en blanco
      FillConsoleOutputCharacter(hConsole, ' ', ancho, inicio_fila, &escritos);
      // Aplica el color de fondo actual a esos espacios en blanco
      FillConsoleOutputAttribute(hConsole, atributo, ancho, inicio_fila, &escritos);
    }
    _gotoxy(1, 1); // Igual a Borland, al limpiar, regresa a (1,1) relativo.
  } // _clrscr

  void _clreol()
  {
    COORD coord;
    DWORD escrito;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info; // Obtener la posición actual del cursor
    GetConsoleScreenBufferInfo(hStdOut, &info);

    coord = info.dwCursorPosition;
    // Calcular cuantos caracteres faltan para el final de la línea
    int longitud = info.dwSize.X - coord.X;
    // Sobrescribir con espacios y restaurar posición del cursor
    FillConsoleOutputCharacter(hStdOut, ' ', longitud, coord, &escrito);
    SetConsoleCursorPosition(hStdOut, coord);
  } // _clreol

  void MnsgBox(short x, short y, string mensaje, char alinea, short ancho = 0)
  {
    _gotoxy(x, y);
    if (alinea == 'i')
      cout << setw(ancho) << left << mensaje;
    else
      cout << setw(ancho) << right << mensaje;
  } // MnsgBox

  void MnsgBox(short x, short y, string mensaje)
  {
    _gotoxy(x, y);
    cout << mensaje;
  } // MnsgBox

  void MnsgBox(short x, short y, short colorText, string mensaje)
  {
    _gotoxy(x, y);
    _textcolor(colorText);
    cout << mensaje;
  } // MnsgBox

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
  } // Marco

  string Separador(int ancho = 60, char car = '-')
  {
    return string(ancho, car);
  } // Separador

  void OcultarCursor()
  {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(consoleHandle, &info);
    info.bVisible = FALSE; // Oculta el cursor
    SetConsoleCursorInfo(consoleHandle, &info);
  } // OcultarCursor

  void MostrarCursor()
  {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(consoleHandle, &info);
    info.bVisible = TRUE; // Muestra el cursor
    SetConsoleCursorInfo(consoleHandle, &info);
  } // MostrarCursor

  void Espera(short tiempo)
  {
    _textcolor(7);
    MnsgBox(5, 20, "Espere " + to_string(tiempo / 1000) + " segundos");
    _textcolor(15);
    Sleep(tiempo);
    _gotoxy(5, 20);
    _clreol();
  } // Espera

  void BarraTitulo()
  {
    SetConsoleTitleA("Home Banking Haedo");
  } // BarraTitulo

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
  } // Pausa

  void BloquearCambioTamaño()
  {
    HWND hwnd = GetConsoleWindow(); // Obtiene identificador de la ventana.
    // Obtiene los estilos actuales de la ventana
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_MAXIMIZEBOX; // Quita botones de maximizar y redimensionar.
    style &= ~WS_SIZEBOX;
    SetWindowLong(hwnd, GWL_STYLE, style); // Aplica los nuevos estilos
  } // BloquearCambioTamaño

  string Left(str25 cad, short cant)
  {
    return string(cad, cant);
  } // Left

  int MenuNavegar(char aMenu[][25], short lIni, short lFin, short cIni)
  {
    int Menu = lIni;

    OcultarCursor();
    MnsgBox(6, 20, "Flecha ↑ ↓ para moverse sobre el menú");
    MnsgBox(3, 21, "Tecla ESCAPE seleccionar la opción del menú");
    // GoToXY(cIni,lIni);
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
    } // Fin While
    MostrarCursor();
    return Menu - lIni;
  } // MenuNavegar

} // Screen

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
  } // GetTime

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
  } // GetDate

  int FechaHoy()
  {
    int año, mes, dia, dsem;

    GetDate(año, mes, dia, dsem);
    cout << "Dia: " << setw(2) << dia << " Mes: " << setw(2) << mes
         << " Año:" << ' ' << setw(2) << año << endl;
    return dia * 10000 + mes * 100 + año;
  } // FechaHoy

} // FechaHora

/* CIERRE DE FUNCIONES BRINDADAS POR EL PROFESOR */



namespace EntradaSalida {
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
                GetAsyncKeyState(VK_RETURN) & 0x8000)
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
      archivo.ignore(); // sacar espacio antes del detalle
      archivo.get(vrMovimientosCA[i].detalle, 25);
      archivo.ignore(); // sacar espacio después del detalle
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
    ofstream archivo(nombreArchivo, ios::app); // Abrir en modo append para agregar al final del archivo

    archivo << endl;
    archivo <<        setw(2)  << rMovimientoCA.dia;
    archivo << " " << setw(2)  << rMovimientoCA.mes;
    archivo << " " << setw(4)  << rMovimientoCA.anio;
    archivo << " " << setw(1)  << rMovimientoCA.tipoMov;
    archivo << " " << setw(25) << rMovimientoCA.detalle;
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

    /*for (i = 0; i < maxMovCA && cardMovCA + i < maxMovCA; i++)
    {
      if (vrMovimientosCA[cardMovCA + i].dia == dia &&
          vrMovimientosCA[cardMovCA + i].mes == mes &&
          vrMovimientosCA*/

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
      archivo.ignore(); // sacar espacio antes del detalle
      archivo.get(vrMovTD[i].detalle, 25);
      archivo.ignore(); // sacar espacio después del detalle
      archivo >> vrMovTD[i].importe;
      archivo.ignore();

      agregarMovimientoVRCA(
        vrMovCA, cardMovCA, i,
        vrMovTD[i].dia, vrMovTD[i].mes, vrMovTD[i].anio, DEBE, vrMovTD[i].detalle, vrMovTD[i].importe
      );

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
      // archivo.ignore();

      posMovCA = cardMovCA + i;

      agregarMovimientoVRCA(
        vrMovCA, cardMovCA, i,
        vrMovTC[i].dia, vrMovTC[i].mes, vrMovTC[i].anio, DEBE, vrMovTC[i].detalle, vrMovTC[i].importe
      );

      i++;
    }

    archivo.close();

    cardMovTC = i;
    cardMovCA += i;
  }

  /* void AgregarRegistroCA(sMovimientoCA vrMovimientosCA[], int &card, char moneda)
  {

  }

  void Cargar(sMovimientoCA vrMovimientosCA[], int &card, char moneda)
  {

  } */
}

namespace Ordenar
{
  void agregarEnOrden(string vector[], string elem, short &card)
  {
    while (card > 1 && elem < vector[card - 1]) {
      vector[card] = vector[card - 1];
      card--;
    }
    vector[card] = elem;
  }

  void agregarEnOrdenCA(sMovimientoCA vrMovCA[], sMovimientoCA elem, short &card)
  {
    int fechaElem = elem.anio * 10000 + elem.mes  * 100 + elem.dia;

    while (card > 1 && fechaElem < vrMovCA[card - 1].anio * 10000 + vrMovCA[card - 1].mes  * 100 + vrMovCA[card - 1].dia) {
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
                 vrMovimientosCA[i].mes  * 100 +
                 vrMovimientosCA[i].dia;
        fecha2 = vrMovimientosCA[i+1].anio * 10000 +
                 vrMovimientosCA[i+1].mes  * 100 +
                 vrMovimientosCA[i+1].dia;
        if (fecha1 > fecha2)
        {
          swap(vrMovimientosCA[i], vrMovimientosCA[i + 1]);
          ordenado = false;
        }
      }
    }while(!ordenado);
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
                 vrMovimientosTD[i].mes  * 100 +
                 vrMovimientosTD[i].dia;
        fecha2 = vrMovimientosTD[i+1].anio * 10000 +
                 vrMovimientosTD[i+1].mes  * 100 +
                 vrMovimientosTD[i+1].dia;
        if (fecha1 > fecha2)
        {
          swap(vrMovimientosTD[i], vrMovimientosTD[i + 1]);
          ordenado = false;
        }
      }
    }while(!ordenado);
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
                 vrMovimientosTC[i].mes  * 100 +
                 vrMovimientosTC[i].dia;
        fecha2 = vrMovimientosTC[i+1].anio * 10000 +
                 vrMovimientosTC[i+1].mes  * 100 +
                 vrMovimientosTC[i+1].dia;
        if (fecha1 > fecha2)
        {
          swap(vrMovimientosTC[i], vrMovimientosTC[i + 1]);
          ordenado = false;
        }
      }
    }while(!ordenado);
  }
}

namespace Menues
{
  void Menu_Login(sUsuario vrUsuarios[], sUsuario &rUsuario, bool &correr);
  void Menu_Principal(sUsuario rUsuario, bool &correr);
  void Menu_Cuentas(sMovimientoCA vrMovimientosCA[], int cardMovCA);
  void Submenu_Cuentas(sMovimientoCA vrMovimientosCA[], int &cardMovCA, short divisa);
  void Menu_TransferirDinero();
  void Submenu_TransferirDinero(char metodo);
  void Menu_TarjetaDebito(sMovimientoTD vrMovimientosTD[], int &cardMovTD);
  void Menu_TarjetaCredito(sMovimientoTC vrMovimientosTC[], int &cardMovTC);
  void Menu_Logout(bool &correr);

  void clrFullScr()
  {
    system("CLS");
  }

  void pararFullScr()
  {
    MnsgBox(2, _wherey() + 2, "Oprima la tecla ESPACIO para continuar...");
    GetAsyncKeyState(VK_SPACE);
    while (not GetAsyncKeyState(VK_SPACE))
      Sleep(1000);
    //BloquearCambioTamaño
  }

  void inicMostDeco()
  {
    limpiarEstadoTeclas();
    limpiarBufferEntrada();
    //_textbackground(NEGRO);
    //_clrscr();
    clrFullScr();
    _textbackground(AMARILLO);
    Marco(0, 0, COLS-2, LINEAS-2+4, AZUL_CLARO);
    MnsgBox(COLS-16, 0, AZUL, "Banco Haedo");
    ActualizaColores(BLANCO_BRILLANTE, NEGRO);
  }

  void mostRotuloMenu(string mensaje)
  {
    MnsgBox(COLS-mensaje.length()-5, 2, mensaje);
  }

  void obtenerOpcionSeleccionada(int &opcionSeleccionada, str24 opciones[], short numOpciones) {
    for (int i = 0; i < numOpciones; i++)
    {
      _gotoxy(5, i + 1);
      cout << opciones[i];
    }
    opcionSeleccionada = MenuNavegar(opciones, 1, numOpciones, 5);
  }

  void Menu_Login(sUsuario vrUsuarios[], sUsuario &rUsuario, bool &correr)
  {
    const int MAX_INTENTOS = 3;
    int contador = 0, seEncontro, dni;
    string usuario, clave;
    bool esValido = false;
    //Screen::_gotoxy(2,2);

    do
    {
      contador++;
      inicMostDeco();

      MnsgBox(2, 2, "Bienvenido al Sistema de Home Banking del Banco Haedo");
      obtenerEntrada(dni,     2, 4, "Ingrese su DNI     : ");
      obtenerEntrada(usuario, 2, 5, "Ingrese su usuario : ");
      obtenerEntrada(clave,   2, 6, "Ingrese su clave   : ");

      seEncontro = busBinVecDNI(vrUsuarios, 5, dni);
      esValido = (seEncontro != -1 && vrUsuarios[seEncontro].usuario == usuario && vrUsuarios[seEncontro].clave == clave);

      if (!esValido)
        MnsgBox(2,12,"Datos incorrectos. Intento " + to_string(contador) + "/" + to_string(MAX_INTENTOS));
      Espera(3000); // Espera 3 segundos antes de permitir otro intento
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
    const int MAX_MOV_TD   = 20;
    const int MAX_MOV_TC   = 20;
    const int MAX_MOV_CA   = MAX_MOV_TD + MAX_MOV_TC + 25;
    int opcionSeleccionada, cardMovCA, cardMovTD, cardMovTC;
    sMovimientoCA vrMovimientosCA[MAX_MOV_CA]; // Array para almacenar los movimientos leídos
    sMovimientoTD vrMovimientosTD[MAX_MOV_TD];
    sMovimientoTC vrMovimientosTC[MAX_MOV_TC];

    str24 opciones[NUM_OPCIONES] = {
      " [] Cuentas",
      "]>[ Transferir dinero",
      "[¯] Tarjetas",
      "+$? Simulacion P.F.",
      "$>U Compra/Venta dolares",
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
      "  ó Logout"
    };

    inicMenuPrincipal(rUsuario.nombre,
                      vrMovimientosCA, cardMovCA, MAX_MOV_CA,
                      vrMovimientosTD, cardMovTD,
                      vrMovimientosTC, cardMovTC);

    //_window(0, 0, 80, 52);
    //void _window(short izq, short sup, short der, short inf)

    obtenerOpcionSeleccionada(opcionSeleccionada, opciones, NUM_OPCIONES);
    switch (opcionSeleccionada)
    {
      case 0:
        Menu_Cuentas(vrMovimientosCA, cardMovCA);
        break;
      case 1:
        Menu_TransferirDinero();
        break;
      case 2:
        mostrarTexto("Tarjetas seleccionado.");
        break;
      case 3:
        mostrarTexto("Simulación P.F. seleccionada.");
        break;
      case 4:
        mostrarTexto("Compra/Venta de dólares seleccionado.");
        break;
      case 5:
        mostrarTexto("Inversión Plazo Fijo seleccionado.");
        break;
      case 6:
        mostrarTexto("Recargar seleccionado.");
        break;
      case 7:
        mostrarTexto("Generar Token seleccionado.");
        break;
      case 8:
        mostrarTexto("Mostrar CBU seleccionado.");
        break;
      case 9:
        mostrarTexto("Mis Cuentas seleccionado.");
        break;
      case 10:
        mostrarTexto("Datos personales seleccionado.");
        break;
      case 11:
        mostrarTexto("Modificar clave seleccionado.");
        break;
      case 12:
        mostrarTexto("Movimientos de Caja de Ahorros seleccionado.");
        break;
      case 13:
        Menu_TarjetaDebito(vrMovimientosTD, cardMovTD);
        break;
      case 14:
        Menu_TarjetaCredito(vrMovimientosTC, cardMovTC);
        break;
      case 15:
        mostrarTexto("Crear nueva cuenta seleccionado.");
        break;
      case 16:
        mostrarTexto("Depósito seleccionado.");
        break;
      case 17:
        mostrarTexto("Compras seleccionado.");
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
      "Pesos"
    };

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

  // string fmtImporte(float importe, short divisa)
  // {
  //   //string importeStr = to_string(importe);
  //   //char* importeChar = new char[importeStr.length() + 1];
  //   //strcpy(importeChar, importeStr.c_str());
  //   //strcat(divisasSimbolos[divisa]);
  //   short enteros, decimales;
  //   div_t

  //   enteros   = (int)importe;
  //   decimales = importe * 100;

  //   //char* importeChar = new char[decimales + 1];
  //   //sprintf(importeChar, "%.2f", importe);

  //   return divisasSimbolos[divisa] + to_string(enteros) + "." + to_string(decimales);
  // }
  //
  void inicReporte(string nombreReporte) {
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
    // DesBloquearCambioTamaño()

    inicReporte("Movimientos Caja de Ahorro");

    cout << Separador(75, '-')
         << endl << left
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

      //_gotoxy(5, i + 5);
      cout  <<        right << setw(3)  << i + 1
            << " "          << setw(2)  << vrMovimientosCA[i].dia << "-" << setw(2) << vrMovimientosCA[i].mes << "-" << setw(4)  << vrMovimientosCA[i].anio
            << " "                      << vrMovimientosCA[i].tipoMov
            << " " << left  << setw(25) << vrMovimientosCA[i].detalle;

      if (vrMovimientosCA[i].tipoMov == DEBE) {
        cout << " " << right << setw(10) << fixed << setprecision(2) << importe
             << " " << string(10, ' ');
        saldoCA -= importe;
      } else {
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

  void Menu_TransferirDinero()
  {
    const int NUM_OPCIONES = 2;
    int opcionSeleccionada;
    str24 opciones[NUM_OPCIONES] = {
      "CBU",
      "Celular"
    };

    inicMostDeco();

    mostRotuloMenu("Seleccione método de transferencia.");
    obtenerOpcionSeleccionada(opcionSeleccionada, opciones, NUM_OPCIONES);
    MnsgBox(2, 20, to_string(opcionSeleccionada));

    switch (opcionSeleccionada)
    {
      case CBU:
        Submenu_TransferirDinero('A');
        break;
      case CELULAR:
        Submenu_TransferirDinero('C');
        break;
    }
  }

  void Submenu_TransferirDinero(char metodo)
  {
    string destinatario;
    double monto;
    inicMostDeco();

    if (metodo == 'A')
      MnsgBox(10, 3, "Transferencia por CBU seleccionada.");
    else
      MnsgBox(10, 3, "Transferencia por Celular seleccionada.");

    do{
      obtenerEntrada(destinatario, 10, 7, "Ingrese el destinatario: ");
    } while(destinatario == "");

    do {
      obtenerEntrada(monto, 10, 9, "Ingrese el monto a transferir: ");
    } while(monto <= 0);

    MnsgBox(2,_wherey()+2, "Transferencia realizada");

    pararFullScr();
  }


  void Menu_TarjetaDebito(sMovimientoTD vrMovimientosTD[], int &cardMovTD)
  {
    float totalTD = 0;
    clrFullScr();
    // DesBloquearCambioTamaño()
    //
    inicReporte("Movimientos Tarjeta Débito");

    cout << Separador()
         << endl
         << "Mov."
         << " " << setw(10) << "Fecha"
         << " " << setw(25) << "Detalle"
         << " " << "Importe" // << setw(10)
         << endl
         << Separador()
         << endl;
    for (int i = 0; i < cardMovTD; i++)
    {
      //_gotoxy(5, i + 5);
      cout << setw(4) << i + 1
            << " " << setw(2)  << vrMovimientosTD[i].dia << "-" << setw(2) << vrMovimientosTD[i].mes << "-" << setw(4)  << vrMovimientosTD[i].anio
            << " " << setw(25) << vrMovimientosTD[i].detalle
            << " " << setw(10)  << vrMovimientosTD[i].importe
            << endl;
      totalTD += vrMovimientosTD[i].importe;
    }
    cout  << Separador() << endl
          << setw(42) << "Total TD: $"
          << setw(10)  << totalTD
          << endl
          << Separador() << endl;

    pararFullScr();
  }

  void Menu_TarjetaCredito(sMovimientoTC vrMovimientosTC[], int &cardMovTC)
  {
    float totalTC = 0;
    clrFullScr();
    // DesBloquearCambioTamaño()

    inicReporte("Movimientos Tarjeta Crédito");

    cout << Separador()
         << endl
         << "Mov."
         << " " << setw(10) << "Fecha"
         << " " << setw(25) << "Detalle"
         << " " << setw(5)  << "Cuota"
         << " " << "Importe" // << setw(10)
         << endl
         << Separador()
         << endl;
    for (int i = 0; i < cardMovTC; i++)
    {
      //_gotoxy(5, i + 5);
      cout << setw(4) << i + 1
            << " " << setw(2)  << vrMovimientosTC[i].dia << "-" << setw(2) << vrMovimientosTC[i].mes << "-" << setw(4)  << vrMovimientosTC[i].anio
            << " " << setw(25) << vrMovimientosTC[i].detalle
            << " " << setw(5)  << vrMovimientosTC[i].cuota
            << " " << setw(10)  << vrMovimientosTC[i].importe
            << endl;
      totalTC += vrMovimientosTC[i].importe;
    }
    cout  << Separador() << endl
          << setw(42) << "Total TC: $"
          << setw(10) << totalTC
          << endl
          << Separador() << endl;

    pararFullScr();
  }

  void Menu_Logout(bool &correr)
  {
    Pausa("Logout seleccionado.");
    correr = false;
    //exit(0);
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
      {56789012, "Luis Ramirez", 120600, "luisr", "clave345", 5678901234, "luisramirez@gmail.com", "Calle 012", 543210987, 5566778899}
  };
  sUsuario rUsuario;

  SetConsoleOutputCP(CP_UTF8); // Cambia codificación de salida a UTF-8. https://share.gemini.google/EpwcvMRWv77G

  BarraTitulo();
  // _window(0, 0, 80, 25);
  Menues::Menu_Login(vrUsuarios, rUsuario, correr);
  while(correr)
    Menues::Menu_Principal(rUsuario, correr);
}

int main()
{
  SistemaHomeBanking();
  return 0;
}
