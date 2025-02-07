#pragma once

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanelSlot.h"

#include "Components/ListView.h"
#include "Components/TileView.h"
#include "Components/TreeView.h"
#include "Components/BackgroundBlur.h"

#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Animation/WidgetAnimation.h"

#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/SlateBlueprintLibrary.h"

#include "Slate/SceneViewport.h"
#include "Slate/SlateBrushAsset.h"

enum class EEntryWidgetMouseState
{
	None,
	MouseOn,
	Select
};