// Copyright 2014 Joseph Riedel. All Rights Reserved.

#include "Assert.h"
#include "Variants.h"
#include <vector>
#include <map>

#define MAKE_BUILTIN_VARIANT(__Name, __Type) \
class Var##__Name : public ICefRuntimeVariant##__Name, public CefBase \
{ \
	IMPLEMENT_REFCOUNTING(Var##__Name); \
public:\
	__Type Value;\
	Var##__Name(__Type InValue) : Value(InValue) { AddRef(); } \
	virtual EVariantType GetType() OVERRIDE{ return TYPE_##__Name; } \
	virtual __Type GetValue() OVERRIDE{ return Value; } \
};

namespace BuiltIns {
	MAKE_BUILTIN_VARIANT(Int, int)
	MAKE_BUILTIN_VARIANT(Double, double)
	MAKE_BUILTIN_VARIANT(Bool, bool)
} // builtins

class VarString : public ICefRuntimeVariantString
{
	IMPLEMENT_REFCOUNTING(VarString);
public:
	static const int MaxStringBuff = 16;
	char Fixed[MaxStringBuff];
	char* Value;

	VarString(const char* InValue)
	{
		AddRef();

		int len = (int)strlen(InValue);
		if (len + 1 > MaxStringBuff)
		{
			Value = (char*)malloc(len + 1);
			
		}
		else
		{
			Value = Fixed;
		}

		memcpy(Value, InValue, len);
		Value[len] = 0;
	}

	virtual ~VarString()
	{
		if (Value != Fixed)
		{
			free(Value);
		}
	}

	virtual EVariantType GetType()
	{
		return TYPE_String;
	}

	virtual const char* GetValue() 
	{
		return Value;
	}
};

class VarNull : public ICefRuntimeVariantNull
{
public:

	static VarNull StaticInstance;

	virtual int AddRef() OVERRIDE { return 1; }
	virtual int Release() OVERRIDE { return 1; }
	virtual EVariantType GetType() { return TYPE_Null; }
};

VarNull VarNull::StaticInstance;

class VarUndefined : public ICefRuntimeVariantUndefined
{
public:

	static VarUndefined StaticInstance;

	virtual int AddRef() OVERRIDE{ return 1; }
	virtual int Release() OVERRIDE{ return 1; }
	virtual EVariantType GetType() { return TYPE_Undefined; }
};

VarUndefined VarUndefined::StaticInstance;

class VarList : public ICefRuntimeVariantList
{
	IMPLEMENT_REFCOUNTING(VarList);
public:
	std::vector<CefRefPtr<ICefRuntimeVariant> > List;

	VarList(int InSize, bool InDefaultNulls)
	{
		AddRef();

		List.resize(InSize);

		if (InDefaultNulls)
		{
			for (int i = 0; i < InSize; ++i)
			{
				List[i] = &VarNull::StaticInstance;
			}
		}
	}

	virtual EVariantType GetType() { return TYPE_List; }

	virtual void SetSize(int InSize) OVERRIDE
	{
		int CurrentSize = (int)List.size();
		List.resize(InSize);

		if (InSize > CurrentSize)
		{
			for (int i = CurrentSize; i < InSize; ++i)
			{
				List[i] = &VarNull::StaticInstance;
			}
		}
	}

	virtual int GetSize() OVERRIDE
	{
		return (int)List.size();
	}

	virtual ICefRuntimeVariant* GetValue(int InIndex) OVERRIDE
	{
		return List[InIndex].get();
	}

	virtual void SetValue(int InIndex, ICefRuntimeVariant* InValue) OVERRIDE
	{
		if (List.size() <= InIndex)
		{
			List.resize(InIndex+1);
		}

		List[InIndex] = InValue;
	}
};

class VarBlob : public ICefRuntimeVariantBlob
{
	IMPLEMENT_REFCOUNTING(VarBlob);
public:
	void* Data;
	size_t Size;

	VarBlob(const void* InData, size_t InSize) : Size(InSize)
	{
		AddRef();

		if (InSize > 0)
		{
			Data = malloc(InSize);
			if (InData)
			{
				memcpy(Data, InData, InSize);
			}
		}
		else
		{
			Data = nullptr;
		}
	}

	virtual ~VarBlob()
	{
		if (Data)
		{
			free(Data);
		}
	}

	virtual EVariantType GetType() OVERRIDE { return TYPE_Blob; }
	virtual size_t GetSize() OVERRIDE { return Size; }
	virtual const void* GetData() OVERRIDE{ return Data; }
};

class VarDictionary : public ICefRuntimeVariantDictionary
{
	IMPLEMENT_REFCOUNTING(VarDictionary);
public:
	typedef std::map<CefString, CefRefPtr<ICefRuntimeVariant> > MapType;
	MapType Map;

	VarDictionary()
	{
		AddRef();
	}

	virtual EVariantType GetType() OVERRIDE{ return TYPE_Dictionary; }

	virtual void Clear() OVERRIDE
	{
		Map.clear();
	}

	virtual bool HasKey(const char* InKey, EVariantType& OutVariantType) OVERRIDE
	{
		ICefRuntimeVariant* Value = GetValue(InKey);
		if (Value)
		{
			OutVariantType = Value->GetType();
		}
		
		return Value != nullptr;
	}

	virtual void Remove(const char* InKey) OVERRIDE
	{
		Map.erase(CefString(InKey));
	}

	virtual ICefRuntimeVariant* GetValue(const char* InKey)
	{
		MapType::iterator it = Map.find(CefString(InKey));
		if (it == Map.end())
		{
			return nullptr;
		}

		return it->second.get();
	}

	virtual void SetValue(const char* InKey, ICefRuntimeVariant* InVariant)
	{
		Map[CefString(InKey)] = InVariant;
	}

	virtual ICefRuntimeVariantList* GetKeys()
	{
		ICefRuntimeVariantList* NewList = new VarList((int)Map.size(), false);
		
		int Index = 0;
		for (MapType::iterator it = Map.begin(); it != Map.end(); ++it)
		{
			NewList->SetValue(Index++, new VarString(it->first.ToString().c_str()));
		}

		return NewList;
	}
};

class VariantFactory : public ICefRuntimeVariantFactory
{
public:

	virtual ICefRuntimeVariantUndefined* CreateUndefined() OVERRIDE
	{
		return &VarUndefined::StaticInstance;
	}

	virtual ICefRuntimeVariantNull* CreateNull() OVERRIDE
	{
		return &VarNull::StaticInstance;
	}

	virtual ICefRuntimeVariantInt* CreateInt(int InValue) OVERRIDE
	{
		return new BuiltIns::VarInt(InValue);
	}

	virtual ICefRuntimeVariantDouble* CreateDouble(double InValue) OVERRIDE
	{
		return new BuiltIns::VarDouble(InValue);
	}

	virtual ICefRuntimeVariantBool* CreateBool(bool InValue) OVERRIDE
	{
		return new BuiltIns::VarBool(InValue);
	}

	virtual ICefRuntimeVariantString* CreateString(const char* InValue) OVERRIDE
	{
		return new VarString(InValue);
	}

	virtual ICefRuntimeVariantList* CreateList(int InSize, bool InDefaultNulls) OVERRIDE
	{
		return new VarList(InSize, InDefaultNulls);
	}

	virtual ICefRuntimeVariantBlob* CreateBlob(const void* InData, size_t InSize) OVERRIDE
	{
		return new VarBlob(InData, InSize);
	}

	virtual ICefRuntimeVariantDictionary* CreateDictionary() OVERRIDE
	{
		return new VarDictionary();
	}
};

ICefRuntimeVariantFactory* GetStaticVariantFactory()
{
	static VariantFactory StaticInstance;
	return &StaticInstance;
}

CefRefPtr<CefV8Value> ListToV8Array_RenderThread(CefRefPtr<CefListValue> InList)
{
	REQUIRE_V8_CONTEXT()
	ASSERT(InList.get() && InList->IsValid());
	
	const size_t ListSize = InList->GetSize();

	CefRefPtr<CefV8Value> Array = CefV8Value::CreateArray((int)ListSize);

	for (size_t i = 0; i < ListSize; ++i)
	{
		Array->SetValue((int)i, ListItemToV8Value_RenderThread(InList, (int)i));
	}

	return Array;
}

CefRefPtr<CefListValue> V8ArrayToList_RenderThread(CefRefPtr<CefV8Value> InArray)
{
	REQUIRE_V8_CONTEXT()
	ASSERT(InArray.get() && InArray->IsValid());
	
	const int ArrayLength = InArray->GetArrayLength();

	CefRefPtr<CefListValue> List = CefListValue::Create();
	List->SetSize(ArrayLength);

	for (int i = 0; i < ArrayLength; ++i)
	{
		V8ValueToListItem_RenderThread(InArray->GetValue(i), List, i);
	}

	return List;
}

CefRefPtr<CefV8Value> ListItemToV8Value_RenderThread(CefRefPtr<CefListValue> InList, int InIndex)
{
	REQUIRE_V8_CONTEXT()
	ASSERT(InList.get() && InList->IsValid());

	switch (InList->GetType(InIndex))
	{
	case VTYPE_NULL:
		return CefV8Value::CreateNull();
	case VTYPE_BOOL:
		return CefV8Value::CreateBool(InList->GetBool(InIndex));
	case VTYPE_INT:
		return CefV8Value::CreateInt(InList->GetInt(InIndex));
	case VTYPE_DOUBLE:
		return CefV8Value::CreateDouble(InList->GetDouble(InIndex));
	case VTYPE_STRING:
		return CefV8Value::CreateString(InList->GetString(InIndex));
	case VTYPE_LIST:
		return ListToV8Array_RenderThread(InList->GetList(InIndex));
	}

	return nullptr;
}

void V8ValueToListItem_RenderThread(CefRefPtr<CefV8Value> InValue, CefRefPtr<CefListValue> InList, int InIndex)
{
	REQUIRE_V8_CONTEXT()
	ASSERT(InValue.get() && InValue->IsValid());
	ASSERT(InList.get() && InList->IsValid());

	if (InValue->IsBool())
	{
		InList->SetBool(InIndex, InValue->GetBoolValue());
	}
	else if (InValue->IsInt())
	{
		InList->SetInt(InIndex, InValue->GetIntValue());
	}
	else if (InValue->IsUInt())
	{
		InList->SetInt(InIndex, (int)InValue->GetUIntValue());
	}
	else if (InValue->IsDouble())
	{
		InList->SetDouble(InIndex, InValue->GetDoubleValue());
	}
	else if (InValue->IsString())
	{
		InList->SetString(InIndex, InValue->GetStringValue());
	}
	else if (InValue->IsArray())
	{
		InList->SetList(InIndex, V8ArrayToList_RenderThread(InValue));
	}
	else
	{
		InList->SetNull(InIndex);
	}
}

void SetCefListItemFromVariant(CefRefPtr<CefListValue> InList, int InIndex, ICefRuntimeVariant* InVariant)
{
	switch (InVariant->GetType())
	{
	case ICefRuntimeVariant::TYPE_Undefined:
		InList->SetNull(InIndex);
		break;
	case ICefRuntimeVariant::TYPE_Null:
		InList->SetNull(InIndex);
		break;
	case ICefRuntimeVariant::TYPE_Int:
		InList->SetInt(InIndex, static_cast<ICefRuntimeVariantInt*>(InVariant)->GetValue());
		break;
	case ICefRuntimeVariant::TYPE_Double:
		InList->SetDouble(InIndex, static_cast<ICefRuntimeVariantDouble*>(InVariant)->GetValue());
		break;
	case ICefRuntimeVariant::TYPE_Bool:
		InList->SetBool(InIndex, static_cast<ICefRuntimeVariantBool*>(InVariant)->GetValue());
		break;
	case ICefRuntimeVariant::TYPE_String:
		InList->SetString(InIndex, static_cast<ICefRuntimeVariantString*>(InVariant)->GetValue());
		break;
	case ICefRuntimeVariant::TYPE_List:
		InList->SetList(InIndex, VariantListToCefList(static_cast<ICefRuntimeVariantList*>(InVariant)));
		break;
	}
}

CefRefPtr<CefListValue> VariantListToCefList(ICefRuntimeVariantList* InList)
{
	CefRefPtr<CefListValue> List = CefListValue::Create();
	SetCefListFromVariantList(List, InList);
	return List;
}

void SetCefListFromVariantList(CefRefPtr<CefListValue> InList, ICefRuntimeVariantList* InVariantList)
{
	ASSERT(InList.get() && InList->IsValid());

	const int ListSize = InVariantList->GetSize();
	InList->SetSize(ListSize);

	for (int i = 0; i < ListSize; ++i)
	{
		SetCefListItemFromVariant(InList, i, InVariantList->GetValue(i));
	}
}

ICefRuntimeVariantBlob* CefBinaryToVariant(CefRefPtr<CefBinaryValue> InValue)
{
	ASSERT(InValue.get() && InValue->IsValid());

	VarBlob* Blob = new VarBlob(nullptr, InValue->GetSize());
	if (Blob->Data && (Blob->Size > 0))
	{
		InValue->GetData(Blob->Data, Blob->Size, 0);
	}

	return Blob;
}

ICefRuntimeVariantDictionary* CefDictionaryToVariant(CefRefPtr<CefDictionaryValue> InValue)
{
	ASSERT(InValue.get() && InValue->IsValid());

	CefDictionaryValue::KeyList Keys;
	InValue->GetKeys(Keys);

	VarDictionary* Dictionary = new VarDictionary();

	for (CefDictionaryValue::KeyList::iterator it = Keys.begin(); it != Keys.end(); ++it)
	{
		Dictionary->SetValue((*it).ToString().c_str(), CefDictionaryKeyValueToVariant(InValue, *it));
	}

	return Dictionary;
}

ICefRuntimeVariant* CefDictionaryKeyValueToVariant(CefRefPtr<CefDictionaryValue> InValue, const CefString& Key)
{
	CefValueType Type = InValue->GetType(Key);

	ASSERT(Type != VTYPE_INVALID);

	switch (Type)
	{
	case VTYPE_NULL:
		return &VarNull::StaticInstance;
	case VTYPE_BOOL:
		return new BuiltIns::VarBool(InValue->GetBool(Key));
	case VTYPE_INT:
		return new BuiltIns::VarInt(InValue->GetInt(Key));
	case VTYPE_DOUBLE:
		return new BuiltIns::VarDouble(InValue->GetDouble(Key));
	case VTYPE_STRING:
		return new VarString(InValue->GetString(Key).ToString().c_str());
	case VTYPE_BINARY:
		return CefBinaryToVariant(InValue->GetBinary(Key));
	case VTYPE_DICTIONARY:
		return CefDictionaryToVariant(InValue->GetDictionary(Key));
	case VTYPE_LIST:
		return CefListToVariant(InValue->GetList(Key));
	}

	return nullptr;
}

ICefRuntimeVariantList* CefListToVariant(CefRefPtr<CefListValue> InValue)
{
	ASSERT(InValue.get() && InValue->IsValid());

	const int ListSize = (int)InValue->GetSize();

	VarList* List = new VarList(ListSize, false);

	for (int i = 0; i < ListSize; ++i)
	{
		List->SetValue(i, CefListItemToVariant(InValue, i));
	}

	return List;
}

ICefRuntimeVariant* CefListItemToVariant(CefRefPtr<CefListValue> InValue, int InIndex)
{
	CefValueType Type = InValue->GetType(InIndex);

	ASSERT(Type != VTYPE_INVALID);

	switch (Type)
	{
	case VTYPE_NULL:
		return &VarNull::StaticInstance;
	case VTYPE_BOOL:
		return new BuiltIns::VarBool(InValue->GetBool(InIndex));
	case VTYPE_INT:
		return new BuiltIns::VarInt(InValue->GetInt(InIndex));
	case VTYPE_DOUBLE:
		return new BuiltIns::VarDouble(InValue->GetDouble(InIndex));
	case VTYPE_STRING:
		return new VarString(InValue->GetString(InIndex).ToString().c_str());
	case VTYPE_BINARY:
		return CefBinaryToVariant(InValue->GetBinary(InIndex));
	case VTYPE_DICTIONARY:
		return CefDictionaryToVariant(InValue->GetDictionary(InIndex));
	case VTYPE_LIST:
		return CefListToVariant(InValue->GetList(InIndex));
	}

	return nullptr;
}

