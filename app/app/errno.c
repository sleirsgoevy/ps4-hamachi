void* __error();

void* ___errno_location()
{
    return __error();
}

void* __errno_location()
{
    return __error();
}
