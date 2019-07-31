//  Copyright ⓒ 2018-2019 ThePhD.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/out_ptr/ for documentation.

// Example adapted from The Boost C++ Libraries book and blog
// https://theboostcpplibraries.com/boost.smartpointers-special-smart-pointers#ex.smartpointers_09

#include <boost/intrusive_ptr.hpp>
#include <phd/out_ptr.hpp>
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

	phd::intrusive_ptr<IDispatch> dispatch_ptr(nullptr);
	// put the return value into dispatch_ptr,
	// along with the "false" argument to
	// not AddRef when putting
	// the output parameter into the intrusive_ptr
	HRESULT cci_result = CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER,
		__uuidof(IDispatch), phd::out_ptr::out_ptr<void*>(dispatch_ptr, false));
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
