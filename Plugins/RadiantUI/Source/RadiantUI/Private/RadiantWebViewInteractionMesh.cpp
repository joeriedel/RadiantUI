// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"

bool URadiantWebViewInteractionMesh::GetUVForPosition(int InTriIndex, const FVector& InPoint, FVector2D& OutUV)
{
	for (int i = 0; i < Sections.Num(); ++i)
	{
		const FRadiantWebViewInteractionMeshSection& Section = Sections[i];

		if ((InTriIndex >= (int)Section.FirstTriangle) && (InTriIndex <= (int)Section.LastTriangle))
		{
			const FRadiantWebViewInteractionMeshTriangle& Tri = Section.Triangles[InTriIndex - (int)Section.FirstTriangle];
			FVector BaryCentricCoord = FMath::ComputeBaryCentric2D(InPoint, Tri.Verts[0], Tri.Verts[1], Tri.Verts[2]);
			OutUV = (Tri.UV[0] * BaryCentricCoord.X) + (Tri.UV[1] * BaryCentricCoord.Y) + (Tri.UV[2] * BaryCentricCoord.Z);
			return true;
		}
	}

	return false;
}