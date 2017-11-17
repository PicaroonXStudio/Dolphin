// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"

class FEnumComboCustomization : public IPropertyTypeCustomization
{
public:
	virtual TSharedRef<SWidget> OnEnumValueContent(TSharedRef<IPropertyHandle> PropertyHandle, UEnum* EnumType, FExecuteAction EnumValueComboChangeDelegate) const;
	virtual TSharedRef<SWidget> OnEnumValueContent(TSharedRef<IPropertyHandle> PropertyHandle, UEnum* EnumType) const;

	virtual FText GetEnumValueDesc(TSharedRef<IPropertyHandle> PropertyHandle, UEnum *EnumType) const;

	virtual void OnEnumValueComboChange(TSharedRef<IPropertyHandle> PropertyHandle, uint8 Index, FExecuteAction EnumValueComboChangeDelegate);
	virtual TSharedRef<SWidget> CreateComboButton(TSharedRef<IPropertyHandle> PropertyHandle, UEnum *EnumType, FExecuteAction EnumValueComboChangeDelegate);
	virtual TSharedRef<SWidget> CreateComboButton(TSharedRef<IPropertyHandle> PropertyHandle, UEnum *EnumType);

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override = 0;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override = 0;

};