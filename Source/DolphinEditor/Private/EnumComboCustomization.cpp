// Fill out your copyright notice in the Description page of Project Settings.

#include "EnumComboCustomization.h"
#include "MultiBoxBuilder.h"
#include "SComboButton.h"


TSharedRef<SWidget> FEnumComboCustomization::OnEnumValueContent(TSharedRef<IPropertyHandle> PropertyHandle, UEnum *EnumType, FExecuteAction EnumValueComboChangeDelegate) const
{
	FMenuBuilder MenuBuilder(true, NULL);

	for (int32 i = 0; i < EnumType->NumEnums() - 1; i++)
	{
		FString DisplayedName = EnumType->GetEnumText(i).ToString();
		FUIAction ItemAction(FExecuteAction::CreateSP(this, &FEnumComboCustomization::OnEnumValueComboChange, PropertyHandle, (uint8)EnumType->GetValueByIndex(i), EnumValueComboChangeDelegate));
		MenuBuilder.AddMenuEntry(FText::FromString(DisplayedName), TAttribute<FText>(), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget> FEnumComboCustomization::OnEnumValueContent(TSharedRef<IPropertyHandle> PropertyHandle, UEnum* EnumType) const
{
	FExecuteAction Delegate;
	return OnEnumValueContent(PropertyHandle, EnumType, Delegate);
}

FText FEnumComboCustomization::GetEnumValueDesc(TSharedRef<IPropertyHandle> PropertyHandle, UEnum *EnumType) const
{
	uint8 CurrentIntValue;

	FPropertyAccess::Result Result = PropertyHandle->GetValue(CurrentIntValue);
	if (Result == FPropertyAccess::Success)
	{
		return EnumType->GetEnumTextByValue(CurrentIntValue);

	}

	return FText::GetEmpty();
}


void FEnumComboCustomization::OnEnumValueComboChange(TSharedRef<IPropertyHandle> PropertyHandle, uint8 Index, FExecuteAction EnumValueComboChangeDelegate)
{
	PropertyHandle->SetValue(Index);
	if (EnumValueComboChangeDelegate.IsBound())
	{
		EnumValueComboChangeDelegate.Execute();
	}
}

TSharedRef<SWidget> FEnumComboCustomization::CreateComboButton(TSharedRef<IPropertyHandle> PropertyHandle,UEnum *EnumType, FExecuteAction EnumValueComboChangeDelegate)
{
	return SNew(SComboButton)
		.OnGetMenuContent(this, &FEnumComboCustomization::OnEnumValueContent, PropertyHandle, EnumType, EnumValueComboChangeDelegate)
		.ContentPadding(FMargin(2.0f, 2.0f))
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(this, &FEnumComboCustomization::GetEnumValueDesc, PropertyHandle, EnumType)
		.Font(IDetailLayoutBuilder::GetDetailFont())
		];
}

TSharedRef<SWidget> FEnumComboCustomization::CreateComboButton(TSharedRef<IPropertyHandle> PropertyHandle, UEnum *EnumType)
{
	FExecuteAction Action;
	return CreateComboButton(PropertyHandle, EnumType, Action);
}

