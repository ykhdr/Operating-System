#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char* argv[]){
	
	void (*func_pointer)();
	void* lib_handler = dlopen("./libhello.so",RTLD_LAZY);
		
	

	if(!lib_handler){
		fprintf(stderr,"dlopen() error: %s\n",dlerror());
		return 1;
	}	
	
	func_pointer = dlsym(lib_handler,"print_hello");
	
	if(!func_pointer){
		fprintf(stderr,"dlsym() error: wrong name of func\n");
		return 1;
	}
		
	(*func_pointer)();
	
	dlclose(lib_handler);
	return 0;
}
