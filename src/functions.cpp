#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include <random>
#include <algorithm>
struct Evento
{
    int x, y, idx;
    bool existAED;
};
struct Objetivo{
    std::vector<bool> vectorCobertura;
    double evalFuncObjetivo;
};
struct Best
{
    std::vector<bool> solution;
    struct Objetivo objetivo;
};
/**
 * Función que toma las cordenadas de 2 eventos y verifica si estas están a una distancia determinada
 * @param initial evento punto de partida
 * @param final evento punto final
 * @param radiusMax radio maximo permitido
 * @return valor booleano que representa si el par de evetos se encuentran a una distancia 
*/
bool withinRadius(Evento initial, Evento final, int radiusMax)
{
    double dx = initial.x - final.x;
    double dy = initial.y - final.y;
    return sqrt(pow(dx, 2) + pow(dy, 2)) <= radiusMax;
}

double probabilidadSA(double obActualMejor, double obCandidato,int T){
    return std::exp((obCandidato - obActualMejor)/T);
}

/**
 * Función que retorna un numero aleatorio entre 0 y 1
 * @return Número aleatorio
 */
double generarAleatorio()
{
    // Generador de números aleatorios con una semilla basada en el tiempo
    std::default_random_engine generador(time(0));
    std::uniform_real_distribution<double> distribucion(0.0, 1.0);

    return distribucion(generador);
}

/**
* Función que recibe un vector y un indice y retorna un vector en donde el i-esimo elemento flipeado
* @param inicial  vector solución
* @param idx  indice en el que se hará un bitflip
* @returns vector con el movimiento aplicado
*/
std::vector<bool> bitflip(std::vector<bool> inicial, int idx)
{
    inicial[idx] = 1 - inicial[idx];
    return inicial;
}
// void printPoints(Best vectorSol, int numEventos_, std::vector<bool> initialState, std::vector<Evento> info){
//     std::vector<bool> v = vectorSol.solution; //.objetivo.evalFuncObjetivo
//     int cantidadPreInstalados = std::count(initialState.begin(), initialState.end(), 1);
//     int cantidadInstalados = std::count(v.begin(), v.end(), 1) - cantidadPreInstalados;
//     int counterPre = 0;
//     int counterPost = 0;
//     int counter = 0;
//     std::vector<Evento> instalaciones(cantidadInstalados);
//     for (int i = 0; i < numEventos_; ++i) {
//         if (counter == vectorSol.objetivo.evalFuncObjetivo ) break;
//         if (v[i] == 1) {
//             if(initialState[i]) continue;
//             if (counterPre < cantidadPreInstalados)
//             {
//                 std::cout << "AED instalado en (" << info[i].x <<  "," << info[i].y <<") (nuevo)\n";
//                 counterPre++;
//             }
//             else if ( counterPost < cantidadInstalados ){
//                 std::cout << "AED instalado en (" << info[i].x <<  "," << info[i].y <<") (reubicado)\n";
//                 counterPost++;
//             }
//             counter++;
//         }

//     }   
// }
void printPoints(Best vectorSol, int numEventos_, std::vector<bool> initialState, std::vector<Evento> info) {
    std::vector<bool> v = vectorSol.solution;
    int cantidadInstalados = std::count(v.begin(), v.end(), true);
    int cantidadReubicados = 0;
    int considerados = 0;
    // Recorrer la solución final
    for (int i = 0; i < numEventos_; ++i) {
        if (initialState[i] == 1 && v[i] == 0)
        {
            cantidadReubicados++;
        }
    }
    for (int i = 0; i < numEventos_; i++){
        if(v[i] == 1 && initialState[i] == 1){
            considerados++;
                continue;
        }
        else if (v[i] == 1 && initialState[i] == 0 && considerados<cantidadInstalados)
        {
            std::cout << "AED instalado en (" << info[i].x <<  "," << info[i].y <<") (nuevo)\n";
            considerados++;            
        }
        else if(considerados<cantidadInstalados && cantidadReubicados > 0){
            std::cout << "AED instalado en (" << info[i].x <<  "," << info[i].y <<") (reubicado)\n";
            considerados++;
            cantidadReubicados--;
        }

    }
    
        
        // else if(v[i]){
        //     if(initialState[i] ==1) 
        //     {
        //         //ya existia
        //         considerados++;
        //         continue;
        //     }
        //     else if(  ){
        //         std::cout << "AED instalado en (" << info[i].x <<  "," << info[i].y <<") (nuevo)\n";
        //         considerados++;
        //     }

            
        // }
        
}






/**
 * Función que recibe un vector y un indice inicial e indice final y retorna un vector en donde el los elementos de los indices dados son intercambiados
 * @param inicial  vector solución
 * @param idx  indice en el que se hará un bitflip
 * @returns vector con el movimiento aplicado
 */
std::vector<bool> swap(std::vector<bool> inicial, int id_start, int id_end)
{
    if (inicial[id_start] && !inicial[id_end])
    {
        inicial[id_start] = 0;
        inicial[id_end] = 1;
    }
    return inicial;
}
double calcularCosto(const std::vector<bool> solucion, const std::vector<bool> estadoInicial, int numEventos) {
    double costo = 0.0;

    for (int i = 0; i < numEventos; ++i) {
        if (estadoInicial[i] == 0 && solucion[i] == 1) {
            // Por cada desfibrilador instalado en la solución, se suma 1 al costo
            costo += 1.0;
        }
        if (estadoInicial[i] == 1 && solucion[i] == 0) {
            // Por cada desfibrilador que se encontraba en el estado inicial y que no está en la solución, se resta 0.8 al costo
            costo -= 0.8;
        }
    }
    //std::cout << costo << "\n";
    return costo;
}


