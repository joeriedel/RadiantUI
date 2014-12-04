// Copyright 2014 Joseph Riedel. All Rights Reserved.

#pragma once

class ICefWebView;

class ICefRuntimeVariant
{
public:

	virtual ~ICefRuntimeVariant() {}

	enum EVariantType
	{
		TYPE_Undefined,
		TYPE_Null,
		TYPE_Int,
		TYPE_Double,
		TYPE_Bool,
		TYPE_String,
		TYPE_Function,
		TYPE_List,
		TYPE_Blob,
		TYPE_Dictionary
	};

	virtual int AddRef() = 0;
	virtual int Release() = 0;
	virtual EVariantType GetType() = 0;

	inline bool IsUndefined() { return GetType() == TYPE_Undefined; }
	inline bool IsNull() { return GetType() == TYPE_Undefined; }
	inline bool IsInt() { return GetType() == TYPE_Int; }
	inline bool IsDouble() { return GetType() == TYPE_Double; }
	inline bool IsBool() { return GetType() == TYPE_Bool; }
	inline bool IsString() { return GetType() == TYPE_String; }
	inline bool IsFunction() { return GetType() == TYPE_Function; }
	inline bool IsList() { return GetType() == TYPE_List; }
	inline bool IsBlob() { return GetType() == TYPE_Blob; }
	inline bool IsDictionary() { return GetType() == TYPE_Dictionary; }
};

class ICefRuntimeVariantUndefined : public ICefRuntimeVariant
{
};

class ICefRuntimeVariantNull : public ICefRuntimeVariant
{
};

class ICefRuntimeVariantInt : public ICefRuntimeVariant
{
public:
	virtual int GetValue() = 0;
};

class ICefRuntimeVariantUInt : public ICefRuntimeVariant
{
public:
	virtual unsigned int GetValue() = 0;
};

class ICefRuntimeVariantDouble : public ICefRuntimeVariant
{
public:
	virtual double GetValue() = 0;
};

class ICefRuntimeVariantBool : public ICefRuntimeVariant
{
public:
	virtual bool GetValue() = 0;
};

class ICefRuntimeVariantString : public ICefRuntimeVariant
{
public:
	virtual const char* GetValue() = 0;
};

class ICefRuntimeVariantList : public ICefRuntimeVariant
{
public:
	virtual void SetSize(int InSize) = 0;
	virtual int GetSize() = 0;
	virtual ICefRuntimeVariant* GetValue(int InIndex) = 0;
	virtual void SetValue(int InIndex, ICefRuntimeVariant* InValue) = 0;
};

class ICefRuntimeVariantBlob : public ICefRuntimeVariant
{
public:
	virtual size_t GetSize() = 0;
	virtual const void* GetData() = 0;
};

class ICefRuntimeVariantDictionary : public ICefRuntimeVariant
{
public:
	virtual void Clear() = 0;
	virtual bool HasKey(const char *InKey, EVariantType& OutVariantType) = 0;
	virtual void Remove(const char *InKey) = 0;
	virtual ICefRuntimeVariant* GetValue(const char* InKey) = 0;
	virtual void SetValue(const char* InKey, ICefRuntimeVariant* InVariant) = 0;
	virtual ICefRuntimeVariantList* GetKeys() = 0;
};

class ICefRuntimeVariantFactory
{
public:
	virtual ICefRuntimeVariantUndefined* CreateUndefined() = 0;
	virtual ICefRuntimeVariantNull* CreateNull() = 0;
	virtual ICefRuntimeVariantInt* CreateInt(int InValue) = 0;
	virtual ICefRuntimeVariantDouble* CreateDouble(double InValue) = 0;
	virtual ICefRuntimeVariantBool* CreateBool(bool InValue) = 0;
	virtual ICefRuntimeVariantString* CreateString(const char* InValue) = 0;
	virtual ICefRuntimeVariantList* CreateList(int InSize, bool InDefaultNulls = false) = 0;
	virtual ICefRuntimeVariantBlob* CreateBlob(const void* InData, size_t InSize) = 0;
	virtual ICefRuntimeVariantDictionary* CreateDictionary() = 0;

};

class ICefRuntimeJSNativeHook
{
public:

	virtual ~ICefRuntimeJSNativeHook();

	virtual void Execute(
		ICefWebView* InContext,
		const char* InFunctionName,
		ICefRuntimeVariantList* InArguments
		) = 0;
};
