// Copyright 2014 Joseph Riedel. All Rights Reserved.

#pragma once

#include "include/cef_app.h"
#include "../API/CefRuntimeAPI.h"

ICefRuntimeVariantFactory* GetStaticVariantFactory();
CefRefPtr<CefV8Value> ListToV8Array_RenderThread(CefRefPtr<CefListValue> InList);
CefRefPtr<CefListValue> V8ArrayToList_RenderThread(CefRefPtr<CefV8Value> InArray);
CefRefPtr<CefV8Value> ListItemToV8Value_RenderThread(CefRefPtr<CefListValue> InList, int InIndex);
void V8ValueToListItem_RenderThread(CefRefPtr<CefV8Value> InValue, CefRefPtr<CefListValue> InList, int InIndex);
ICefRuntimeVariantBlob* CefBinaryToVariant(CefRefPtr<CefBinaryValue> InValue);
ICefRuntimeVariantDictionary* CefDictionaryToVariant(CefRefPtr<CefDictionaryValue> InValue);
ICefRuntimeVariant* CefDictionaryKeyValueToVariant(CefRefPtr<CefDictionaryValue> InValue, const CefString& Key);
ICefRuntimeVariantList* CefListToVariant(CefRefPtr<CefListValue> InValue);
ICefRuntimeVariant* CefListItemToVariant(CefRefPtr<CefListValue> InValue, int InIndex);
void SetCefListItemFromVariant(CefRefPtr<CefListValue> InList, int InIndex, ICefRuntimeVariant* InVariant);
CefRefPtr<CefListValue> VariantListToCefList(ICefRuntimeVariantList* InList);
void SetCefListFromVariantList(CefRefPtr<CefListValue> InList, ICefRuntimeVariantList* InVariantList);