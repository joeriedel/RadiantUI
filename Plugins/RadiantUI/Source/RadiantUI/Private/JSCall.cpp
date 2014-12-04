// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"
#include "JSCall.h"

namespace
{
	ICefRuntimeVariant* TranslatePropertyHelper(UProperty* Property, void* Data, ICefRuntimeVariantFactory* VariantFactory)
	{
		ICefRuntimeVariant* Variant = nullptr;

		if (Property->IsA<UFloatProperty>())
		{
			Variant = VariantFactory->CreateDouble(Cast<UFloatProperty>(Property)->GetPropertyValue(Data));
		}
		else if (Property->IsA<UDoubleProperty>())
		{
			Variant = VariantFactory->CreateDouble(Cast<UDoubleProperty>(Property)->GetPropertyValue(Data));
		}
		else if (Property->IsA<UByteProperty>())
		{
			Variant = VariantFactory->CreateInt((int)Cast<UByteProperty>(Property)->GetPropertyValue(Data));
		}
		else if (Property->IsA<UIntProperty>())
		{
			Variant = VariantFactory->CreateInt(Cast<UIntProperty>(Property)->GetPropertyValue(Data));
		}
		else if (Property->IsA<UUInt32Property>())
		{
			Variant = VariantFactory->CreateInt((int)Cast<UUInt32Property>(Property)->GetPropertyValue(Data));
		}
		else if (Property->IsA<UBoolProperty>())
		{
			Variant = VariantFactory->CreateBool(Cast<UBoolProperty>(Property)->GetPropertyValue(Data));
		}
		else if (Property->IsA<UStrProperty>())
		{
			const FString& String = Cast<UStrProperty>(Property)->GetPropertyValue(Data);
			FTCHARToUTF8 Convert(*String);
			Variant = VariantFactory->CreateString(Convert.Get());
		}
		else if (Property->IsA<UNameProperty>())
		{
			const FName& Name = Cast<UNameProperty>(Property)->GetPropertyValue(Data);
			FTCHARToUTF8 Convert(*Name.ToString());
			Variant = VariantFactory->CreateString(Convert.Get());
		}
		else if (Property->IsA<UTextProperty>())
		{
			const FText& Text = Cast<UTextProperty>(Property)->GetPropertyValue(Data);
			FTCHARToUTF8 Convert(*Text.ToString());
			Variant = VariantFactory->CreateString(Convert.Get());
		}
		else if (UStructProperty* StructProperty = Cast<UStructProperty>(Property))
		{
			UScriptStruct* InnerStruct = StructProperty->Struct;
			Variant = FJavaScriptHelper::CreateVariantList(InnerStruct, Data, VariantFactory);
		}
		else if (UArrayProperty* ArrayProperty = Cast<UArrayProperty>(Property))
		{
			FScriptArray* ScriptArray = ArrayProperty->GetPropertyValuePtr(Data);
			if (ScriptArray->Num() > 0)
			{
				ICefRuntimeVariantList *List = VariantFactory->CreateList(0);

				uint8* ArrayBase = (uint8*)ScriptArray->GetData();

				for (int32 i = 0; i < ScriptArray->Num(); ++i)
				{
					ICefRuntimeVariant* Var = TranslatePropertyHelper(ArrayProperty->Inner, ArrayBase + (i*ArrayProperty->Inner->ElementSize), VariantFactory);
					if (Var)
					{
						List->SetValue(List->GetSize(), Var);
					}
				}

				if (List->GetSize() > 0)
				{
					Variant = List;
				}
				else
				{
					List->Release();
				}
			}
		}

		return Variant;
	}

