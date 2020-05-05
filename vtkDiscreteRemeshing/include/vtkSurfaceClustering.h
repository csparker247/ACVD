/***************************************************************************
                          vtkSurfaceClustering.h  -  description
                             -------------------
    begin                : October 2006
    copyright            : (C) 2006 by Sebastien Valette
    email                :
 ***************************************************************************/

/* ---------------------------------------------------------------------

* Copyright (c) CREATIS-LRMN (Centre de Recherche en Imagerie Medicale)
* Author : Sebastien Valette
*
*  This software is governed by the CeCILL-B license under French law and
*  abiding by the rules of distribution of free software. You can  use,
*  modify and/ or redistribute the software under the terms of the CeCILL-B
*  license as circulated by CEA, CNRS and INRIA at the following URL
*  http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
*  or in the file LICENSE.txt.
*
*  As a counterpart to the access to the source code and  rights to copy,
*  modify and redistribute granted by the license, users are provided only
*  with a limited warranty  and the software's author,  the holder of the
*  economic rights,  and the successive licensors  have only  limited
*  liability.
*
*  The fact that you are presently reading this means that you have had
*  knowledge of the CeCILL-B license and that you accept its terms.
* ------------------------------------------------------------------------ */
#ifndef _VTKSURFACECLUSTERING_H_
#define _VTKSURFACECLUSTERING_H_

#include <vtkTriangleFilter.h>

#include "vtkLloydClustering.h"
#include "vtkThreadedClustering.h"
#include "vtkUniformClustering.h"

/**
 * This class provides an abstract layer to interface vtkUniformClustering with
 * vtkSurface objects Two classes derive from vtkSurfaceClustering :
 * vtkVerticesProcessing and vtkTrianglesProcessing
 */

#ifdef DOmultithread
template <class Metric>
class vtkSurfaceClustering : public vtkThreadedClustering<Metric>
#else
#ifdef DOLloydClustering
template <class Metric>
class vtkSurfaceClustering : public vtkLloydClustering<Metric>
#else
template <class Metric>
class vtkSurfaceClustering : public vtkUniformClustering<Metric>
#endif
#endif
{
public:
    /// Sets the Input mesh
    void SetInput(vtkSurface* Input);

    /// Returns the Input mesh
    vtkSurface* GetInput() { return (this->Input); }

    vtkIdType GetNumberOfEdges() { return this->Input->GetNumberOfEdges(); }

    void GetEdgeItemsSure(vtkIdType Item, vtkIdList* VList)
    {
        if (this->ClusteringType == 0)
            this->GetInput()->GetEdgeFaces(Item, VList);
        else {
            vtkIdType v1, v2;
            VList->Reset();
            this->Input->GetEdgeVertices(Item, v1, v2);
            VList->InsertNextId(v1);
            VList->InsertNextId(v2);
        }
    }

    vtkGetMacro(ClusteringType, int);

protected:
    /// Parameter indicating the type of Clustering (0: Faces ;1:Vertices)
    int ClusteringType;

    virtual int GetNumberOfDualItems() = 0;

    void BuildMetric();

    vtkSurfaceClustering();
    ~vtkSurfaceClustering();

    /// The input vtkSurface
    vtkSurface* Input;
};

template <class Metric>
void vtkSurfaceClustering<Metric>::BuildMetric()
{
    this->MetricContext.BuildMetric(
        this->Clusters, this->Input, this->NumberOfClusters,
        this->ClusteringType);
}

template <class Metric>
void vtkSurfaceClustering<Metric>::SetInput(vtkSurface* Input)
{
    if (this->Input)
        this->Input->UnRegister(this);

    vtkIdType NV, *Vertices;
    bool trianglesOnly = true;
    for (vtkIdType i = 0; i < Input->GetNumberOfCells(); i++) {
        if (Input->IsFaceActive(i)) {
            Input->GetCellPoints(i, NV, Vertices);
            if (NV != 3) {
                trianglesOnly = false;
                break;
            }
        }
    }

    if (!trianglesOnly) {
        // triangulate the mesh if it contains polygons
        vtkTriangleFilter* triangulate = vtkTriangleFilter::New();
        triangulate->SetInputData(Input);
        triangulate->PassVertsOff();
        triangulate->Update();
        Input = vtkSurface::New();
        Input->CreateFromPolyData(triangulate->GetOutput());
        triangulate->Delete();
    }

    this->Input = Input;
    if (Input)
        this->Input->Register(this);
}

template <class Metric>
vtkSurfaceClustering<Metric>::vtkSurfaceClustering()
{
    this->Input = 0;
}

template <class Metric>
vtkSurfaceClustering<Metric>::~vtkSurfaceClustering()
{
    if (this->Input)
        this->Input->Delete();
}
#endif
