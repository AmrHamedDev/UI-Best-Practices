// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBar.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SMeshWidget.h"
#include "Slate/SlateVectorArtInstanceData.h"

struct FHealthBarInstance
{
	FVector2D Position;
	FVector2D Offset;
	AActor* Owner = nullptr;

	FHealthBarInstance(AActor* InOwner)
	{
		Owner = InOwner;
	}

	void Update(float DeltaTime)
	{
	}
};

struct FHealthBar
{
private:
	TArray<AActor*> Characters;

public:
	TArray<FHealthBarInstance> Instances;

	FHealthBar()
	{
	}

	void CreateInstances(TArray<AActor*> InCharacters)
	{
		Characters = InCharacters;

		Instances.Empty();

		for (AActor* Actor : Characters)
		{
			FHealthBarInstance HPInstance(Actor);
			Instances.Add(HPInstance);
		}
	}

	void Update(float DeltaTime)
	{
		for (FHealthBarInstance& HealthBarInstance : Instances)
		{
			if (HealthBarInstance.Owner)
			{
				HealthBarInstance.Update(DeltaTime);
			}
			else
			{
				// Todo: Instances.Remove(HealthBarInstance);
				Instances.Empty();
			}
		}
	}
};

class SHealthBarMeshWidget : public SMeshWidget
{
public:
	SLATE_BEGIN_ARGS(SHealthBarMeshWidget) { }
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& Args, UHealthBar& InThis)
	{
		HealthBarWidget = &InThis;
		HealthBar = FHealthBar();
	}

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
	{
		const float Scale = AllottedGeometry.Scale;
		FVector2D TopLeft = AllottedGeometry.LocalToAbsolute(FVector2D::ZeroVector);

		if (MeshId != -1)
		{
			HealthBar.Update(Args.GetDeltaTime());

			FSlateInstanceBufferData PerInstaceUpdate;

			for (FHealthBarInstance Instance : HealthBar.Instances)
			{
				FVector2D Position;
				FVector OwnerLocation = Instance.Owner->GetActorLocation() + FVector(0.f, 0.f, HealthBarWidget->VerticalHealthbarOffset);
				UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(HealthBarWidget->GetOwningPlayer(), OwnerLocation, Position, false);

				Instance.Position = TopLeft + Position;

				FSlateVectorArtInstanceData HPData;
				HPData.SetPosition(Instance.Position + Instance.Offset * Scale);
				HPData.SetScale(Scale);

				PerInstaceUpdate.Add(TArray<UE::Math::TVector4<float>>::ElementType(HPData.GetData()));
			}

			const_cast<SHealthBarMeshWidget*>(this)->UpdatePerInstanceBuffer(MeshId, PerInstaceUpdate);
		}

		return SMeshWidget::OnPaint(Args, AllottedGeometry, MyClippingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	}

public:
	UHealthBar* HealthBarWidget;
	mutable FHealthBar HealthBar;
	int32 MeshId = -1;
};

UHealthBar::UHealthBar()
	: MeshId(-1)
{
}

void UHealthBar::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UHealthBar::InitHealthBarInstances(const TArray<AActor*> Characters)
{
	if (MeshAsset)
	{
		MeshId = MyMesh->AddMesh(*MeshAsset);
		MyMesh->MeshId = MeshId;

		MyMesh->EnableInstancing(MeshId, Characters.Num());
		MyMesh->HealthBar.CreateInstances(Characters);
	}
}

void UHealthBar::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyMesh.Reset();
}

TSharedRef<SWidget> UHealthBar::RebuildWidget()
{
	MyMesh = SNew(SHealthBarMeshWidget, *this);
	return MyMesh.ToSharedRef();
}