#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include <random>
#include <algorithm>
#include "include/functions.h"


int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        std::cerr << "Uso: " << argv[0] << "<ruta de instancia> <número de iteraciones maximas> <temperatura inicial> cantidad de iteraciones para disminución de temperatura> <cambio de temperatura> \n";
        return 1; // Devuelve un código de error si no se proporciona el argumento correcto
    }
    int maxIteraciones = std::atoi(argv[2]);
    int tInicial = std::atoi(argv[3]);
    int stepsDeltaT = std::atoi(argv[4]);
    double dTRate = std::atof(argv[5]);
    std::ifstream inFile;
    inFile.open(argv[1]);

    if (!inFile)
    {
        std::cerr << "No se pudo abrir el archivo.\n";
        return 1; // Devuelve un código de error
    }

    int numEventos;
    float presupuesto, radioCobertura;
    inFile >> numEventos >> presupuesto >> radioCobertura;
    std::cout << "Número de eventos: " << numEventos << "\n";
    std::cout << "Presupuesto: " << presupuesto << "\n";
    std::cout << "Radio de Cobertura: " << radioCobertura << "\n";
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Evento> eventos(numEventos);
    std::vector<bool> solucionInicial(numEventos);
    for (int idx = 0; idx < numEventos; ++idx)
    {
        int x, y;
        bool existAED;

        inFile >> x >> y >> existAED;
        solucionInicial[idx] = existAED;
        eventos[idx] = {x, y, idx, existAED};
    }

    inFile.close();
    std::vector<std::vector<bool>> A(numEventos, std::vector<bool>(numEventos));

    for (int i = 0; i < numEventos; ++i)
    {
        for (int j = 0; j < numEventos; ++j)
        {
            A[i][j] = withinRadius(eventos[i], eventos[j], radioCobertura);
        }
    }



    std::vector<bool> initialState = solucionInicial;
    Best solucionFinal = SA(maxIteraciones,tInicial,solucionInicial,numEventos,A,stepsDeltaT,dTRate, initialState, presupuesto);

    auto end = std::chrono::high_resolution_clock::now();

    // std::cout << "\nSol encontrada\n";
    // for (const auto &i : solucionFinal.solution )
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << "\n";

    
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Numero de eventos cubiertos: " << solucionFinal.objetivo.evalFuncObjetivo << "\n";
    std::cout << "Porcentaje de eventos cubiertos: " << 100*solucionFinal.objetivo.evalFuncObjetivo/numEventos << "%\n";
    std::cout << "Tiempo de ejecucion: " << elapsed.count() << " segundos\n";
    printPoints(solucionFinal, numEventos, initialState, eventos);
    std::cout << "Presupuesto restante: " << presupuesto - calcularCosto(solucionFinal.solution, initialState, numEventos) << "\n";
    return 0;
}
