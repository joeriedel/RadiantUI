// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"

ARadiantStaticMeshWebViewActor::ARadiantStaticMeshWebViewActor(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bCanBeDamaged = false;

#if WITH_EDITORONLY_DATA
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMeshRef(TEXT("/RadiantUI/FlatPanel"));
#endif

	StaticMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshComponent0"));

#if WITH_EDITORONLY_DATA
	StaticMeshComponent->StaticMesh = DefaultMeshRef.Object;
#endif

	StaticMeshComponent->Mobility = EComponentMobility::Static;
	StaticMeshComponent->bGenerateOverlapEvents = false;
	StaticMeshComponent->bIgnoreInstanceForTextureStreaming = true;
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RootComponent = StaticMeshComponent;
}

void ARadiantStaticMeshWebViewActor::PostInitializeComponents()
{
#if WITH_EDITORONLY_DATA
	ExtractInteractionMesh();
#endif
	RegisterMeshComponent(StaticMeshComponent);
	Super::PostInitializeComponents();
}

void ARadiantStaticMeshWebViewActor::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	static const int ArchiveVersion = 1;

	int Version = ArchiveVersion;

	Ar << Version;
	
	bool bCooked = Ar.IsCooking();
	Ar << bCooked;

	if (bCooked)
	{
#if WITH_EDITORONLY_DATA
		check(!Ar.IsLoading());
		if (!InteractionMesh)
		{
			ExtractInteractionMesh();
		}
#else
		check(Ar.IsLoading());
#endif
		
		Ar << InteractionMesh;
	}
}

bool ARadiantStaticMeshWebViewActor::GetUVForPoint(int InTriIndex, const FVector& InPoint, FVector2D& OutUV)
{
	if (InteractionMesh)
	{
		return InteractionMesh->GetUVForPosition(InTriIndex, InPoint, OutUV);
	}

	return false;
}

#if WITH_EDITORONLY_DATA
void ARadiantStaticMeshWebViewActor::ExtractInteractionMesh()
{
	check(InteractionMesh == nullptr);

	UStaticMesh* StaticMesh = StaticMeshComponent->StaticMesh;

	check(StaticMesh->RenderData->LODResources.Num() > 0);
	
	FStaticMeshLODResources& LOD = StaticMesh->RenderData->LODResources[0];

	TArray<uint32> Indices;
	LOD.IndexBuffer.GetCopy(Indices);

	InteractionMesh = ConstructObject<URadiantWebViewInteractionMesh>(URadiantWebViewInteractionMesh::StaticClass(), this);

	// Extract the sections that reference this material:

	for (int i = 0; i < LOD.Sections.Num(); ++i)
	{
		const FStaticMeshSection& StaticMeshSection = LOD.Sections[i];
		if ((StaticMeshSection.NumTriangles > 0) && (StaticMeshSection.MaterialIndex == (int)MaterialIndex))
		{
			FRadiantWebViewInteractionMeshSection InteractionMeshSection;

			InteractionMeshSection.FirstTriangle = StaticMeshSection.FirstIndex / 3;
			InteractionMeshSection.LastTriangle = InteractionMeshSection.FirstTriangle + StaticMeshSection.NumTriangles - 1;

			InteractionMeshSection.Triangles.Reserve(StaticMeshSection.NumTriangles);

			for (int TriangleNum = 0; TriangleNum < (int)StaticMeshSection.NumTriangles; ++TriangleNum)
			{
				FRadiantWebViewInteractionMeshTriangle Tri;

				const int BaseIndex = StaticMeshSection.FirstIndex + (TriangleNum*3);

				for (int i = 0; i < 3; ++i)
				{
					const int Index = Indices[BaseIndex + i];

					Tri.Verts[i] = LOD.PositionVertexBuffer.VertexPosition(Index);
					Tri.UV[i] = LOD.VertexBuffer.GetVertexUV(Index, 0);
				}

				InteractionMeshSection.Triangles.Add(Tri);
			}

			InteractionMesh->Sections.Add(InteractionMeshSection);
		}
	}
}
#endif

#if WITH_EDITOR
void ARadiantStaticMeshWebViewActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.MemberProperty != NULL) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(ARadiantStaticMeshWebViewActor, MaterialIndex)) || 
		(PropertyName == GET_MEMBER_NAME_CHECKED(ARadiantStaticMeshWebViewActor, StaticMeshComponent)))
	{
		if (bActorInitialized)
		{
			if (InteractionMesh)
			{
				InteractionMesh->MarkPendingKill();
				InteractionMesh = nullptr;
			}
			ExtractInteractionMesh();
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