	bool StoreFunctionParameter(const FString& HookName, void* Container, UProperty* Argument, ICefRuntimeVariant* Variant, int ArgumentIndex)
	{
		bool TypeMismatch = false;

		if (Variant->IsInt())
		{
			ICefRuntimeVariantInt* VarInt = static_cast<ICefRuntimeVariantInt*>(Variant);
				
			if (Argument->IsA<UByteProperty>())
			{
				Cast<UByteProperty>(Argument)->SetPropertyValue_InContainer(Container, (uint8)VarInt->GetValue());
			}
			else if (Argument->IsA<UIntProperty>())
			{
				Cast<UIntProperty>(Argument)->SetPropertyValue_InContainer(Container, VarInt->GetValue());
			}
			else if (Argument->IsA<UUInt32Property>())
			{
				Cast<UUInt32Property>(Argument)->SetPropertyValue_InContainer(Container, (int32)VarInt->GetValue());
			}
			else
			{
				TypeMismatch = true;
			}
	
		}
		else if (Variant->IsDouble())
		{
			ICefRuntimeVariantDouble* VarDouble = static_cast<ICefRuntimeVariantDouble*>(Variant);

			if (Argument->IsA<UFloatProperty>())
			{
				Cast<UFloatProperty>(Argument)->SetPropertyValue_InContainer(Container, (float)VarDouble->GetValue());
			}
			else if (Argument->IsA<UDoubleProperty>())
			{
				Cast<UDoubleProperty>(Argument)->SetPropertyValue_InContainer(Container, VarDouble->GetValue());
			}
			else
			{
				TypeMismatch = true;
			}
		}
		else if (Variant->IsBool())
		{
			ICefRuntimeVariantBool* VarBool = static_cast<ICefRuntimeVariantBool*>(Variant);

			if (Argument->IsA<UBoolProperty>())
			{
				Cast<UBoolProperty>(Argument)->SetPropertyValue_InContainer(Container, VarBool->GetValue());
			}
			else
			{
				TypeMismatch = true;
			}
		}
		else if (Variant->IsString())
		{
			ICefRuntimeVariantString* VarString = static_cast<ICefRuntimeVariantString*>(Variant);

			if (Argument->IsA<UStrProperty>())
			{
				FString String(VarString->GetValue());
				Cast<UStrProperty>(Argument)->SetPropertyValue_InContainer(Container, String);
			}
			else if (Argument->IsA<UNameProperty>())
			{
				FString String(VarString->GetValue());
				Cast<UNameProperty>(Argument)->SetPropertyValue_InContainer(Container, *String);
			}
			else if (Argument->IsA<UTextProperty>())
			{
				FString String(VarString->GetValue());
				Cast<UTextProperty>(Argument)->SetPropertyValue_InContainer(Container, FText::FromString(String));
			}
		}
		else if (Variant->IsList())
		{
			ICefRuntimeVariantList* VarList = static_cast<ICefRuntimeVariantList*>(Variant);

			if (UStructProperty* StructProperty = Cast<UStructProperty>(Argument))
			{
				UScriptStruct* InnerStruct = StructProperty->Struct;
				
				void* StructContainer = StructProperty->ContainerPtrToValuePtr<void*>(Container);

				int StructIndex = 0;
				for (TFieldIterator<UProperty> It(InnerStruct); It; ++It, ++StructIndex)
				{
					if (StructIndex >= VarList->GetSize())
					{
						UE_LOG(RadiantUILog, Error, TEXT("JavaScript Hook Function '%s' caller did not supply enough arguments for field '%s'."), *HookName, ArgumentIndex, *(*It)->GetPathName());
						return false;
					}

					if (!StoreFunctionParameter(HookName, StructContainer, *It, VarList->GetValue(StructIndex), ArgumentIndex))
					{
						return false;
					}
				}
			}
			else if (UArrayProperty* ArrayProperty = Cast<UArrayProperty>(Argument))
			{
				FScriptArrayHelper ScriptArray(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void*>(Container));
				check(ScriptArray.Num() == 0);

				if (VarList->GetSize() > 0)
				{
					const int ListSize = VarList->GetSize();
					ScriptArray.Resize(ListSize);

					ICefRuntimeVariant* VarHead = VarList->GetValue(0);

					for (int i = 0; i < ListSize; ++i)
					{
						ICefRuntimeVariant* Var = VarList->GetValue(i);
						if (Var->GetType() != VarHead->GetType())
						{
							UE_LOG(RadiantUILog, Error, TEXT("JavaScript Hook Function '%s' argument %d field '%s' subscript '%d' is an array element, but the provided parameter list contains heterogeneous element types. Array element initializers must all have the same type."), *HookName, ArgumentIndex, *Argument->GetPathName(), i);
							return false;
						}

						if (!StoreFunctionParameter(HookName, ScriptArray.GetRawPtr(i), ArrayProperty->Inner, Var, ArgumentIndex))
						{
							return false;
						}
					}
				}
			}
			else
			{
				TypeMismatch = true;
			}
		}
		
		if (TypeMismatch)
		{
			UE_LOG(RadiantUILog, Error, TEXT("JavaScript Hook Function '%s' argument %d field '%s' type mis-match."), *HookName, ArgumentIndex, *Argument->GetPathName());
		}

		return !TypeMismatch;
	}
}

void FJavaScriptHelper::ExecuteHook(UObject* Receiver, const FString& HookName, ICefRuntimeVariantList* Arguments)
{
	UFunction* Function = Receiver->GetClass()->FindFunctionByName(*HookName);
	if (!Function)
	{
		UE_LOG(RadiantUILog, Error, TEXT("JavaScript Hook Function '%s' was not found"), *HookName);
		return;
	}

	int ArgumentMismatch = -1;

	if (Arguments)
	{
		if (Arguments->GetSize() != Function->NumParms)
		{
			UE_LOG(RadiantUILog, Error, TEXT("JavaScript Hook Function '%s' on Object '%s' was called with the wrong arguments!"), *HookName, *Receiver->GetPathName());
			return;
		}
		
		void* Parms = (uint8*)FMemory_Alloca(Function->ParmsSize);
		FMemory::Memzero(Parms, Function->ParmsSize);

		int ArgumentIndex = 0;
		for (TFieldIterator<UProperty> It(Function); It && (It->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm; ++It, ++ArgumentIndex)
		{
			if (!StoreFunctionParameter(HookName, Parms, *It, Arguments->GetValue(ArgumentIndex), ArgumentIndex))
			{
				return;
			}
		}

		Receiver->ProcessEvent(Function, Parms);
	}
	else
	{
		if (Function->NumParms == 0)
		{
			Receiver->ProcessEvent(Function, nullptr);
		}
		else
		{
			ArgumentMismatch = 0;
		}
	}

	if (ArgumentMismatch >= 0)
	{
		UE_LOG(RadiantUILog, Error, TEXT("JavaScript Hook Function '%s' argument %d type mismatch"), *HookName, ArgumentMismatch);
	}
}

ICefRuntimeVariantList* FJavaScriptHelper::CreateVariantList(UStruct* Class, void* Container, ICefRuntimeVariantFactory* VariantFactory)
{
	ICefRuntimeVariantList* List = VariantFactory->CreateList(0);

	for (TFieldIterator<UProperty> It(Class, EFieldIteratorFlags::ExcludeSuper); It; ++It)
	{
		UProperty *Property = *It;

		ICefRuntimeVariant* Variant = TranslatePropertyHelper(Property, Property->ContainerPtrToValuePtr<void*>(Container, 0), VariantFactory);
		if (Variant)
		{
			List->SetValue(List->GetSize(), Variant);
		}
	}

	if (List->GetSize() < 1)
	{
		List->Release();
		List = nullptr;
	}

	return List;
}
