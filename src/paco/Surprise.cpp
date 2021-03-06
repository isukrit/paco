/* This file is part of PACO-PArtitioning Clustering Optimization a program
* to find network partitions using modular solvers and quality functions.
*
*  Copyright (C) 2016 Carlo Nicolini <carlo.nicolini@iit.it>
*
*  PACO is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 3 of the License, or (at your option) any later version.
*
*  Alternatively, you can redistribute it and/or
*  modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation; either version 2 of
*  the License, or (at your option) any later version.
*
*  PACO is distributed in the hope that it will be useful, but WITHOUT ANY
*  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*  FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License and a copy of the GNU General Public License along with
*  PACO. If not, see <http://www.gnu.org/licenses/>.
*
* If you use PACO for you publication please cite:
*
* "Modular structure of brain functional networks: breaking the resolution limit by Surprise"
* C. Nicolini and A. Bifone, Scientific Reports
* doi:10.1038/srep19250
*
* "Community detection in weighted brain connectivity networks beyond the resolution limit", 
* C.Nicolini. C.Bordier, A.Bifone, arxiv 1609.04316
* https://arxiv.org/abs/1609.04316
*/


/****************************************************************************
* Copyright (C) 2012 Rodrigo Aldecoa and Ignacio Marín *
* *
* This program is free software: you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation, either version 3 of the License, or *
* (at your option) any later version. *
* *
* This program is distributed in the hope that it will be useful, *
* but WITHOUT ANY WARRANTY; without even the implied warranty of *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the *
* GNU General Public License for more details. *
* *
* You should have received a copy of the GNU General Public License *
* along with this program. If not, see <http://www.gnu.org/licenses/>. *
* *
* Contact info: Rodrigo Aldecoa <raldecoa@ibv.csic.es> *
****************************************************************************/
/*
If you use this program, please cite:
Aldecoa R, Marín I (2011)
Deciphering network community structure by Surprise
PLoS ONE 6(9): e24195
*/

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <limits>

#include "AsymptoticSurprise.h"
#include "KLDivergence.h"
#include "Surprise.h"

#ifdef WIN32
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#endif

using std::cerr;
using std::endl;

bool checkArguments(const long p, const long pi, const long m, const long mi)
{
    if ( mi<0 )
    {
        throw std::logic_error("Error computing Surprise: Integer overflow: mi too big");
    }

    if ( m<0 )
    {
        throw std::logic_error("Error computing Surprise: Integer overflow: m too big");
    }

    if ( pi<0 )
    {
        throw std::logic_error("Error computing Surprise: Integer overflow: m too big");
    }

    if ( p<0 )
    {
        throw std::logic_error("Error computing Surprise: Integer overflow, p too big");
    }

    if (pi>p)
    {
        throw std::logic_error("Error computing Surprise: p<pi");
    }

    if (mi>m)
    {
        throw std::logic_error("Error computing Surprise: m<mi");
    }

    if ((m-mi) > (p-pi) )
    {
        throw std::logic_error("Error computing Surprise: p-pi<m-mi");
    }

    if (pi>p)
    {
        throw std::logic_error("Error computing Surprise: pi>p");
    }

    if (mi > pi)
        throw std::logic_error("Error computing Surprise: mi>pi");

    if (mi==0)
        return false;

    if (pi==0)
        return false;

    if ((m-mi) == (p-pi) )
        return false;

    return true;
}

/**
 * @brief computeSurprise
 * @param p
 * @param pi
 * @param m
 * @param mi
 * @return
 */
long double computeSurprise(const long p, const long pi,
                            const long m, const long mi)
{
    try
    {
        bool argsok = checkArguments(p,pi,m,mi);
        if (argsok==false)
            return 0;
    }
    catch ( const std::logic_error &e)
    {
        throw e;
    }

    long double j = mi;
    long double logP = logHyperProbability(p, pi, m, j);
    long double minsum = pi;
    if(m < pi)
        minsum = m;
    bool isEnough = false;
    while(!isEnough && j < minsum)
    {
        j++;
        long double nextLogP = logHyperProbability(p, pi, m, j);
        isEnough = sumLogProbabilities(nextLogP, logP);
    }
    if(logP == 0)
        logP *= -1;
    return -logP;
}

/**
 * @brief computeAsymptoticSurprise
 * @param p
 * @param pi
 * @param m
 * @param mi
 * @return
 */
long double computeAsymptoticSurprise(const long p, const long pi, const long m, const long mi)
{
    try
    {
        bool argsok = checkArguments(p,pi,m,mi);
        if (argsok==false)
            return 0;
    }
    catch ( std::exception &e)
    {
        throw e;
    }
    return asymptoticSurprise(p,pi,m,mi);
}

