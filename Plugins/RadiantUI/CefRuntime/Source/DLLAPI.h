// Copyright 2014 Joseph Riedel. All Rights Reserved.

#pragma once

class ICefRuntimeAPI;
class ICefRuntimeCallbacks;

typedef ICefRuntimeAPI* (*CreateCefRuntimeAPI)(ICefRuntimeCallbacks*);

#define CEFCREATERUNTIMEAPI_SIG "CreateCefRuntimeAPI"
