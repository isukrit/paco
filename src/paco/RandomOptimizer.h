/* This file is part of PACO-PArtitioning Clustering Optimization a program
* to find network partitions using modular solvers and quality functions.
*
*  Copyright (C) 2015 Carlo Nicolini <carlo.nicolini@iit.it>
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
*/

#ifndef _RANDOPTIMIZER_H_
#define _RANDOPTIMIZER_H_

#include "QualityOptimizer.h"

class RandomOptimizer : public QualityOptimizer
{
public:
    RandomOptimizer() {}
    RandomOptimizer(const igraph_t *g, const QualityFunction &fun, const  igraph_vector_t *memb, const igraph_vector_t *weights=NULL);
    virtual ~RandomOptimizer();
    double optimize(const igraph_t *g, const QualityFunction &fun,const  igraph_vector_t *memb, const igraph_vector_t *weights=NULL);

protected:
    double diff_move(const igraph_t *g, const QualityFunction &fun, const igraph_vector_t *memb, int vert, size_t dest_comm);

};

#endif // RANDOMOPTIMIZER_H