/**
 * @brief logStirFac
 * @param n
 * @return Stirling approximation given by Ramanujan ( http://en.wikipedia.org/wiki/Stirling%27s_approximation )
 */
long double logStirFac(long n)
{
    if (n<=1)
        return 1.0;
    else
    {
        return -n + n*log(n) + log(n*(1 + 4.0*n*(1.0 + 2.0*n)))/6.0 + log(M_PI)/2.0;
    }
}

/**
 * @brief logBin
 * @param n
 * @param k
 * @return
 */
long double logBin(long n, long k)
{
    return logStirFac(n) - logStirFac(k) - logStirFac(n - k);
}


/**
 * @brief logHyperProbability
 * @param p
 * @param pi
 * @param m
 * @param mi
 * @return
 */
long double logHyperProbability(const long& p, const long& pi,
                                const long& m, const long& mi)
{
    long double logH = logC(pi, mi) + logC(p - pi, m - mi) - logC(p, m);
    return logH / log(10.0);
}

/**
 * @brief logVarianceHyperProbability
 * @param p
 * @param pi
 * @param m
 * @param mi
 * @return
 */
/*
long double logVarianceHyperProbability(const long& p, const long& pi,
                                const long& m, const long& mi)
{
    return logH / log(10.0);
}
*/

/**
 * @brief logC
 * @param n
 * @param k
 * @return
 */
long double logC(const long &n, const long &k)
{
    if(k == n || !k)
        return 0;
    else if(n > 1000 && k > 1000) // Stirling's binomial coeff approximation
    {
        return logStirFac(n) - logStirFac(k) - logStirFac(n-k);
    }
    else
    {
        long double t = n - k;
        if(t < k)
            t = k;
        long double logC = sumRange(t + 1, n) - sumFactorial(n - t);
        return logC;
    }
}


/**
 * @brief sumRange
 * @param min
 * @param max
 * @return
 */
long double sumRange(const long& min, const long& max)
{
    long double sum = 0.0;
    for(long double i = min; i <= max; ++i)
        sum += log(i);
    return sum;
}


/**
 * @brief sumFactorial
 * @param n
 * @return
 */
long double sumFactorial(const long& n)
{
    if(n > 1000)
    {
        return n * log(n) - n;
    }
    else
    {
        long double sum = 0.0;
        for(int i = 2; i <= n; ++i)
            sum += log(i);
        return sum;
    }
}

/**
 * @brief sumLogProbabilities
 * @param nextLogP
 * @param logP
 * @return
 */
bool sumLogProbabilities(const long double& nextLogP, long double& logP)
{
    if(nextLogP == 0)
        return true;
    // Several optimizations to avoid over/underflow problems
    long double common, diffExponent;
    if(nextLogP > logP)
    {
        common = nextLogP;
        diffExponent = logP - common;
    }
    else
    {
        common = logP;
        diffExponent = nextLogP - common;
    }
    logP = common + ( (log1pl(pow(10, diffExponent))) / log(10) ); //changed log(1+x) with log1pl(x)
    // The cumulative summation stops when the increasing is less than 10e-4
    if(nextLogP - logP < -4)
        return true;
    return false;
}

long double computeConditionedSurprise(const long p, const long pi,
                            const long m, const long mi, const long ni)
{
    try
    {
        bool argsok = checkArguments(p,pi,m,mi);
        if (argsok==false)
            return 0;
    }
    catch ( const std::logic_error &e)
    {
        throw e;
    }
    // Using the addiction-subtraction formula
    // https://en.wikipedia.org/wiki/List_of_logarithmic_identities
    double surprise = computeSurprise(p,pi,m,mi);
    double conditional = computeSurprise(p,pi,m,ni-1);
    return surprise-conditional;
}

/**
 * @brief cumulativeHyperGeometricDistribution Returns the logarithm of the cumulative inverse hypergeometric distribution
 * sum_{i=drawWhiteBalls}^{drawnBalls} binom(whiteBallsInUrn, i) binom(totalBallsInUrn-whiteBallsInUrn, drawnBalls - i) / binom(totalBallsInUrn,drawnBalls)
 * @param totalBallsInUrn
 * @param whiteBallsInUrn
 * @param drawnBalls
 * @param drawnWhiteBalls
 * @return
 */
long double cumulativeHyperGeometricDistribution(const long totalBallsInUrn, const long whiteBallsInUrn, const long drawnBalls, const long drawnWhiteBalls)
{
    try
    {
        bool argsok = checkArguments(totalBallsInUrn,whiteBallsInUrn,drawnBalls,drawnWhiteBalls);
        if (argsok==false)
            return 0;
    }
    catch ( const std::logic_error &e)
    {
        throw e;
    }

    double logchyper = computeSurprise(totalBallsInUrn,whiteBallsInUrn,drawnBalls,drawnWhiteBalls)/log(10);
    return logchyper;
}
