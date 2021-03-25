// Copyright ⓒ 2018-2021 ThePhD.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//  See https://github.com/ThePhD/out_ptr/blob/master/docs/out_ptr.adoc for documentation.

// Example adapted from The Boost C++ Libraries book and blog
// https://theboostcpplibraries.com/boost.smartpointers-special-smart-pointers#ex.smartpointers_09

#include <boost/intrusive_ptr.hpp>
#include <ztd/out_ptr.hpp>
#include <atlbase.h>
#include <iostream>
#include <stdexcept>

void intrusive_ptr_add_ref(IDispatch* p) {
	p->AddRef();
}
void intrusive_ptr_release(IDispatch* p) {
	p->Release();
}

void COM_folder_check() {
	CLSID clsid;
	CLSIDFromProgID(CComBSTR{ "Scripting.FileSystemObject" }, &clsid);

	boost::intrusive_ptr<IDispatch> dispatch_ptr(nullptr);
	// put the return value into dispatch_ptr,
	// along with the "false" argument to
	// not AddRef when putting
	// the output parameter into the intrusive_ptr
	HRESULT cci_result = CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER,
		__uuidof(IDispatch), ztd::out_ptr::out_ptr<void*>(dispatch_ptr, false));
	if (FAILED(cci_result)) {
		throw std::runtime_error("failed to create IDispatch Instance");
	}
	// use it however you like...
	CComDispatchDriver dd{ dispatch_ptr.get() };
	CComVariant arg{ "C:\\Windows" };
	CComVariant ret{ false };
	HRESULT inv1_result = dd.Invoke1(CComBSTR{ "FolderExists" }, &arg, &ret);
	if (FAILED(inv1_result)) {
		throw std::runtime_error("failed to run Invoke1 for FolderExists");
	}
	std::cout << "Check if the C:\\Windows folder exists: "
			<< std::boolalpha
			<< (ret.boolVal != 0)
			<< std::endl;
}

int main(int, char* []) {
	CoInitialize(0);

	COM_folder_check();

	CoUninitialize();

	return 0;
}