/**
 * @param sol
 * @param numEventos
 * @param matriz A
*/
Objetivo funcionObjetivo(std::vector<bool> sol, int numEventos, std::vector<std::vector<bool>> A, std::vector<bool> initialState, int presupuesto)
{
    double count = 0;
    std::vector<bool> Y(numEventos, 0);
    for (int i = 0; i < numEventos; ++i)
    {
        if (sol[i])
        {
            for (int j = 0; j < numEventos; ++j)
            {
                if (Y[j] == 0 && A[i][j])
                {
                    //std::cout << A[i][j] << "\n";
                    Y[j] = 1;
                    count++;
                }
            }
        }
    }
    //printVector(Y);
    return {Y,count};
}

/**
 * @param maxIteraciones_ cantidad de iteraciones permitidas.
 * @param tInicial_ Temperatura inicial
 * @param sol_ vector solución inicial
 * @param numEventos_ numero de eventos a cubrir
 * @param A_ Matriz de cobertura
 * @param stepsDeltaT_ cantidad de iteraciones para reducir la temperatura
 * @param dTRate Decremento porcentual de la temperatura luego de stepsDeltaT_ iteraciones
*/
Best SA(int maxIteraciones_, int tInicial_, std::vector<bool> sol_, int numEventos_, std::vector<std::vector<bool>> A_, int stepsDeltaT_, double dTRate_,  std::vector<bool> initialState,int presupuesto )
{
    int iteracion = 0;
    int stepT = 0;
    Objetivo obInicial = funcionObjetivo(sol_, numEventos_, A_, initialState ,presupuesto);
    Best bestGlobal = {sol_, obInicial};
    Best bestLocal = {sol_, obInicial};
    int T = tInicial_;
    while (iteracion < maxIteraciones_){
        int cantidadUnos = std::count(bestLocal.solution.begin(), bestLocal.solution.end(), 1);
        double proporcionUnos = static_cast<double>(cantidadUnos) / numEventos_;
        if ( generarAleatorio() < proporcionUnos)
        {

            Objetivo mejorEvaluacion = {std::vector<bool> (0), -1};
            std::vector<bool> mejorCandidato(numEventos_);
            for (int i = 0; i < numEventos_; ++i)
            {
                if (bestLocal.solution[i] != 1) continue;
                for (int j = 0; j < numEventos_; ++j)
                {
                    if ( bestLocal.solution[j] != 0) continue;
                    std::vector<bool> copia = bestLocal.solution;
                    std::swap(copia[i], copia[j]);
                    double costo = calcularCosto(copia, initialState, numEventos_);
                    if( costo > presupuesto && costo > 0 ) {
                        continue;}
                    Objetivo objetivoCopia = funcionObjetivo(copia, numEventos_, A_, initialState, presupuesto);
                    if (objetivoCopia.evalFuncObjetivo > mejorEvaluacion.evalFuncObjetivo)
                    {
                        mejorCandidato = copia;
                        mejorEvaluacion = objetivoCopia;

                    }
                    
                }
                
            }
            if(mejorEvaluacion.evalFuncObjetivo > bestLocal.objetivo.evalFuncObjetivo){
                bestLocal = {mejorCandidato, mejorEvaluacion};
                }
            else if (mejorEvaluacion.evalFuncObjetivo <= bestLocal.objetivo.evalFuncObjetivo){
                double r = generarAleatorio();
                if ( probabilidadSA(bestLocal.objetivo.evalFuncObjetivo, mejorEvaluacion.evalFuncObjetivo, T) <= r){
                    bestLocal = {mejorCandidato, mejorEvaluacion};
                    }            
            } 
            



        }
        else{
            // BITFLIP
            Objetivo mejorEvaluacion = {std::vector<bool> (0), -1};
            std::vector<bool> mejorCandidato(numEventos_);
            for (int j = 0; j < numEventos_; j++){   
                std::vector<bool> copia = bestLocal.solution; 
                copia = bitflip(copia, j);
                    double costo = calcularCosto(copia, initialState, numEventos_);
                    if( costo > presupuesto && costo > 0 ) {
                        continue;}
                Objetivo objetivoCopia = funcionObjetivo(copia, numEventos_, A_, initialState, presupuesto);
                if(objetivoCopia.evalFuncObjetivo > mejorEvaluacion.evalFuncObjetivo ){
                    mejorCandidato = copia;
                    mejorEvaluacion = objetivoCopia;
                }

            }
            if(mejorEvaluacion.evalFuncObjetivo > bestLocal.objetivo.evalFuncObjetivo){
                bestLocal = {mejorCandidato, mejorEvaluacion};
                }
            else if (mejorEvaluacion.evalFuncObjetivo <= bestLocal.objetivo.evalFuncObjetivo){
                double r = generarAleatorio();
                if ( probabilidadSA(bestLocal.objetivo.evalFuncObjetivo, mejorEvaluacion.evalFuncObjetivo, T) <= r){
                    bestLocal = {mejorCandidato, mejorEvaluacion};
                    }            
            } 
        }
        if (bestLocal.objetivo.evalFuncObjetivo > bestGlobal.objetivo.evalFuncObjetivo){
            //std::cout << "valores bestlocal, bestglobal: "<< bestLocal.objetivo.evalFuncObjetivo << " " << bestGlobal.objetivo.evalFuncObjetivo << "\n";
            bestGlobal = bestLocal;
        } 
        if( stepT == stepsDeltaT_){
            T = T*(1-dTRate_);
            //std::cout << "it: " << iteracion << "\n stepT: " << stepT << "\n";
            stepT = 0;
        }
        stepT++;
        iteracion++;
    }
    std::cout << "T final:" << T << "\niteracion:" << iteracion <<"\n";
    return bestGlobal;
}