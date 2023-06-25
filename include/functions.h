#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>

struct Evento
{
    int x, y, idx;
    bool existAED;
};

struct Objetivo
{
    std::vector<bool> vectorCobertura;
    double evalFuncObjetivo;
};

struct Best
{
    std::vector<bool> solution;
    struct Objetivo objetivo;
};

bool withinRadius(Evento initial, Evento final, int radiusMax);
double probabilidadSA(double obActualMejor, double obCandidato, int T);
double generarAleatorio();
std::vector<bool> bitflip(std::vector<bool> inicial, int idx);
void printPoints(Best vectorSol, int numEventos_, std::vector<bool> initialState, std::vector<Evento> info);
std::vector<bool> swap(std::vector<bool> inicial, int id_start, int id_end);
double calcularCosto(const std::vector<bool> solucion, const std::vector<bool> estadoInicial, int numEventos);
Objetivo funcionObjetivo(std::vector<bool> sol, int numEventos, std::vector<std::vector<bool>> A, std::vector<bool> initialState, int presupuesto);
Best SA(int maxIteraciones_, int tInicial_, std::vector<bool> sol_, int numEventos_, std::vector<std::vector<bool>> A_, int stepsDeltaT_, double dTRate_, std::vector<bool> initialState, int presupuesto);

#endif 