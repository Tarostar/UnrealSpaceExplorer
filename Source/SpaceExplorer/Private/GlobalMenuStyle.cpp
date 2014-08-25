

#include "SpaceExplorer.h"
#include "GlobalMenuStyle.h"

void FGlobalStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
}

const FName FGlobalStyle::TypeName = TEXT("FGlobalStyle");

const FName FGlobalStyle::GetTypeName() const
{
	static const FName TypeName = TEXT("FGlobalStyle");
	return TypeName;
}

const FGlobalStyle& FGlobalStyle::GetDefault()
{
	static FGlobalStyle Default;
	return Default;
}

UGlobalMenuStyle::UGlobalMenuStyle(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
}

const struct FSlateWidgetStyle* const UGlobalMenuStyle::GetStyle() const
{
	return static_cast<const struct FSlateWidgetStyle*>(&MenuStyle);
}
