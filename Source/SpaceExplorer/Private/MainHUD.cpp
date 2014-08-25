

#include "SpaceExplorer.h"
#include "MainHUD.h"
#include "MainMenu.h"
//#include "MyUIWidget.h"


AMainHUD::AMainHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

void AMainHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GEngine && GEngine->GameViewport)
	{
		UGameViewportClient* Viewport = GEngine->GameViewport;

		SAssignNew(MainMenu, SMainMenu).MainHUD(TWeakObjectPtr<AMainHUD>(this));

		Viewport->AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(MainMenu.ToSharedRef()));
	}
}

/*void AMainHUD::BeginPlay()
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////So far only TSharedPtr<SMyUIWidget> has been created, now create the actual object.
	/////Create a SMyUIWidget on heap, our MyUIWidget shared pointer provides handle to object
	/////Widget will not self-destruct unless the HUD's SharedPtr (and all other SharedPtrs) destruct first.
	SAssignNew(MyUIWidget, SMyUIWidget);// .OwnerHUD(this);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////Pass our viewport a weak ptr to our widget
	if (GEngine->IsValidLowLevel())
	{
		GEngine->GameViewport->
			//Viewport's weak ptr will not give Viewport ownership of Widget
			AddViewportWidgetContent(SNew(SWeakWidget).PossiblyNullContent(MyUIWidget.ToSharedRef()));
	}

	if (MyUIWidget.IsValid())
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/////Set widget's properties as visible (sets child widget's properties recursively)
		MyUIWidget->SetVisibility(EVisibility::Visible);
	}
}*/