// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"
#include "EnumComboCustomization.h"


class DOLPHINEDITOR_API FDolphinResponseCallbackBinderCustomization : public FEnumComboCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	void GenerateNodeArrayElementWidget(TSharedRef<IPropertyHandle> PropertyHandle, int32 ArrayIndex, IDetailChildrenBuilder& ChildrenBuilder);

	void Clean();

private:
	TSharedPtr<IPropertyHandle> ProtocolModule;
	TSharedPtr<IPropertyHandle> SpecificCallback;
	UEnum* EProtocolModuleType;
};
