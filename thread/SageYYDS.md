在 C++ 中，非静态成员函数是依赖于对象存在的，必须通过对象才能访问。而线程函数是不依赖于对象存在的，可以直接使用类名调用，因此必须是静态函数。

在上面的代码中，`threadfunc` 函数是作为线程函数使用的，因此必须是静态函数。如果不将其声明为静态函数，那么在创建线程时编译器就会报错，因为线程函数必须是一个全局函数或静态成员函数。

另外，因为静态成员函数不依赖于对象的存在，因此无法访问非静态成员变量和非静态成员函数。在这个代码中，`thread_pool` 类中唯一的非静态成员变量是 `tasks`，而 `threadfunc` 函数中确实需要访问 `tasks` 变量，这里使用了 `static_cast` 将 `void*` 类型的 `arg` 参数转换为 `thread_pool*` 类型，就可以在静态成员函数中访问非静态成员变量了。