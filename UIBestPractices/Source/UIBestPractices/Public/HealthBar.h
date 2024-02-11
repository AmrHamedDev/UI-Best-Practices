// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "HealthBar.generated.h"

class USlateVectorArtData;

/**
 * 
 */
UCLASS()
class UIBESTPRACTICES_API UHealthBar : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	USlateVectorArtData* MeshAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float VerticalHealthbarOffset = 100.f;

public:
	UHealthBar();

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	UFUNCTION(BlueprintCallable)
	void InitHealthBarInstances(const TArray<AActor*> Characters);

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

private:
	TSharedPtr<class SHealthBarMeshWidget> MyMesh;

	int32 MeshId;

	friend class SHealthBarMeshWidget;
};
