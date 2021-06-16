// Fill out your copyright notice in the Description page of Project Settings.

#include "DolphinProtocolSelectorCustomization.h"
#include "DetailLayoutBuilder.h"
#include "Engine.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "DolphinResponseCallbackStructs.h"

#define LOCTEXT_NAMESPACE "MyStructCustomization"

TSharedRef<IPropertyTypeCustomization> FDolphinResponseCallbackBinderCustomization::MakeInstance()
{
	return MakeShareable(new FDolphinResponseCallbackBinderCustomization());
}

void FDolphinResponseCallbackBinderCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
	.ValueContent()
		.MinDesiredWidth(500)
		[
			PropertyHandle->CreatePropertyValueWidget()
		];
}

void FDolphinResponseCallbackBinderCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ProtocolModule = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDolphinResponseCallbackBinder, ProtocolModule));
	SpecificCallback = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDolphinResponseCallbackBinder, SpecificCallback));

	EProtocolModuleType = FindObject<UEnum>(ANY_PACKAGE, TEXT("EProtocolModule"), true);
	
	TSharedRef<IPropertyHandle> PropertyHandleX = ProtocolModule->AsShared();

	ChildBuilder.AddCustomRow(LOCTEXT("Selector", "Selector"))
		.NameContent()
		[
			ProtocolModule->CreatePropertyNameWidget()
		]
	.ValueContent()
		[
			CreateComboButton(PropertyHandleX, EProtocolModuleType, FExecuteAction::CreateSP(this, &FDolphinResponseCallbackBinderCustomization::Clean))
		];

	ChildBuilder.AddCustomRow(LOCTEXT("Selector", "Selector"))
		.NameContent()
		[
			SpecificCallback->CreatePropertyNameWidget()
		]
	.ValueContent()
		[
			SpecificCallback->CreatePropertyValueWidget()
		];

	
	TSharedRef<FDetailArrayBuilder> NodeArrayBuilder = MakeShareable(new FDetailArrayBuilder(SpecificCallback.ToSharedRef(), false));
	NodeArrayBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(this, &FDolphinResponseCallbackBinderCustomization::GenerateNodeArrayElementWidget));

	ChildBuilder.AddCustomBuilder(NodeArrayBuilder);
}

void FDolphinResponseCallbackBinderCustomization::GenerateNodeArrayElementWidget(TSharedRef<IPropertyHandle> PropertyHandle, int32 ArrayIndex, IDetailChildrenBuilder& ChildrenBuilder)
{
	UEnum* SelectedEnumType;

	uint8 CurrentIntValue = 0;
	TSharedPtr<IPropertyHandle> PropertHandle = ProtocolModule;
	if (PropertHandle->IsValidHandle())
	{
		FPropertyAccess::Result Result = PropertHandle->GetValue(CurrentIntValue);

		FString DisplayedName = EProtocolModuleType->GetEnumNameStringByValue(CurrentIntValue);
		SelectedEnumType = FindObject<UEnum>(ANY_PACKAGE, *FString::Printf(TEXT("E%sProtocol"), *DisplayedName), true);
	}
	else {
		SelectedEnumType = FindObject<UEnum>(ANY_PACKAGE, TEXT("EUIDataError"), true);
	}

	if (!SelectedEnumType)
	{
		TSharedPtr<IPropertyHandleArray> PropertyHandleArray = SpecificCallback->AsArray();
		PropertyHandleArray->DeleteItem(ArrayIndex);
		return;
	}

	TSharedPtr<IPropertyHandle> SpecificProtocol = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDolphinResponseCallback, SpecificProtocol));
	TSharedPtr<IPropertyHandle> CallbackName = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FDolphinResponseCallback, CallbackName));
	
	TSharedRef<IPropertyHandle> PropertyHandleX = SpecificProtocol->AsShared();

	ChildrenBuilder.AddCustomRow(LOCTEXT("Selector", "Selector"))
		.NameContent()
		[
			CreateComboButton(PropertyHandleX, SelectedEnumType)
		]
	.ValueContent()
		[
			CallbackName->CreatePropertyValueWidget()
		];

}


void FDolphinResponseCallbackBinderCustomization::Clean()
{
		SpecificCallback->AsArray()->EmptyArray();
}

#undef LOCTEXT_NAMESPACE