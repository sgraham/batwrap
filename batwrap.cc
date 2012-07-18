// Copyright (c) 2012 Google Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <windows.h>
#include <string>
#include <vector>
#include <stdlib.h>

// Does not embed the batch file, just makes an .exe that calls a .bat file of
// the same name as whatever the exe is named. This means that 'blah' will be
// runnable via via CreateProcess if its directory is in the path and
// 'blah' is specified as lpApplicationName.

int main() {
  using namespace std;
  vector<wstring> to_run;

  // Get best guess at shell.
  wchar_t* comspec = _wgetenv(L"COMSPEC");
  if (comspec == NULL) {
    wchar_t* sysroot = _wgetenv(L"SystemRoot");
    if (!sysroot) {
      to_run.push_back(L"cmd.exe");
    } else {
      to_run.push_back(wstring(sysroot) + wstring(L"\\System32\\cmd.exe"));
    }
  } else {
    to_run.push_back(comspec);
  }

  to_run.push_back(L"/s /c");

  // Get the name of this exe, and substitute .bat in there.
  wchar_t module_name[_MAX_PATH];
  if (!GetModuleFileName(NULL, module_name, sizeof(module_name))) {
    fprintf(stderr, "Couldn't GetModuleFileName\n");
    return -1;
  }

  wchar_t* ext = wcsrchr(module_name, '.');
  if (!ext) {
    fprintf(stderr, "Couldn't find extension of self!\n");
    return -1;
  }
  wcscpy(ext, L".bat");
  to_run.push_back(module_name);

  // Get the args passed to the original invocation to pass them along.
  int num_args;
  LPWSTR* args = CommandLineToArgvW(GetCommandLine(), &num_args);
  for (int i = 1; i < num_args; ++i)
    to_run.push_back(args[i]);
  LocalFree(args);

  wstring final;
  int count = 0;
  for (vector<wstring>::iterator i(to_run.begin()); i != to_run.end(); ++i) {
    // TODO: Is the backslash handling correct?
    if (count++ < 2)
      final += *i + wstring(L" ");
    else
      final += wstring(L"\"") + *i + wstring(L"\" ");
    if (count == 2) {
      final += L"\"";
    }
  }
  final.back() = '\"';
  if (_wgetenv(L"BATWRAP_DEBUG"))
    wprintf(L"BATWRAP_DEBUG running: %s\n", final.c_str());
  return _wsystem(final.c_str());
}
